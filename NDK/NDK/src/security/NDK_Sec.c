/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：    产品开发部
* 日    期：    2012-08-17
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
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

/** @addtogroup 休眠控制 
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

/** @} */ //休眠控制
 

/** @addtogroup 安全
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
 *@brief    读取安全接口版本
 *@retval   pszVerInfoOut   版本信息（小于16字节）
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszVerInfoOut为NULL)
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
 *@brief    获取随机数
 *@param    nRandLen        需要获取的长度
 *@retval   pvRandom        随机数缓冲
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pvRandom为NULL)
 *@li   NDK_ERR         操作失败
*/
NEXPORT int NDK_SecGetRandom(int nRandLen , void *pvRandom)
{
    if (pvRandom == NULL) {
        return NDK_ERR_PARA;
    }
    return secp_methodcall(SECP_METHOD_NDK_GET_RNG, &nRandLen, sizeof(int), pvRandom, nRandLen, NULL);
}

/**
 *@brief    设置安全配置
 *@details  1、用户一旦通过此函数设置了安全配置信息，则后续操作根据此设置的配置信息进行控制。
            2、每个应用拥有自己的配置，不会影响到其他应用的安全配置。
 *@param    unCfgInfo       配置信息，按比特位进行设置，bit0保留暂不使用
 *			比特位置1 表示开启限制， 置0表示关闭限制，具体定义如下
			bit[1] - 判断密钥唯一性 
			bit[2] - 判断密钥专用性 
			bit[3] - 判断敏感次数限制 
			bit[4] - 判断密钥保护强度 
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_SecSetCfg(uint unCfgInfo)
{
    return secp_methodcall(SECP_METHOD_NDK_SEC_SET_CFG, &unCfgInfo, sizeof(int), NULL, 0, NULL);
}


/**
 *@brief    读取安全配置
 *@retval   punCfgInfo      配置信息
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(punCfgInfo为NULL)
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
 *@brief    读取密钥kcv值
 *@details  获取密钥的KCV值,以供对话双方进行密钥验证,用指定的密钥及算法对一段数据进行加密,并返回部分数据密文。
 *@param    ucKeyType       密钥类型
 *@param    ucKeyIdx        密钥序号
 *@param    pstKcvInfoOut   KCV加密模式
 *@retval   pstKcvInfoOut   KCV值
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pstKcvInfoOut为NULL)
 *@li   NDK_ERR         操作失败
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
 *@brief    擦除所有密钥
 *@return
 *@li   NDK_OK      操作成功
 *@li   NDK_ERR     操作失败
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
 *@brief    写入一个密钥,包括TLK,TMK和TWK的写入、发散,并可以选择使用KCV验证密钥正确性。
 *@details
    PED采用三层密钥体系,自上到下的顺序依次为：
    TLK－Terminal Key Loading Key
        收单行或POS运营商的私有密钥,由收单行或者POS运营商在安全环境下直接写入。
        该密钥每个PED终端只有一个,其索引号自1至1

    TMK－Terminal Master Key＝Acquirer Master Key
        终端主密钥,或者称为收单行主密钥。该类密钥可有100个,索引号自1至100
        TMK可以在安全环境下直接写入,直接写入TMK,并通过TMK分散TWK的方式与MK/SK的密钥体系一致。
    TWK－Transaction working key = Transaction Pin Key + Transaction MAC Key + Terminal DES Enc Key + Terminal DES DEC/ENC Key
        终端工作密钥,进行PIN密文、MAC等运算的密钥。该类密钥可有100个,索引号自1至100。
        TPK:用于应用输入PIN后,计算PIN Block。
        TAK:用于应用报文通讯中,计算MAC。
        TEK:用于对应用中敏感数据进行DES/TDES加密传输或存储。
        TDK:用于对应用中敏感数据进行DES/TDES加解密运用
    TWK可以在安全环境下直接写入,直接写入TWK与Fixed Key密钥体系一致。每个密钥有其索引号,长度,用途和标签。
    其中密钥的标签是在写入密钥前通过API设定的,以授权该密钥的使用权限并保证密钥不会被滥用。

    DUKPT密钥机制：
    DUKPT【Derived Unique Key Per Transaction】密钥体系是一次交易一密钥的密钥体系,其每笔交易的工作密钥【PIN、MAC】是不同的。
    它引入了KSN【Key Serial Number】的概念,KSN是能实现一次一密的关键因子。 每个KSN对应的密钥，根据密钥用途，产生出不同的密钥。
    该类密钥可有10组。在写入TIK的时候,需要选择组的索引号,在使用DUKPT密钥时选择对应的组索引。
 *@param    pstKeyInfoIn        密钥信息
 *@param    pstKcvInfoIn        密钥校验信息
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pstKeyInfoIn、pstKcvInfoIn为NULL、密钥长度不等于8/16/24、不是扩展TR-31格式的安装包)
 *@li   NDK_ERR_MACLLOC 内存空间不足
 *@li   NDK_ERR                   操作失败
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
    /*若长度不等于8/16/24，判断是否为扩展TR-31格式的安装包*/
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
 *@brief    设置两次计算PINBlock或者计算MAC之间最小间隔时间
 *@details  PINBLOCK间隔时间的计算方式：
            默认为120秒那只能调用4次,即TPKIntervalTimeMs默认值为30秒,调用该函数重新设置后,限制为4*TPKIntervalTimeMs时间内只能调用4次。
            比如传入的TPKIntervalTimeMs为20000(ms),则80秒内只能调用4次
 *@param    unTPKIntervalTimeMs PIN密钥计算间隔时间，0-采用默认值，0xFFFFFFFF，不改变
 *@param    unTAKIntervalTimeMs MAC密钥计算间隔时间，0-采用默认值，0xFFFFFFFF，不改变
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_SecSetIntervaltime(uint unTPKIntervalTimeMs, uint unTAKIntervalTimeMs)
{
    return -1;
}

/**
 *@brief    设置功能键功能
 *@details  对密码输入过程中，功能键用途进行定义
 *@param    ucType  功能用途类型定义
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_SecSetFunctionKey(uchar ucType)
{
    return -1;
}

/**
 *@brief    计算MAC
 *@param    ucKeyIdx        密钥序号
 *@param    psDataIn        输入数据
 *@param    nDataInLen      输入数据长度
 *@param    ucMod           MAC计算模式 参考/ref EM_SEC_MAC "EM_SEC_MAC"
 *@retval   psMacOut        MAC值，长度8字节
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR         操作失败
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_MACLLOC     内存空间不足
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
 *@brief    获取PIN Block
 *@param    ucKeyIdx        密钥序号
 *@param    pszExpPinLenIn  密码长度，可使用,进行分割，例如：0,4,6
 *@param    pszDataIn       按ISO9564要求的输入PIN BLOCK
 *@param    ucMode          计算模式 参考/ref EM_SEC_PIN "EM_SEC_PIN"
 *@param    nTimeOutMs      超时时间（不允许大于120秒）单位:ms
 *@retval   psPinBlockOut   PIN Block输出,该参数传入NULL时，PIN结果通过NDK_SecGetPinResult函数获取
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR         操作失败
 *@li   NDK_ERR_MACLLOC     内存空间不足
 *@li   NDK_ERR_SECP_PARAM      参数非法(计算模式非法)
 *@li   NDK_ERR_PARA            参数非法(时间参数非法)
*/
NEXPORT int NDK_SecGetPin(uchar ucKeyIdx, uchar *pszExpPinLenIn, const uchar *pszDataIn, uchar *psPinBlockOut, uchar ucMode, uint nTimeOutMs)
{
    return __SecVPPGetPin(SEC_VPP_MASTER_SESSION, ucKeyIdx, pszExpPinLenIn, pszDataIn, NULL, 0, NULL, psPinBlockOut, ucMode, nTimeOutMs);
}

/**
 *@brief    计算DES
 *@details  使用指定密钥进行des计算，注意：1~100序号进行加解密
 *@param    ucKeyIdx        DES密钥序号
 *@param    psDataIn        数据信息
 *@param    unDataInLen     数据长度
 *@param    ucMode          加密模式 参考/ref EM_SEC_DES "EM_SEC_DES"
 *@retval   psDataOut       数据输出信息
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_        操作失败
 *@li   NDK_ERR_SECP_PARAM  参数非法(数据长度不是8的整数倍)
 *@li   NDK_ERR_MACLLOC 内存空间不足
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
//      ret = 8 - ret;      /*补齐到8字节整倍数的填充长度*/
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
 *@brief    校验脱机明文PIN
 *@details  获取明文PIN,然后按照应用提供的卡片命令与卡片通道号,将明文PIN BLOCK直接发送给卡片(PIN BLOCK格式在用法部分描述)。
 *@param    ucIccSlot       IC卡号
 *@param    pszExpPinLenIn  密码长度，可使用,进行分割，例如：0,4,6
 *@param    ucMode          IC卡计算模式(只支持EMV)
 *@param    unTimeoutMs     超时时间
 *@retval   psIccRespOut    卡片应答码
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(超时参数非法)
 *@li   NDK_ERR_SECP_PARAM      参数非法(ucMode非法等)
 *@li   NDK_ERR_MACLLOC               内存空间不足
 *@li   NDK_ERR                                           操作失败
*/
NEXPORT int NDK_SecVerifyPlainPin(uchar ucIccSlot, uchar *pszExpPinLenIn, uchar *psIccRespOut, uchar ucMode,  uint unTimeoutMs)
{
    return __SecVPPGetPin(SEC_VPP_EMV_OFFLINE_CLEARPIN, 0, pszExpPinLenIn, NULL, NULL, 0, NULL, psIccRespOut, SEC_PIN_ISO9564_2, unTimeoutMs);
}

/**
 *@brief    校验脱机明文PIN
 *@details  先获取明文PIN,再用应用提供的RsaPinKey对明文PIN按照EMV规范进行加密,然后用应用提供的卡片命令与卡片通道号,将密文PIN直接发送给卡片
 *@param    ucIccSlot       IC卡号
 *@param    pszExpPinLenIn  密码长度，可使用,进行分割，例如：0,4,6
 *@param    pstRsaPinKeyIn  RSA密钥数据
 *@param    ucMode          IC卡计算模式(只支持EMV)
 *@param    unTimeoutMs     超时时间
 *@retval   psIccRespOut    卡片应答码
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(超时参数非法)
 *@li   NDK_ERR_SECP_PARAM      参数非法(ucMode非法等)
 *@li   NDK_ERR_MACLLOC               内存空间不足
 *@li   NDK_ERR                                           操作失
*/
NEXPORT int NDK_SecVerifyCipherPin(uchar ucIccSlot, uchar *pszExpPinLenIn, ST_SEC_RSA_KEY *pstRsaPinKeyIn, uchar *psIccRespOut, uchar ucMode, uint unTimeoutMs)
{
    return __SecVPPGetPin(SEC_VPP_EMV_OFFLINE_ENCPIN, 0, pszExpPinLenIn, NULL, (const uchar *)pstRsaPinKeyIn, sizeof(ST_SEC_RSA_KEY), NULL, psIccRespOut, SEC_PIN_ISO9564_2, unTimeoutMs);
}

/**
 *@brief    安装DUKPT密钥
 *@param    ucGroupIdx      密钥组ID
 *@param    ucSrcKeyIdx     原密钥ID（用来加密初始密钥值的密钥ID）
 *@param    ucKeyLen        密钥长度
 *@param    psKeyValueIn    初始密钥值
 *@param    psKsnIn         KSN值
 *@param    pstKcvInfoIn    Kcv信息
 *@return
 *@li   NDK_OK                      操作成功
 *@li   NDK_ERR_PARA          参数非法
 *@li   NDK_ERR                     操作失败
 *@li   NDK_ERR_MACLLOC              内存空间不足
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
    } else {    /*若长度不等于16，则认为是扩展TR-31格式的安装包*/
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
 *@brief    获取DUKPT值
 *@param    ucGroupIdx      DUKPT密钥组ID
 *@retval   psKsnOut        当前KSN号
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psKsnOut为NULL)
 *@li   NDK_ERR         操作失败
*/
NEXPORT int NDK_SecGetDukptKsn(uchar ucGroupIdx, uchar * psKsnOut)
{
    if (psKsnOut == NULL) {
        return NDK_ERR_PARA;
    }

    return secp_methodcall(SECP_METHOD_NDK_GET_DUKPT_KSN, &ucGroupIdx, sizeof(uchar), psKsnOut, 10, NULL);
}

/**
 *@brief    KSN号增加
 *@param    ucGroupIdx      DUKPT密钥组ID
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR         操作失败
*/
NEXPORT int NDK_SecIncreaseDukptKsn(uchar ucGroupIdx)
{
    return secp_methodcall(SECP_METHOD_NDK_INCREASE_DUKPT_KSN, &ucGroupIdx, sizeof(uchar), NULL, 0, NULL);
}

/**
 *@brief    获取DUKPT密钥的PIN Block
 *@param    ucGroupIdx      密钥序号
 *@param    pszExpPinLenIn  密码长度，可使用,进行分割，例如：0,4,6
 *@param    psDataIn        按ISO9564要求的输入PIN BLOCK
 *@param    ucMode          计算模式
 *@param    unTimeoutMs     超时时间
 *@retval   psKsnOut        当前KSN号
 *@retval   psPinBlockOut   PIN Block输出
 *@return
 *@li   NDK_OK                  操作成功
 *@li   NDK_ERR_PARA            参数非法(超时参数非法)
 *@li   NDK_ERR_SECP_PARAM      参数非法(ucMode非法等)
 *@li   NDK_ERR_MACLLOC         内存空间不足
 *@li   NDK_ERR                 操作失败
*/
NEXPORT int NDK_SecGetPinDukpt(uchar ucGroupIdx, uchar *pszExpPinLenIn, uchar * psDataIn, uchar* psKsnOut, uchar *psPinBlockOut, uchar ucMode, uint unTimeoutMs)
{
    return __SecVPPGetPin(SEC_VPP_DUKPT_NO_ITERATE, ucGroupIdx, pszExpPinLenIn, psDataIn, NULL, 0, psKsnOut, psPinBlockOut, ucMode, unTimeoutMs);
}

/**
 *@brief    计算DUKPT密钥MAC
 *@param    ucGroupIdx      密钥组号
 *@param    psDataIn        输入数据
 *@param    nDataInLen      输入数据长度
 *@param    ucMod           MAC计算模式
 *@retval   psMacOut        MAC值，长度8字节
 *@retval   psKsnOut        当前KSN号
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_MACLLOC 内存空间不足
 *@li   NDK_ERR     操作失败
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
 *@brief    计算DES
 *@details  使用指定密钥进行des计算
 *@param    ucGroupIdx      DUKPT密钥组号
 *@param    ucKeyVarType    密钥类型
 *@param    psIV            初始向量
 *@param    psDataIn        数据信息
 *@param    usDataInLen     数据长度
 *@param    ucMode          加密模式
 *@retval   psDataOut       数据输出信息
 *@retval   psKsnOut        当前KSN号
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_SECP_PARAM      参数非法(数据长度不是8的整数倍)
 *@li   NDK_ERR_MACLLOC     内存空间不足
 *@li   NDK_ERR     操作失败
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
//      ret = 8 - ret;      /*补齐到8字节整倍数的填充长度*/
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
 *@brief    安装RSA密钥
 *@param    ucRSAKeyIndex   密钥序号
 *@param    pstRsakeyIn     RSA密钥信息
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR     操作失败
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
 *@brief    RSA密钥对加解密
 *@details  该函数进行RSA加密或解密运算,加密或解密通过选用不同的密钥实现。如(Modul,Exp)选用私有密钥,则进行加密;如选用公开密钥,则进行解密。
            psDataIn的第一个字节必须小于psModule的第一个字节。 该函数可实现长度不超过2048 bits 的RSA运算。
            输入的数据开辟的缓冲须是模长度+1。
 *@param    ucRSAKeyIndex   密钥序号，支持共10组RSA密钥存储。序号0~9
 *@param    psDataIn        待加密数据,长度和模等长。使用BCD码存储。
 *@param    nDataLen        输入数据长度
 *@retval   psDataOut       输出数据,和模等长，使用BCD码存储。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_MACLLOC 内存空间不足
 *@li   NDK_ERR                     操作失败
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
 *@brief    获取键盘输入状态
 *@retval   psPinBlock      pinblock数据
 *@retval   nStatus         状态值
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(psPinBlock、nStatus为NULL)
 *@li   NDK_ERR_SECP_PARAM  参数非法
 *@li   NDK_ERR             操作失败
 *@li   NDK_ERR_SECVP_NOT_ACTIVE    VPP没有激活，第一次调用VPPInit
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
 *@brief    获取键盘输入状态
 *@retval   psPinBlock      pinblock数据
 *@retval   psKsn           当前DUKPT的KSN值
 *@retval   nStatus         状态值
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(psPinBlock/psKsn/nStatus为NULL)
 *@li   NDK_ERR_SECVP_NOT_ACTIVE    VPP没有激活，第一次调用VPPInit
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
 *@brief    设置密钥属主应用名称
 *@details  仅供系统应用(Keyloader)使用，通过该接口指定后续安装密钥的属主名称。
 *          当安装密钥的时候，系统安全服务将会判断调用者身份，再决定是否采用该函数设置的密钥属主名称：
 *          -针对普通用户程序：
 *              该设置无效，系统安全服务会直接指定安装密钥的属主为当前用户程序
 *          -针对系统应用程序：
 *              判断若是Keyloader系统程序，则安全服务采用NDK_SecSetKeyOwner()设置的应用名为当前安装密钥的属主，
 *                  如果Keyloader未设置过密钥属主，则默认密钥属主指定为Keyloader本身
 *              若非Keyloader系统程序，则直接以当前系统应用为密钥属主
 *@param    pszName         密钥属主应用名称(长度小于256)，若传递的是空字符串，则会清空之前设置的密钥属主
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszName为NULL或者应用名称长度大于等于256)
 *@li   NDK_ERR         操作失败
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
    inlen++;    /*字符串结束符也需发送*/
    ret = secp_methodcall(SECP_METHOD_NDK_SET_KEY_OWNER, pszName, inlen, NULL, 0, NULL);
    return ret;
}

/**
 *@brief    获取安全攻击状态
 *@retval   status          安全攻击状态参考/ref EM_SEC_TAMPER_STATUS "EM_SEC_TAMPER_STATUS"
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(status为NULL)
 *@li   NDK_ERR         操作失败
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
        //增加判断ISO9564_0和ISO9564_3需要传入主账号
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

    //长度计算=结构长度+主账号字串长度+pinblock+附加数据+预期的pin长度
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

        if (ret==SEC_VPP_NULL) { //未取到有用事件，再次进行循环
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
            memcpy(pinblock, event_buf, recvlen);//长度域在pinblock首位
            break;
        } else {
        	pm_control(0);
            if (ret<0) { //非VPP返回的正常事件，而是其他模块异常，强制终止vpp服务
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

    if (psPinBlockOut!=NULL) { //pinblock获取为阻塞模式
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
                if (ret == SEC_VPP_ENTER_KEY) {     //确认键按下，可返回数据
                    SECDEBUG( "status==SEC_VPP_KEY_ENTER\r\n");
                    ret = 0;
                    break;
                } else if (ret == SEC_VPP_CANCEL_KEY) {     //取消键按下，返回错误
                    SECDEBUG( "status==NDK_ERR_SECVP_GET_ESC\r\n");
                    ret = NDK_ERR_SECVP_GET_ESC;
                    break;
                } else if (ret == SEC_VPP_PIN_KEY || ret == SEC_VPP_BACKSPACE_KEY || ret == SEC_VPP_CLEAR_KEY) {
                    ret = 0;
                    SECDEBUG( "NDK_SecGetPinResult status=%d\r\n", ret);
                    //由于本函数在存在pinblock时，界面由状态栏处理，这里对其他键不处理
                    //继续循环等待
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

/** @} */ // 安全模块结束

/* End of this file */
