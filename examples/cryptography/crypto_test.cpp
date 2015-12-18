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
    std::cout << publicKey << "\n";
    std::string keys = base64Encode(publicKey);
    std::cout << base64Encode(publicKey) << "\n";
    std::cout << keys.size() << "\n";
    std::string backagain = base64Decode(keys);
    std::cout << backagain << "\n";
    test.recieveKey(backagain);
    std::string secret = base64Encode(test.getSecret());
    std::cout << secret << "\n";
    std::cout << secret.size() << "\n";
    std::cout << base64Decode(secret).size() << "\n";
    
    return 0;
}
