/*********************************************************************
* Filename:   md5.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding MD5 implementation.
*********************************************************************/

#ifndef PSDK_MD5_H
#define PSDK_MD5_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define MD5_BLOCK_SIZE 16               // MD5 outputs a 16 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD_PSDK;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
   BYTE data[64];
   WORD_PSDK datalen;
   unsigned long long bitlen;
   WORD_PSDK state[4];
} MD5_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void md5_init(MD5_CTX *ctx);
void md5_update(MD5_CTX *ctx, const BYTE data[], size_t len);
void md5_final(MD5_CTX *ctx, BYTE hash[]);

#ifdef __cplusplus
}
#endif

#endif   // MD5_H
