/*
 * crypto.cpp
 * Author: Aven Bross
 * Date: 8/30/2015
 * 
 * Description:
 * Encryption classes wrapping openssl functionality.s
*/

#include "crypto.h"

/*
 * class ECDH
 * Wraps openssl and performs ECDH exchange
 */

// Initialize context and generate key pair
ECDH::ECDH(): _done(false), _dead(false){
    // Create an Elliptic Curve Group object and set it up to use the secp256k1 curve
	if(NULL == (group = EC_GROUP_new_by_curve_name(NID_secp256k1))) handleErrors();
	
	// Create an Elliptic Curve Key object
	if(NULL == (key = EC_KEY_new())) handleErrors();
	
	// Set the key to use the group we created
	if(1 != EC_KEY_set_group(key, group)) handleErrors();
	
	if(NULL == (bn_ctx = BN_CTX_new())) handleErrors();
	
	// Generate the private and public key
	if(1 != EC_KEY_generate_key(key)) handleErrors();
}

// Returns the public key for sharing with peer
std::string ECDH::getPublicKey() const{
    if(_dead){
        return std::string("");
    }
    
    unsigned char buffer[65];
    
    const EC_POINT * point = EC_KEY_get0_public_key(key);
    
    std::cout << EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED, buffer, 0, bn_ctx);
    
    return std::string("");
}

// Recieve public key from peer
void ECDH::recieveKey(const std::string & otherKey){
    if(_done || _dead) return;
    
    // Grab the keys
    EC_POINT * peerkey;
    
    if(1 != EC_POINT_oct2point(group, peerkey, (unsigned char*)otherKey.c_str(), otherKey.size(), bn_ctx)) handleErrors();

    /* Calculate the size of the buffer for the shared secret */
	field_size = EC_GROUP_get_degree(group);
	secret_len = (field_size+7)/8;

	/* Allocate the memory for the shared secret */
	if(NULL == (secret = (unsigned char*)OPENSSL_malloc(secret_len))) handleErrors();

	/* Derive the shared secret */
	secret_len = ECDH_compute_key(secret, secret_len, peerkey, key, NULL);
	
	EC_POINT_free(peerkey);
	
	_done = true;
}

// Retrieve shared secret
std::string ECDH::getSecret() const{
    if(!_done || _dead){
        return std::string("");
    }
    else{
        return std::string((char*)secret, secret_len);
    }
}

// Handle errors reported by openssl API
void ECDH::handleErrors(){
    _dead = true;
}

// Free up memory
ECDH::~ECDH(){
    if(key != NULL) EC_KEY_free(key);
    if(group != NULL) EC_GROUP_free(group);
	if(bn_ctx != NULL) BN_CTX_free(bn_ctx);
	if(secret != NULL) delete secret;
}



/*
 * Crypto Functions
 */

// Encode string in base 64
std::string base64Encode(const std::string & str){
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
    
    // Encode data via conversion BIO
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
    
    // Free BIO memory
    BIO_flush(b64);
    
    // Grab pointer to results
    unsigned char* data;
    long length = BIO_get_mem_data(mem, &data);
    
    // Convert results to string
    return std::string((char *)data, length);
}

// Compute sha1 hash of string
std::string sha1(const std::string & str){
    // Buffer to store hash result
    unsigned char obuf[20];

    // Compute sha1 hash of input string
    SHA1((unsigned char *)str.c_str(), str.size(), obuf);
    
    // Convert results to string
    return std::string((char *)obuf, 20);
}
