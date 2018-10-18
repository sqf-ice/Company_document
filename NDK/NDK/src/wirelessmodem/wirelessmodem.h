/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：	产品开发部
* 日    期：	2012-08-17
* 版	本：	V1.00
* 最后修改人：csl
* 最后修改日期：
*/
#ifndef _WLMODEM_H_H_
#define _WLMODEM_H_H_

#include "NDK_WlModem.h"

#define TIOCM_LE	0x001
#define TIOCM_DTR	0x002
#define TIOCM_RTS	0x004
#define TIOCM_ST	0x008
#define TIOCM_SR	0x010
#define TIOCM_CTS	0x020
#define TIOCM_CAR	0x040
#define TIOCM_RNG	0x080
#define TIOCM_DSR	0x100
#define TIOCM_CD	TIOCM_CAR
#define TIOCM_RI	TIOCM_RNG
#define TIOCM_OUT1	0x2000
#define TIOCM_OUT2	0x4000
#define TIOCM_LOOP	0x8000

#define WIRELESS_AUX	2				///<无线模块连接串口
#define WIRELESS_MUX1   3
#define WIRELESS_MUX2   4
#define WIRELESS_MUXE   5

#define WIRELESS_BPS115200 0
#define WIRELESS_BPS230400 1

#define WLSMDM_DEV_MAGIC    'W'
#define WLM_IOCG_OPEN			_IO(WLSMDM_DEV_MAGIC, 0)
#define WLM_IOCG_CLOSE  		_IO(WLSMDM_DEV_MAGIC, 1)
#define WLM_IOCG_RST			_IO(WLSMDM_DEV_MAGIC, 2)
#define WLM_IOCG_HOT_RST		_IO(WLSMDM_DEV_MAGIC, 3)
#define WLM_IOCG_GET_TYPE		_IO(WLSMDM_DEV_MAGIC, 4)
#define WLM_IOCG_GET_OPEN		_IO(WLSMDM_DEV_MAGIC, 5)
#define WLM_IOCG_GET_WORK		_IO(WLSMDM_DEV_MAGIC, 6)
#define WLM_IOCG_CTL_DTR		_IOW(WLSMDM_DEV_MAGIC, 7, int)
#define WLM_IOCG_GET_DCD		_IO(WLSMDM_DEV_MAGIC, 8)

#define WLM_LINE_MODE		1			///<函数以行模式返回
#define WLM_STRING_MODE		0			///<函数将收到的所有字符串返回

#define WLM_SQ_LIMIT_DEFAULT	8			//信号值缺省判断
#define WLM_SQ_APP_LIMIT		15			//应用判断的限制值

enum stWLMREADSTATUS {
    WLM_STATUS,     /*模块状态，例如打开还是关闭的*/
    WLM_DSRSTATUS, /*模块DSR管脚状态*/
    WLM_CTSSTATUS, /*模块CTS管脚状态*/
    WLM_DCDSTATUS, /*模块DCD管脚状态*/
    WLM_DATASTATUS /*模块数据通讯状态*/
};

//无线模块类型
enum WLMTYPE {
    WLM_NONE=0x00,
    WLM_CDMA_XXXX=0x80,
    WLM_GPRS_MC39I=0x02,
    WLM_GPRS_SM300=0x03,
    WLM_GPRS_SM700=0x05,
    WLM_CDMA_DTGS800=0x81,
    WLM_CDMA_DTM228C=0x82,
    WLM_CDMA_MC8331=0x83,
    WLM_CDMA_EM200=0x84,
    WLM_CDMA_CE910=0x85,
    WLM_CDMA_MC8618=0x86,
	WLM_GPRS_M72=0x06,
	WLM_GPRS_BGS2=0x07,
    WLM_GPRS_G610=0x08,
    WLM_GPRS_QW200=0x0a,
    WLM_GPRS_SIM800C=0x0b,
    WLM_GPRS_BGS1=0x0c,
    WLM_WCDMA_H350=0x41,
    WLM_WCDMA_EHS5_USB=0x42,
    WLM_WCDMA_EHS5_SERIAL=0x43,
	WLM_EVDO_DE910=0xc1,
	WLM_EVDO_DE910_USB=0xc2
};


/**
*无线AT命令映射
*/
typedef struct {
    EM_WLM_CMD WLMAtCmdNo;			///<At命令编号
    char *pszAtCmdStr;					///<At命令编号对应的命令串
} WLMATCMDMAP;

/**
*无线应答
*/
typedef struct {
    EM_WLM_STATUS WLMRespone;				///<返回回响应类型
    char *pszWlmRespone;				///<模块返回串
    char *pszPubilcRespone;				///<输出到应用程序串
} WLMRESPTABLE;

typedef struct {
    int cmd;
    WLMRESPTABLE *pRespTable;
} WLMCMDRESPTABLE;

/**
 *	AT命令转换
 */
typedef struct {
    int nWLMType;
    WLMATCMDMAP *pWlmAtCmdMap;
    WLMCMDRESPTABLE *pWlmRespTable;
} WLMATDrv;

enum stWLMWRITESTATUS {
    WLM_OPEN,       /*打开无线模块*/
    WLM_CLOSE,		/*关闭无线模块*/
    WLM_HOTRESET,	/*软复位*/
    WLM_RESET,		/*硬复位*/
    WLM_DTR_SET,	/*置位DRT管脚*/
    WLM_DTR_CLR,	/*清DRT管脚*/
    WLM_RTS_SET,	/*置位RTS管脚*/
    WLM_RTS_CLR		/*清RTS管脚*/
};


int ndk_NET_WLMSendAtCmd(EM_WLM_CMD emAtCmdNo, const char *pszAtCmd, const char *pszInputParam);
int ndk_NET_WLMGetAtRet(char *pszOutput, unsigned int nMaxlen, unsigned int nMilTimeOut, int iLineMode);


#endif
/* End of this file */

