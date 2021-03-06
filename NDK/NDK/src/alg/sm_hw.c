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

////////////////////////////// SM3LIB FUNCTION /////////////////////////////////////
/*
 * 函数原型 ccpSM3Start( void )
 * 功能说明 启动CCP运算
 * 参数说明
 *       无
 * 输出数据
 *       无
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int ccpSM3Start(void)
{
    GM_HASH_START tmpdata;

    tmpdata.AlgID = SM3_HASH;
    tmpdata.Mode= LOOP_MODE_30M;

    if(gm_hash_start(&tmpdata) != 0)
        return NDK_ERR;
    else
        return NDK_OK;
}
/*
 * 函数原型 ccpSM3Update( unsigned char *pDat )
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
int ccpSM3Update( unsigned char *pDat,  unsigned int len )
{
    int iret = 0;
    int cnt = 0;
    unsigned char hash_result[32] = {0};
    GM_ALG tmpdata;

    if( pDat == NULL || len <= 0 || (len & 63) )
        return NDK_ERR_PARA;

    tmpdata.AlgID = SM3_HASH;
    tmpdata.oData = hash_result;

    while(len >= 64) {
        tmpdata.iLen = 64;
        tmpdata.iData = pDat + 64*cnt;
        iret = gm_hash_updata(&tmpdata);
        if(iret != 0) {
            PDEBUG("error!");
            return NDK_ERR;
        }
        len -= 64;
        cnt++;
    }

    return NDK_OK;
}

/*
 * 函数原型 ccpSM3Final( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
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
int ccpSM3Final( unsigned char *pDat, unsigned int len, unsigned char *pHashDat )
{
    int iret = 0;
    int cnt = 0;
    GM_ALG tmpdata;

    if(pDat == NULL || pHashDat == NULL)
        return NDK_ERR_PARA;

    PDEBUG_DATA("final sm3 buffer:", pDat, len);

    tmpdata.AlgID = SM3_HASH;
    tmpdata.oData = pHashDat;

    while(len >= 64) {
        tmpdata.iLen = 64;
        tmpdata.iData = pDat + 64*cnt;
        iret = gm_hash_updata(&tmpdata);
        if(iret != 0) {
            PDEBUG("error!");
            return NDK_ERR;
        }
        len -= 64;
        cnt++;
    }

    tmpdata.iLen = len;
    tmpdata.iData = pDat + 64*cnt;
    iret = gm_hash_final(&tmpdata);
    if(iret != 0) {
        PDEBUG("error!");
        return NDK_ERR;
    }

    return NDK_OK;
}

/*
 * 函数原型 ccpSM3Compute( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
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
int ccpSM3Compute( unsigned char *pDat, unsigned int len, unsigned char *pHashDat )
{
    int iret = 0;
    int cnt = 0;
    GM_HASH_START startdata;
    GM_ALG tmpdata;

    if(len <= 0 || pDat == NULL || pHashDat == NULL)
        return NDK_ERR_PARA;

    PDEBUG_DATA("sm3 buffer:", pDat, len);
    //init
    startdata.AlgID = SM3_HASH;
    startdata.Mode= LOOP_MODE_30M;

    if(gm_hash_start(&startdata) != 0)
        return NDK_ERR;

    tmpdata.AlgID = SM3_HASH;
    tmpdata.oData = pHashDat;

    //update
    while(len >= 64) {
        tmpdata.iLen = 64;
        tmpdata.iData = pDat + 64*cnt;
        iret = gm_hash_updata(&tmpdata);
        if(iret != 0) {
            PDEBUG("error!");
            return NDK_ERR;
        }
        len -= 64;
        cnt++;
    }

    //final
    tmpdata.iLen = len;
    tmpdata.iData = pDat + 64*cnt;
    iret = gm_hash_final(&tmpdata);
    if(iret != 0) {
        PDEBUG("error!");
        return NDK_ERR;
    }

    return NDK_OK;
}


////////////////////////////// SM2LIB FUNCTION /////////////////////////////////////
//导入SM2 公钥
int ccpSM2InputPKey(unsigned char *buf)
{
    GM_KEY tmpdata;
    stSM2Parameter stSM2_Para;

    memcpy(stSM2_Para.PubKey, buf, 64);
    tmpdata.KeyID = 0;
    tmpdata.KeyType = SM2_PUB;
    tmpdata.KeyBuf = &stSM2_Para;

    return gm_cipher_key_input(&tmpdata);
}

//导入SM2 私钥
int ccpSM2InputSKey(unsigned char *buf)
{
    GM_KEY tmpdata;
    stSM2Parameter stSM2_Para;

    memcpy(stSM2_Para.PriKey, buf, 32);
    tmpdata.KeyID = 0;
    tmpdata.KeyType = SM2_PRI;
    tmpdata.KeyBuf = &stSM2_Para;

    return gm_cipher_key_input(&tmpdata);
}

/*
 * 函数原型 ccpSM2KeyPairGen( unsigned char *eccpubkey, unsigned char *eccprikey )
 * 功能说明 生成SM2密钥对
 * 参数说明
 * 输出数据
 *     [OUT]  *eccpubKey         公钥 64字节
 *     [OUT]  *eccprikey         私钥 32字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int ccpSM2KeyPairGen( unsigned char *eccpubkey, unsigned char *eccprikey )
{
    GM_KEY tmpdata;
    stSM2Parameter stSM2_Para;
    int iret;

    tmpdata.KeyID = 0;
    tmpdata.KeyType= SM2_PUB;
    tmpdata.KeyBuf= &stSM2_Para;

    iret = gm_cipher_gen_key(&tmpdata);
    if(iret != 0)
        return NDK_ERR;

    if(eccpubkey != NULL)
        memcpy(eccpubkey,  stSM2_Para.PubKey, 64);
    if(eccprikey != NULL)
        memcpy(eccprikey,  stSM2_Para.PriKey, 32);

    return NDK_OK;
}

/*
 * 函数原型 ccpSM2Encrypt( unsigned char *eccpubkey, unsigned char *Message, unsigned short MessageLen, unsigned short *Crypto, unsigned short *CryptoLen )
 * 功能说明 Crypto=Enc(msg,key)
 * 参数说明
 *     [IN]   *eccpubkey         加密公钥
 *     [IN]   *Message           明文数据
 *     [IN]   MessageLen         数据长度
 *     [OUT]  *Crypto            密文数据(按照C1C3C2的顺序进行排列)
 *     [OUT]  *CryptoLen         密文数据长度(密文数据长度比明文数据长96个字节)
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法 (输入为空、明文长度 >（1024 - 96）字节)
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int ccpSM2Encrypt( unsigned char *eccpubkey, unsigned char *Message, unsigned short MessageLen, unsigned char *Crypto, unsigned short *CryptoLen )
{
    GM_ALG tmpdata;
    int iret = 0;

    if(MessageLen > (1024 - 96) || MessageLen == 0 || eccpubkey == NULL || Message == NULL || Crypto == NULL || CryptoLen == NULL)
        return NDK_ERR_PARA;

    iret = ccpSM2InputPKey(eccpubkey);
    if(iret != 0)
        return NDK_ERR;

    tmpdata.AlgID = SM2_ENC;
    tmpdata.iLen = MessageLen;
    tmpdata.iData = Message;
    tmpdata.oLen= CryptoLen;
    tmpdata.oData= Crypto;
    if(MessageLen < 159) {
        PDEBUG("-----sm2 calc");
        iret = gm_cipher_oper(&tmpdata);
    } else {
        PDEBUG("-----sm2 large calc");
        iret = gm_cipher_sm2_calc(&tmpdata);
    }
    if(iret != 0) {
        PDEBUG("error  iret = %d", iret);
        return NDK_ERR;
    }

    /*PDEBUG_DATA("C1:", Crypto, 64);*/
    /*PDEBUG_DATA("C3:", Crypto + 64, 32);*/
    /*PDEBUG_DATA("C2:", Crypto + 64 + 32, MessageLen);*/
    *CryptoLen = MessageLen + 96;

    return iret;
}

/*
 * 简介：SM2解密函数，目前版本应对的密文应按C1C3C2排列
 * 函数原型 ccpSM2Decrypt( unsigned char *eccprikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
 * 功能说明 密钥协商
 * 参数说明
 *     [IN]   *eccprikey         解密私钥
 *     [IN]   *Crypto            密文数据
 *     [IN]   CryptoLen          密文数据长度
 *     [OUT]  *Message           明文数据
 *     [OUT]  *MessageLen        数据长度
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法(输入为空、密文长度 > 1024字�)
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int ccpSM2Decrypt( unsigned char *eccprikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
{
    GM_ALG tmpdata;
    int iret = 0;

    if(CryptoLen > 1024 || CryptoLen == 0 || eccprikey == NULL || Message == NULL || Crypto == NULL || MessageLen == NULL)
        return NDK_ERR_PARA;

    iret = ccpSM2InputSKey(eccprikey);
    if(iret != 0) {
        PDEBUG("here iret = %d", iret);
        return NDK_ERR;
    }

    PDEBUG_DATA("Crypto", Crypto, CryptoLen);
    tmpdata.AlgID = SM2_DEC;
    tmpdata.iLen = CryptoLen;
    tmpdata.iData= Crypto;
    tmpdata.oLen = MessageLen;
    tmpdata.oData= Message;

    if(CryptoLen < 159) {
        PDEBUG("-----sm2 calc");
        iret = gm_cipher_oper(&tmpdata);
    } else {
        PDEBUG("-----sm2 large calc");
        iret = gm_cipher_sm2_calc(&tmpdata);
    }
    if(iret != 0) {
        PDEBUG("here iret = %d", iret);
        return NDK_ERR;
    }
    *MessageLen = CryptoLen - 96;

    return iret;
}


/*
 * 简介：SM2签名，无摘要生成功能，需要直接输入计算完毕的e
 * 函数原型 ccpSM2Sign(unsigned char *eccprikey, unsigned char *e, unsigned char *output )
 * 功能说明 (r,s)=sign(e,key)
 * 参数说明
 *     [IN]   *eccprikey         签名私钥
 *     [IN]   *e                 被签名数据的摘要值（32字节）
 *     [OUT]  *output            签名后数据（64字节）
 * 返值说明
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法 （输入为空）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int ccpSM2Sign(unsigned char *eccprikey, unsigned char *e, unsigned char *output )
{
    GM_ALG tmpdata;
    int iret = 0;

    if(eccprikey == NULL || e == NULL || output == NULL)
        return NDK_ERR_PARA;

    iret = ccpSM2InputSKey(eccprikey);
    if(iret != 0)
        return NDK_ERR;

    tmpdata.AlgID = SM2_SIGN_E;
    tmpdata.iLen = 32;
    tmpdata.iData = e;  //对于旧驱动第一个字节标示模式，1-带 id的签名； 0 -不带id
    tmpdata.oData = output;

    iret = gm_cipher_oper(&tmpdata);
    if(iret != 0) {
        PDEBUG("ERROR iret = %d", iret);
        return NDK_ERR;
    }

    return iret;
}

/*
 * 简介：SM2验签函数
 * 函数原型 unsigned char ccpSM2Verify( unsigned char *pPublicKey, unsigned char *e, unsigned char *pSignedData )
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
int ccpSM2Verify( unsigned char *eccpubkey, unsigned char *e, unsigned char *pSignedData )
{
    GM_ALG tmpdata;
    unsigned char checkdata[32+64] = {0};
    int iret = 0;

    if(eccpubkey == NULL || e == NULL || pSignedData== NULL)
        return NDK_ERR_PARA;

    memcpy(checkdata, e, 32);
    memcpy(checkdata+32, pSignedData, 64);

    iret = ccpSM2InputPKey(eccpubkey);
    if(iret != 0)
        return NDK_ERR;

    tmpdata.AlgID = SM2_VERY_E;
    tmpdata.iLen = 32 + 64;
    tmpdata.iData = checkdata;

    iret = gm_cipher_oper(&tmpdata);
    if(iret != 0) {
        PDEBUG("ERROR iret = %d", iret);
        return NDK_ERR;
    }

    return iret;
}

/*
 * 简介：用于签名摘要生成
 * 函数原型 int ccpSM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData);
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
const unsigned char PBOC_SIGN_ID[] = "1234567812345678";
int ccpSM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData)
{
    GM_ALG tmpdata;
    int iret = 0;
    int cnt = 0;
    //    unsigned char Z_A[32] = {0};
    unsigned char hashbuf[1024] = {0};
    unsigned char *Z_A = hashbuf;

    if(usID > 128 || usM == 0 || pM == NULL || pubKey == NULL || pHashData == NULL)
        return NDK_ERR_PARA;

    if(pID == NULL) {
        pID = PBOC_SIGN_ID;
        usID = strlen (PBOC_SIGN_ID);
    } else if(usID == 0)
        return NDK_ERR_PARA;

    //input pubkey
    iret = ccpSM2InputPKey(pubKey);
    if(iret != 0)
        return NDK_ERR;

    PDEBUG("GEN Z");
    tmpdata.AlgID = SM2_GEN_Z;
    tmpdata.iLen = usID;
    tmpdata.iData = pID;
    tmpdata.oData = Z_A;
    PDEBUG_DATA("ID:", pID, usID);
    iret = gm_cipher_oper(&tmpdata);
    if(iret != 0) {
        PDEBUG("ERR iret= %d", iret);
        return NDK_ERR;
    }
    PDEBUG_DATA("Z_A:", Z_A, 32);

    // calc hash
    if(usM < (1024 - 32)) {
        memcpy(hashbuf+32, pM, usM);
        iret = ccpSM3Compute(hashbuf, usM + 32, pHashData);
        if(iret != 0)
            return NDK_ERR;
        PDEBUG_DATA("pHashData:", pHashData, 32);
        return NDK_OK;
    } else {
        memcpy(hashbuf+32, pM, 992);
        usM -= 992;
        ccpSM3Start();
        PDEBUG_DATA("update data", hashbuf, 1024);
        ccpSM3Update(hashbuf, 1024);
        if(usM < 1024) {
            memcpy(hashbuf, pM + 992, usM);
        } else {
            memcpy(hashbuf, pM + 992, 1024);
        }
        while(usM > 1024) {
            PDEBUG_DATA("update data", hashbuf, 1024);
            ccpSM3Update(hashbuf, 1024);
            usM -= 1024;
            cnt ++;
            if(usM < 1024) {
                memcpy(hashbuf, pM + 992+cnt*1024, usM);
            } else {
                memcpy(hashbuf, pM + 992+cnt*1024, 1024);
            }
        }
        PDEBUG_DATA("final data", hashbuf, usM);
        ccpSM3Final(hashbuf, usM, pHashData);
    }
    return NDK_OK;
}
////////////////////////////// SM4LIB FUNCTION /////////////////////////////////////
/*
 * 函数原型 int ccpSM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode);
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
int ccpSM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode)
{
    unsigned char tmpbuf[16+224] = {0}; //16byte IV  + data
    int cnt = 0;
    int iret = 0;
    GM_ALG tmpdata;
    GM_KEY keydata;

    if(pKey == NULL || pSm4Input == NULL || pSm4Output == NULL || len == 0 || mode >= ALG_SM4_MAX) {
        PDEBUG("here");
        return NDK_ERR_PARA;
    }

    if(len & 15) {
        PDEBUG("here");
        return NDK_ERR_PARA;
    }

    //set key
    keydata.KeyID = 0;
    keydata.KeyType= SM4_16BYTE_PLAINTEXT;
    keydata.KeyBuf = pKey;
    iret = gm_cipher_set_tmpkey(&keydata);
    if(iret != NDK_OK) {
        PDEBUG("ERROR iret = %d", iret);
        return NDK_ERR;
    }

    switch(mode) {
        case ALG_SM4_ENCRYPT_ECB:
            tmpdata.AlgID = SM4_ENC_ECB;
            tmpdata.iData = tmpbuf+16;
            tmpdata.oData = tmpbuf+16;
            break;
        case ALG_SM4_DECRYPT_ECB:
            tmpdata.AlgID = SM4_DEC_ECB;
            tmpdata.iData = tmpbuf+16;
            tmpdata.oData = tmpbuf+16;
            break;
            //CBC 数据前16字节为IV值
        case ALG_SM4_ENCRYPT_CBC:
            if(pIVector == NULL)
                return NDK_ERR_PARA;
            tmpdata.AlgID = SM4_ENC_CBC;
            tmpdata.iData = tmpbuf;
            tmpdata.oData = tmpbuf;
            memcpy(tmpbuf, pIVector, 16);
            break;
        case ALG_SM4_DECRYPT_CBC:
            if(pIVector == NULL)
                return NDK_ERR_PARA;
            tmpdata.AlgID = SM4_DEC_CBC;
            tmpdata.iData = tmpbuf;
            tmpdata.oData = tmpbuf;
            memcpy(tmpbuf, pIVector, 16);
            break;
        default:
            PDEBUG("here");
            return NDK_ERR_PARA;
    }

    while(len > 224) {
        memcpy(tmpbuf+16, pSm4Input+cnt*224, 224);
        tmpdata.iLen = 224;
        if((mode == ALG_SM4_ENCRYPT_CBC) || (mode == ALG_SM4_DECRYPT_CBC))
            tmpdata.iLen += 16;
        /*PDEBUG_DATA("==in data:", tmpdata.iData, tmpdata.iLen);*/
        iret = gm_cipher_oper(&tmpdata);
        if(iret != 0) {
            PDEBUG("ERROR iret = %d", iret);
            return NDK_ERR;
        }
        /*PDEBUG_DATA("==out data:", tmpdata.oData, tmpdata.iLen);*/
        memcpy(pSm4Output+cnt*224, tmpbuf+16, 224);
        len -=  224;
        cnt++;
        //set IV
        if(mode == ALG_SM4_ENCRYPT_CBC)
            memcpy(tmpbuf, tmpbuf+224, 16);
        else if(mode == ALG_SM4_DECRYPT_CBC)
            memcpy(tmpbuf, pSm4Input+cnt*224-16, 16);
    }

    if(len > 0 ) {
        tmpdata.iLen = len;
        if((mode == ALG_SM4_ENCRYPT_CBC) || (mode == ALG_SM4_DECRYPT_CBC))
            tmpdata.iLen += 16;
        memcpy(tmpbuf+16, pSm4Input+cnt*224, len);
        /*PDEBUG_DATA("==in data:",  tmpdata.iData, tmpdata.iLen);*/
        iret = gm_cipher_oper(&tmpdata);
        if(iret != 0) {
            PDEBUG("ERROR iret = %d", iret);
            return NDK_ERR;
        }
        /*PDEBUG_DATA("==out data:",  tmpdata.oData, tmpdata.iLen);*/
    }

    memcpy(pSm4Output+cnt*224, tmpbuf+16, len);
    return NDK_OK;
}

