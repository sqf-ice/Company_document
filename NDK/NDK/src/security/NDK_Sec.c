/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�    ��Ʒ������
* ��    �ڣ�    2012-08-17
* ��    ����    V1.00
* ����޸��ˣ�
* ����޸����ڣ�
*/
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_Sec.h"
#include "sdk_vpp.h"
#include "secp_dbus.h"

/** @addtogroup ���߿��� 
* @{
*/
#define POWER_IOC_MAGIC 'P'
#define POWER_SET_PINBUSY_STATUS	_IOR(POWER_IOC_MAGIC,21,int)

int pm_control(uint isbusy)
{
	int fd;
	int ret;

	fd = open("/dev/power", O_RDWR);
	if (ioctl(fd, POWER_SET_PINBUSY_STATUS, &isbusy) != 0)
		ret = NDK_ERR;
	else
		ret = 0;
	close(fd);

	return ret;
}

/** @} */ //���߿���
 

/** @addtogroup ��ȫ
* @{
*/

#define NDK_SECP_VERSION_STR    "NDK_SEC_1.1.0"

int NDK_SecGetPinResult(uchar *psPinBlock, int *nStatus);

static sec_vpp_data *s_pstSecVppData=NULL;

static int __SecVPPInit(int handle, sec_vpp_data *peData, int nVppType, char *pszExpPinLenIn);
static int __SecVPPEvent(int handle, int flag, uchar *pinblock, uchar *ksnout);
static int __SecVPPGetPin(uint nKeySession,
                          uchar ucKeyIdx,
                          uchar *pszExpPinLenIn,
                          const uchar *pszDataIn,
                          const uchar *psAd,
                          const int sizeAd,
                          uchar *psKsnOut,
                          uchar *psPinBlockOut,
                          uchar ucMode,
                          uint nTimeOutMs);

/**
 *@brief    ��ȡ��ȫ�ӿڰ汾
 *@retval   pszVerInfoOut   �汾��Ϣ��С��16�ֽڣ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszVerInfoOutΪNULL)
*/
NEXPORT int NDK_SecGetVer(uchar * pszVerInfoOut)
{
    if (pszVerInfoOut == NULL) {
        NDK_LOG_DEBUG(NDK_LOG_MODULE_SEC, "call %s pszVerInfoOut==NULL\n", __FUNCTION__);
        return NDK_ERR_PARA;
    }

    sprintf((char *)pszVerInfoOut, "%s", NDK_SECP_VERSION_STR);
    return NDK_OK;
}


/**
 *@brief    ��ȡ�����
 *@param    nRandLen        ��Ҫ��ȡ�ĳ���
 *@retval   pvRandom        ���������
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pvRandomΪNULL)
 *@li   NDK_ERR         ����ʧ��
*/
NEXPORT int NDK_SecGetRandom(int nRandLen , void *pvRandom)
{
    if (pvRandom == NULL) {
        return NDK_ERR_PARA;
    }
    return secp_methodcall(SECP_METHOD_NDK_GET_RNG, &nRandLen, sizeof(int), pvRandom, nRandLen, NULL);
}

/**
 *@brief    ���ð�ȫ����
 *@details  1���û�һ��ͨ���˺��������˰�ȫ������Ϣ��������������ݴ����õ�������Ϣ���п��ơ�
            2��ÿ��Ӧ��ӵ���Լ������ã�����Ӱ�쵽����Ӧ�õİ�ȫ���á�
 *@param    unCfgInfo       ������Ϣ��������λ�������ã�bit0�����ݲ�ʹ��
 *			����λ��1 ��ʾ�������ƣ� ��0��ʾ�ر����ƣ����嶨������
			bit[1] - �ж���ԿΨһ�� 
			bit[2] - �ж���Կר���� 
			bit[3] - �ж����д������� 
			bit[4] - �ж���Կ����ǿ�� 
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_SecSetCfg(uint unCfgInfo)
{
    return secp_methodcall(SECP_METHOD_NDK_SEC_SET_CFG, &unCfgInfo, sizeof(int), NULL, 0, NULL);
}


/**
 *@brief    ��ȡ��ȫ����
 *@retval   punCfgInfo      ������Ϣ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(punCfgInfoΪNULL)
*/
NEXPORT int NDK_SecGetCfg(uint *punCfgInfo)
{
	int ret;

    if(punCfgInfo==NULL) {
        NDK_LOG_DEBUG(NDK_LOG_MODULE_SEC, "call %s param err\n", __FUNCTION__);
        return NDK_ERR_PARA;
    }
    ret = secp_methodcall(SECP_METHOD_NDK_SEC_GET_CFG, NULL, 0, NULL, 0, NULL);
    if (ret>=0) {
        *punCfgInfo = ret;
        return NDK_OK;
    }
    return ret;
}

/**
 *@brief    ��ȡ��Կkcvֵ
 *@details  ��ȡ��Կ��KCVֵ,�Թ��Ի�˫��������Կ��֤,��ָ������Կ���㷨��һ�����ݽ��м���,�����ز����������ġ�
 *@param    ucKeyType       ��Կ����
 *@param    ucKeyIdx        ��Կ���
 *@param    pstKcvInfoOut   KCV����ģʽ
 *@retval   pstKcvInfoOut   KCVֵ
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pstKcvInfoOutΪNULL)
 *@li   NDK_ERR         ����ʧ��
*/
NEXPORT int NDK_SecGetKcv(uchar ucKeyType, uchar ucKeyIdx, ST_SEC_KCV_INFO *pstKcvInfoOut)
{
    ST_SECP_NDK_GETKCV_IN stGetKcvin;
    int ret;

    if (pstKcvInfoOut == NULL) {
        return NDK_ERR_PARA;
    }
    stGetKcvin.keytype = ucKeyType;
    stGetKcvin.keyidx = ucKeyIdx;
    stGetKcvin.stKcvInfo = *pstKcvInfoOut;
    ret = secp_methodcall(SECP_METHOD_NDK_GET_KCV, &stGetKcvin, sizeof(ST_SECP_NDK_GETKCV_IN), pstKcvInfoOut, sizeof(ST_SEC_KCV_INFO), NULL);
    return ret;
}

/**
 *@brief    ����������Կ
 *@return
 *@li   NDK_OK      �����ɹ�
 *@li   NDK_ERR     ����ʧ��
*/
NEXPORT int NDK_SecKeyErase(void)
{
    return secp_methodcall(SECP_METHOD_NDK_KEY_ERASE, NULL, 0, NULL, 0, NULL);
}

NEXPORT int NDK_SecKeyDelete(uchar ucKeyIdx, uchar ucKeyType)
{
	uchar param[2];

	switch( ucKeyType & KEY_TYPE_MASK ) {
	/*switch( ucKeyType ) {*/
	case SEC_KEY_TYPE_TLK:
	case SEC_KEY_TYPE_TMK:
	case SEC_KEY_TYPE_TPK:
	case SEC_KEY_TYPE_TAK:
	case SEC_KEY_TYPE_TDK:
		break;
	default:
		return NDK_ERR_PARA;
	}
	param[0] = ucKeyIdx;
	param[1] = ucKeyType;
    return secp_methodcall(SECP_METHOD_NDK_KEY_DELETE, param, sizeof(param), NULL, 0, NULL);
}

/**
 *@brief    д��һ����Կ,����TLK,TMK��TWK��д�롢��ɢ,������ѡ��ʹ��KCV��֤��Կ��ȷ�ԡ�
 *@details
    PED����������Կ��ϵ,���ϵ��µ�˳������Ϊ��
    TLK��Terminal Key Loading Key
        �յ��л�POS��Ӫ�̵�˽����Կ,���յ��л���POS��Ӫ���ڰ�ȫ������ֱ��д�롣
        ����Կÿ��PED�ն�ֻ��һ��,����������1��1

    TMK��Terminal Master Key��Acquirer Master Key
        �ն�����Կ,���߳�Ϊ�յ�������Կ��������Կ����100��,��������1��100
        TMK�����ڰ�ȫ������ֱ��д��,ֱ��д��TMK,��ͨ��TMK��ɢTWK�ķ�ʽ��MK/SK����Կ��ϵһ�¡�
    TWK��Transaction working key = Transaction Pin Key + Transaction MAC Key + Terminal DES Enc Key + Terminal DES DEC/ENC Key
        �ն˹�����Կ,����PIN���ġ�MAC���������Կ��������Կ����100��,��������1��100��
        TPK:����Ӧ������PIN��,����PIN Block��
        TAK:����Ӧ�ñ���ͨѶ��,����MAC��
        TEK:���ڶ�Ӧ�����������ݽ���DES/TDES���ܴ����洢��
        TDK:���ڶ�Ӧ�����������ݽ���DES/TDES�ӽ�������
    TWK�����ڰ�ȫ������ֱ��д��,ֱ��д��TWK��Fixed Key��Կ��ϵһ�¡�ÿ����Կ����������,����,��;�ͱ�ǩ��
    ������Կ�ı�ǩ����д����Կǰͨ��API�趨��,����Ȩ����Կ��ʹ��Ȩ�޲���֤��Կ���ᱻ���á�

    DUKPT��Կ���ƣ�
    DUKPT��Derived Unique Key Per Transaction����Կ��ϵ��һ�ν���һ��Կ����Կ��ϵ,��ÿ�ʽ��׵Ĺ�����Կ��PIN��MAC���ǲ�ͬ�ġ�
    ��������KSN��Key Serial Number���ĸ���,KSN����ʵ��һ��һ�ܵĹؼ����ӡ� ÿ��KSN��Ӧ����Կ��������Կ��;����������ͬ����Կ��
    ������Կ����10�顣��д��TIK��ʱ��,��Ҫѡ�����������,��ʹ��DUKPT��Կʱѡ���Ӧ����������
 *@param    pstKeyInfoIn        ��Կ��Ϣ
 *@param    pstKcvInfoIn        ��ԿУ����Ϣ
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pstKeyInfoIn��pstKcvInfoInΪNULL����Կ���Ȳ�����8/16/24��������չTR-31��ʽ�İ�װ��)
 *@li   NDK_ERR_MACLLOC �ڴ�ռ䲻��
 *@li   NDK_ERR                   ����ʧ��
*/
NEXPORT int NDK_SecLoadKey(ST_SEC_KEY_INFO * pstKeyInfoIn, ST_SEC_KCV_INFO * pstKcvInfoIn)
{
    ST_SECP_NDK_LOADKEY_IN *stLoadKeyIn;
    ST_EXTEND_KEYBLOCK *pkeyblock;
    int ret;
    int arglen;

    if (pstKeyInfoIn==NULL || pstKcvInfoIn==NULL) {
        return NDK_ERR_PARA;
    }
    /*�����Ȳ�����8/16/24���ж��Ƿ�Ϊ��չTR-31��ʽ�İ�װ��*/
    if (pstKeyInfoIn->nDstKeyLen != 8 && pstKeyInfoIn->nDstKeyLen != 16 && pstKeyInfoIn->nDstKeyLen != 24) {
        if (pstKeyInfoIn->nDstKeyLen != sizeof(ST_EXTEND_KEYBLOCK)) {
            return NDK_ERR_PARA;
        }
        pkeyblock = (ST_EXTEND_KEYBLOCK*)pstKeyInfoIn->sDstKeyValue;
        if (pkeyblock->format != SEC_KEYBLOCK_FMT_TR31) {
            return NDK_ERR_PARA;
        }
        arglen = sizeof(ST_SECP_NDK_LOADKEY_IN) + pkeyblock->len;
        stLoadKeyIn = (ST_SECP_NDK_LOADKEY_IN *)malloc(arglen);
        if (stLoadKeyIn == NULL)
            return NDK_ERR_MACLLOC;
        memcpy(stLoadKeyIn->extend_keyblock, pkeyblock->pblock, pkeyblock->len);
    } else {
        arglen = sizeof(ST_SECP_NDK_LOADKEY_IN);
        stLoadKeyIn = (ST_SECP_NDK_LOADKEY_IN *)malloc(arglen);
        if (stLoadKeyIn == NULL)
            return NDK_ERR_MACLLOC;
    }
    stLoadKeyIn->stSeckeyInfo = *pstKeyInfoIn;
    stLoadKeyIn->stSecKcvInfo = *pstKcvInfoIn;

    ret = secp_methodcall(SECP_METHOD_NDK_LOAD_KEY, stLoadKeyIn, arglen, NULL, 0, NULL);
    free(stLoadKeyIn);
    return ret;
}

/**
 *@brief    �������μ���PINBlock���߼���MAC֮����С���ʱ��
 *@details  PINBLOCK���ʱ��ļ��㷽ʽ��
            Ĭ��Ϊ120����ֻ�ܵ���4��,��TPKIntervalTimeMsĬ��ֵΪ30��,���øú����������ú�,����Ϊ4*TPKIntervalTimeMsʱ����ֻ�ܵ���4�Ρ�
            ���紫���TPKIntervalTimeMsΪ20000(ms),��80����ֻ�ܵ���4��
 *@param    unTPKIntervalTimeMs PIN��Կ������ʱ�䣬0-����Ĭ��ֵ��0xFFFFFFFF�����ı�
 *@param    unTAKIntervalTimeMs MAC��Կ������ʱ�䣬0-����Ĭ��ֵ��0xFFFFFFFF�����ı�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_SecSetIntervaltime(uint unTPKIntervalTimeMs, uint unTAKIntervalTimeMs)
{
    return -1;
}

/**
 *@brief    ���ù��ܼ�����
 *@details  ��������������У����ܼ���;���ж���
 *@param    ucType  ������;���Ͷ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_SecSetFunctionKey(uchar ucType)
{
    return -1;
}

/**
 *@brief    ����MAC
 *@param    ucKeyIdx        ��Կ���
 *@param    psDataIn        ��������
 *@param    nDataInLen      �������ݳ���
 *@param    ucMod           MAC����ģʽ �ο�/ref EM_SEC_MAC "EM_SEC_MAC"
 *@retval   psMacOut        MACֵ������8�ֽ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR         ����ʧ��
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR_MACLLOC     �ڴ�ռ䲻��
*/
NEXPORT int NDK_SecGetMac(uchar ucKeyIdx, uchar *psDataIn, int nDataInLen, uchar *psMacOut, uchar ucMod)
{
    ST_SECP_NDK_GETMAC_IN *indata;
    int inlen;
    int ret;

    if ((psDataIn==NULL) || (psMacOut==NULL) || (nDataInLen<=0)) {
        return NDK_ERR_PARA;
    }

    inlen = sizeof(ST_SECP_NDK_GETMAC_IN) + nDataInLen;
    if ((indata=(ST_SECP_NDK_GETMAC_IN *)malloc(inlen)) == NULL) {
        return NDK_ERR_MACLLOC;
    }

    indata->ucKeyIdx = ucKeyIdx;
    indata->ucMod = ucMod;
    indata->nDataInLen = nDataInLen;
    memcpy((char *)(indata)+sizeof(ST_SECP_NDK_GETMAC_IN), psDataIn, nDataInLen);
    ret = secp_methodcall(SECP_METHOD_NDK_CALC_MAC, indata, inlen, psMacOut, 16, NULL);
    free(indata);
    return ret;
}

/**
 *@brief    ��ȡPIN Block
 *@param    ucKeyIdx        ��Կ���
 *@param    pszExpPinLenIn  ���볤�ȣ���ʹ��,���зָ���磺0,4,6
 *@param    pszDataIn       ��ISO9564Ҫ�������PIN BLOCK
 *@param    ucMode          ����ģʽ �ο�/ref EM_SEC_PIN "EM_SEC_PIN"
 *@param    nTimeOutMs      ��ʱʱ�䣨���������120�룩��λ:ms
 *@retval   psPinBlockOut   PIN Block���,�ò�������NULLʱ��PIN���ͨ��NDK_SecGetPinResult������ȡ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR         ����ʧ��
 *@li   NDK_ERR_MACLLOC     �ڴ�ռ䲻��
 *@li   NDK_ERR_SECP_PARAM      �����Ƿ�(����ģʽ�Ƿ�)
 *@li   NDK_ERR_PARA            �����Ƿ�(ʱ������Ƿ�)
*/
NEXPORT int NDK_SecGetPin(uchar ucKeyIdx, uchar *pszExpPinLenIn, const uchar *pszDataIn, uchar *psPinBlockOut, uchar ucMode, uint nTimeOutMs)
{
    return __SecVPPGetPin(SEC_VPP_MASTER_SESSION, ucKeyIdx, pszExpPinLenIn, pszDataIn, NULL, 0, NULL, psPinBlockOut, ucMode, nTimeOutMs);
}

/**
 *@brief    ����DES
 *@details  ʹ��ָ����Կ����des���㣬ע�⣺1~100��Ž��мӽ���
 *@param    ucKeyIdx        DES��Կ���
 *@param    psDataIn        ������Ϣ
 *@param    unDataInLen     ���ݳ���
 *@param    ucMode          ����ģʽ �ο�/ref EM_SEC_DES "EM_SEC_DES"
 *@retval   psDataOut       ���������Ϣ
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR_        ����ʧ��
 *@li   NDK_ERR_SECP_PARAM  �����Ƿ�(���ݳ��Ȳ���8��������)
 *@li   NDK_ERR_MACLLOC �ڴ�ռ䲻��
*/
NEXPORT int NDK_SecCalcDes(uchar ucKeyType, uchar ucKeyIdx, uchar * psDataIn, int nDataInLen, uchar *psDataOut, uchar ucMode)
{
    ST_SECP_NDK_GETDES_IN *indata;
    int inlen;
    int ret;

    if ((psDataIn==NULL) || (psDataOut==NULL) || nDataInLen<=0) {
        return NDK_ERR_PARA;
    }

    ret = nDataInLen%8;
    if (ret) {
//      ret = 8 - ret;      /*���뵽8�ֽ�����������䳤��*/
        return NDK_ERR_SECP_PARAM;
    }

	if((ucMode == SEC_SM4_ENCRYPT) || (ucMode == SEC_SM4_DECRYPT)){
		if((nDataInLen%16) != 0)
			return NDK_ERR_SECP_PARAM;
	}

    inlen = sizeof(ST_SECP_NDK_GETDES_IN) + nDataInLen + ret;
    if ((indata=(ST_SECP_NDK_GETDES_IN *)malloc(inlen)) == NULL) {
        return NDK_ERR_MACLLOC;
    }

    indata->ucKeyIdx = ucKeyIdx;
    indata->ucMod = ucMode;
    indata->ucKeyType = ucKeyType;
    indata->nDataInLen = nDataInLen + ret;
    memcpy((char *)(indata)+sizeof(ST_SECP_NDK_GETDES_IN), psDataIn, nDataInLen);
    if (ret) {
        memset((char *)(indata) + sizeof(ST_SECP_NDK_GETDES_IN) + nDataInLen, 0 , ret);
        nDataInLen += ret;
    }
    ret = secp_methodcall(SECP_METHOD_NDK_CALC_DES, indata, inlen, psDataOut, nDataInLen, NULL);
    free(indata);
    return ret;
}

/**
 *@brief    У���ѻ�����PIN
 *@details  ��ȡ����PIN,Ȼ����Ӧ���ṩ�Ŀ�Ƭ�����뿨Ƭͨ����,������PIN BLOCKֱ�ӷ��͸���Ƭ(PIN BLOCK��ʽ���÷���������)��
 *@param    ucIccSlot       IC����
 *@param    pszExpPinLenIn  ���볤�ȣ���ʹ��,���зָ���磺0,4,6
 *@param    ucMode          IC������ģʽ(ֻ֧��EMV)
 *@param    unTimeoutMs     ��ʱʱ��
 *@retval   psIccRespOut    ��ƬӦ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA            �����Ƿ�(��ʱ�����Ƿ�)
 *@li   NDK_ERR_SECP_PARAM      �����Ƿ�(ucMode�Ƿ���)
 *@li   NDK_ERR_MACLLOC               �ڴ�ռ䲻��
 *@li   NDK_ERR                                           ����ʧ��
*/
NEXPORT int NDK_SecVerifyPlainPin(uchar ucIccSlot, uchar *pszExpPinLenIn, uchar *psIccRespOut, uchar ucMode,  uint unTimeoutMs)
{
    return __SecVPPGetPin(SEC_VPP_EMV_OFFLINE_CLEARPIN, 0, pszExpPinLenIn, NULL, NULL, 0, NULL, psIccRespOut, SEC_PIN_ISO9564_2, unTimeoutMs);
}

/**
 *@brief    У���ѻ�����PIN
 *@details  �Ȼ�ȡ����PIN,����Ӧ���ṩ��RsaPinKey������PIN����EMV�淶���м���,Ȼ����Ӧ���ṩ�Ŀ�Ƭ�����뿨Ƭͨ����,������PINֱ�ӷ��͸���Ƭ
 *@param    ucIccSlot       IC����
 *@param    pszExpPinLenIn  ���볤�ȣ���ʹ��,���зָ���磺0,4,6
 *@param    pstRsaPinKeyIn  RSA��Կ����
 *@param    ucMode          IC������ģʽ(ֻ֧��EMV)
 *@param    unTimeoutMs     ��ʱʱ��
 *@retval   psIccRespOut    ��ƬӦ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA            �����Ƿ�(��ʱ�����Ƿ�)
 *@li   NDK_ERR_SECP_PARAM      �����Ƿ�(ucMode�Ƿ���)
 *@li   NDK_ERR_MACLLOC               �ڴ�ռ䲻��
 *@li   NDK_ERR                                           ����ʧ
*/
NEXPORT int NDK_SecVerifyCipherPin(uchar ucIccSlot, uchar *pszExpPinLenIn, ST_SEC_RSA_KEY *pstRsaPinKeyIn, uchar *psIccRespOut, uchar ucMode, uint unTimeoutMs)
{
    return __SecVPPGetPin(SEC_VPP_EMV_OFFLINE_ENCPIN, 0, pszExpPinLenIn, NULL, (const uchar *)pstRsaPinKeyIn, sizeof(ST_SEC_RSA_KEY), NULL, psIccRespOut, SEC_PIN_ISO9564_2, unTimeoutMs);
}

/**
 *@brief    ��װDUKPT��Կ
 *@param    ucGroupIdx      ��Կ��ID
 *@param    ucSrcKeyIdx     ԭ��ԿID���������ܳ�ʼ��Կֵ����ԿID��
 *@param    ucKeyLen        ��Կ����
 *@param    psKeyValueIn    ��ʼ��Կֵ
 *@param    psKsnIn         KSNֵ
 *@param    pstKcvInfoIn    Kcv��Ϣ
 *@return
 *@li   NDK_OK                      �����ɹ�
 *@li   NDK_ERR_PARA          �����Ƿ�
 *@li   NDK_ERR                     ����ʧ��
 *@li   NDK_ERR_MACLLOC              �ڴ�ռ䲻��
*/
NEXPORT int NDK_SecLoadTIK(uchar ucGroupIdx, uchar ucSrcKeyIdx, uchar ucKeyLen, uchar * psKeyValueIn, uchar * psKsnIn, ST_SEC_KCV_INFO * pstKcvInfoIn)
{
    ST_SECP_NDK_LOADTIK_IN *stLoadTIKIn;
    ST_EXTEND_KEYBLOCK *pkeyblock;
    int ret;
    int arglen;
    ST_SEC_KCV_INFO kcv_none;
    
    if (psKeyValueIn==NULL || ucKeyLen < 16) {
        return NDK_ERR_PARA;
    }
	if (pstKcvInfoIn == NULL) {
		memset(&kcv_none, 0, sizeof(ST_SEC_KCV_INFO));
		kcv_none.nCheckMode =SEC_KCV_NONE ;
		pstKcvInfoIn = &kcv_none;
	}
	
    if (ucKeyLen == 16) {
        if (psKsnIn == NULL)
            return NDK_ERR_PARA;
        arglen = sizeof(ST_SECP_NDK_LOADTIK_IN);
        stLoadTIKIn = (ST_SECP_NDK_LOADTIK_IN *)malloc(arglen);
        if (stLoadTIKIn == NULL)
            return NDK_ERR_MACLLOC;
        stLoadTIKIn->nKeyLen = ucKeyLen;
        memcpy(stLoadTIKIn->sKeyValue, psKeyValueIn, 16);
        memcpy(stLoadTIKIn->sKsn, psKsnIn, 10);
    } else {    /*�����Ȳ�����16������Ϊ����չTR-31��ʽ�İ�װ��*/
        arglen = sizeof(ST_SECP_NDK_LOADTIK_IN) + ucKeyLen;
        stLoadTIKIn = (ST_SECP_NDK_LOADTIK_IN *)malloc(arglen);
        if (stLoadTIKIn == NULL)
            return NDK_ERR_MACLLOC;

        pkeyblock = (ST_EXTEND_KEYBLOCK *)stLoadTIKIn->sKeyValue;
        pkeyblock->format = SEC_KEYBLOCK_FMT_TR31;
        pkeyblock->len = ucKeyLen;
        stLoadTIKIn->nKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
        memcpy(stLoadTIKIn->extend_keyblock, psKeyValueIn, ucKeyLen);
    }
    stLoadTIKIn->ucGroupIdx = ucGroupIdx;
    stLoadTIKIn->ucKekIdx = ucSrcKeyIdx;

    stLoadTIKIn->stSecKcvInfo = *pstKcvInfoIn;

    ret = secp_methodcall(SECP_METHOD_NDK_LOAD_TIK, stLoadTIKIn, arglen, NULL, 0, NULL);
    free(stLoadTIKIn);
    return ret;
}

/**
 *@brief    ��ȡDUKPTֵ
 *@param    ucGroupIdx      DUKPT��Կ��ID
 *@retval   psKsnOut        ��ǰKSN��
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psKsnOutΪNULL)
 *@li   NDK_ERR         ����ʧ��
*/
NEXPORT int NDK_SecGetDukptKsn(uchar ucGroupIdx, uchar * psKsnOut)
{
    if (psKsnOut == NULL) {
        return NDK_ERR_PARA;
    }

    return secp_methodcall(SECP_METHOD_NDK_GET_DUKPT_KSN, &ucGroupIdx, sizeof(uchar), psKsnOut, 10, NULL);
}

/**
 *@brief    KSN������
 *@param    ucGroupIdx      DUKPT��Կ��ID
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR         ����ʧ��
*/
NEXPORT int NDK_SecIncreaseDukptKsn(uchar ucGroupIdx)
{
    return secp_methodcall(SECP_METHOD_NDK_INCREASE_DUKPT_KSN, &ucGroupIdx, sizeof(uchar), NULL, 0, NULL);
}

/**
 *@brief    ��ȡDUKPT��Կ��PIN Block
 *@param    ucGroupIdx      ��Կ���
 *@param    pszExpPinLenIn  ���볤�ȣ���ʹ��,���зָ���磺0,4,6
 *@param    psDataIn        ��ISO9564Ҫ�������PIN BLOCK
 *@param    ucMode          ����ģʽ
 *@param    unTimeoutMs     ��ʱʱ��
 *@retval   psKsnOut        ��ǰKSN��
 *@retval   psPinBlockOut   PIN Block���
 *@return
 *@li   NDK_OK                  �����ɹ�
 *@li   NDK_ERR_PARA            �����Ƿ�(��ʱ�����Ƿ�)
 *@li   NDK_ERR_SECP_PARAM      �����Ƿ�(ucMode�Ƿ���)
 *@li   NDK_ERR_MACLLOC         �ڴ�ռ䲻��
 *@li   NDK_ERR                 ����ʧ��
*/
NEXPORT int NDK_SecGetPinDukpt(uchar ucGroupIdx, uchar *pszExpPinLenIn, uchar * psDataIn, uchar* psKsnOut, uchar *psPinBlockOut, uchar ucMode, uint unTimeoutMs)
{
    return __SecVPPGetPin(SEC_VPP_DUKPT_NO_ITERATE, ucGroupIdx, pszExpPinLenIn, psDataIn, NULL, 0, psKsnOut, psPinBlockOut, ucMode, unTimeoutMs);
}

/**
 *@brief    ����DUKPT��ԿMAC
 *@param    ucGroupIdx      ��Կ���
 *@param    psDataIn        ��������
 *@param    nDataInLen      �������ݳ���
 *@param    ucMod           MAC����ģʽ
 *@retval   psMacOut        MACֵ������8�ֽ�
 *@retval   psKsnOut        ��ǰKSN��
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR_MACLLOC �ڴ�ռ䲻��
 *@li   NDK_ERR     ����ʧ��
*/
NEXPORT int NDK_SecGetMacDukpt(uchar ucGroupIdx, uchar *psDataIn, int nDataInLen, uchar *psMacOut, uchar *psKsnOut, uchar ucMode)
{
    ST_SECP_NDK_GETMAC_IN *indata;
    int inlen;
    int ret;
    ST_SECP_NDK_DUKPT_MAC_OUT mac_output;

    if ((psDataIn==NULL) || (psMacOut==NULL) || psKsnOut==NULL || nDataInLen<=0) {
        return NDK_ERR_PARA;
    }

    inlen = sizeof(ST_SECP_NDK_GETMAC_IN)+nDataInLen;
    if ((indata=(ST_SECP_NDK_GETMAC_IN *)malloc(inlen)) == NULL) {
        return NDK_ERR_MACLLOC;
    }

    indata->ucKeyIdx = ucGroupIdx;
    indata->ucMod = ucMode;
    indata->nDataInLen = nDataInLen;
    memcpy((char *)(indata)+sizeof(ST_SECP_NDK_GETMAC_IN), psDataIn, nDataInLen);
    ret = secp_methodcall(SECP_METHOD_NDK_CALC_MAC_DUKPT, indata, inlen, &mac_output, sizeof(ST_SECP_NDK_DUKPT_MAC_OUT), NULL);
    free(indata);
    if (ret == 0) {
        memcpy(psMacOut, mac_output.mac, sizeof(mac_output.mac));
        memcpy(psKsnOut, mac_output.ksn, sizeof(mac_output.ksn));
    }
    return ret;
}

/**
 *@brief    ����DES
 *@details  ʹ��ָ����Կ����des����
 *@param    ucGroupIdx      DUKPT��Կ���
 *@param    ucKeyVarType    ��Կ����
 *@param    psIV            ��ʼ����
 *@param    psDataIn        ������Ϣ
 *@param    usDataInLen     ���ݳ���
 *@param    ucMode          ����ģʽ
 *@retval   psDataOut       ���������Ϣ
 *@retval   psKsnOut        ��ǰKSN��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR_SECP_PARAM      �����Ƿ�(���ݳ��Ȳ���8��������)
 *@li   NDK_ERR_MACLLOC     �ڴ�ռ䲻��
 *@li   NDK_ERR     ����ʧ��
*/
NEXPORT int NDK_SecCalcDesDukpt(uchar ucGroupIdx, uchar ucKeyVarType, uchar *psIV, ushort usDataInLen, uchar *psDataIn,uchar *psDataOut,uchar *psKsnOut ,uchar ucMode)
{
    ST_SECP_NDK_DUKPT_GETDES_IN *indata;
    int inlen;
    int ret;

    if ((psDataIn==NULL) || (psDataOut==NULL) || (usDataInLen==0) || (psKsnOut==NULL)) {
        return NDK_ERR_PARA;
    }
    ret = usDataInLen%8;
    if (ret) {
//      ret = 8 - ret;      /*���뵽8�ֽ�����������䳤��*/
        return NDK_ERR_SECP_PARAM;
    }
    inlen = sizeof(ST_SECP_NDK_DUKPT_GETDES_IN) + usDataInLen + ret;
    if ((indata=(ST_SECP_NDK_DUKPT_GETDES_IN *)malloc(inlen)) == NULL) {
        return NDK_ERR_MACLLOC;
    }

    if (psIV == NULL)
        memset(indata->ucIV, 0, sizeof(indata->ucIV));
    else
        memcpy(indata->ucIV, psIV, sizeof(indata->ucIV));
    indata->ucKeyIdx = ucGroupIdx;
    indata->ucMod = ucMode;
    indata->ucKeyType = ucKeyVarType;
    indata->nDataInLen = usDataInLen + ret;
    memcpy((char *)(indata) + sizeof(ST_SECP_NDK_DUKPT_GETDES_IN), psDataIn, usDataInLen);
    if (ret) {
        memset((char *)(indata) + sizeof(ST_SECP_NDK_DUKPT_GETDES_IN) + usDataInLen, 0 , ret);
        usDataInLen += ret;
    }
    ret = secp_methodcall(SECP_METHOD_NDK_CALC_DES_DUKPT, indata, inlen, indata, usDataInLen + 10, NULL);
    if (ret == 0) {
        memcpy(psKsnOut, (uchar*)indata, 10);
        memcpy(psDataOut, (uchar*)indata + 10, usDataInLen);
    }
    free(indata);
    return ret;
}

/**
 *@brief    ��װRSA��Կ
 *@param    ucRSAKeyIndex   ��Կ���
 *@param    pstRsakeyIn     RSA��Կ��Ϣ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR     ����ʧ��
*/
NEXPORT int NDK_SecLoadRsaKey(uchar ucRSAKeyIndex, ST_SEC_RSA_KEY *pstRsakeyIn)
{
    ST_SECP_NDK_LOADRSAKEY_IN stLoadKey;

    SECDEBUG("call NDK_SecLoadRsaKey\r\n");
    if(pstRsakeyIn==NULL) {
        return NDK_ERR_PARA;
    }

    stLoadKey.ucRSAKeyIndex = ucRSAKeyIndex;
    stLoadKey.stSecRsaKey = *pstRsakeyIn;
    return secp_methodcall(SECP_METHOD_NDK_LOAD_RSA_KEY, &stLoadKey, sizeof(ST_SECP_NDK_LOADRSAKEY_IN), NULL, 0, NULL);
}

/**
 *@brief    RSA��Կ�Լӽ���
 *@details  �ú�������RSA���ܻ��������,���ܻ����ͨ��ѡ�ò�ͬ����Կʵ�֡���(Modul,Exp)ѡ��˽����Կ,����м���;��ѡ�ù�����Կ,����н��ܡ�
            psDataIn�ĵ�һ���ֽڱ���С��psModule�ĵ�һ���ֽڡ� �ú�����ʵ�ֳ��Ȳ�����2048 bits ��RSA���㡣
            ��������ݿ��ٵĻ�������ģ����+1��
 *@param    ucRSAKeyIndex   ��Կ��ţ�֧�ֹ�10��RSA��Կ�洢�����0~9
 *@param    psDataIn        ����������,���Ⱥ�ģ�ȳ���ʹ��BCD��洢��
 *@param    nDataLen        �������ݳ���
 *@retval   psDataOut       �������,��ģ�ȳ���ʹ��BCD��洢��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR_MACLLOC �ڴ�ռ䲻��
 *@li   NDK_ERR                     ����ʧ��
*/
NEXPORT int NDK_SecRecover(uchar ucRSAKeyIndex, const uchar *psDataIn, int nDataLen, uchar *psDataOut)
{
    ST_SECP_NDK_RSARECOVER_IN *indata;
    int inlen;

    if ((psDataIn==NULL) || (psDataOut==NULL) || (nDataLen>MAX_RSA_MODULUS_LEN/2) || (nDataLen<=0)) {
        return NDK_ERR_PARA;
    }

    inlen = nDataLen+sizeof(ST_SECP_NDK_RSARECOVER_IN);
    if ((indata=(ST_SECP_NDK_RSARECOVER_IN *)malloc(inlen)) == NULL) {
        return NDK_ERR_MACLLOC;
    }

    indata->ucRSAKeyIndex = ucRSAKeyIndex;
    indata->nDataLen = nDataLen;
    memcpy((uchar *)indata+sizeof(ST_SECP_NDK_RSARECOVER_IN), psDataIn, nDataLen);

    return secp_methodcall(SECP_METHOD_NDK_RSA_RECOVER, indata, inlen, psDataOut, inlen, NULL);
}

/**
 *@brief    ��ȡ��������״̬
 *@retval   psPinBlock      pinblock����
 *@retval   nStatus         ״ֵ̬
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psPinBlock��nStatusΪNULL)
 *@li   NDK_ERR_SECP_PARAM  �����Ƿ�
 *@li   NDK_ERR             ����ʧ��
 *@li   NDK_ERR_SECVP_NOT_ACTIVE    VPPû�м����һ�ε���VPPInit
*/
NEXPORT int NDK_SecGetPinResult(uchar *psPinBlock, int *nStatus)
{
    int ret;

    if (psPinBlock == NULL || nStatus == NULL)
        return NDK_ERR_PARA;

    ret = __SecVPPEvent(0, 1, psPinBlock, NULL);
    switch(ret) {
        case SEC_VPP_PIN_KEY:
            *nStatus = SEC_VPP_KEY_PIN;
            ret = 0;
            break;
        case SEC_VPP_BACKSPACE_KEY:
            *nStatus = SEC_VPP_KEY_BACKSPACE;
            ret = 0;
            break;
        case SEC_VPP_CLEAR_KEY:
            *nStatus = SEC_VPP_KEY_CLEAR;
            ret = 0;
            break;
        case SEC_VPP_ENTER_KEY:
            *nStatus = SEC_VPP_KEY_ENTER;
            ret = 0;
            break;
        case SEC_VPP_CANCEL_KEY:
            *nStatus = SEC_VPP_KEY_ESC;
            ret = 0;
            break;
        default:
            if (ret>=0) {
                ret = NDK_ERR_SECVP_VPP-ret;
            }
            break;
    }

    SECDEBUG( "NDK_SecGetPinResult ret=%d\r\n", ret);
    return ret;
}

/**
 *@brief    ��ȡ��������״̬
 *@retval   psPinBlock      pinblock����
 *@retval   psKsn           ��ǰDUKPT��KSNֵ
 *@retval   nStatus         ״ֵ̬
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psPinBlock/psKsn/nStatusΪNULL)
 *@li   NDK_ERR_SECVP_NOT_ACTIVE    VPPû�м����һ�ε���VPPInit
*/
NEXPORT int NDK_SecGetPinResultDukpt(uchar *psPinBlock, uchar *psKsn, int *nStatus)
{
    int ret;

    if (psPinBlock == NULL || psKsn == NULL || nStatus == NULL)
        return NDK_ERR_PARA;

    if (s_pstSecVppData==NULL)
        return (NDK_ERR_SECVP_VPP - SEC_VPP_NOT_ACTIVE);
    if (s_pstSecVppData->type != SEC_VPP_DUKPT && s_pstSecVppData->type != SEC_VPP_DUKPT_NO_ITERATE) {
        return (NDK_ERR_SECVP_VPP - SEC_VPP_NOT_ACTIVE);
    }

    ret = __SecVPPEvent(0, 1, psPinBlock, psKsn);
    switch(ret) {
        case SEC_VPP_PIN_KEY:
            *nStatus = SEC_VPP_KEY_PIN;
            ret = 0;
            break;
        case SEC_VPP_BACKSPACE_KEY:
            *nStatus = SEC_VPP_KEY_BACKSPACE;
            ret = 0;
            break;
        case SEC_VPP_CLEAR_KEY:
            *nStatus = SEC_VPP_KEY_CLEAR;
            ret = 0;
            break;
        case SEC_VPP_ENTER_KEY:
            *nStatus = SEC_VPP_KEY_ENTER;
            ret = 0;
            break;
        case SEC_VPP_CANCEL_KEY:
            *nStatus = SEC_VPP_KEY_ESC;
            ret = 0;
            break;
        default:
            if (ret>=0) {
                ret = NDK_ERR_SECVP_VPP-ret;
            }
            break;
    }

    SECDEBUG( "NDK_SecGetPinResultDukpt ret=%d\r\n", ret);
    return ret;
}

/**
 *@brief    ������Կ����Ӧ������
 *@details  ����ϵͳӦ��(Keyloader)ʹ�ã�ͨ���ýӿ�ָ��������װ��Կ���������ơ�
 *          ����װ��Կ��ʱ��ϵͳ��ȫ���񽫻��жϵ�������ݣ��پ����Ƿ���øú������õ���Կ�������ƣ�
 *          -�����ͨ�û�����
 *              ��������Ч��ϵͳ��ȫ�����ֱ��ָ����װ��Կ������Ϊ��ǰ�û�����
 *          -���ϵͳӦ�ó���
 *              �ж�����Keyloaderϵͳ������ȫ�������NDK_SecSetKeyOwner()���õ�Ӧ����Ϊ��ǰ��װ��Կ��������
 *                  ���Keyloaderδ���ù���Կ��������Ĭ����Կ����ָ��ΪKeyloader����
 *              ����Keyloaderϵͳ������ֱ���Ե�ǰϵͳӦ��Ϊ��Կ����
 *@param    pszName         ��Կ����Ӧ������(����С��256)�������ݵ��ǿ��ַ�����������֮ǰ���õ���Կ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszNameΪNULL����Ӧ�����Ƴ��ȴ��ڵ���256)
 *@li   NDK_ERR         ����ʧ��
*/
NEXPORT int NDK_SecSetKeyOwner(char *pszName)
{
    int inlen;
    int ret;
    char *null_str = "";

    if (pszName==NULL) {
        pszName = null_str;
    }
    inlen = strlen(pszName);
    if (inlen >= 256) {
        return NDK_ERR_PARA;
    }
    inlen++;    /*�ַ���������Ҳ�跢��*/
    ret = secp_methodcall(SECP_METHOD_NDK_SET_KEY_OWNER, pszName, inlen, NULL, 0, NULL);
    return ret;
}

/**
 *@brief    ��ȡ��ȫ����״̬
 *@retval   status          ��ȫ����״̬�ο�/ref EM_SEC_TAMPER_STATUS "EM_SEC_TAMPER_STATUS"
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(statusΪNULL)
 *@li   NDK_ERR         ����ʧ��
*/
NEXPORT int NDK_SecGetTamperStatus(int *status)
{
    int ret;

    if (status == NULL) {
        return NDK_ERR_PARA;
    }
    ret = secp_methodcall(SECP_METHOD_NDK_GET_TAMPER_STATUS, NULL, 0, NULL, 0, NULL);
    if (ret>=0) {
        *status = ret;
        return NDK_OK;
    } else {
        return ret;
    }
}

static int __SecVPPInit(int handle, sec_vpp_data *peData, int nVppType, char *pszExpPinLenIn)
{
    char szBufNull[8]="";
    char sAd[8]= {0};
    unsigned char pinblock[16]= {0};
    int ret;
    int inlen;
    ST_SECP_NDK_PININIT_IN *indata;
    char *pOff;

    if (((peData==NULL) || (peData->size!=sizeof(sec_vpp_data)))
        || ((peData->type < SEC_VPP_DUKPT) || (peData->type >= SEC_VPP_INVALID_SESSION))
        || ((peData->indexKey < 0) || (peData->indexKey > 256))
        || ((peData->PINBlockFormat<SEC_PINBLKFMT_0) || (peData->PINBlockFormat>=SEC_PINBLKFMT_MAX))
        || ((peData->timeout<5) || (peData->timeout>200))) {
        SECDEBUG("peData->type=%d,peData->indexKey=%d,peData->PINBlockFormat=%d,peData->timeout=%d\r\n",
                 peData->type,peData->indexKey,peData->PINBlockFormat,peData->timeout);
        return NDK_ERR_PARA;
    }

    if (peData->pPAN==NULL) {
        //�����ж�ISO9564_0��ISO9564_3��Ҫ�������˺�
        if ((peData->PINBlockFormat==SEC_PINBLKFMT_0) || (peData->PINBlockFormat==SEC_PINBLKFMT_3) || ((peData->PINBlockFormat>=SEC_PINBLKFMT_SM4_2) && (peData->PINBlockFormat<=SEC_PINBLKFMT_SM4_5))) {
            return NDK_ERR_PARA;
        } else {
            peData->pPAN = szBufNull;
        }
    }
    if (peData->sizeAD==0) peData->pAD = sAd;
    if (peData->pPINBlock==NULL) peData->pPINBlock = pinblock;
    if (pszExpPinLenIn==NULL) {
//           pszExpPinLenIn = szBufNull;
        return NDK_ERR_PARA;
    } else {
        int tmplen;
        char *pszTmp=pszExpPinLenIn;
        while(1) {
            tmplen = atoi(pszTmp);
            if((tmplen>12) || (tmplen<0)) {
                return NDK_ERR_PARA;
            }

            if ((pszTmp = strstr(pszTmp, ","))!=NULL) {
                pszTmp += 1;
            } else {
                break;
            }
        }
    }

    //���ȼ���=�ṹ����+���˺��ִ�����+pinblock+��������+Ԥ�ڵ�pin����
    inlen = sizeof(ST_SECP_NDK_PININIT_IN)+(strlen(peData->pPAN)+1)+8+peData->sizeAD+(strlen(pszExpPinLenIn)+1);
    if ((indata=(ST_SECP_NDK_PININIT_IN *)malloc(inlen)) == NULL) {
        return NDK_ERR_MACLLOC;
    }
    indata->stSecVppData = *peData;
    indata->nVppType = nVppType;
    pOff = (char *)((char *)indata+sizeof(ST_SECP_NDK_PININIT_IN));
    strcpy(pOff, peData->pPAN);
    pOff += strlen(peData->pPAN)+1;
    memcpy(pOff, peData->pPINBlock, 8);
    pOff += 8;
    memcpy(pOff, peData->pAD, peData->sizeAD);
    pOff += peData->sizeAD;
    strcpy(pOff, pszExpPinLenIn);
    SECDEBUG("pszExpPinLenIn=%s pOff=%s %p\r\n", pszExpPinLenIn, pOff, pOff);
    ret = secp_methodcall(SECP_METHOD_VPP_INIT, indata, inlen, NULL,0, NULL);
    if (ret == 0) {
    	pm_control(1);
        SECDEBUG( "set s_pstSecVppData=peData\r\n");
        s_pstSecVppData = peData;
    }
    free(indata);
    return ret;
}

static int __SecVPPEvent(int handle, int flag, uchar *pinblock, uchar *ksnout)
{
    int ret;
    int recvlen;
    char event_buf[8+10+8];

    if (s_pstSecVppData==NULL)  return SEC_VPP_NOT_ACTIVE;
    if (s_pstSecVppData->type == SEC_VPP_DUKPT || s_pstSecVppData->type == SEC_VPP_DUKPT_NO_ITERATE) {
        if (ksnout == NULL)
            return NDK_ERR_SECP_PARAM;
        recvlen = 8+10;     /*PINBLOCK[8]+KSN[10]*/
    } else if(s_pstSecVppData->PINBlockFormat >= SEC_PINBLKFMT_SM4_1){
        ksnout = NULL;
        recvlen = 16;
    }else{
        ksnout = NULL;
        recvlen = 8;
    }

    while(1) {
        ret = secp_methodcall(SECP_METHOD_VPP_EVENT, &flag, sizeof(int), event_buf, recvlen, NULL);

        if (ret==SEC_VPP_NULL) { //δȡ�������¼����ٴν���ѭ��
            usleep(1000);
        } else if(ret == SEC_VPP_ENTER_KEY) {
        	pm_control(0);
            if (ksnout){//dukpt
                memcpy(ksnout, event_buf+8, 10);
		if(pinblock)
	                memcpy(pinblock, event_buf, 8);
            }else if(pinblock)
                memcpy(pinblock, event_buf, recvlen);
			
            s_pstSecVppData = NULL;
            break;
        } else if ((ret == SEC_VPP_PIN_KEY) || (ret == SEC_VPP_BACKSPACE_KEY) || (ret == SEC_VPP_CLEAR_KEY)) {
            memcpy(pinblock, event_buf, recvlen);//��������pinblock��λ
            break;
        } else {
        	pm_control(0);
            if (ret<0) { //��VPP���ص������¼�����������ģ���쳣��ǿ����ֹvpp����
                flag = 0;
                secp_methodcall(SECP_METHOD_VPP_EVENT, &flag, sizeof(int), pinblock, recvlen, NULL);
            }

            SECDEBUG( "set s_pstSecVppData=NLL\r\n");
            s_pstSecVppData = NULL;
            break;
        }
    }

    SECDEBUG( "set __SecVPPEvent=ret=%d\r\n", ret);
    return ret;
}

sec_vpp_data stSecVppData;

static int __SecVPPGetPin(uint nKeySession, uchar ucKeyIdx, uchar *pszExpPinLenIn, const uchar *pszDataIn, const uchar *psAD, int sizeAD, uchar *psKsnOut, uchar *psPinBlockOut, uchar ucMode, uint nTimeOutMs)
{
    int ret=0;
   
    memset((char *)&stSecVppData, 0, sizeof(sec_vpp_data));

    stSecVppData.size = sizeof(sec_vpp_data);
    stSecVppData.indexKey = ucKeyIdx;
    stSecVppData.pPAN = (char *)pszDataIn;
    switch(ucMode) {
        case SEC_PIN_ISO9564_0:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_0;
            break;
        case SEC_PIN_ISO9564_1:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_1;
            break;
        case SEC_PIN_ISO9564_2:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_2;
            break;
        case SEC_PIN_ISO9564_3:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_3;
            break;
        case SEC_PIN_SM4_1:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_SM4_1;
            break;
        case SEC_PIN_SM4_2:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_SM4_2;
            break;
        case SEC_PIN_SM4_3:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_SM4_3;
			break;
        case SEC_PIN_SM4_4:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_SM4_4;
			break;
        case SEC_PIN_SM4_5:
            stSecVppData.PINBlockFormat = SEC_PINBLKFMT_SM4_5;
            break;
        default:
            return NDK_ERR_SECP_PARAM;
    }

    if ((nTimeOutMs<(5*1000)) || (nTimeOutMs>200*1000)) {
        return NDK_ERR_PARA;
    }
    stSecVppData.timeout = nTimeOutMs/1000;
    stSecVppData.type = nKeySession;
    if (nKeySession == SEC_VPP_EMV_OFFLINE_ENCPIN) {
        if (psAD==NULL) {
            return NDK_ERR_SECP_PARAM;
        } else {
            if (!memcmp(((ST_SEC_RSA_KEY *)psAD)->sExponent, "\x00\x00\x03", 3)) {
                stSecVppData.Exponent = 3;
            } else if (!memcmp(((ST_SEC_RSA_KEY *)psAD)->sExponent, "\x01\x00\x01", 3)) {
                stSecVppData.Exponent = 0x1001;
            } else {
                return NDK_ERR_SECP_PARAM;
            }

            stSecVppData.sizeAD = (((ST_SEC_RSA_KEY *)psAD)->usBits+7)/8;
            stSecVppData.pAD = ((ST_SEC_RSA_KEY *)psAD)->sModulus;
        }
    }

    if (psPinBlockOut!=NULL) { //pinblock��ȡΪ����ģʽ
        if (nKeySession == SEC_VPP_DUKPT || nKeySession == SEC_VPP_DUKPT_NO_ITERATE) {
            if (psKsnOut == NULL) {
                return NDK_ERR_SECP_PARAM;
            }
        } else {
            psKsnOut = NULL;
        }
        ret = __SecVPPInit(0, &stSecVppData, SEC_VPP_TYPE_NDK|SEC_VPP_TYPE_STATUS_ENABLE,(char *)pszExpPinLenIn);
        if (ret == 0) {
            while(1) {
                ret = __SecVPPEvent(0, 1, psPinBlockOut, psKsnOut);
                if (ret == SEC_VPP_ENTER_KEY) {     //ȷ�ϼ����£��ɷ�������
                    SECDEBUG( "status==SEC_VPP_KEY_ENTER\r\n");
                    ret = 0;
                    break;
                } else if (ret == SEC_VPP_CANCEL_KEY) {     //ȡ�������£����ش���
                    SECDEBUG( "status==NDK_ERR_SECVP_GET_ESC\r\n");
                    ret = NDK_ERR_SECVP_GET_ESC;
                    break;
                } else if (ret == SEC_VPP_PIN_KEY || ret == SEC_VPP_BACKSPACE_KEY || ret == SEC_VPP_CLEAR_KEY) {
                    ret = 0;
                    SECDEBUG( "NDK_SecGetPinResult status=%d\r\n", ret);
                    //���ڱ������ڴ���pinblockʱ��������״̬�����������������������
                    //����ѭ���ȴ�
                } else {
                    if (ret>=0) {
                        ret = NDK_ERR_SECVP_VPP-ret;
                    }
                    SECDEBUG( "NDK_SecGetPinResult ret=%d\r\n", ret);
                    break;
                }
            }
        }
    } else {
        ret = __SecVPPInit(0, &stSecVppData, SEC_VPP_TYPE_NDK|SEC_VPP_TYPE_STATUS_DISABLE,(char *)pszExpPinLenIn);
    }

    return ret;
}

int NDK_SecSetRtcTime(struct tm *t)
{
    if (t==NULL) {
        //fprintf(stderr, "[%s]<line:%d>Null-pointer input\n",__func__,__LINE__);
        return NDK_ERR_SECP_PARAM;
    }
    return secp_methodcall(SECP_METHOD_NDK_SET_RTC_TIME, t, sizeof(struct tm), NULL, 0, NULL);
}

/** @} */ // ��ȫģ�����

/* End of this file */
