/*
 * crypto_test.cpp
 * Author: Aven Bross
 * Date: 3/4/2015
 * 
 * Testing cryptography functions
 */
 
#include "../../cryptography/crypto.h"
#include <iostream>
#include <cstring>
#include <cstdio>

using std::cout;
using std::cin;

int main(){
    ECDH test;
    
    std::string publicKey = test.getPublicKey();
    
    std::cout << base64Encode(publicKey);
    
    return 0;
}
