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
 * º¯ÊýÔ­ÐÍ ccpSM3Start( void )
 * ¹¦ÄÜËµÃ÷ Æô¶¯CCPÔËËã
 * ²ÎÊýËµÃ÷
 *       ÎÞ
 * Êä³öÊý¾Ý
 *       ÎÞ
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
 * º¯ÊýÔ­ÐÍ ccpSM3Update( unsigned char *pDat )
 * ¹¦ÄÜËµÃ÷ updateÒ»¸ö·Ö×éÊý¾Ý£¬Êý¾ÝÎª64×Ö½ÚÕûÊý±¶
 * ²ÎÊýËµÃ÷
 *       [IN]    pDat Ò»¸ö·Ö×éÊý¾Ý£¬64×Ö½ÚÕûÊý±¶
 * Êä³öÊý¾Ý
 *       ÎÞ
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
 * º¯ÊýÔ­ÐÍ ccpSM3Final( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * ¹¦ÄÜËµÃ÷ ¼ÆËã×îºóÒ»×éÊý¾Ý£¬Êä³öÕªÒª
 * ²ÎÊýËµÃ÷
 *       [IN]    pDat    ×îºóÒ»¸ö·Ö×éÊý¾Ý
 *       [IN]    len     ×îºóÒ»×éÊý¾Ý³¤¶È
 * Êä³öÊý¾Ý
 *       [OUT]   pHashDat Êä³öÕªÒªÊý¾Ý£¬32×Ö½Ú
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
 * º¯ÊýÔ­ÐÍ ccpSM3Compute( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * ¹¦ÄÜËµÃ÷ ¼ÆËãÊäÈëÊý¾ÝµÄÕªÒª£¬º¯ÊýÄÚ²¿Íê³ÉÌî³ä£¬Êä³öÕªÒª
 * ²ÎÊýËµÃ÷
 *       [IN]    pDat    ÊäÈëÊý¾Ý
 *       [IN]    len     ÊäÈëÊý¾Ý³¤¶È
 * Êä³öÊý¾Ý
 *       [OUT]   pHashDat Êä³öÕªÒªÊý¾Ý£¬32×Ö½Ú
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨£¨ÊäÈëÎª¿Õ£©
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
//µ¼ÈëSM2 ¹«Ô¿
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

//µ¼ÈëSM2 Ë½Ô¿
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
 * º¯ÊýÔ­ÐÍ ccpSM2KeyPairGen( unsigned char *eccpubkey, unsigned char *eccprikey )
 * ¹¦ÄÜËµÃ÷ Éú³ÉSM2ÃÜÔ¿¶Ô
 * ²ÎÊýËµÃ÷
 * Êä³öÊý¾Ý
 *     [OUT]  *eccpubKey         ¹«Ô¿ 64×Ö½Ú
 *     [OUT]  *eccprikey         Ë½Ô¿ 32×Ö½Ú
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
 * º¯ÊýÔ­ÐÍ ccpSM2Encrypt( unsigned char *eccpubkey, unsigned char *Message, unsigned short MessageLen, unsigned short *Crypto, unsigned short *CryptoLen )
 * ¹¦ÄÜËµÃ÷ Crypto=Enc(msg,key)
 * ²ÎÊýËµÃ÷
 *     [IN]   *eccpubkey         ¼ÓÃÜ¹«Ô¿
 *     [IN]   *Message           Ã÷ÎÄÊý¾Ý
 *     [IN]   MessageLen         Êý¾Ý³¤¶È
 *     [OUT]  *Crypto            ÃÜÎÄÊý¾Ý(°´ÕÕC1C3C2µÄË³Ðò½øÐÐÅÅÁÐ)
 *     [OUT]  *CryptoLen         ÃÜÎÄÊý¾Ý³¤¶È(ÃÜÎÄÊý¾Ý³¤¶È±ÈÃ÷ÎÄÊý¾Ý³¤96¸ö×Ö½Ú)
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨ (ÊäÈëÎª¿Õ¡¢Ã÷ÎÄ³¤¶È >£¨1024 - 96£©×Ö½Ú)
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
 * ¼ò½é£ºSM2½âÃÜº¯Êý£¬Ä¿Ç°°æ±¾Ó¦¶ÔµÄÃÜÎÄÓ¦°´C1C3C2ÅÅÁÐ
 * º¯ÊýÔ­ÐÍ ccpSM2Decrypt( unsigned char *eccprikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
 * ¹¦ÄÜËµÃ÷ ÃÜÔ¿Ð­ÉÌ
 * ²ÎÊýËµÃ÷
 *     [IN]   *eccprikey         ½âÃÜË½Ô¿
 *     [IN]   *Crypto            ÃÜÎÄÊý¾Ý
 *     [IN]   CryptoLen          ÃÜÎÄÊý¾Ý³¤¶È
 *     [OUT]  *Message           Ã÷ÎÄÊý¾Ý
 *     [OUT]  *MessageLen        Êý¾Ý³¤¶È
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨(ÊäÈëÎª¿Õ¡¢ÃÜÎÄ³¤¶È > 1024×ÖÚ)
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
 * ¼ò½é£ºSM2Ç©Ãû£¬ÎÞÕªÒªÉú³É¹¦ÄÜ£¬ÐèÒªÖ±½ÓÊäÈë¼ÆËãÍê±ÏµÄe
 * º¯ÊýÔ­ÐÍ ccpSM2Sign(unsigned char *eccprikey, unsigned char *e, unsigned char *output )
 * ¹¦ÄÜËµÃ÷ (r,s)=sign(e,key)
 * ²ÎÊýËµÃ÷
 *     [IN]   *eccprikey         Ç©ÃûË½Ô¿
 *     [IN]   *e                 ±»Ç©ÃûÊý¾ÝµÄÕªÒªÖµ£¨32×Ö½Ú£©
 *     [OUT]  *output            Ç©ÃûºóÊý¾Ý£¨64×Ö½Ú£©
 * ·µÖµËµÃ÷
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨ £¨ÊäÈëÎª¿Õ£©
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
    tmpdata.iData = e;  //¶ÔÓÚ¾ÉÇý¶¯µÚÒ»¸ö×Ö½Ú±êÊ¾Ä£Ê½£¬1-´ø idµÄÇ©Ãû£» 0 -²»´øid
    tmpdata.oData = output;

    iret = gm_cipher_oper(&tmpdata);
    if(iret != 0) {
        PDEBUG("ERROR iret = %d", iret);
        return NDK_ERR;
    }

    return iret;
}

/*
 * ¼ò½é£ºSM2ÑéÇ©º¯Êý
 * º¯ÊýÔ­ÐÍ unsigned char ccpSM2Verify( unsigned char *pPublicKey, unsigned char *e, unsigned char *pSignedData )
 * ¹¦ÄÜËµÃ÷ if(signarture is ture)
 * ²ÎÊýËµÃ÷
 *     [IN]   *pPublicKey        ÑéÖ¤¹«Ô¿
 *     [IN]   *e                 ±»Ç©ÃûÊý¾ÝµÄÕªÒªÖµ£¨32×Ö½Ú£©
 *     [IN]   *pSignedData       Ç©ÃûºóÊý¾Ý£¨64×Ö½Ú£©
 * ·µÖµËµÃ÷ -  NDK_OK ÑéÇ©³É¹¦
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨(ÊäÈë²ÎÊýÎª¿Õ)
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ÑéÇ©Ê§°Ü
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
 * ¼ò½é£ºÓÃÓÚÇ©ÃûÕªÒªÉú³É
 * º¯ÊýÔ­ÐÍ int ccpSM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData);
 * ¹¦ÄÜËµÃ÷ ¸ù¾ÝÊäÈëID,MessageºÍ¹«Ô¿£¬¼ÆËã³öÓÃÓÚÇ©ÃûµÄÕªÒªÊý¾Ý
 * ²ÎÊýËµÃ÷
 *     [IN]  usID    ID³¤¶È
 *     [IN]  pID     IDÊý¾ÝÊäÈë(*µ±´«ÈëÎªNULLÊ±,Ê¹ÓÃPBOC3.0Ä¬ÈÏID×öÔËËã)
 *     [IN]  usM     Message³¤¶È
 *     [IN]  pM      MessageÊý¾ÝÊäÈë
 *     [IN]  pubKey  ¹«Ô¿Êý¾ÝÊäÈë
 *     [OUT] pHashData: ¸ù¾ÝÊäÈë¼ÆËã³öÓÃÓÚÇ©ÃûµÄ32×Ö½ÚÕªÒª
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨£¨³ýpIDÍâ²ÎÊýÊäÈëÎª¿Õ£©
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
 * º¯ÊýÔ­ÐÍ int ccpSM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode);
 * ¹¦ÄÜËµÃ÷ ¸ù¾ÝÊäÈëµÄÃÜÔ¿ºÍÄ£Ê½¶ÔÊäÈëµÄÊý¾Ý£¨16×Ö½Ú£©½øÐÐSM4ÔËËã
 * ²ÎÊýËµÃ÷
 *       [IN]    pKey    ÊäÈëÃÜÔ¿£¬³¤¶ÈÎª16×Ö½Ú
 *       [IN]    pIVector    ³õÊ¼ÏòÁ¿£¬³¤¶ÈÎª16×Ö½Ú£¨ECBÄ£Ê½ÔÊÐíÎª¿Õ£©
 *       [IN]    len ÊäÈëÊý¾Ý³¤¶È
 *       [IN]    pSm4Input   ÊäÈëÊý¾Ý
 *       [IN]    mode    ÔËËãÄ£Ê½(²Î¿¼¡±EM_ALG_SM4¡°)
 * Êä³öÊý¾Ý
 *       [OUT]   pSm4Output Êä³öÊý¾Ý£¬16×Ö½Ú
 * ·µÖµËµÃ÷ -  NDK_OK ²Ù×÷³É¹¦
 *             NDK_ERR_PARA ²ÎÊý·Ç·¨£¨ÊäÈëÎª¿Õ£¬Êý¾Ý³¤¶È²»ÊÇ16µÄÕûÊý±¶£¬Ä£Ê½·Ç·¨£©
 *             NDK_ERR_OPEN_DEV  ´ò¿ªÉè±¸ÎÄ¼þÊ§°Ü
 *             NDK_ERR ²Ù×÷Ê§°Ü
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
            //CBC Êý¾ÝÇ°16×Ö½ÚÎªIVÖµ
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

