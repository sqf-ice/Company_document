/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：
* 日    期：    2012-08-27
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
*/
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>    /*PPSIX终端控制定义*/
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include "../serial/NDK_Serial.h"
#include "wirelessmodem.h"
#include "../ppp/ppp.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "../public/delay.h"
#include "../public/config.h"


#define MAX_PORT_NUM        3   ///<最大支持的端口数目
#define WLM_DEV_FILE_NAME   "/dev/wls"
#define WLM_DEFAULT_BUF_LEN 1024    /*缺省的数据缓冲长度*/
#define WLM_DEFAULT_TIMEOUT 1500    /*缺省超时时间1500ms*/

#ifdef NDK_DEBUG
#define NDK_DEBUG_WLM
#endif

#ifdef NDK_DEBUG_WLM
#define TRACE_WDEV(fmt, args...) NDK_LOG_DEBUG(NDK_LOG_MODULE_WLM,fmt, ##args)
#else
#define TRACE_WDEV(fmt, args...)
#endif

/**
*内部函数声明
*/
static int WLMWriteStatus(int StatusType);
static int WLMDrvCtrl(int nType);
static int WlmDtrCtrl(int nType);
static int PortCMGet(unsigned int *puiStatus);
static int PortCMBIC(unsigned int uiBitFlag);
static int PortCMBIS(unsigned int uiBitFlag);
static int WlmGetStatus(int nType);
int ndk_WLMGetType(void);
int ndk_WLMReadStatus(int StatusType);


/**
*内部变量
*/
int g_AuxNo = WIRELESS_AUX;
int m_AuxInitFlag = 0;
static int nPortFd = -1;
const char *szPortName[MAX_PORT_NUM] = {"/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2"};  ///<串口设备名称


/**
 *@brief    无线MODEM的硬件复位
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_OPEN_DEV        打开设备文件失败
 *@li   NDK_ERR_IOCTL       驱动调用失败
*/
NEXPORT int NDK_WlModemReset(void)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_WLM,"call %s\n",__func__);
    return WLMWriteStatus(WLM_RESET);
}


/**
 *@brief    关闭无线MODEM模块
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_OPEN_DEV        打开设备文件失败
 *@li   NDK_ERR_IOCTL       驱动调用失败
*/
NEXPORT int NDK_WlModemClose(void)
{
    return WLMWriteStatus(WLM_CLOSE);
}


/**
*@fn        NET_WLMCheckSIM(char * )
*@brief     取无线版本
*@param     pszVer 信号强度 0-100, 255 未知
*@return    @li NDK_OK              成功
            @li NET_ERROR           失败
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
static int NET_WLMCheckSIM(const char *pszPINPassWord, unsigned int uiMilTimeOut)
{
    int nRet;
    char szRespone[256];
    EM_WLM_STATUS status;
    ST_ATCMD_PACK stAtcmd;

    stAtcmd.AtCmdNo=WLM_CMD_CPIN;
    stAtcmd.pcAddParam=NULL;
    stAtcmd.pcAtCmd=NULL;
    nRet=NDK_WlSendATCmd(&stAtcmd, szRespone, sizeof(szRespone), WLM_DEFAULT_TIMEOUT, &status);
    if ((NDK_OK == nRet) && (status==WLM_STATUS_OK)) {
        if (strstr(szRespone, "READY")) {   //判断SIM卡是否准备好
            return NDK_OK;
        } else if (strstr(szRespone, "SIM PIN")) {  //处理SIM卡PIN情况
            if (pszPINPassWord == NULL) {
                return NDK_ERR_PIN_LOCKED;
            }

            //解pin
            stAtcmd.AtCmdNo=WLM_CMD_CPIN0;
            stAtcmd.pcAddParam=(char *)pszPINPassWord;
            stAtcmd.pcAtCmd=NULL;
            nRet=NDK_WlSendATCmd(&stAtcmd, szRespone, sizeof(szRespone), uiMilTimeOut, &status);
            if ((NDK_OK != nRet) || (status!=WLM_STATUS_OK)) {
                return NDK_ERR_PIN;
            }

            //再次验证
            stAtcmd.AtCmdNo=WLM_CMD_CPIN;
            stAtcmd.pcAddParam=NULL;
            stAtcmd.pcAtCmd=NULL;
            nRet=NDK_WlSendATCmd(&stAtcmd, szRespone, sizeof(szRespone), uiMilTimeOut, &status);
            if ((NDK_OK == nRet) && (status == WLM_STATUS_OK) && (strstr(szRespone, "READY"))) {
                return NDK_OK;
            }

            return NDK_ERR_PIN;
        } else {
            return NDK_ERR_PIN_UNDEFINE;
        }
    }

    return NDK_ERR_NO_SIMCARD;
}

/**
 *@brief    无线MODEM初始化，切换串口到无线并判断模块AT指令能否正常响应，检测SIM卡
 *@param    nTimeout    超时时间，单位毫秒
 *@param    pszPINPassWord  PIN码
 *@retval       pemStatus   执行成功返回无线状态，失败返回错误
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pemStatus为NULL,nTimeout超时参数非法)
 *@li   NDK_ERR_OVERFLOW    缓冲溢出
 *@li   NDK_ERR         操作失败
 *@li   NDK_ERR_TIMEOUT 超时错误
 *@li   NDK_ERR_PIN_LOCKED  SIM卡被锁定
 *@li   NDK_ERR_PIN SIM卡密码错误
 *@li   NDK_ERR_PIN_UNDEFINE    SIM卡未定义错误
 *@li   NDK_ERR_NO_SIMCARD  无SIM卡
*/
NEXPORT int NDK_WlInit(int nTimeout,const char *pszPINPassWord,EM_WLM_STATUS *pemStatus)
{
    int nRet = -1;
    char szRespone[256];
    ST_ATCMD_PACK stAtcmd;

    if ((NULL == pemStatus) || (nTimeout < 0)) {
        return NDK_ERR_PARA;
    }
    *pemStatus = -1;

    stAtcmd.AtCmdNo=WLM_CMD_E0;
    stAtcmd.pcAddParam=NULL;
    stAtcmd.pcAtCmd=NULL;
    nRet=NDK_WlSendATCmd(&stAtcmd, szRespone, sizeof(szRespone), 2000, pemStatus);
    if ((NDK_OK != nRet) || (*pemStatus!=WLM_STATUS_OK)) {
        *pemStatus = WLM_STATUS_ERROR;
        return NDK_ERR_WLM_SEND_AT_FAIL;
    }

    nRet = NET_WLMCheckSIM(pszPINPassWord, nTimeout);   /**<检测SIM卡*/
    if (nRet < 0) {
        return nRet;
    }
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_WLM,"%s succ,current wls state is %d\n",__func__,*pemStatus);
    return NDK_OK;
}


/**
 *@brief    获取无线MODEM信号强度
 *@retval   pnSq    取到的信号强度，取到的值    0-31 为成功，99 为未知,-1 为失败
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pnSq为NULL)
 *@li   NDK_ERR         操作失败
*/
NEXPORT int NDK_WlModemGetSQ(int *pnSq)
{
    int nRet = -1;
    ST_ATCMD_PACK stAtcmd;
    char szResponse[64];
    char *pszOff;
    unsigned int sq_default;
    EM_WLM_STATUS status;

    if (NULL == pnSq) {
        return NDK_ERR_PARA;
    }

    stAtcmd.AtCmdNo=WLM_CMD_CSQ;
    stAtcmd.pcAddParam=NULL;
    stAtcmd.pcAtCmd=NULL;
    nRet=NDK_WlSendATCmd(&stAtcmd, szResponse, sizeof(szResponse), WLM_DEFAULT_TIMEOUT, &status);
    if ((NDK_OK == nRet) && (status==WLM_STATUS_OK)) {
        if ((pszOff=strstr(szResponse,"SQ:"))==NULL) {
            return NDK_ERR;
        }

        *pnSq = atoi(pszOff+4);
        if (ndk_getconfig("wls", "sq_limit", CFG_INT, &sq_default)<0) {
            sq_default = 8;
        }
        /**<由于应用限制值为15，因此单小于15的有效信号值时都返回15，使得应用程序初始化通过。ym 2011-07-13*/
        if ((*pnSq >= sq_default) && (*pnSq < WLM_SQ_APP_LIMIT)) {
            *pnSq = WLM_SQ_APP_LIMIT;
            return NDK_OK;
        }/**<由于应用限制值为15，因此>=15的无效信号值时都返回14，使得应用程序初始化失败。ym 2011-07-13*/
        else if ((*pnSq < sq_default) && (*pnSq >= WLM_SQ_APP_LIMIT)) {
            *pnSq = WLM_SQ_APP_LIMIT-1;
            return NDK_OK;
        } else {
            if(pnSq!=NULL)
                NDK_COMM_LOG_INFO(NDK_LOG_MODULE_WLM,"%s succ,current wlsmodem signal is %d\n",__func__,*pnSq);
            return NDK_OK;
        }
    }

    return NDK_ERR;
}

/**
 *@brief    开启或关闭系统获取无线模块信号强度。当 nx>=0 并且 ny>=0 时候，使能系统任务栏中的无线图标获取更新当前无线信号，此外均关闭无线信号值的刷新
 *@param    nx  显示无线信号起始横坐标
 *@param    ny  显示无线信号起始纵坐标
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_WlShowSignalQuality(int nx, int ny)
{
    return NDK_OK;
}

/**
 *@brief    向无线模块发送AT指令和接收返回响应
 *@param    pstATCmdPack    AT指令数据包
 *@param    unMaxlen    缓冲最大长度（pszOutput缓冲区长度）（=0时使用缺省长度1024）
 *@param    unTimeout   命令超时时间，单位：MS
 *@retval   pszOutput   输出串
 *@retval   pemStatus   执行成功返回无线状态，失败返回 WLM_STATUS_ERROR 失败
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pemStatus、pstATCmdPack、pszOutput为NULL)
 *@li   NDK_ERR_OPEN_DEV    打开设备文件错误(设备未打开或打开失败)
 *@li   NDK_ERR_USB_LINE_UNCONNECT      USB线未插
 *@li   NDK_ERR_WRITE               写文件失败
 *@li   NDK_ERR             操作失败
 *@li   NDK_ERR_EMPTY           返回空串
 *@li   NDK_ERR_OVERFLOW    缓冲溢出
*/
NEXPORT int NDK_WlSendATCmd(const ST_ATCMD_PACK *pstATCmdPack,char *pszOutput,uint unMaxlen,uint unTimeout,EM_WLM_STATUS *pemStatus)
{
    int ret = -1;

    if ((NULL == pemStatus) || (NULL == pstATCmdPack) || (NULL == pszOutput)) {
        return NDK_ERR_PARA;
    }
    *pemStatus = -1;
		enablewirelessmodemport();
    ndk_stop_sq_flush();
    ret = ndk_NET_WLMSendAtCmd(pstATCmdPack->AtCmdNo, pstATCmdPack->pcAtCmd, pstATCmdPack->pcAddParam);
    if (ret < 0) {
        ndk_allow_sq_flush();
        return ret;
    }


    ret = ndk_NET_WLMGetAtRet(pszOutput, unMaxlen, unTimeout, 0);
    if (ret < 0) {
        *pemStatus = WLM_STATUS_ERROR;
        ndk_allow_sq_flush();
        return ret;
    } else {
        *pemStatus = ret;
        ndk_allow_sq_flush();
        return NDK_OK;
    }
}

/**
*@fn    GetReturnDigtString( const char *pszInString, char *pszOutDigtString)
*@brief     获取返回中的数字string
*@param
*@param
*@return
*@li   NDK_OK              操作成功
*@li   其它EM_NDK_ERR      操作失败

*/

int GetReturnDigtString(const char *pszInString,char *pszOutDigtString)
{
    char *Start=NULL;
    char *End =NULL;
    Start = strstr(pszInString,"\n");
    End = strstr(Start+1,"\r");
    if(Start==NULL||End==NULL) {
        return NDK_ERR_PARA;
    }
    *End = '\0';
    while(1) {
        if((*Start)>='0'&&(*Start)<='9')
            break;
        Start++;
        if(End<=Start) {
            return NDK_ERR_PARA;
        }
    }
    while(1) {
        if((*End<'0')||(*End>'9')) {
            End--;
            continue;
        }
        if(End<=Start) {
            return NDK_ERR_PARA;
        }
        End++;
        *(End)='\0';
        break;
    }
    strncpy(pszOutDigtString,Start,End - Start+1);
    return 0;
}

int ndk_GetSimIccidStr(char *pszInString,char *pszOutDigtString)
{
	int i,value;
	char *Start=NULL;
    char *End =NULL;
	int bgs2_flag = 0;
    Start = strstr(pszInString,"8986");
    if(Start == NULL){
		 Start = strstr(pszInString,"9868");
		 bgs2_flag = 1;
    }
    if(Start == NULL) {
        return NDK_ERR_PARA;
    }
	strncpy(pszOutDigtString,Start,20);
	if(bgs2_flag == 1){
		for(i=0;i<10;i++){
			value = *(pszOutDigtString+i*2);
			*(pszOutDigtString+i*2) = *(pszOutDigtString+i*2+1);
			*(pszOutDigtString+i*2+1) = value;
		}
	}
	return 0;
}

/**
*@fn        NDK_GetWlsCCID( char *pszCCid)
*@brief         获取无线CCID
*@param pszCCid，SIM卡的标识，总长20位，由0-F组成
*@param
*@return
*@li   NDK_OK              操作成功
*@li   其它EM_NDK_ERR      操作失败
*/
int ndk_WlGetCCID(char *pszCCid)
{
    int nRet;
    char szTemp[256];
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    if(pszCCid==NULL) {
        return NDK_ERR_PARA;
    }


    atm_cmd.AtCmdNo=WLM_CMD_CCID;
    atm_cmd.pcAtCmd=NULL;
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,szTemp,sizeof(szTemp),10*1000,&pemStatus);

    if(nRet==NDK_OK) {
        return ndk_GetSimIccidStr(szTemp,pszCCid);
    }
    return nRet;
}
/**
*@fn        NDK_GetWlsIMEI( char *pszValue)
*@brief         获取无线IMEI
*@param pszValue,IMEI值，总长为15位，由数字0-9组成
*@param
*@return
*@li   NDK_OK              操作成功
*@li   其它EM_NDK_ERR      操作失败

*/
int ndk_WlGetIMEI(char *pszValue)
{
    int nRet;
    char szTemp[256];
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    if(pszValue==NULL) {
        return NDK_ERR_PARA;
    }
    atm_cmd.AtCmdNo=WLM_CMD_CGSN;
    atm_cmd.pcAtCmd=NULL;
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,szTemp,sizeof(szTemp),10*1000,&pemStatus);

    if(nRet== NDK_OK) {
        return GetReturnDigtString(szTemp,pszValue);
    }
    return nRet;
}

/**
*@fn        NDK_GetWlsIMSI( char *pszValue)
*@brief         获取无线IMSI,
*@param pszValue,IMEI值，总长不超过15位，使用0-9数字
*@param
*@return
*@li   NDK_OK              操作成功
*@li   其它EM_NDK_ERR      操作失败

*/
int ndk_WlGetIMSI(char *pszValue)
{
    int nRet;
    char szTemp[256];
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    if(pszValue==NULL) {
        return NDK_ERR_PARA;
    }
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+CIMI";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,szTemp,sizeof(szTemp),10*1000,&pemStatus);
    if(nRet==NDK_OK) {
        return GetReturnDigtString(szTemp,pszValue);
    }
    return nRet;
}

/**
 *@fn       NDK_WlGetInfo(EM_WLM_TYPE_INFO emtype,char *pszValue,uint nBufLen);
 *@brief        获取无线CCID,IMSI,IMEI
 *@param        emtype      无线CCID,IMSI,IMEI的等枚举
 *@param        pszValue    无线CCID,IMSI,IMEI的信息
 *@param        nBufLen     缓冲区长度需>=21,CCID总长20位，由0-F组成,IMSI总长不超过15，由0-9组成，IMEI总长不超过15，由0-9组成.
 *@return
 *@li   NDK_OK             操作成功
 *@li   NDK_ERR_PARA       参数非法(emType非法、pszValue为NULL)
 *@li   NDK_ERR            操作失败
*/
NEXPORT int NDK_WlGetInfo(EM_WLM_TYPE_INFO emType,char *pszValue,uint unBufLen)
{
    int nRet;
    if((emType<0)||(emType>=WLM_INFO_UNDEFINE)||(pszValue==NULL))
        return NDK_ERR_PARA;

    switch(emType) {
        case WLM_INFO_CCID:
            if (unBufLen<21)
                return NDK_ERR_PARA;
            nRet=ndk_WlGetCCID(pszValue);
            break;
        case WLM_INFO_IMSI:
            if (unBufLen<16)
                return NDK_ERR_PARA;
            nRet=ndk_WlGetIMSI(pszValue);
            break;
        case WLM_INFO_IMEI:
            if (unBufLen<16)
                return NDK_ERR_PARA;
            nRet=ndk_WlGetIMEI(pszValue);
            break;
        case WLM_INFO_UNDEFINE:

            return NDK_ERR_PARA;

            break;
        default:
            return NDK_ERR;

    }

    return nRet;
}


/**
 *@brief    关闭射频(暂未支持)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_WlCloseRF(void)
{
    return NDK_ERR;
}


/**
 *@brief    选择SIM卡(暂未支持)
 *@param    ucSimNo SIM卡号
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_WlSelSIM(uchar ucSimNo)
{
    return NDK_ERR;
}

/************************************************************************/


/**
*@fn        :WLMWriteStatus()
*@brief         :写状态
*@param in  :[StatusType] 状态类型
*@return
*/
int WLMWriteStatus(int StatusType)
{
    int nRet=-1;

    switch (StatusType) {
        case WLM_OPEN:      /*打开无线模块*/
            nRet = WLMDrvCtrl(WLM_IOCG_OPEN);
			#if 0
            if(access("/tmp/gsm_tmp",F_OK)==0) {
                ndk_enableWirelessMux();
                m_AuxInitFlag=0;
            }
			#endif
			ndk_InitWirelessAux();
			m_AuxInitFlag=0;
            break;

        case WLM_CLOSE:     /*关闭无线模块*/
            ndk_closeWirelessAux();
            nRet = WLMDrvCtrl(WLM_IOCG_CLOSE);
            break;

        case WLM_HOTRESET:  /*软复位*/
            nRet = WLMDrvCtrl(WLM_IOCG_HOT_RST);
            break;

        case WLM_RESET:     /*硬复位*/
            ndk_closeWirelessAux();
            nRet = WLMDrvCtrl(WLM_IOCG_RST);
            ndk_InitWirelessAux();
			m_AuxInitFlag=0;
            break;

        case WLM_DTR_SET:   /*置位DRT管脚*/
            nRet = WlmDtrCtrl(1);
            break;

        case WLM_DTR_CLR:   /*清DRT管脚*/
            nRet = WlmDtrCtrl(0);
            break;

        case WLM_RTS_SET:   /*置位RTS管脚*/
            nRet = PortCMBIC(TIOCM_RTS);
            break;

        case WLM_RTS_CLR:   /*清RTS管脚*/
            nRet = PortCMBIS(TIOCM_RTS);
            break;

        default:
            break;
    }

    return nRet;
}



/**
*@fn        :WLMReadStatus()
*@brief         :状态
*@param in  :[StatusType] 状态类型
*@return
*/
int ndk_WLMReadStatus(int StatusType)
{
    int nRet=-1;
    unsigned int nTmp = 0;
    EM_PPP_STATUS nPPPStatus = -1;


    switch (StatusType) {
        case WLM_STATUS:
            nRet = WlmGetStatus(WLM_IOCG_GET_OPEN);
            break;

        case WLM_DSRSTATUS:
            nRet = PortCMGet(&nTmp);
            if (0 == nRet) {
                nRet = (nTmp&TIOCM_DSR)?1:0;
            }
            break;

        case WLM_CTSSTATUS:
            nRet = PortCMGet(&nTmp);
            if (0 == nRet) {
                nRet = (nTmp&TIOCM_CTS)?1:0;
            }
            break;

        case WLM_DCDSTATUS:
            nRet = WlmGetStatus(WLM_IOCG_GET_DCD);
            break;

        case WLM_DATASTATUS:
            if (ndk_NET_PPPGetStatus(&nPPPStatus) != 0) {
                nRet = -1;
            } else {
                if (PPP_STATUS_DISCONNECT == nPPPStatus) {
                    nRet = 0;
                } else {
                    nRet = 1;
                }
            }
            break;

        default:
            TRACE_WDEV("param err!");
            break;
    }

    return nRet;
}


/**
*@fn        :WLMDrvCtrl()
*@brief         :驱动控制接口
*@param in  :[nType] 控制类型
*@return
*@li        NDK_OK = 0 --- 成功
*@li        NDK_ERR = -1 --- 失败
*/
static int WLMDrvCtrl(int nType)
{
    int fd = -1;

    if ((fd=open(WLM_DEV_FILE_NAME, O_RDWR)) < 0) {
        TRACE_WDEV("open file err!");
        return NDK_ERR_OPEN_DEV;
    }

    while (1) { //判断在驱动中是否有工作队列
        if (ioctl(fd, WLM_IOCG_GET_WORK)) {
            break;
        }
        usleep(100*1000);
    }

    if (ioctl(fd, nType) < 0) {
        TRACE_WDEV("iocg type=%d err!", nType);
        close(fd);
        return NDK_ERR_IOCTL;
    }

    while (1) {
        if (ioctl(fd, WLM_IOCG_GET_WORK)) {
            break;
        }
        usleep(100*1000);
    }

    close(fd);

    if ((WLM_IOCG_HOT_RST == nType) || (WLM_IOCG_RST == nType) || (WLM_IOCG_OPEN == nType)) {
        ndk_write_rst_flag();
    }

    return NDK_OK;
}


/**
*@fn        :WlmDtrCtrl()
*@brief         :DTR控制
*@param in  :[nType] :1 DTR拉高; 0 DTR拉低
*@return
*@li        NDK_OK = 0 --- 成功
*@li        NDK_ERR = -1 --- 失败
*/
static int WlmDtrCtrl(int nType)
{
    int fd = -1;
    int ret = -1;

    if ((fd=open(WLM_DEV_FILE_NAME, O_RDWR)) < 0) {
        TRACE_WDEV("open file err!");
        return NDK_ERR_OPEN_DEV;
    }

    while (1) { //判断在驱动中是否有工作队列
        if (ioctl(fd, WLM_IOCG_GET_WORK)) {
            break;
        }
        ndk_msdelay(100);
    }

    ret = ioctl(fd, WLM_IOCG_CTL_DTR, &nType);
    close(fd);

    return ret;
}


/**
*@fn        :PortCMBIS()
*@brief         :设置串口某位控制信号
*@param in  :[uiBitFlag] 端口控制信号
*@return
*@li        NDK_OK = 0 --- 成功
*@li        NDK_ERR = -1 --- 失败
*/
static int PortCMBIS(unsigned int uiBitFlag)
{
    if (nPortFd < 0) {
        if ((nPortFd = open(szPortName[g_AuxNo], O_RDWR)) < 0) {
            return NDK_ERR_OPEN_DEV;
        }
    }

    if (ioctl(nPortFd, TIOCMBIS, &uiBitFlag) < 0) {
        return NDK_ERR;
    }

    return NDK_OK;
}


/**
*@fn        :PortCMBIC()
*@brief         :清除串口某位控制信号
*@param in  :[uiBitFlag] 端口控制信号
*@return
*@li        NDK_OK = 0 --- 成功
*@li        NDK_ERR = -1 --- 失败
*/
static int PortCMBIC(unsigned int uiBitFlag)
{
    if (nPortFd < 0) {
        if ((nPortFd = open(szPortName[g_AuxNo], O_RDWR)) < 0) {
            return NDK_ERR_OPEN_DEV;
        }
    }

    if (ioctl(nPortFd, TIOCMBIC, &uiBitFlag) < 0) {
        return NDK_ERR;
    }

    return NDK_OK;
}


/**
*@fn        :PortCMGet()
*@brief         :获取串口控制信号
*@param in  :[uiBitFlag] 端口控制信号
*@return
*@li        NDK_OK = 0 --- 成功
*@li        NDK_ERR = -1 --- 失败
*/
static int PortCMGet(unsigned int *puiStatus)
{
    if (nPortFd < 0) {
        if ((nPortFd = open(szPortName[g_AuxNo], O_RDWR)) < 0) {
            return NDK_ERR_OPEN_DEV;
        }
    }

    if (ioctl(nPortFd, TIOCMGET, puiStatus) < 0) {
        return NDK_ERR;
    }

    return NDK_OK;
}


/**
*@fn        :WlmGetStatus()
*@brief         :获取模块状态
*@return
*@li        无线模块类型
*/
static int WlmGetStatus(int nType)
{
    int fd;
    int type;

    if ((fd=open(WLM_DEV_FILE_NAME, O_RDWR)) < 0) {
        TRACE_WDEV("WlmGetStatus-->open file err!");
        return NDK_ERR_OPEN_DEV;
    }

    while (1) { //判断在驱动中是否有工作队列
        if (ioctl(fd, WLM_IOCG_GET_WORK)) {
            break;
        }
        ndk_msdelay(100);
    }

    type = ioctl(fd, nType);
    close(fd);

    if (type < 0) {
        return WLM_NONE;
    }
    return type;
}


/**
*@fn        :WLMGetType()
*@brief         :获取无线模块类型
*@return
*@li        无线模块类型
*/
int ndk_WLMGetType(void)
{
    return WlmGetStatus(WLM_IOCG_GET_TYPE);
}


/**
*@brief     初始化串口
*@param     无
*@return    无
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
int ndk_InitAux(void)
{
    int wls_bps = 0;

    if (!m_AuxInitFlag) {

        ndk_getconfig("wls", "bps", CFG_INT, &wls_bps);
        if(wls_bps == 1) {
            TRACE_WDEV("init wlm aux 230400");
            //  g_AuxNo = PORT_NUM_MUX1;
            g_AuxNo = WIRELESS_AUX;
            if (NDK_PortOpen(g_AuxNo, "230400")<0) {
                usleep(1000);
                if (NDK_PortOpen(g_AuxNo, "230400")<0) { //初始化错误后再初始化一次，如果仍然错误，则返回 yanm 20101119
                    return -1;
                }
            }
        } else {
            TRACE_WDEV("init wlm aux 115200");
            g_AuxNo = WIRELESS_AUX;
            //为防止多进程同时初始化tty设备时，可能导致进程初始化错误，增加判断串口初始化是否成功 yanm 20101119
            if (NDK_PortOpen(g_AuxNo, "115200")<0) {
                usleep(1000);
                if (NDK_PortOpen(g_AuxNo, "115200")<0) { //初始化错误后再初始化一次，如果仍然错误，则返回 yanm 20101119
                    return -1;
                }
            }
        }

        m_AuxInitFlag=1;
    }

    return 0;
}
