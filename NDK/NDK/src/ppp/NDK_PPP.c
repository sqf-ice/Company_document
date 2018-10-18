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
 *@brief    配置PPP参数
 *@param    pstPPPCfg   ppp参数结构(设备类型设置缺省时为无线)
 *@param    nValidLen   参数pstPPPCfg的有效长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pstPPPCfg为NULL)
 *@li   NDK_ERR_PPP_DEVICE      PPP无效设备
 *@li   NDK_ERR_SHM     共享内存出错
 *@li   NDK_ERR_PPP_PARAM       PPP参数出错
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
 *@brief    PPP拨号，绑定sim卡拨号
 *@param    pszUsername 用户名
 *@param    pszPassword 密码
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszUsername/pszUsername为NULL)
 *@li   NDK_ERR_PPP_OPEN    PPP已打开
 *@li   NDK_ERR_SHM     共享内存出错
 *@li   NDK_ERR_PPP_DEVICE      PPP无效设备
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
 *@brief    PPP拨号,未绑定sim卡拨号。(供自检和烤机调用并不开接口给应用使用)
 *@param    pszUsername 用户名
 *@param    pszPassword 密码
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszUsername/pszUsername为NULL)
 *@li   NDK_ERR_PPP_OPEN    PPP已打开
 *@li   NDK_ERR_SHM     共享内存出错
 *@li   NDK_ERR_PPP_DEVICE      PPP无效设备
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
 *@brief    PPP挂断
 *@param    nHangupType 挂断类型 0 非阻塞挂断 1 阻塞挂断
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(nHangupType非法)
 *@li   NDK_ERR_TIMEOUT     超时错误
 *@li   NDK_ERR_SHM     共享内存出错
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
        for (i = 900; i > 0; i--) { /**<等待挂断时间90S*/
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
 *@brief    获取PPP状态
 *@retval   pemStatus   返回PPP状态,为NULL则不执行该操作
 *@retval   pnErrCode   返回PPP连接错误,为NULL则不执行该操作
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(pemStatus/pnErrCode为NULL)
 *@li   NDK_ERR_SHM             共享内存出错
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
 *@brief    获取本地地址和主机地址
 *@retval   pulLocalAddr    返回本地地址,为NULL则不接收
 *@retval   pulHostAddr 返回主机地址,为NULL则不接收
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pulLocalAddr/pulHostAddr为NULL)
 *@li   NDK_ERR         操作失败(获取PPP地址信息出错)
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
 *@brief    将一个点分十进制的IP转换成一个长整数型数
 *@param    pszIp   IP地址字符串
 *@retval   pulIpAddr   返回转换后的长整形数
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszIp/pulIpAddr为NULL)
 *@li   NDK_ERR         操作失败(地址非法)
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

