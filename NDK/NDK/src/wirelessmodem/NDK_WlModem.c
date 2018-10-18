/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�
* ��    �ڣ�    2012-08-27
* ��    ����    V1.00
* ����޸��ˣ�
* ����޸����ڣ�
*/
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>    /*PPSIX�ն˿��ƶ���*/
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


#define MAX_PORT_NUM        3   ///<���֧�ֵĶ˿���Ŀ
#define WLM_DEV_FILE_NAME   "/dev/wls"
#define WLM_DEFAULT_BUF_LEN 1024    /*ȱʡ�����ݻ��峤��*/
#define WLM_DEFAULT_TIMEOUT 1500    /*ȱʡ��ʱʱ��1500ms*/

#ifdef NDK_DEBUG
#define NDK_DEBUG_WLM
#endif

#ifdef NDK_DEBUG_WLM
#define TRACE_WDEV(fmt, args...) NDK_LOG_DEBUG(NDK_LOG_MODULE_WLM,fmt, ##args)
#else
#define TRACE_WDEV(fmt, args...)
#endif

/**
*�ڲ���������
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
*�ڲ�����
*/
int g_AuxNo = WIRELESS_AUX;
int m_AuxInitFlag = 0;
static int nPortFd = -1;
const char *szPortName[MAX_PORT_NUM] = {"/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2"};  ///<�����豸����


/**
 *@brief    ����MODEM��Ӳ����λ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_OPEN_DEV        ���豸�ļ�ʧ��
 *@li   NDK_ERR_IOCTL       ��������ʧ��
*/
NEXPORT int NDK_WlModemReset(void)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_WLM,"call %s\n",__func__);
    return WLMWriteStatus(WLM_RESET);
}


/**
 *@brief    �ر�����MODEMģ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_OPEN_DEV        ���豸�ļ�ʧ��
 *@li   NDK_ERR_IOCTL       ��������ʧ��
*/
NEXPORT int NDK_WlModemClose(void)
{
    return WLMWriteStatus(WLM_CLOSE);
}


/**
*@fn        NET_WLMCheckSIM(char * )
*@brief     ȡ���߰汾
*@param     pszVer �ź�ǿ�� 0-100, 255 δ֪
*@return    @li NDK_OK              �ɹ�
            @li NET_ERROR           ʧ��
*@section   history     �޸���ʷ
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
        if (strstr(szRespone, "READY")) {   //�ж�SIM���Ƿ�׼����
            return NDK_OK;
        } else if (strstr(szRespone, "SIM PIN")) {  //����SIM��PIN���
            if (pszPINPassWord == NULL) {
                return NDK_ERR_PIN_LOCKED;
            }

            //��pin
            stAtcmd.AtCmdNo=WLM_CMD_CPIN0;
            stAtcmd.pcAddParam=(char *)pszPINPassWord;
            stAtcmd.pcAtCmd=NULL;
            nRet=NDK_WlSendATCmd(&stAtcmd, szRespone, sizeof(szRespone), uiMilTimeOut, &status);
            if ((NDK_OK != nRet) || (status!=WLM_STATUS_OK)) {
                return NDK_ERR_PIN;
            }

            //�ٴ���֤
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
 *@brief    ����MODEM��ʼ�����л����ڵ����߲��ж�ģ��ATָ���ܷ�������Ӧ�����SIM��
 *@param    nTimeout    ��ʱʱ�䣬��λ����
 *@param    pszPINPassWord  PIN��
 *@retval       pemStatus   ִ�гɹ���������״̬��ʧ�ܷ��ش���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pemStatusΪNULL,nTimeout��ʱ�����Ƿ�)
 *@li   NDK_ERR_OVERFLOW    �������
 *@li   NDK_ERR         ����ʧ��
 *@li   NDK_ERR_TIMEOUT ��ʱ����
 *@li   NDK_ERR_PIN_LOCKED  SIM��������
 *@li   NDK_ERR_PIN SIM���������
 *@li   NDK_ERR_PIN_UNDEFINE    SIM��δ�������
 *@li   NDK_ERR_NO_SIMCARD  ��SIM��
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

    nRet = NET_WLMCheckSIM(pszPINPassWord, nTimeout);   /**<���SIM��*/
    if (nRet < 0) {
        return nRet;
    }
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_WLM,"%s succ,current wls state is %d\n",__func__,*pemStatus);
    return NDK_OK;
}


/**
 *@brief    ��ȡ����MODEM�ź�ǿ��
 *@retval   pnSq    ȡ�����ź�ǿ�ȣ�ȡ����ֵ    0-31 Ϊ�ɹ���99 Ϊδ֪,-1 Ϊʧ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pnSqΪNULL)
 *@li   NDK_ERR         ����ʧ��
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
        /**<����Ӧ������ֵΪ15����˵�С��15����Ч�ź�ֵʱ������15��ʹ��Ӧ�ó����ʼ��ͨ����ym 2011-07-13*/
        if ((*pnSq >= sq_default) && (*pnSq < WLM_SQ_APP_LIMIT)) {
            *pnSq = WLM_SQ_APP_LIMIT;
            return NDK_OK;
        }/**<����Ӧ������ֵΪ15�����>=15����Ч�ź�ֵʱ������14��ʹ��Ӧ�ó����ʼ��ʧ�ܡ�ym 2011-07-13*/
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
 *@brief    ������ر�ϵͳ��ȡ����ģ���ź�ǿ�ȡ��� nx>=0 ���� ny>=0 ʱ��ʹ��ϵͳ�������е�����ͼ���ȡ���µ�ǰ�����źţ�������ر������ź�ֵ��ˢ��
 *@param    nx  ��ʾ�����ź���ʼ������
 *@param    ny  ��ʾ�����ź���ʼ������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERRCODE      ����ʧ��
*/
NEXPORT int NDK_WlShowSignalQuality(int nx, int ny)
{
    return NDK_OK;
}

/**
 *@brief    ������ģ�鷢��ATָ��ͽ��շ�����Ӧ
 *@param    pstATCmdPack    ATָ�����ݰ�
 *@param    unMaxlen    ������󳤶ȣ�pszOutput���������ȣ���=0ʱʹ��ȱʡ����1024��
 *@param    unTimeout   ���ʱʱ�䣬��λ��MS
 *@retval   pszOutput   �����
 *@retval   pemStatus   ִ�гɹ���������״̬��ʧ�ܷ��� WLM_STATUS_ERROR ʧ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pemStatus��pstATCmdPack��pszOutputΪNULL)
 *@li   NDK_ERR_OPEN_DEV    ���豸�ļ�����(�豸δ�򿪻��ʧ��)
 *@li   NDK_ERR_USB_LINE_UNCONNECT      USB��δ��
 *@li   NDK_ERR_WRITE               д�ļ�ʧ��
 *@li   NDK_ERR             ����ʧ��
 *@li   NDK_ERR_EMPTY           ���ؿմ�
 *@li   NDK_ERR_OVERFLOW    �������
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
*@brief     ��ȡ�����е�����string
*@param
*@param
*@return
*@li   NDK_OK              �����ɹ�
*@li   ����EM_NDK_ERR      ����ʧ��

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
*@brief         ��ȡ����CCID
*@param pszCCid��SIM���ı�ʶ���ܳ�20λ����0-F���
*@param
*@return
*@li   NDK_OK              �����ɹ�
*@li   ����EM_NDK_ERR      ����ʧ��
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
*@brief         ��ȡ����IMEI
*@param pszValue,IMEIֵ���ܳ�Ϊ15λ��������0-9���
*@param
*@return
*@li   NDK_OK              �����ɹ�
*@li   ����EM_NDK_ERR      ����ʧ��

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
*@brief         ��ȡ����IMSI,
*@param pszValue,IMEIֵ���ܳ�������15λ��ʹ��0-9����
*@param
*@return
*@li   NDK_OK              �����ɹ�
*@li   ����EM_NDK_ERR      ����ʧ��

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
 *@brief        ��ȡ����CCID,IMSI,IMEI
 *@param        emtype      ����CCID,IMSI,IMEI�ĵ�ö��
 *@param        pszValue    ����CCID,IMSI,IMEI����Ϣ
 *@param        nBufLen     ������������>=21,CCID�ܳ�20λ����0-F���,IMSI�ܳ�������15����0-9��ɣ�IMEI�ܳ�������15����0-9���.
 *@return
 *@li   NDK_OK             �����ɹ�
 *@li   NDK_ERR_PARA       �����Ƿ�(emType�Ƿ���pszValueΪNULL)
 *@li   NDK_ERR            ����ʧ��
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
 *@brief    �ر���Ƶ(��δ֧��)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERRCODE      ����ʧ��
*/
NEXPORT int NDK_WlCloseRF(void)
{
    return NDK_ERR;
}


/**
 *@brief    ѡ��SIM��(��δ֧��)
 *@param    ucSimNo SIM����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERRCODE      ����ʧ��
*/
NEXPORT int NDK_WlSelSIM(uchar ucSimNo)
{
    return NDK_ERR;
}

/************************************************************************/


/**
*@fn        :WLMWriteStatus()
*@brief         :д״̬
*@param in  :[StatusType] ״̬����
*@return
*/
int WLMWriteStatus(int StatusType)
{
    int nRet=-1;

    switch (StatusType) {
        case WLM_OPEN:      /*������ģ��*/
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

        case WLM_CLOSE:     /*�ر�����ģ��*/
            ndk_closeWirelessAux();
            nRet = WLMDrvCtrl(WLM_IOCG_CLOSE);
            break;

        case WLM_HOTRESET:  /*��λ*/
            nRet = WLMDrvCtrl(WLM_IOCG_HOT_RST);
            break;

        case WLM_RESET:     /*Ӳ��λ*/
            ndk_closeWirelessAux();
            nRet = WLMDrvCtrl(WLM_IOCG_RST);
            ndk_InitWirelessAux();
			m_AuxInitFlag=0;
            break;

        case WLM_DTR_SET:   /*��λDRT�ܽ�*/
            nRet = WlmDtrCtrl(1);
            break;

        case WLM_DTR_CLR:   /*��DRT�ܽ�*/
            nRet = WlmDtrCtrl(0);
            break;

        case WLM_RTS_SET:   /*��λRTS�ܽ�*/
            nRet = PortCMBIC(TIOCM_RTS);
            break;

        case WLM_RTS_CLR:   /*��RTS�ܽ�*/
            nRet = PortCMBIS(TIOCM_RTS);
            break;

        default:
            break;
    }

    return nRet;
}



/**
*@fn        :WLMReadStatus()
*@brief         :״̬
*@param in  :[StatusType] ״̬����
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
*@brief         :�������ƽӿ�
*@param in  :[nType] ��������
*@return
*@li        NDK_OK = 0 --- �ɹ�
*@li        NDK_ERR = -1 --- ʧ��
*/
static int WLMDrvCtrl(int nType)
{
    int fd = -1;

    if ((fd=open(WLM_DEV_FILE_NAME, O_RDWR)) < 0) {
        TRACE_WDEV("open file err!");
        return NDK_ERR_OPEN_DEV;
    }

    while (1) { //�ж����������Ƿ��й�������
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
*@brief         :DTR����
*@param in  :[nType] :1 DTR����; 0 DTR����
*@return
*@li        NDK_OK = 0 --- �ɹ�
*@li        NDK_ERR = -1 --- ʧ��
*/
static int WlmDtrCtrl(int nType)
{
    int fd = -1;
    int ret = -1;

    if ((fd=open(WLM_DEV_FILE_NAME, O_RDWR)) < 0) {
        TRACE_WDEV("open file err!");
        return NDK_ERR_OPEN_DEV;
    }

    while (1) { //�ж����������Ƿ��й�������
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
*@brief         :���ô���ĳλ�����ź�
*@param in  :[uiBitFlag] �˿ڿ����ź�
*@return
*@li        NDK_OK = 0 --- �ɹ�
*@li        NDK_ERR = -1 --- ʧ��
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
*@brief         :�������ĳλ�����ź�
*@param in  :[uiBitFlag] �˿ڿ����ź�
*@return
*@li        NDK_OK = 0 --- �ɹ�
*@li        NDK_ERR = -1 --- ʧ��
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
*@brief         :��ȡ���ڿ����ź�
*@param in  :[uiBitFlag] �˿ڿ����ź�
*@return
*@li        NDK_OK = 0 --- �ɹ�
*@li        NDK_ERR = -1 --- ʧ��
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
*@brief         :��ȡģ��״̬
*@return
*@li        ����ģ������
*/
static int WlmGetStatus(int nType)
{
    int fd;
    int type;

    if ((fd=open(WLM_DEV_FILE_NAME, O_RDWR)) < 0) {
        TRACE_WDEV("WlmGetStatus-->open file err!");
        return NDK_ERR_OPEN_DEV;
    }

    while (1) { //�ж����������Ƿ��й�������
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
*@brief         :��ȡ����ģ������
*@return
*@li        ����ģ������
*/
int ndk_WLMGetType(void)
{
    return WlmGetStatus(WLM_IOCG_GET_TYPE);
}


/**
*@brief     ��ʼ������
*@param     ��
*@return    ��
*@section   history     �޸���ʷ
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
                if (NDK_PortOpen(g_AuxNo, "230400")<0) { //��ʼ��������ٳ�ʼ��һ�Σ������Ȼ�����򷵻� yanm 20101119
                    return -1;
                }
            }
        } else {
            TRACE_WDEV("init wlm aux 115200");
            g_AuxNo = WIRELESS_AUX;
            //Ϊ��ֹ�����ͬʱ��ʼ��tty�豸ʱ�����ܵ��½��̳�ʼ�����������жϴ��ڳ�ʼ���Ƿ�ɹ� yanm 20101119
            if (NDK_PortOpen(g_AuxNo, "115200")<0) {
                usleep(1000);
                if (NDK_PortOpen(g_AuxNo, "115200")<0) { //��ʼ��������ٳ�ʼ��һ�Σ������Ȼ�����򷵻� yanm 20101119
                    return -1;
                }
            }
        }

        m_AuxInitFlag=1;
    }

    return 0;
}
