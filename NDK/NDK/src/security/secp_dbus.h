#ifndef _KM_DBUS_H_
#define _KM_DBUS_H_

#include <dbus/dbus-glib.h>

//#define PHOENIX_DBUS_SERVER

#define PHOENIX_DBUS_NAME		"nl.phoenix.secpserver"
#define PHOENIX_DBUS_OBJECT		"/OBJECT"
#define PHOENIX_DBUS_INTERFACE	"nl.secp.interface"

typedef enum{
	SECP_METHOD_NDK_GET_RNG,
	SECP_METHOD_NDK_GET_KCV,
	SECP_METHOD_NDK_KEY_ERASE,
	SECP_METHOD_NDK_LOAD_KEY,
	SECP_METHOD_NDK_SET_INTERVALTIME,
	SECP_METHOD_NDK_SET_FUNCTIONKEY,
	SECP_METHOD_NDK_CALC_MAC,
	SECP_METHOD_NDK_GET_PIN,
	SECP_METHOD_NDK_CALC_DES,
	SECP_METHOD_VPP_INIT,
	SECP_METHOD_VPP_EVENT,
	SECP_METHOD_NDK_SET_KEY_OWNER,
	SECP_METHOD_NDK_LOAD_TIK,
	SECP_METHOD_NDK_GET_DUKPT_KSN,
	SECP_METHOD_NDK_INCREASE_DUKPT_KSN,
	SECP_METHOD_NDK_GET_PIN_DUKPT,
	SECP_METHOD_NDK_CALC_MAC_DUKPT,
	SECP_METHOD_NDK_CALC_DES_DUKPT,
	SECP_METHOD_NDK_GET_TAMPER_STATUS,
	SECP_METHOD_NDK_LOAD_RSA_KEY,
	SECP_METHOD_NDK_RSA_RECOVER,
	SECP_METHOD_NDK_SEC_GET_CFG,
	SECP_METHOD_NDK_SEC_SET_CFG,
	SECP_METHOD_NDK_KEY_DELETE,
	SECP_METHOD_NDK_SET_RTC_TIME,
}EM_SECP_METHOD;

#ifdef PHOENIX_DBUS_SERVER
typedef struct SecpObject
{
  GObject parent;
}SecpObject;

typedef struct SecpObjectClass
{
  GObjectClass parent;
}SecpObjectClass;
#endif

typedef struct{
	unsigned char keytype;
	unsigned char keyidx;
	ST_SEC_KCV_INFO stKcvInfo;
}ST_SECP_NDK_GETKCV_IN;

typedef struct{
    ST_SEC_KEY_INFO stSeckeyInfo;
    ST_SEC_KCV_INFO stSecKcvInfo;
    unsigned char extend_keyblock[0];	/*用于扩展TR31-BLOCK安装包数据*/
}ST_SECP_NDK_LOADKEY_IN;

typedef struct {
    unsigned char ucKeyIdx;
    unsigned char ucMod;
    unsigned char *pSDataIn;
    int nDataInLen;
}ST_SECP_NDK_GETMAC_IN;

typedef struct {
    unsigned char ucKeyIdx;
    unsigned char ucKeyType;
    unsigned char ucMod;
    unsigned char *pSDataIn;
    int nDataInLen;
}ST_SECP_NDK_GETDES_IN;

typedef struct{
	int    nKeyLen;
	uchar  sKeyValue[16];
	uchar  sKsn[10];
	uchar  ucGroupIdx;
	uchar  ucKekIdx;
    ST_SEC_KCV_INFO stSecKcvInfo;
    unsigned char extend_keyblock[0];	/*用于扩展TR31-BLOCK安装包数据*/
}ST_SECP_NDK_LOADTIK_IN;

typedef struct{
	char srcName[32];
	char srvVersion[32];
}ST_SECP_SDK_GET_SETVICEHANDLE_IN;

typedef struct{
	int algId;
	unsigned char *pData;
	int sizeData;
	unsigned char *pWK;
	int nWK_Len;
	unsigned char *pIVector;
	int sizeIVlen;
	int lastBlockFlag;
	int flags;
	int nKeyID;
}ST_SECP_SDK_CALC_MAC_IN;

typedef struct{
    sec_vpp_data stSecVppData;
    int nVppType;
    char *pszExpPinLenIn;
}ST_SECP_NDK_PININIT_IN;

typedef struct {
    int nDataInLen;
    unsigned char *pSDataIn;
    unsigned char ucIV[8];
    unsigned char ucKeyIdx;
    unsigned char ucKeyType;
    unsigned char ucMod;
    unsigned char reserved;
}ST_SECP_NDK_DUKPT_GETDES_IN;

typedef struct{
	uchar mac[8];
	uchar ksn[10];
}ST_SECP_NDK_DUKPT_MAC_OUT;

typedef struct{
	uchar pin[8];
	uchar ksn[10];
}ST_SECP_NDK_DUKPT_PIN_OUT;

typedef struct{
	uchar ucRSAKeyIndex;
	ST_SEC_RSA_KEY stSecRsaKey;
}ST_SECP_NDK_LOADRSAKEY_IN;

typedef struct{
	uchar ucRSAKeyIndex;
	int  nDataLen;
}ST_SECP_NDK_RSARECOVER_IN;

#ifndef PHOENIX_DBUS_SERVER
int secp_methodcall(int method_id, void *arg, int arglen, void *out, int maxoutlen, int *outlen);
#endif

#endif
