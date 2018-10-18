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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>

#include "ppp.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "../public/delay.h"


/**
 *@brief    ����PPP����
 *@param    pstPPPCfg   ppp�����ṹ(�豸��������ȱʡʱΪ����)
 *@param    nValidLen   ����pstPPPCfg����Ч����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pstPPPCfgΪNULL)
 *@li   NDK_ERR_PPP_DEVICE      PPP��Ч�豸
 *@li   NDK_ERR_SHM     �����ڴ����
 *@li   NDK_ERR_PPP_PARAM       PPP��������
*/
NEXPORT int NDK_PppSetCfg(ST_PPP_CFG *pstPPPCfg, uint nuValidLen)
{
    int nRet = -1;

    if (NULL == pstPPPCfg) {
        return NDK_ERR_PARA;
    }

    if ((nRet = ndk_NET_PPPSetParam(pstPPPCfg, nuValidLen)) < 0) {
        return nRet;
    }

    return NDK_OK;
}


/**
 *@brief    PPP���ţ���sim������
 *@param    pszUsername �û���
 *@param    pszPassword ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszUsername/pszUsernameΪNULL)
 *@li   NDK_ERR_PPP_OPEN    PPP�Ѵ�
 *@li   NDK_ERR_SHM     �����ڴ����
 *@li   NDK_ERR_PPP_DEVICE      PPP��Ч�豸
*/
NEXPORT int NDK_PppDial(const char *pszUsername,const char *pszPassword)
{
    int nRet = -1;

    if ((NULL == pszUsername) || (NULL == pszPassword)) {
        return NDK_ERR_PARA;
    }

    ndk_netSetLogin(pszUsername, pszPassword);

    nRet = ndk_NET_PPPOpen(1,1);
    if (nRet<0) {
        return nRet;
    }
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_PPP,"call %s,username=%s,password=%s\n",__func__,pszUsername,pszPassword);
    return NDK_OK;
}
/**
 *@brief    PPP����,δ��sim�����š�(���Լ�Ϳ������ò������ӿڸ�Ӧ��ʹ��)
 *@param    pszUsername �û���
 *@param    pszPassword ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszUsername/pszUsernameΪNULL)
 *@li   NDK_ERR_PPP_OPEN    PPP�Ѵ�
 *@li   NDK_ERR_SHM     �����ڴ����
 *@li   NDK_ERR_PPP_DEVICE      PPP��Ч�豸
*/
int ndk_pppdial(const char *pszUsername,const char *pszPassword)
{
    int nRet = -1;

    if ((NULL == pszUsername) || (NULL == pszPassword)) {
        return NDK_ERR_PARA;
    }

    ndk_netSetLogin(pszUsername, pszPassword);

    nRet = ndk_NET_PPPOpen(1,0);
    if (nRet<0) {
        return nRet;
    }
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_PPP,"call %s,username=%s,password=%s\n",__func__,pszUsername,pszPassword);
    return NDK_OK;
}

/**
 *@brief    PPP�Ҷ�
 *@param    nHangupType �Ҷ����� 0 �������Ҷ� 1 �����Ҷ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(nHangupType�Ƿ�)
 *@li   NDK_ERR_TIMEOUT     ��ʱ����
 *@li   NDK_ERR_SHM     �����ڴ����
*/
NEXPORT int NDK_PppHangup(int nHangupType)
{
    int nRet = -1;
    int i = 0;
    EM_PPP_STATUS nStatus;

    if ((nHangupType != 0) && (nHangupType != 1)) {
        NDK_COMM_LOG_INFO(NDK_LOG_MODULE_PPP,"%s fail,error return-%s\n",__func__,"NDK_ERR_PARA");
        return NDK_ERR_PARA;
    }

    nRet = ndk_NET_PPPClose();

    if (nHangupType && (NDK_OK == nRet)) {
        for (i = 900; i > 0; i--) { /**<�ȴ��Ҷ�ʱ��90S*/
            ndk_NET_PPPGetStatus(&nStatus);
            if (nStatus == PPP_STATUS_DISCONNECT) {
                nRet = 0;
                break;
            }
            ndk_msdelay(100);
        }

        if (0 == i) {
            TRACE_PPP("NetPPPHangup time out");
            nRet = NDK_ERR_TIMEOUT;
        }
    }
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_PPP,"%s fail,error return-%d\n",__func__,nRet);
    return nRet;
}


/**
 *@brief    ��ȡPPP״̬
 *@retval   pemStatus   ����PPP״̬,ΪNULL��ִ�иò���
 *@retval   pnErrCode   ����PPP���Ӵ���,ΪNULL��ִ�иò���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA            �����Ƿ�(pemStatus/pnErrCodeΪNULL)
 *@li   NDK_ERR_SHM             �����ڴ����
*/
NEXPORT int NDK_PppCheck(EM_PPP_STATUS *pemStatus, int *pnErrCode)
{
    int ret = -1;

    if ((NULL == pemStatus) && (NULL == pnErrCode)) {
        return NDK_ERR_PARA;
    }

    if (NULL != pemStatus) {
        if ((ret = ndk_NET_PPPGetStatus(pemStatus)) < 0) {
            return ret;
        }
    }

    if (NULL != pnErrCode) {
        *pnErrCode= ndk_getPPPerrorcode(0);
    }

    return NDK_OK;
}


/**
 *@brief    ��ȡ���ص�ַ��������ַ
 *@retval   pulLocalAddr    ���ر��ص�ַ,ΪNULL�򲻽���
 *@retval   pulHostAddr ����������ַ,ΪNULL�򲻽���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pulLocalAddr/pulHostAddrΪNULL)
 *@li   NDK_ERR         ����ʧ��(��ȡPPP��ַ��Ϣ����)
*/
NEXPORT int NDK_PppGetAddr(ulong *pulLocalAddr, ulong *pulHostAddr)
{
    unsigned long nRet;
    if ((NULL == pulLocalAddr) && (NULL == pulHostAddr)) {
        return NDK_ERR_PARA;
    }

    if (NULL != pulLocalAddr) {
        //*pulLocalAddr = ndk_getPPP0Addr("inet addr:");
        nRet = ndk_getPPP0Addr("inet addr:");
        if(nRet==0) {
            return NDK_ERR;
        } else {
            *pulLocalAddr =nRet;
        }
    }

    if (NULL != pulHostAddr) {
        //*pulHostAddr = ndk_getPPP0Addr("P-t-P:");
        nRet = ndk_getPPP0Addr("P-t-P:");
        if(nRet==0) {
            return NDK_ERR;
        } else {
            *pulHostAddr =nRet;
        }
    }
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_PPP,"%s succ,localaddr=%s,hostaddr=%s\n",__func__,pulLocalAddr,pulHostAddr);
    return NDK_OK;
}


/**
 *@brief    ��һ�����ʮ���Ƶ�IPת����һ������������
 *@param    pszIp   IP��ַ�ַ���
 *@retval   pulIpAddr   ����ת����ĳ�������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszIp/pulIpAddrΪNULL)
 *@li   NDK_ERR         ����ʧ��(��ַ�Ƿ�)
*/
NEXPORT int NDK_PppAddrChange(register const char *pszIp, ulong *pulIpAddr)
{
    if ((NULL == pszIp) || (NULL == pulIpAddr)) {
        return NDK_ERR_PARA;
    }

    *pulIpAddr = inet_addr(pszIp);

    if(*pulIpAddr == INADDR_NONE) {
        return NDK_ERR;
    }

    return NDK_OK;
}

