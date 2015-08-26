/*
 * aes_test.cpp
 * Author: Aven Bross
 * Date: 3/4/2015
 * 
 * Testing AES functionality
 */
 
#include <iostream>
#include "../../../networking/osl/socket.h"

using std::cout;
using std::cin;

int main(){
    skt_ip_t ip = { 127, 0, 0, 1 };
    unsigned int port = 9999;
    
    std::string msg = "hiiii!";
    int socket = skt_connect(ip, port, 5);
    while(1){
        cin >> msg;
        skt_sendN(socket, msg.c_str(), msg.size()+1);
    }
    return 0;
}
