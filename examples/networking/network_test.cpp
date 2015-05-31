/*
 * aes_test.cpp
 * Author: Aven Bross
 * Date: 3/4/2015
 * 
 * Testing AES functionality
 */
 
#include "../../networking/server.h"
#include <iostream>
#include "../../networking/osl/socket.h"

using std::cout;
using std::cin;

int main(){
    TCPServer s(9999);
    s.start();
    skt_ip_t ip = { 127, 0, 0, 1 };
    unsigned int port = 9999;
    
    while(1){
        if(s.isDead()){
            s.start();
        }
        std::cin.ignore();
    }
    return 0;
}
