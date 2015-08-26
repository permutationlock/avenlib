/*
 * aes_test.cpp
 * Author: Aven Bross
 * Date: 3/4/2015
 * 
 * Testing AES functionality
 */
 
#include "../../../networking/websocket_server.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include "../../../networking/osl/socket.h"

using std::cout;
using std::cin;

int main(){
    WebSocketServer s(9999);
    s.start();
    
    while(1){
        if(s.isDead()){
            s.start();
        }
        std::cin.ignore();
    }
    return 0;
}
