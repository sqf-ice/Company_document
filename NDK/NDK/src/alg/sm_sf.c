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
 * ����ԭ�� SM3Start( void )
 * ����˵�� ����CCP����
 * ����˵��
 *       ��
 * �������
 *       ��
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
 */
int SM3Start(void)
{
    sm3_starts( &Ctx );
    return NDK_OK;
}
/*
 * ����ԭ�� SM3Update( unsigned char *pDat )
 * ����˵�� updateһ���������ݣ�����Ϊ64�ֽ�������
 * ����˵��
 *       [IN]    pDat һ���������ݣ�64�ֽ�������
 * �������
 *       ��
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR_PARA �����Ƿ�
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� SM3Final( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * ����˵�� �������һ�����ݣ����ժҪ
 * ����˵��
 *       [IN]    pDat    ���һ����������
 *       [IN]    len     ���һ�����ݳ���
 * �������
 *       [OUT]   pHashDat ���ժҪ���ݣ�32�ֽ�
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� SM3Compute( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * ����˵�� �����������ݵ�ժҪ�������ڲ������䣬���ժҪ
 * ����˵��
 *       [IN]    pDat    ��������
 *       [IN]    len     �������ݳ���
 * �������
 *       [OUT]   pHashDat ���ժҪ���ݣ�32�ֽ�
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�������Ϊ�գ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
*@brief     DES3����,������Կ�������ּ���Ϊ���ܻ��ǽ���
*@param     pszAsciiBuf ASCII���뻺��
*@param     nLen ASCII����
*@param     cType ת������
*@param     pszBcdBuf ���BCD�뻺��
*@return    @li 0               �ɹ�
            @li -1              ����
*@section   history     �޸���ʷ
                \<author\>  \<time\>    \<desc\>
*/
static int AscToHex(const unsigned char* pszAsciiBuf, int nLen, char cType, unsigned char* pszBcdBuf)
{
    int i = 0;
    char cTmp, cTmp1;

    if (pszAsciiBuf == NULL) {
        return -1;
    }

    if ((nLen&0x01) && cType) { /*�б��Ƿ�Ϊ�����Լ����Ǳ߶���*/
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
*@brief     DES3����,������Կ�������ּ���Ϊ���ܻ��ǽ���
*@param     pszBcdBuf ����BCD�뻺��
*@param     nLen ASCII����
*@param     cType ת������
*@param     pszAsciiBuf ASCII�������
*@return    @li 0               �ɹ�
            @li -1              ����
*@section   history     �޸���ʷ
                \<author\>  \<time\>    \<desc\>
*/
static int HexToAsc(const unsigned char* pszBcdBuf, int nLen, char cType, unsigned char* pszAsciiBuf)
{
    int i = 0;

    if (pszBcdBuf == NULL) {
        return -1;
    }
    if (nLen & 0x01 && cType) { /*�б��Ƿ�Ϊ�����Լ����Ǳ߶���*/
        /*0��1��*/
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

    //��֤�Ƿ���������
    if (!EC_POINT_is_on_curve(ecp->group, P->ec_point, ecp->ctx)) {
        PDEBUG("EC_POINT_is_on_curve: 0\n");
        return 0;
    } else {
        PDEBUG("EC_POINT_is_on_curve: 1\n");
        return 1;
    }
}

/*
 * ����ԭ�� SM2KeyPairGen( unsigned char *eubkey, unsigned char *erikey )
 * ����˵�� ����SM2��Կ��
 * ����˵��
 * �������
 *     [OUT]  *eubKey         ��Կ 64�ֽ�
 *     [OUT]  *erikey         ˽Կ 32�ֽ�
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� SM2Encrypt( unsigned char *eubkey, unsigned char *Message, unsigned short MessageLen, unsigned short *Crypto, unsigned short *CryptoLen )
 * ����˵�� Crypto=Enc(msg,key)
 * ����˵��
 *     [IN]   *eubkey         ���ܹ�Կ
 *     [IN]   *Message           ��������
 *     [IN]   MessageLen         ���ݳ���
 *     [OUT]  *Crypto            ��������(����C1C3C2��˳���������)
 *     [OUT]  *CryptoLen         �������ݳ���(�������ݳ��ȱ��������ݳ�96���ֽ�)
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ� (����Ϊ�ա����ĳ��� >��1024 - 96���ֽ�)
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ��飺SM2���ܺ�����Ŀǰ�汾Ӧ�Ե�����Ӧ��C1C3C2����
 * ����ԭ�� SM2Decrypt( unsigned char *erikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
 * ����˵�� ��ԿЭ��
 * ����˵��
 *     [IN]   *erikey         ����˽Կ
 *     [IN]   *Crypto            ��������
 *     [IN]   CryptoLen          �������ݳ���
 *     [OUT]  *Message           ��������
 *     [OUT]  *MessageLen        ���ݳ���
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�(����Ϊ�ա����ĳ��� > 1024��?
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ��飺SM2ǩ������ժҪ���ɹ��ܣ���Ҫֱ�����������ϵ�e
 * ����ԭ�� SM2Sign(unsigned char *erikey, unsigned char *e, unsigned char *output )
 * ����˵�� (r,s)=sign(e,key)
 * ����˵��
 *     [IN]   *erikey         ǩ��˽Կ
 *     [IN]   *e                 ��ǩ�����ݵ�ժҪֵ��32�ֽڣ�
 *     [OUT]  *output            ǩ�������ݣ�64�ֽڣ�
 * ��ֵ˵��
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ� ������Ϊ�գ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ��飺SM2��ǩ����
 * ����ԭ�� unsigned char SM2Verify( unsigned char *pPublicKey, unsigned char *e, unsigned char *pSignedData )
 * ����˵�� if(signarture is ture)
 * ����˵��
 *     [IN]   *pPublicKey        ��֤��Կ
 *     [IN]   *e                 ��ǩ�����ݵ�ժҪֵ��32�ֽڣ�
 *     [IN]   *pSignedData       ǩ�������ݣ�64�ֽڣ�
 * ��ֵ˵�� -  NDK_OK ��ǩ�ɹ�
 *             NDK_ERR_PARA �����Ƿ�(�������Ϊ��)
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ��ǩʧ��
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
 * ��飺����ǩ��ժҪ����
 * ����ԭ�� int SM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData);
 * ����˵�� ��������ID,Message�͹�Կ�����������ǩ����ժҪ����
 * ����˵��
 *     [IN]  usID    ID����
 *     [IN]  pID     ID��������(*������ΪNULLʱ,ʹ��PBOC3.0Ĭ��ID������)
 *     [IN]  usM     Message����
 *     [IN]  pM      Message��������
 *     [IN]  pubKey  ��Կ��������
 *     [OUT] pHashData: ����������������ǩ����32�ֽ�ժҪ
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�����pID���������Ϊ�գ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� int SM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode);
 * ����˵�� �����������Կ��ģʽ����������ݣ�16�ֽڣ�����SM4����
 * ����˵��
 *       [IN]    pKey    ������Կ������Ϊ16�ֽ�
 *       [IN]    pIVector    ��ʼ����������Ϊ16�ֽڣ�ECBģʽ����Ϊ�գ�
 *       [IN]    len �������ݳ���
 *       [IN]    pSm4Input   ��������
 *       [IN]    mode    ����ģʽ(�ο���EM_ALG_SM4��)
 * �������
 *       [OUT]   pSm4Output ������ݣ�16�ֽ�
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�������Ϊ�գ����ݳ��Ȳ���16����������ģʽ�Ƿ���
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
            //CBC ����ǰ16�ֽ�ΪIVֵ
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


