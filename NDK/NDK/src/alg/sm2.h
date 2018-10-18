/*
 * =====================================================================================
 *
 *       Filename:  cmd.h
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  04/23/2015 02:23:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Johnny (wzy), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef 	_SM2_H
#define 	_SM2_H

#include "sm2_common.h"
#include "ec_param.h"
#include "xy_ecpoint.h"
#include "sm2_ec_key.h"
#include "util.h"
#include "sm2_test_param.h"


typedef struct {
    BYTE *message;
    int message_byte_length;
    BYTE *ID;
    int ENTL;
    BYTE k[MAX_POINT_BYTE_LENGTH];  //签名中产生随机数
    BYTE private_key[MAX_POINT_BYTE_LENGTH];
    struct {
        BYTE x[MAX_POINT_BYTE_LENGTH];
        BYTE y[MAX_POINT_BYTE_LENGTH];
    } public_key;
    BYTE Z[HASH_BYTE_LENGTH];
    BYTE r[MAX_POINT_BYTE_LENGTH];
    BYTE s[MAX_POINT_BYTE_LENGTH];
    BYTE R[MAX_POINT_BYTE_LENGTH];
} sm2_sign_st;

typedef struct {
    BYTE *message;
    int message_byte_length;
    //BYTE *encrypt;
    BYTE *decrypt;
    int klen_bit;

    BYTE k[MAX_POINT_BYTE_LENGTH];  //随机数
    BYTE private_key[MAX_POINT_BYTE_LENGTH];
    struct {
        BYTE x[MAX_POINT_BYTE_LENGTH];
        BYTE y[MAX_POINT_BYTE_LENGTH];
    } public_key;

    BYTE C[1024];    // C_1 || C_2 || C_3
    BYTE C_1[1024];
    BYTE C_2[1024];  //加密后的消息
    BYTE C_3[1024];
} message_st;

extern const unsigned char PBOC_SIGN_ID[];
extern int secp_get_rng(unsigned char *pbuf, int rnglen);
extern void sm2_sign_e(ec_param *ecp, BYTE *e, sm2_sign_st *sign);
extern int  sm2_verify_e(ec_param *ecp, BYTE *e, sm2_sign_st *sign);

#endif


 
