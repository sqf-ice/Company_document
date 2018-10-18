#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "gm_drv.h"
#include "NDK.h"

#include "sm2.h"
#include "sm3.h"
#include "sm4.h"
#include "util.h"

sm3_context Ctx;

////////////////////////////// SM3LIB FUNCTION /////////////////////////////////////
/*
 * 函数原型 SM3Start( void )
 * 功能说明 启动CCP运算
 * 参数说明
 *       无
 * 输出数据
 *       无
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM3Start(void)
{
    sm3_starts( &Ctx );
    return NDK_OK;
}
/*
 * 函数原型 SM3Update( unsigned char *pDat )
 * 功能说明 update一个分组数据，数据为64字节整数倍
 * 参数说明
 *       [IN]    pDat 一个分组数据，64字节整数倍
 * 输出数据
 *       无
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR_PARA 参数非法
 *             NDK_ERR 操作失败
 */
int SM3Update( unsigned char *pDat,  unsigned int len )
{
    int cnt = 0;
    unsigned char hash_result[32] = {0};
    GM_ALG tmpdata;

    if( pDat == NULL || len == 0 || (len & 63) )
        return NDK_ERR_PARA;

    sm3_update( &Ctx, pDat, len);
    return NDK_OK;
}

/*
 * 函数原型 SM3Final( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * 功能说明 计算最后一组数据，输出摘要
 * 参数说明
 *       [IN]    pDat    最后一个分组数据
 *       [IN]    len     最后一组数据长度
 * 输出数据
 *       [OUT]   pHashDat 输出摘要数据，32字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM3Final( unsigned char *pDat, unsigned int len, unsigned char *pHashDat )
{
    int iret = 0;
    int cnt = 0;
    GM_ALG tmpdata;

    if(len <0 || pDat == NULL || pHashDat == NULL)
        return NDK_ERR_PARA;

    PDEBUG_DATA("final sm3 buffer:", pDat, len);

    sm3_update(&Ctx, pDat, len);
    sm3_finish( &Ctx, pHashDat);
    memset( &Ctx, 0, sizeof( sm3_context ) );
    return NDK_OK;
}

/*
 * 函数原型 SM3Compute( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * 功能说明 计算输入数据的摘要，函数内部完成填充，输出摘要
 * 参数说明
 *       [IN]    pDat    输入数据
 *       [IN]    len     输入数据长度
 * 输出数据
 *       [OUT]   pHashDat 输出摘要数据，32字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法（输入为空）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM3Compute( unsigned char *pDat, unsigned int len, unsigned char *pHashDat )
{
    if(len <= 0 || pDat == NULL || pHashDat == NULL)
        return NDK_ERR_PARA;

    PDEBUG_DATA("sm3 buffer:", pDat, len);
    sm3(pDat, len, pHashDat);
    return NDK_OK;
}

/**
*@fn        AscToHex(const unsigned char*, int, char , int, unsigned char *)
*@brief     DES3计算,根据密钥类型区分计算为加密还是解密
*@param     pszAsciiBuf ASCII输入缓冲
*@param     nLen ASCII长度
*@param     cType 转换类型
*@param     pszBcdBuf 输出BCD码缓冲
*@return    @li 0               成功
            @li -1              错误
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
static int AscToHex(const unsigned char* pszAsciiBuf, int nLen, char cType, unsigned char* pszBcdBuf)
{
    int i = 0;
    char cTmp, cTmp1;

    if (pszAsciiBuf == NULL) {
        return -1;
    }

    if ((nLen&0x01) && cType) { /*判别是否为奇数以及往那边对齐*/
        cTmp1 = 0 ;
    } else {
        cTmp1 = 0x55 ;
    }

    for (i = 0; i < nLen; pszAsciiBuf ++, i ++) {
        if ( *pszAsciiBuf >= 'a' ) {
            cTmp = *pszAsciiBuf - 'a' + 10 ;
        } else if ( *pszAsciiBuf >= 'A' ) {
            cTmp = *pszAsciiBuf - 'A' + 10 ;
        } else if ( *pszAsciiBuf >= '0' ) {
            cTmp = *pszAsciiBuf - '0' ;
        } else {
            cTmp = *pszAsciiBuf;
            cTmp&=0x0f;
        }

        if ( cTmp1 == 0x55 ) {
            cTmp1 = cTmp;
        } else {
            *pszBcdBuf ++ = cTmp1 << 4 | cTmp;
            cTmp1 = 0x55;
        }
    }
    if (cTmp1 != 0x55) {
        *pszBcdBuf = cTmp1 << 4;
    }

    return 0;
}

/**
*@fn        HexToAsc(const unsigned char*, int, char , int, unsigned char *)
*@brief     DES3计算,根据密钥类型区分计算为加密还是解密
*@param     pszBcdBuf 输入BCD码缓冲
*@param     nLen ASCII长度
*@param     cType 转换类型
*@param     pszAsciiBuf ASCII输出缓冲
*@return    @li 0               成功
            @li -1              错误
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
static int HexToAsc(const unsigned char* pszBcdBuf, int nLen, char cType, unsigned char* pszAsciiBuf)
{
    int i = 0;

    if (pszBcdBuf == NULL) {
        return -1;
    }
    if (nLen & 0x01 && cType) { /*判别是否为奇数以及往那边对齐*/
        /*0左，1右*/
        i = 1;
        nLen ++;
    } else {
        i = 0;
    }
    for (; i < nLen; i ++, pszAsciiBuf ++) {
        if (i & 0x01) {
            *pszAsciiBuf = *pszBcdBuf ++ & 0x0f;
        } else {
            *pszAsciiBuf = *pszBcdBuf >> 4;
        }
        if (*pszAsciiBuf > 9) {
            *pszAsciiBuf += 'A' - 10;
        } else {
            *pszAsciiBuf += '0';
        }

    }
    *pszAsciiBuf = 0;
    return 0;
}


////////////////////////////// SM2LIB FUNCTION /////////////////////////////////////
int IsSM2SKeyCorrect(ec_param *ecp, unsigned char *erikey)
{
    unsigned char n[32];

    AscToHex(sm2_param_recommand[5], strlen(sm2_param_recommand[5]), 0, n);
    n[31] = n[31] -2;
    PDEBUG_DATA("n-2=", n, sizeof(n));
    if(memcmp(erikey, n, sizeof(n)) > 0) {
        PDEBUG("prikey error!");
        return 0;
    }

    memset(n, 0, sizeof(n));
    if(memcmp(erikey, n, sizeof(n)) == 0) {
        PDEBUG("prikey error!");
        return 0;
    }

    return 1;
}

int IsPointOnCurve(ec_param *ecp, unsigned char *X, unsigned char *Y)
{
    BIGNUM *P_x;
    BIGNUM *P_y;
    xy_ecpoint *P;

    P_x = BN_new();
    P_y = BN_new();
    P = xy_ecpoint_new(ecp);

    BN_bin2bn(X, ecp->point_byte_length, P_x);
    BN_bin2bn(Y, ecp->point_byte_length, P_y);
    xy_ecpoint_init_xy(P, P_x, P_y, ecp);

    //验证是否在曲线上
    if (!EC_POINT_is_on_curve(ecp->group, P->ec_point, ecp->ctx)) {
        PDEBUG("EC_POINT_is_on_curve: 0\n");
        return 0;
    } else {
        PDEBUG("EC_POINT_is_on_curve: 1\n");
        return 1;
    }
}

/*
 * 函数原型 SM2KeyPairGen( unsigned char *eubkey, unsigned char *erikey )
 * 功能说明 生成SM2密钥对
 * 参数说明
 * 输出数据
 *     [OUT]  *eubKey         公钥 64字节
 *     [OUT]  *erikey         私钥 32字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM2KeyPairGen( unsigned char *eubkey, unsigned char *erikey )
{
    int iret;
    ec_param *ecp;
    char sTmp[64+1] = {0};
    sm2_ec_key *eck;
    unsigned    char tmp_erikey[32] = {0};
    unsigned  char tmp_eubkey[64] = {0};

    ecp = ec_param_new();
    ec_param_init(ecp, sm2_param_recommand, TYPE_GFp, 256);

    PDEBUG("here");
    eck = sm2_ec_key_new(ecp);
    PDEBUG("here");

again:
    // d = [1, n-2];
    while(1) {
        secp_get_rng(tmp_erikey, 32);
        if(IsSM2SKeyCorrect(ecp, tmp_erikey))
            break;
    }

    PDEBUG_DATA("eripkey", tmp_erikey, 32);
    PDEBUG("here");
    HexToAsc(tmp_erikey, 64, 0, sTmp);
    PDEBUG("eripkey: %s", sTmp);
    sm2_ec_key_init(eck, sTmp, ecp);
//  sm2_ec_calc_pkey(tmp_erikey, 32, tmp_eubkey);
    BN_bn2bin(eck->P->x, tmp_eubkey);
    BN_bn2bin(eck->P->y, tmp_eubkey+32);
    if(!IsPointOnCurve(ecp, tmp_eubkey, tmp_eubkey + 32))
        goto again;

    if(eubkey != NULL) {
        PDEBUG_DATA("ecubkey", tmp_eubkey, 64);
        memcpy(eubkey, tmp_eubkey, 64);
    }

    if(erikey != NULL)
        memcpy(erikey, tmp_erikey, 32);

    sm2_ec_key_free(eck);
    ec_param_free(ecp);

    return NDK_OK;
}

/*
 * 函数原型 SM2Encrypt( unsigned char *eubkey, unsigned char *Message, unsigned short MessageLen, unsigned short *Crypto, unsigned short *CryptoLen )
 * 功能说明 Crypto=Enc(msg,key)
 * 参数说明
 *     [IN]   *eubkey         加密公钥
 *     [IN]   *Message           明文数据
 *     [IN]   MessageLen         数据长度
 *     [OUT]  *Crypto            密文数据(按照C1C3C2的顺序进行排列)
 *     [OUT]  *CryptoLen         密文数据长度(密文数据长度比明文数据长96个字节)
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法 (输入为空、明文长度 >（1024 - 96）字节)
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM2Encrypt( unsigned char *eubkey, unsigned char *Message, unsigned short MessageLen, unsigned char *Crypto, unsigned short *CryptoLen )
{
    int iret = 0;
    ec_param *ecp;
    message_st message_data;

    if(MessageLen > (1024 - 96) || MessageLen == 0 || eubkey == NULL || Message == NULL || Crypto == NULL || CryptoLen == NULL)
        return NDK_ERR_PARA;

    //param init
    memset(&message_data, 0, sizeof(message_st));
    ecp = ec_param_new();
    ec_param_init(ecp, sm2_param_recommand, TYPE_GFp, 256);

    if(!IsPointOnCurve(ecp, eubkey, eubkey+32))
        return NDK_ERR;

    //enc init
    memcpy(message_data.public_key.x, eubkey, 32);
    memcpy(message_data.public_key.y, eubkey+32, 32);
    message_data.message = Message;
    message_data.message_byte_length = MessageLen;
    message_data.klen_bit = message_data.message_byte_length * 8;
    while(1) {
        secp_get_rng(message_data.k, 32);
        if((message_data.k[31] < 0xFF) || (message_data.k[30] < 0xFF)) {
            PDEBUG_DATA("message_data.k", message_data.k, 32);
            break;
        }
    }

    sm2_encrypt(ecp, &message_data);

    memcpy(Crypto, message_data.C, MessageLen + 96);
    *CryptoLen = MessageLen + 96;

    ec_param_free(ecp);

    return iret;
}

/*
 * 简介：SM2解密函数，目前版本应对的密文应按C1C3C2排列
 * 函数原型 SM2Decrypt( unsigned char *erikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
 * 功能说明 密钥协商
 * 参数说明
 *     [IN]   *erikey         解密私钥
 *     [IN]   *Crypto            密文数据
 *     [IN]   CryptoLen          密文数据长度
 *     [OUT]  *Message           明文数据
 *     [OUT]  *MessageLen        数据长度
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法(输入为空、密文长度 > 1024字?
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM2Decrypt( unsigned char *erikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
{
    int iret = 0;
    ec_param *ecp;
    message_st message_data;

    if(CryptoLen > 1024 || CryptoLen == 0 || erikey == NULL || Message == NULL || Crypto == NULL || MessageLen == NULL)
        return NDK_ERR_PARA;

    //param init
    memset(&message_data, 0, sizeof(message_st));
    ecp = ec_param_new();
    ec_param_init(ecp, sm2_param_recommand, TYPE_GFp, 256);

    if(!IsSM2SKeyCorrect(ecp, erikey))
        return NDK_ERR;

    message_data.message_byte_length = CryptoLen - 96;
    message_data.klen_bit = message_data.message_byte_length * 8;

    memcpy(message_data.private_key, erikey, 32);
    memcpy(message_data.C, Crypto, CryptoLen);
    message_data.decrypt = (BYTE *)OPENSSL_malloc(message_data.message_byte_length + 1);
    memset(message_data.decrypt, 0, message_data.message_byte_length+1);

    iret = sm2_decrypt(ecp, &message_data);
    PDEBUG_DATA("dec:",  message_data.decrypt, CryptoLen - 96);

    memcpy(Message, message_data.decrypt, CryptoLen - 96);
    *MessageLen = CryptoLen - 96;

    OPENSSL_free(message_data.decrypt);
    ec_param_free(ecp);
    return iret;
}


/*
 * 简介：SM2签名，无摘要生成功能，需要直接输入计算完毕的e
 * 函数原型 SM2Sign(unsigned char *erikey, unsigned char *e, unsigned char *output )
 * 功能说明 (r,s)=sign(e,key)
 * 参数说明
 *     [IN]   *erikey         签名私钥
 *     [IN]   *e                 被签名数据的摘要值（32字节）
 *     [OUT]  *output            签名后数据（64字节）
 * 返值说明
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法 （输入为空）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM2Sign(unsigned char *erikey, unsigned char *e, unsigned char *output )
{
    int iret = 0;
    ec_param *ecp;
    /*sm2_ec_key *key_A;*/
    sm2_sign_st sign;

    if(erikey == NULL || e == NULL || output == NULL)
        return NDK_ERR_PARA;

    PDEBUG("here");
    //param init
    memset(&sign, 0, sizeof(sm2_sign_st));
    ecp = ec_param_new();
    ec_param_init(ecp, sm2_param_recommand, TYPE_GFp, 256);

    if(!IsSM2SKeyCorrect(ecp, erikey))
        return NDK_ERR;

    //key init
    memcpy(sign.private_key, erikey, 32);
    while(1) {
        secp_get_rng(sign.k, 32);
        if((sign.k[31] < 0xFF) || (sign.k[30] < 0xFF))
            break;
    }

    PDEBUG("here");
    //output
    sm2_sign_e(ecp, e, &sign);
    memcpy(output, sign.r, 32);
    memcpy(output + 32, sign.s, 32);

    /*sm2_ec_key_free(key_A);*/
    ec_param_free(ecp);

    return NDK_OK;
}

/*
 * 简介：SM2验签函数
 * 函数原型 unsigned char SM2Verify( unsigned char *pPublicKey, unsigned char *e, unsigned char *pSignedData )
 * 功能说明 if(signarture is ture)
 * 参数说明
 *     [IN]   *pPublicKey        验证公钥
 *     [IN]   *e                 被签名数据的摘要值（32字节）
 *     [IN]   *pSignedData       签名后数据（64字节）
 * 返值说明 -  NDK_OK 验签成功
 *             NDK_ERR_PARA 参数非法(输入参数为空)
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 验签失败
 */
int SM2Verify( unsigned char *eubkey, unsigned char *e, unsigned char *pSignedData )
{
    int iret = 0;
    ec_param *ecp;
    sm2_sign_st sign;


    if(eubkey == NULL || e == NULL || pSignedData== NULL)
        return NDK_ERR_PARA;

    //param init
    memset(&sign, 0, sizeof(sm2_sign_st));
    ecp = ec_param_new();
    ec_param_init(ecp, sm2_param_recommand, TYPE_GFp, 256);

    if(!IsPointOnCurve(ecp, eubkey, eubkey+32)) {
        PDEBUG_DATA("pubkey", eubkey, 64);
        PDEBUG("error here!!");
        return NDK_ERR;
    }

    //very init
    memcpy(sign.public_key.x, eubkey, 32);
    memcpy(sign.public_key.y, eubkey+32, 32);
    memcpy(sign.r, pSignedData, 32);
    memcpy(sign.s, pSignedData + 32, 32);

    //very e
    iret = sm2_verify_e(ecp, e, &sign);

    ec_param_free(ecp);

    if(iret != 0)
        return NDK_ERR;

    return NDK_OK;
}

/*
 * 简介：用于签名摘要生成
 * 函数原型 int SM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData);
 * 功能说明 根据输入ID,Message和公钥，计算出用于签名的摘要数据
 * 参数说明
 *     [IN]  usID    ID长度
 *     [IN]  pID     ID数据输入(*当传入为NULL时,使用PBOC3.0默认ID做运算)
 *     [IN]  usM     Message长度
 *     [IN]  pM      Message数据输入
 *     [IN]  pubKey  公钥数据输入
 *     [OUT] pHashData: 根据输入计算出用于签名的32字节摘要
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法（除pID外参数输入为空）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData)
{
    int iret = 0;
    int cnt = 0;
    unsigned char Z_A[32] = {0};
    unsigned char hashbuf[1024] = {0};
    int offset = 0;
    sm3_context ctx;

    if(usID > 128 || usM == 0 || pM == NULL || pubKey == NULL || pHashData == NULL)
        return NDK_ERR_PARA;

    if(pID == NULL) {
        pID = PBOC_SIGN_ID;
        usID = strlen(PBOC_SIGN_ID);
    } else if(usID == 0)
        return NDK_ERR_PARA;

    hashbuf[0] = ((usID*8) >> 8)& 0xFF;
    hashbuf[1] = (usID*8) & 0xFF;
    offset += 2;
    memcpy(hashbuf + offset, pID, usID);
    offset += usID;

    for(cnt = 0; cnt < 4; cnt++) {
        PDEBUG("sm2_param[%d] = %s", cnt+1, sm2_param_recommand[cnt+1]);
        AscToHex(sm2_param_recommand[cnt+1], strlen(sm2_param_recommand[cnt+1]), 0, hashbuf+offset);
        offset += strlen(sm2_param_recommand[cnt+1])/2;
    }

    memcpy(hashbuf + offset, pubKey, 64);
    offset += 64;
    PDEBUG_DATA("hashbuf", hashbuf, offset);
    sm3(hashbuf, offset, Z_A);
    PDEBUG_DATA("Z_A:", Z_A, 32);

    sm3_starts(&ctx);
    sm3_update(&ctx, Z_A, 32);
    sm3_update(&ctx, pM, usM);
    PDEBUG_DATA("hash data", pM, usM);
    sm3_finish(&ctx, pHashData);

    return NDK_OK;
}

////////////////////////////// SM4LIB FUNCTION /////////////////////////////////////
/*
 * 函数原型 int SM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode);
 * 功能说明 根据输入的密钥和模式对输入的数据（16字节）进行SM4运算
 * 参数说明
 *       [IN]    pKey    输入密钥，长度为16字节
 *       [IN]    pIVector    初始向量，长度为16字节（ECB模式允许为空）
 *       [IN]    len 输入数据长度
 *       [IN]    pSm4Input   输入数据
 *       [IN]    mode    运算模式(参考”EM_ALG_SM4“)
 * 输出数据
 *       [OUT]   pSm4Output 输出数据，16字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法（输入为空，数据长度不是16的整数倍，模式非法）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int SM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode)
{
    sm4_context ctx;

    if(pKey == NULL || pSm4Input == NULL || pSm4Output == NULL || len == 0 || mode >= ALG_SM4_MAX) {
        PDEBUG("here");
        return NDK_ERR_PARA;
    }

    if(len & 15) {
        PDEBUG("here");
        return NDK_ERR_PARA;
    }

    switch(mode) {
        case ALG_SM4_ENCRYPT_ECB:
            sm4_setkey_enc(&ctx,pKey);
            sm4_crypt_ecb(&ctx,1,len,pSm4Input,pSm4Output);
            break;
        case ALG_SM4_DECRYPT_ECB:
            sm4_setkey_dec(&ctx,pKey);
            sm4_crypt_ecb(&ctx,0,len,pSm4Input,pSm4Output);
            break;
            //CBC 数据前16字节为IV值
        case ALG_SM4_ENCRYPT_CBC:
            if(pIVector == NULL)
                return NDK_ERR_PARA;
            sm4_setkey_enc(&ctx,pKey);
            sm4_crypt_cbc(&ctx,1,len,pIVector,pSm4Input,pSm4Output);
            break;
        case ALG_SM4_DECRYPT_CBC:
            if(pIVector == NULL)
                return NDK_ERR_PARA;
            sm4_setkey_dec(&ctx,pKey);
            sm4_crypt_cbc(&ctx,0,len,pIVector,pSm4Input,pSm4Output);
            break;
        default:
            return NDK_ERR_PARA;
    }
    return NDK_OK;
}


