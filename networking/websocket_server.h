/*
 * websocket_server.h
 * Author: Aven Bross
 * Date: 8/21/2015
 * 
 * Description:
 * Multithreaded websocket server.
*/

#include <regex>
#include <sstream>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "server.h"

// Subclass of TCPServer that recieves and handles websocket connections
class WebSocketServer : public TCPServer{
public:
    using TCPServer::TCPServer;
    
protected:
    // Make a new UDP connection for the server
    virtual std::shared_ptr<TCPConnection> makeConnection(int socket, const sockaddr & clientAddress);
};

// Connection class representing a WebSocket connection to remote host
class WebSocketConnection : public TCPConnection {
public:
    WebSocketConnection(int socket, Server * server, const sockaddr & toAddress);

    // Send message via websocket protocol
    virtual void sendMessage(const std::string & message);
    
protected:
    
    // Parse TCP message and handle websocket responsse
    virtual void onMessage(const std::string & message);
    
    // Handle websocket message from client
    virtual void loop();
    
    // Encode string in base 64
    std::string base64Encode(const std::string & str);
    
    // Compute sha1 hash of string
    std::string sha1(const std::string & str);
    
    // Parse handshake and respond if correct
    bool parseHandshake(const std::vector<std::string> & buffer);
    
    // Send message via raw TCP
    void sendTCP(const std::string & message);
    
    bool _handshake;
    static const std::string _magicString;  // Magic string constant for finding handshake keys
};
