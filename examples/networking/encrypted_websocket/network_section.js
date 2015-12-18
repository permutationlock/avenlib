// NETWORK AND SECURITY CODE

var network = {

    // Eliptic curve key exchange
    curve : ECCbitcoin.secp256k1,
    privateKey : bigInt(0),
    sharedKey : null,

    // Code maps
    mesType : { ERROR:0, KEY_SHARE:1, CREATE:2, LOGIN:3, VERIFY:4, UPDATE: 5 },
    createCode : { SUCCESS:0, UNAME_CHAR:1, UNAME_LENGTH:2, PASS_CHAR:3, PASS_LENGTH:4 },
    loginCode : { SUCCESS:0, BAD_PASS:1, NO_USER:2, VERIFY:3 },
    verifyCode : { SUCCESS:0, BAD_KEY:1, NO_USER:2, LOGIN:3 },
    updateCode : { SUCCESS:0, LOGIN:1, VERIFY:2, NO_USER:3 },   // NO_USER should never happen
    errorCode : { DATABASE:0, DECRYPT:1, BAD_REQ:2, NO_KEY:3 },

    // Encrypt the given message with the shared key and send
    sendEncrypted : function(message,mtype){
        // Check for shared key
        if(sharedKey == null) return false;
        
        // Format message
        var full = mtype + ',' + message;
        
        // Message length as hex string
        var origLength = (full.length).toString(16);
        
        // Generate 128 bit initialization vector
        var iv  = bigInt(ECCbitcoin.getRandomBits(128)).toString(16);
        while(iv.length < 32){
            iv = '0' + iv;
        }
        
        // Generate byte arrays and compute ciphertext
        var plaintext = cryptoHelpers.convertStringToByteArray(full);
        var ivArray = cryptoHelpers.toNumbers(iv);
        console.log(sharedKey.slice(0,32));
        var ciphertext = slowAES.encrypt(plaintext, slowAES.modeOfOperation.CBC, sharedKey.slice(0,32), ivArray);
        
        // Compute hex string from byte array
        var encrypted = cryptoHelpers.toHex(ciphertext);
        
        console.log(origLength + "," + iv + "," + encrypted)
        
        // Send encrypted message
        this.ws.send(origLength + "," + iv + "," + encrypted);
        return true;
    },

    // Decrypt incoming message
    decrypt : function(request){
        // Check if key exchange has been made
        if(sharedKey == null)
            return request;
        
        // DECRYPT DATA
        
        // Decrypt message with sharedKey and sent length and IV
        /*var origLength = cryptoHelpers.toNumbers(args[0]);  // Apparently not needed for js
        var iv = cryptoHelpers.toNumbers(args[1]);
        var ciphertext = cryptoHelpers.toNumbers(args[2]);
        console.log(sharedKey.slice(0,32));
        var plaintext = slowAES.decrypt(ciphertext, slowAES.modeOfOperation.CBC, sharedKey.slice(0,32), iv);
        return cryptoHelpers.convertByteArrayToString(plaintext);*/
    },

    // Handle server request
    handleRequest : function(request){
        // Split up request components
        var args = request.split(",");
        
        // Respond to message based on type
        var kind = args[0];
        
        // Check if keys have been shared
        if(sharedKey == null){
            switch(kind){
                case this.mesType.KEY_SHARE:
                    // Recieve public key
                    recieveKey(args);
                    break;
                case this.mesType.ERROR:
                    // Error exchanging just re-share
                    this.shareKey();
                    break;
                default:
                    // Unrecognized or malformed message
                    this.sendEncrypted(errorCode.BAD_REQ, mesType.ERROR);
            }
        }
        else{
            switch(kind){
                default:
                    break;
            }
        }
    },

    // Generate private key and share corresponding public key
    shareKey : function(){
        alert("HOLY FUCKING SHIT");
        this.privateKey = bigInt(ECCbitcoin.getRandomBits(256)).mod(curve.p)
        var point = this.curve.mul(curve.G, privateKey);
        var keyX = point.get_x().toString(16);
        while(keyX.length < 64){
            keyX = '0' + keyX;
        }
        var keyY = point.get_y().toString(16);
        while(keyY.length < 64){
            keyY = '0' + keyY;
        }
        this.ws.send("0,"+keyX+","+keyY);
    },

    // Recieve server public key
    recieveKey : function(args){
        var x = bigInt(args[0], 16);
        var y = bigInt(args[1], 16);
        var z = bigInt("1", 16);
        var point = new ECpoint(curve,x,y,z);
        var sharedPoint = curve.mul(point,privateKey);
        var keyX = sharedPoint.get_x().toString(16);
        while(keyX.length < 64){
            keyX = '0' + keyX;
        }
        var keyY = sharedPoint.get_y().toString(16);
        while(keyY.length < 64){
            keyY = '0' + keyY;
        }
        this.sharedKey = cryptoHelpers.toNumbers(keyX+keyY);
        console.log(keyX+keyY);
        console.log(keyX.length);
    },

    // Establish connection
    connect : function(){
        alert("OHOH");
        this.ws = new WebSocket("ws://127.0.0.1:9999/");
        this.ws.onopen = this.shareKey;
        this.ws.onmessage = function(e){
            var request = new Uint8Array(e.data);
            handleRequest(decrypt(request));
        }
    },

    // If for some reason there is no key and you are connected, just reconnect
    noKeyError : function(){
        this.ws.close();
        connect();
    }
};