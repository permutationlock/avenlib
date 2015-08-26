/*
 * websocket_server.cpp
 * Author: Aven Bross
 * Date: 8/21/2015
 * 
 * Description:
 * Multithreaded websocket server.
*/

#include "websocket_server.h"

/*
 * class WebSocketServer : TCPServer
 * Subclass of TCPServer that recieves and handles websocket connections
 */

// Make a new connection for the server
std::shared_ptr<TCPConnection> WebSocketServer::makeConnection(int socket, const sockaddr & clientAddress){
    return std::make_shared<WebSocketConnection>(socket, this, clientAddress);
}


/*
 * class WebSocketConnection : TCPConnection
 * Connection class representing a WebSocket connection to remote host
 */

// Magic string constant for finding handshake keys
const std::string WebSocketConnection::_magicString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

// Constructor, does any initializations necessary then calls parent constructor
WebSocketConnection::WebSocketConnection(int socket, Server * server, const sockaddr & toAddress):
  TCPConnection(socket, server, toAddress){
    std::cout << "WEBSOCKET CREATED\n";
}
 
// Send message via websocket protocol
void WebSocketConnection::sendMessage(const std::string & message){
    if(!_handshake){
        // No handshake, cannot send via websocket
        return;
    }
}
    
// Parse TCP message and handle websocket responsse
void WebSocketConnection::onMessage(const std::string & message){
    std::cout << message << "\n";
}

// Handle websocket message from client
void WebSocketConnection::loop(){
    std::cout << "WEBSOCKET LOOP\n";
    const char *term=" \t\r\n";
    char c;
    std::vector<std::string> buffer;
    std::string message = "";
    
    while(!_dead){
        if(!_handshake){
            if(skt_recvN(_socket,&c,1) != 0){
                fail(); // Connection failure
            }
            else if(strchr(term,c)){
	            if(c=='\r') continue; // Will be CR/LF; wait for LF
	            else if(message.compare("")){
	                buffer.push_back(message);
	                message = "";
	            }
	            else{
	                _handshake = parseHandshake(buffer);
	            }
            }
            else{
                message+=c; // normal character
            }
        }
        else{
            
        }
    }
}

// Encode string in base 64
std::string WebSocketConnection::base64Encode(const std::string & str){
    // BIO to perform base 64 encoding
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    
    // Memory BIO to store results
    BIO *mem = BIO_new(BIO_s_mem());
    
    // Connect base 64 conversion BIO to the memory BIO
    BIO_push(b64, mem);
    
    // Encode data into BIO
    bool done = false;
    int res = 0;
    
    while(!done){
        res = BIO_write(b64, (unsigned char *)str.c_str(), str.size());

        if(res <= 0){
            if(BIO_should_retry(b64)){
                continue;
            }
            else{
                // Handle errors maybe...
            }
        }
        else{
            done = true;
        }
    }
    
    BIO_flush(b64);
    
    // Grab pointer to results
    unsigned char* data;
    long length = BIO_get_mem_data(mem, &data);
    
    // Convert results to string
    return std::string((char *)data, length);
}

// Compute sha1 hash of string
std::string WebSocketConnection::sha1(const std::string & str){
    // Buffer to store hash result
    unsigned char obuf[20];

    // Compute sha1 hash of input string
    SHA1((unsigned char *)str.c_str(), str.size(), obuf);
    
    // Convert results to string
    return std::string((char *)obuf, 20);
}

// Parse handshake and respond if correct
bool WebSocketConnection::parseHandshake(const std::vector<std::string> & buffer){
    // Create error HTTP response
    std::string error("HTTP/1.1 404 Not Found\nConetent-type: text/html\nContent-length: 0");
    error.append("\n\n");
    
    // Check HTTP header is correct
    if(buffer[0].compare("GET") || buffer[1].compare("/") || buffer[2].compare("HTTP/1.1")){
        sendTCP(error);
        return false;
    }
    
    // Read request and organize by attribute name
    std::map<std::string, std::vector<std::string>> attributes;
    std::string header = "";
    for(int i=3; i<buffer.size(); i++){
        if(buffer[i].back() == ':'){
            // Remove ':' from end and store key
            header = buffer[i].substr(0,buffer[i].size()-1);
        }
        else if(header.compare("")){
            attributes[header].push_back(buffer[i]);
        }
    }
    
    // Check if client sent a key
    if(attributes.count("Sec-WebSocket-Key") < 1){
        sendTCP(error);
        return false;
    }
    
    // Compute server accept key from client key
    std::string key = base64Encode(sha1(attributes["Sec-WebSocket-Key"].front() + _magicString));
    
    // Make sure client attributes are correct
    if(attributes["Sec-WebSocket-Version"].front().compare("13") ||
       attributes["Upgrade"].front().compare("websocket") ||
       attributes["Connection"].front().compare("Upgrade")){
        sendTCP(error);
        return false;
    }
    
    // Generate server response
    std::string response("HTTP/1.1 101 Switching Protocols\nUpgrade: websocket\nConnection: Upgrade\n");
    response.append("Connection: Upgrade\nSec-WebSocket-Accept: ");
    response.append(key);
    response.append("\n\n");
    sendTCP(response);
    
    // Connection open
    onOpen();
    
    return true;
}

// Send error message to client
void WebSocketConnection::sendTCP(const std::string & message){
    if(skt_sendN(_socket, message.c_str(), message.size()+1) != 0){
        fail();
    }
}
