/*
 * server.h
 * Author: Aven Bross
 * Date: 5/23/2015
 * 
 * Description:
 * Multithreaded server to recieve and manage connections.
*/

#ifndef __SERVER_H
#define __SERVER_H

#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "osl/socket.h"

// Forward declaration
class Connection;

// Virtual server class representing a multithreaded server
class Server{
public:
    // Constructor
    Server(unsigned int port);
    
    // Start server
    void start();
    
    // Stop server
    void stop();
    
    // Kill a connection
    void kill(const sockaddr & connectionAddress);
    
    // Check server status
    bool isDead();
    
    // Virtual destructor
    virtual ~Server();
    
protected:
    // Main server loop
    virtual void loop();
    
    // List containing server connection threads
    std::unordered_map<std::string,std::shared_ptr<Connection>> _connections;
    
    std::thread _thread; // Server thread
    bool _dead; // Server state
    int _socket; // Server socket
    std::mutex _mutex;  // Connection list mutex
};

// Subclass of server that handles TCP connections
class TCPServer : public Server{
public:
    // Inherit constructor
    using Server::Server;
 
protected:
    // TCP server loop
    virtual void loop();
};

// Subclass of server that handles UDP connections
class UDPServer : public Server{
public:
    // Inherit constructor
    using Server::Server;
    
protected:
    // UDP server loop
    virtual void loop();
};


// Connection class representing a connection to remote host
class Connection {
public:
    // Constructor takes ptr to server
    Connection(int socket, Server * server, const sockaddr & toAddress);
    
    // Send message to connection recipient, blocks waiting for send
    virtual void sendMessage(const std::string & message) = 0;
    
    void kill();
    
    // Destructor
    virtual ~Connection();
    
protected:
    // Called on connection creation
    virtual void onOpen();
    
    // Called when new message is recieved
    virtual void onMessage(const std::string & message);
    
    // Pointer to server running this connection
    Server * _server;
    
    // Calls onMessage when new messages are recieved then blocks
    virtual void loop() = 0;
    
    sockaddr _toAddress; // Connection address
    int _socket; // Connection socket
    bool _dead; // Connection status
};


// Connection class representing a TCP connection to remote host
class TCPConnection : public Connection {
public:
    // Inherit constructor
    TCPConnection(int socket, Server * server, const sockaddr & toAddress);
    
    // Send message to connection recipient
    virtual void sendMessage(const std::string & message);
    
    ~TCPConnection();
    
protected:
    // Called on connection closure
    virtual void onClose();
    
    // Calls onMessage when new messages are recieved then blocks
    virtual void loop();
    
    // Connection failure routine
    virtual void fail();
    
    std::thread _thread;    // Thread to run connection
};


// Connection class representing a UDP connection to remote host
class UDPConnection : public Connection {
public:
    // Overloaded constructor to add address parameter
    UDPConnection(int socket, Server * server, const sockaddr & toAddress);
    
    // Called to push messages to this connection
    void push(const std::string & message);
    
    // Send message to connection recipient
    void sendMessage(const std::string & message);
    
    ~UDPConnection();
    
protected:
    // Called on connection closure
    virtual void onClose();
    
    // Calls onMessage when new messages are recieved then blocks
    virtual void loop();
    
    // Connection failure routine
    virtual void fail();
    
    std::mutex _messageMutex;   // Mutex for message recieves
    std::condition_variable _cv; // Condition variable for message recieves
    std::queue<std::string> _messageQueue;   // Queue of new messages
    std::thread _thread;    // Thread to run connection
};


std::string to_string(const sockaddr & addr);

#endif