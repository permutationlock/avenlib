/*
 * ecc_bitcoin_jacobi.js
 * Author: Aven Bross - javascript translation
 *         Orion Lawlor - original python jacobian ecc
 * Date: 3-6-2015
 * Eliptic curve crypto with jacobian optimization
 * REQUIRES BigInteger.js or BigInteger.min.js
 *   from: https://github.com/peterolson/BigInteger.js
 */


/*
 * Module object
 */

var ECCbitcoin = {
    // Removes spaces and returns bigInt
    hex2int: function(hexstr){
        hexstr = hexstr.replace(/\s+/g, '');
        return bigInt(hexstr, 16);
    },

    // Get cryptographically secure random numbers
    // number of bits must be divisible by 8
    getRandomBits: function(bits){
        // Get random bytes
        var buffer = new Uint8Array(bits/8);
        window.crypto.getRandomValues(buffer);  // Only works in chrome
        
        var value = bigInt(0);
        for(var i = buffer.length - 1; i >= 0; i--){
            value = value.multiply(256).add(buffer[i]);
        }
        
        return value;
    },

    // Half extended gcd
    //   only calculates lastx (aa)
    //   does not calculate lasty (bb)
    half_extended_gcd: function(aa, bb){
        var lastrem = bigInt(aa).abs(); 
        var rem = bigInt(bb).abs();
        var x = bigInt(0);
        var lastx = bigInt(1);
        
        while(!(rem.equals(0))){
            var oldRem = bigInt(rem);
            var temp = lastrem.divmod(rem);
            var quotient = temp.quotient;
            rem = temp.remainder;
            lastrem = oldRem;
            
            var oldX = bigInt(x);
            x = lastx.subtract(bigInt(quotient).multiply(x));
            lastx = oldX;
        }
        
        return [lastrem, lastx];
    },

    // Computes multiplicative inverse of a mod m
    modular_inverse: function(a, m){
        var temp = this.half_extended_gcd(a,m);
        
        if(!(temp[0].equals(1))){
            console.log("No inverse found");
        }
        var inverse = bigInt(temp[1]).mod(m);
        if(inverse.sign){
            inverse = inverse.add(m);
        }
        return inverse;
    }
};


/*
 * Eliptic curve cryptography classes
 */

// An elliptic curve has these fields:
//   p: the prime used to mod all coordinates
//   a: linear part of curve: y^2 = x^3 + ax + b
//   b: constant part of curve
//   G: a curve point (G.x,G.y) used as a "generator"
//   n: the order of the generator
var ECcurve = function(){
    // Prime field multiplication: return a*b mod p
    this.field_mul = function(a,b){
        return bigInt(a).multiply(b).mod(this.p);
    };

    // Prime field division: return num/den mod p
    this.field_div = function(num,den){
        var inverse_den = ECCbitcoin.modular_inverse(bigInt(den).mod(this.p), this.p);
        return this.field_mul(bigInt(num).mod(this.p), inverse_den);
    };

    // Prime field exponentiation: raise num to power mod p
    this.field_exp = function(num,power){
        return bigInt(num).mod(this.p).modPow(power, this.p);
    };

    // Return the special identity point
    //   We pick x=p, y=0
    this.identity = function(){
        return new ECpoint(this, this.p, 0, 1);
    };

    // Return true if point Q lies on our curve
    this.touches = function(Q){
        var x = Q.get_x();
        var y = Q.get_y();
        var y2 = y.multiply(y).mod(this.p);
        var x3ab = this.field_mul(x.multiply(x).mod(this.p).add(this.a), x).add(this.b).mod(this.p);
        return y2.equals(x3ab);
    };

    // Return the slope of the tangent of this curve at point Q
    this.tangent = function(Q){
        return this.field_div(Q.x.multiply(Q.x).multiply(3).add(this.a), Q.y.multiply(2));
    };

    // Return a doubled version of this elliptic curve point
    //  Closely follows Gueron & Krasnov 2013 figure 2
    this.double = function(Q){
        if(Q.x.equals(this.p)) // doubling the identity
            return Q;
        var S = Q.x.multiply(Q.y).multiply(Q.y).multiply(4).mod(this.p);
        var Z2 = Q.z.multiply(Q.z);
        var Z4 = Z2.multiply(Z2).mod(this.p);
        var M = Q.x.multiply(Q.x).multiply(3).add(bigInt(this.a).multiply(Z4));
        var x = M.multiply(M).subtract(bigInt(S).multiply(2)).mod(this.p);
        if(x.sign) x = x.add(this.p);
        var Y2 = Q.y.multiply(Q.y);
        var y = M.multiply(S.subtract(x)).subtract(Y2.multiply(Y2).multiply(8)).mod(this.p);
        if(y.sign) y = y.add(this.p);
        var z = Q.z.multiply(Q.y).multiply(2).mod(this.p);
        return new ECpoint(this, x, y, z);
    };

    // Return the "sum" of these elliptic curve points
    //  Closely follows Gueron & Krasnov 2013 figure 2
    this.add = function(Q1,Q2){
        // Identity special cases
        if(Q1.x.equals(this.p)) // Q1 is identity
            return Q2;
        if(Q2.x.equals(this.p)) // Q2 is identity
            return Q1;
        var Q1z2 = Q1.z.multiply(Q1.z);
        var Q2z2 = Q2.z.multiply(Q2.z);
        var xs1 = Q1.x.multiply(Q2z2).mod(this.p);
        var xs2 = Q2.x.multiply(Q1z2).mod(this.p);
        var ys1 = Q1.y.multiply(Q2z2).multiply(Q2.z).mod(this.p);
        var ys2 = Q2.y.multiply(Q1z2).multiply(Q1.z).mod(this.p);
        
        // Equality special cases
        if(xs1.equals(xs2)){
            if(ys1.equals(ys2)) // adding point to this
                return this.double(Q1);
            else // vertical pair--result is the identity
                return this.identity();
        }

        // Ordinary case
        var xd = xs2.subtract(xs1).mod(this.p);
        if(xd.sign) xd = xd.add(this.p);
        var yd = ys2.subtract(ys1).mod(this.p);
        if(yd.sign) yd = yd.add(this.p);
        var xd2 = xd.multiply(xd).mod(this.p);
        var xd3 = xd2.multiply(xd).mod(this.p);
        var x = yd.multiply(yd).subtract(xd3).subtract(xs1.multiply(xd2).multiply(2)).mod(this.p);
        if(x.sign) x = x.add(this.p);
        var y = yd.multiply(bigInt(xs1).multiply(xd2).subtract(x)).subtract(ys1.multiply(xd3)).mod(this.p);
        if(y.sign) y = y.add(this.p);
        var z = xd.multiply(Q1.z).multiply(Q2.z).mod(this.p);
        return new ECpoint(this, x, y, z);
    };

    // "Multiply" this elliptic curve point Q by the integer m
    //    Often the point Q will be the generator G
    this.mul = function(Q,i){
        var R = this.identity(); // return point
        var m = bigInt(i);
        while(!(m.equals(0))){  // binary multiply loop
            if(m.and(1).equals(1)){ // bit is set
                R = this.add(R, Q);
            }
            m = m.shiftRight(1);
            if(!(m.equals(0))){
                Q = this.double(Q);
            }
        }
        
        return R;
    };
};
            

// A point on an elliptic curve: (x,y)
//   Using special (X,Y,Z) Jacobian point representation
var ECpoint = function(curve, x, y, z){
    // A point on an elliptic curve (x/z^2,y/z^3)
    this.curve=curve
    this.x = bigInt(x);
    this.y = bigInt(y);
    this.z = bigInt(z);
    // This this-check has a big performance cost.
    //if(!(x.equals(curve.p)) && !(curve.touches(this)))
    //    console.log(" ECpoint left curve: " + this);

    // "Add" this point to another point on the same curve
    this.add = function(Q2){
        return this.curve.add(this, Q2);
    };

    // "Multiply" this point by a scalar
    this.mul = function(m){
        return this.curve.mul(this, m);
    };
    
    // Extract non-projective X and Y coordinates
    //   This is the only time we need the expensive modular inverse
    this.get_x = function(){
        return this.curve.field_div(this.x, this.z.modPow(2,this.curve.p));
    };
    
    this.get_y = function(){
        return this.curve.field_div(this.y, this.z.modPow(3,this.curve.p));
    };

    // console.log this ECpoint
    this.toString = function(){
        if(this.x.equals(this.curve.p))
            var ret = "identity_point";
        else
            var ret = "(" + this.get_x().toString(16) + ", " + this.get_y().toString(16) + ")";
        return ret;
    };
};

// This is the BitCoin elliptic curve, SECG secp256k1
//   See http://www.secg.org/SEC2-Ver-1.0.pdf
ECCbitcoin.secp256k1 = new ECcurve();
ECCbitcoin.secp256k1.p = ECCbitcoin.hex2int("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
ECCbitcoin.secp256k1.a = bigInt(0); // it's a Koblitz curve, with no linear part
ECCbitcoin.secp256k1.b = bigInt(7);
ECCbitcoin.secp256k1.n = ECCbitcoin.hex2int("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE BAAEDCE6 AF48A03B BFD25E8C D0364141");

// SEC's "04" means they're representing the generator point's X,Y parts explicitly.
//  The compressed "02" form means storing only x (you compute Y)
ECCbitcoin.secp256k1.G = new ECpoint(ECCbitcoin.secp256k1,
  ECCbitcoin.hex2int("79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798"),
  ECCbitcoin.hex2int("483ADA77 26A3C465 5DA4FBFC 0E1108A8 FD17B448 A6855419 9C47D08F FB10D4B8"),
  ECCbitcoin.hex2int("1")  // projective coordinates
);
