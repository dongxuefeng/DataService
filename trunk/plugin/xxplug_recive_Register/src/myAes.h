//
//  myAes.h
//  
//
//  Created by X on 12-10-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//


#ifndef __myAes__H__
#define __myAes__H__

#include <stdlib.h>


#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
    uint32 erk[64];     /* encryption round keys */
    uint32 drk[64];     /* decryption round keys */
    int nr;             /* number of rounds */
}
aes_context;


// 数据缓存
struct myBuffer {
    
    myBuffer():_bytes(NULL), _len(0){}
    
    myBuffer(void* buf, int len);
    
    ~myBuffer() {
        if(_bytes) { 
            free(_bytes);
            _bytes = NULL; 
        }
    }
    
    void appendBuffer(void* buff, int len);
	void clearBuffer();
    
    void* _bytes;
    int _len;
};

// ==============================================
class myAes {
public:
    
    myAes();
    
    // 设置密码
    void set128Key(const char* key);
    
    // 加密
    void encryption(myBuffer& buf);
    
    // 解密
    void decryption(myBuffer& buf);
    
private:
    
    void aes_gen_tables();
    int  aes_set_key( aes_context *ctx, uint8 *key, int nbits );
    void aes_encrypt( aes_context *ctx, uint8 input[16], uint8 output[16] );
    void aes_decrypt( aes_context *ctx, uint8 input[16], uint8 output[16] );
    
    
private:
    
    aes_context        _ctx;
    
private:
    
    int KT_init;
    
    /* forward S-box & tables */
    uint32 KT0[256];
    uint32 KT1[256];
    uint32 KT2[256];
    uint32 KT3[256];
    
    uint32 FSb[256];
    uint32 FT0[256]; 
    uint32 FT1[256]; 
    uint32 FT2[256]; 
    uint32 FT3[256]; 
    
    /* reverse S-box & tables */
    
    uint32 RSb[256];
    uint32 RT0[256];
    uint32 RT1[256];
    uint32 RT2[256];
    uint32 RT3[256];
    
    /* round constants */
    
    uint32 RCON[10];
    
    /* tables generation flag */
    
    int do_init;
    
};





#endif /* aes.h */
