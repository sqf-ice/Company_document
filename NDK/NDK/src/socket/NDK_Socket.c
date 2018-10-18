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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <time.h>
#include <setjmp.h>
#include <signal.h>
#include <arpa/inet.h>


#include "NDK_Socket.h"
#include "NDK.h"
#include "NDK_debug.h"

#ifdef NDK_DEBUG
#define NDK_DEBUG_SOCKET
#endif

#ifdef NDK_DEBUG_SOCKET
#define TRACE_SOCKET(fmt, args...) NDK_LOG_DEBUG(NDK_LOG_MODULE_SOCKET,fmt, ##args)
#define TRACE_HEX_SOCKET(fmt, args...) NDK_LOG_INFO(NDK_LOG_MODULE_SOCKET,fmt, ##args)
#else
#define TRACE_SOCKET(fmt, args...)
#define TRACE_HEX_SOCKET(fmt, args...)
#endif

#define TCP_CLOSE_TIME_WAIT 2

/**<内部函数声明*/
static int getnetworkerrcode();

/**<库函数声明*/
extern void notifier_start();
extern int Is_Eth_PullOut(const char *hostaddr);

/**<内部变量*/


/**
 *@brief    打开TCP通讯通道
 *@retval   punFd   返回TCP通道句柄
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(punFd为NULL)
 *@li   NDK_ERR_TCP_ALLOC       无法非配
 *@li   NDK_ERR_TCP_PARAM       无效参数
 *@li   NDK_ERR_TCP_TIMEOUT     传输超时
 *@li   NDK_ERR_TCP_INVADDR     无效地址
 *@li   NDK_ERR_TCP_CONNECT     没有连接
 *@li   NDK_ERR_TCP_PROTOCOL        协议错误
 *@li   NDK_ERR_TCP_NETWORK     网络错误
*/
NEXPORT int NDK_TcpOpen(uint *punFd)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"call %s\n",__func__);
    int ret = -1;


    if (NULL == punFd) {
        return NDK_ERR_PARA;
    }

    ret = socket(PF_INET, SOCK_STREAM, 0);
    if (ret < 0) {
        ret = getnetworkerrcode();
        return ret;
    }

    *punFd = ret;
    return NDK_OK;
}


/**
 *@brief    关闭TCP通讯通道
 *@param    unFd    要关闭的TCP通道句柄
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR     操作失败
*/
NEXPORT int NDK_TcpClose(uint unFd)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"call %s\n",__func__);
    int ret = -1;
    if(unFd==0)
        return NDK_ERR;
    ret = shutdown(unFd, SHUT_RDWR);    /**<禁止收发,并不关闭套接口*/
	errno =0;
    ret=close(unFd);
    if(ret<0)
        return NDK_ERR;
    else if(errno==EBADF)
        return NDK_ERR;

    return NDK_OK;
}


/**
 *@brief    等待TCP关闭成功，一旦关闭及时退出，调用NDK_TcpClose()后，必须继续调用该函数确保TCP链路完全关闭
 *@param    unFd    TCP通道句柄
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR     操作失败(unFd非法等)
*/
NEXPORT int NDK_TcpWait(uint unFd)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"call %s\n",__func__);
    int ret = -1;
    char buf[1];
    int val = 0;
    time_t startTM,currentTM;


    if (0 == unFd) {
        return NDK_ERR;
    }

    val = fcntl(unFd, F_GETFL, 0);
    fcntl(unFd, F_SETFL, val | O_NONBLOCK); /**<设置为非阻塞*/

    /**
    *等待对方发送FIN，才能继续挂断TCP，否则会造成半链接
    */
    //while ((ret = recv(unFd, buf, 1, 0)) > 0);
    startTM=time(NULL);
    while(1) { //linsx modify 2013/7/15 16:25:26 for EAGAIN error
        currentTM=time(NULL);
        if(currentTM-startTM>=TCP_CLOSE_TIME_WAIT) {
            fprintf(stderr,"[%s] timeout\n",__func__);
            break;
        }
        ret = recv(unFd, buf, 1, 0);
        if(ret==0) { //链路已经结束
            break;
        } else if((ret<0)&&(errno == EAGAIN)) { //由于td 句柄noblock 可能出现EAGAIN问题
            continue;
        } else if(ret<0) { //其他错误,退出循环
            break;
        }
    }
    close(unFd);
    return NDK_OK;
}


/**
 *@brief    绑定本端的IP地址和端口号
 *@param    unFd    TCP通道句柄
 *@param    pszMyIp 源地址
 *@param    usMyPort    源端口
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA    参数非法(pszMyIp为NULL)
 *@li   NDK_ERR_TCP_PARAM   无效参数(unFd非法、源地址不合法)
 *@li   NDK_ERR_TCP_ALLOC       无法非配
 *@li   NDK_ERR_TCP_TIMEOUT     传输超时
 *@li   NDK_ERR_TCP_INVADDR     无效地址
 *@li   NDK_ERR_TCP_CONNECT     没有连接
 *@li   NDK_ERR_TCP_PROTOCOL        协议错误
 *@li   NDK_ERR_TCP_NETWORK     网络错误
*/
NEXPORT int NDK_TcpBind(uint unFd, const char *pszMyIp, ushort usMyPort)
{
    int ret = -1;
    struct sockaddr_in pstMyAddr;

    if (NULL == pszMyIp) {
        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR_PARA");
        return NDK_ERR_PARA;
    }
    if (0 == unFd) {
        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR_TCP_PARAM");
        return NDK_ERR_TCP_PARAM;
    }

    if(inet_addr(pszMyIp) == -1) {  /**<判断IP是否合法*/
        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR_TCP_PARAM");
        return NDK_ERR_TCP_PARAM;
    }

    pstMyAddr.sin_family = AF_INET;
    pstMyAddr.sin_port = usMyPort;
    pstMyAddr.sin_addr.s_addr = inet_addr(pszMyIp);

    ret = bind(unFd, (struct sockaddr *)&pstMyAddr, sizeof(struct sockaddr));
    if (ret < 0) {
        if((ret = bind(unFd, (struct sockaddr *)&pstMyAddr, sizeof(struct sockaddr)))<0) {
            ret = getnetworkerrcode();
            NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%d\n",__func__,__LINE__,ret);
            return ret;
        }
    }
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"%s succ,current bind ip=%s,port=%d\n",__func__,pszMyIp,usMyPort);
    return NDK_OK;
}


/**
 *@brief    连接服务器
 *@param    unFd    TCP通道句柄
 *@param    pszRemoteIp 远程地址
 *@param    usRemotePort    远程端口
 *@param    unTimeout   远程连接超时时间，单位为秒
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA    参数非法(pszRemoteIp为NULL)
 *@li   NDK_ERR             操作失败
 *@li   NDK_ERR_TCP_TIMEOUT             超时错误
 *@li   NDK_ERR_LINUX_TCP_TIMEOUT         TCP远程端口错误
 *@li   NDK_ERR_LINUX_TCP_REFUSE         TCP远程端口被拒绝
 *@li   NDK_ERR_LINUX_TCP_NOT_OPEN         TCP句柄未打开错误
*/
NEXPORT int NDK_TcpConnect(uint unFd, const char *pszRemoteIp, ushort usRemotePort, uint unTimeout)
{
    int ret = -1;
    long arg;
    fd_set myset;
    socklen_t lon;
    int valopt=0;
    struct timeval tv;
    struct sockaddr_in addr;
    struct sockaddr_in pstRemoteAddr;
    int iTempRet;

    if (NULL == pszRemoteIp) {
        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR_PARA");
        return NDK_ERR_PARA;
    }

    pstRemoteAddr.sin_family = AF_INET;
    pstRemoteAddr.sin_port = usRemotePort;
    pstRemoteAddr.sin_addr.s_addr = inet_addr(pszRemoteIp);
    memcpy(&addr, &pstRemoteAddr, sizeof(struct sockaddr_in));
    addr.sin_port = htons(addr.sin_port);

    /**<设置非阻塞*/
    if( (arg = fcntl(unFd, F_GETFL, NULL)) < 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR");
        return NDK_ERR;
    }
    arg |= O_NONBLOCK;
    if( fcntl(unFd, F_SETFL, arg) < 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR");
        return NDK_ERR;
    }
	if(Is_Eth_PullOut(pszRemoteIp)==1)
		return NDK_ERR_ETH_PULLOUT;
    tv.tv_sec = unTimeout;
    tv.tv_usec = 0;
    TRACE_SOCKET("Start socket connect");
    ret = connect(unFd,  (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (ret < 0) {
        //TRACE_SOCKET("connect return ret=%d\n",ret);
        if (errno == EINPROGRESS) {
            do {
                FD_ZERO(&myset);
                FD_SET(unFd, &myset);
                ret = select(unFd+1, NULL, &myset, NULL, &tv);
                if (ret < 0) {
                    if (EINTR == errno) {
                        continue;
                    }
                    iTempRet=errno;//先提取errno,perror 后面取可能造成错误。
                    perror("[NDK] NDK_TcpConnect ERROR:");
                    //TRACE_SOCKET("connect select return ret=%d,errno=%d\n",ret,iTempRet);
                    shutdown(unFd, SHUT_RDWR);  /**<释放连接*/
                    NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%d\n",__func__,__LINE__,NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                    return (NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                } else if (ret > 0) {
                    /**<Socket selected for write*/
                    lon = sizeof(int);
                    if (getsockopt(unFd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
                        iTempRet=errno;//先提取errno,perror 后面取可能造成错误。
                        perror("[NDK] NDK_TcpConnect ERROR:");
                        //TRACE_SOCKET("connect getsockopt return error,errno=%d\n",iTempRet);
                        shutdown(unFd, SHUT_RDWR);  /**<释放连接*/
                        //return NDK_ERR_TCP_NETWORK;
                        return (NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                    }
                    if (valopt) {
                        iTempRet=valopt;//先提取errno,perror 后面取可能造成错误。
                        perror("[NDK] NDK_TcpConnect ERROR:");
                        TRACE_SOCKET("connect getsockopt valopt=%d,errno=%d\n",valopt,iTempRet);
                        shutdown(unFd, SHUT_RDWR);  /**<释放连接*/
                        //return NDK_ERR_TCP_NETWORK;
                        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%d\n",__func__,__LINE__,NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                        return (NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                    }
                    break;
                } else {
                    //TRACE_SOCKET("connect select ret=%d\n",ret);
                    shutdown(unFd, SHUT_RDWR);  /**<释放连接*/
                    NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR_TCP_TIMEOUT");
                    return NDK_ERR_TCP_TIMEOUT;
                }
            } while (1);
        } else {
            iTempRet=errno;//先提取errno,perror 后面取可能造成错误。
            perror("[NDK] NDK_TcpConnect ERROR:");
            TRACE_SOCKET("connect return ret!=EINPROGRESS,errno=%d\n",iTempRet);
            shutdown(unFd, SHUT_RDWR);  /**<释放连接*/
            //return NDK_ERR_TCP_NETWORK;
            NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%d\n",__func__,__LINE__,NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
            return (NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
        }
    }

    /**<设置回原来状态*/
    if( (arg = fcntl(unFd, F_GETFL, NULL)) < 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR");
        return NDK_ERR;
    }
    arg &= (~O_NONBLOCK);
    if( fcntl(unFd, F_SETFL, arg) < 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR");
        return NDK_ERR;
    }

    notifier_start();
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"%s succ,current connect ip=%s,port=%d,timeout=%d\n",__func__,pszRemoteIp,usRemotePort,unTimeout);
    return NDK_OK;
}


/**
 *@brief    监听申请的连接
 *@param    unFd    TCP通道句柄
 *@param    nBacklog    等待连接队列的最大长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_TCP_ALLOC       无法非配
 *@li   NDK_ERR_TCP_PARAM       无效参数
 *@li   NDK_ERR_TCP_TIMEOUT     传输超时
 *@li   NDK_ERR_TCP_INVADDR     无效地址
 *@li   NDK_ERR_TCP_CONNECT     没有连接
 *@li   NDK_ERR_TCP_PROTOCOL        协议错误
 *@li   NDK_ERR_TCP_NETWORK     网络错误
*/
NEXPORT int NDK_TcpListen(uint unFd, int nBacklog)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"call %s\n",__func__);
    int ret = -1;

    ret = listen(unFd, nBacklog);
    if (ret < 0) {
        ret = getnetworkerrcode();
        return ret;
    }

    return NDK_OK;
}



/**
 *@brief    接收连接请求
 *@param    unFd    TCP通道句柄
 *@param    pszPeerIp   接收连接实体的地址
 *@param    usPeerPort  接收连接实体的端口
 *@retval       punNewFd    返回TCP通道句柄
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszPeerIp/punNewFd为NULL)
 *@li   NDK_ERR     操作失败(调用accept()失败返回)
*/
NEXPORT int NDK_TcpAccept(uint unFd, const char *pszPeerIp, ushort usPeerPort, uint *punNewFd)
{
    int ret = -1;
    int len = sizeof(struct sockaddr);
    struct sockaddr_in pstPeerAddr;

    if ((NULL == pszPeerIp) || (NULL == punNewFd)) {
        return NDK_ERR_PARA;
    }

    pstPeerAddr.sin_family = AF_INET;
    pstPeerAddr.sin_port = usPeerPort;
    pstPeerAddr.sin_addr.s_addr = inet_addr(pszPeerIp);

    ret = accept(unFd, (struct sockaddr *)&pstPeerAddr, (socklen_t *)&len);
    if (ret < 0) {
        return NDK_ERR;
    }

    *punNewFd = ret;    /**<接收到新的socket句柄,用于发送和接收数据*/
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"%s succ,current accept ip=%s,port=%d\n",__func__,pszPeerIp,usPeerPort);
    return NDK_OK;
}


/**
*@brief 发送数据
*@param unFd    TCP通道句柄
*@param pInbuf  发送缓冲区的地址
*@param unLen   发送数据的长度
*@param unTimeout   超时时间，单位为秒
*@retval    punWriteLen 接收实际发送长度,如果为NULL则不接收
*@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pInbuf为NULL)
 *@li   NDK_ERR_TCP_SEND        发送错误(调用send()失败返回)
*/
NEXPORT int NDK_TcpWrite(uint unFd, const void *pInbuf, uint unLen, uint unTimeout, uint *punWriteLen)
{
    int n = -1;
    sigset_t newmask, oldmask;
    struct timeval timeout;

    if (NULL == pInbuf) {
        return NDK_ERR_PARA;
    }

    timeout.tv_sec = unTimeout;
    timeout.tv_usec= 0;
    signal(SIGPIPE, SIG_IGN);
    setsockopt(unFd, SOL_SOCKET,SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
    /**
    *无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，
    *此处可能被该信号中断，因此需要暂时屏蔽
    */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    n = send(unFd, pInbuf, unLen, 0);
    if ((n < 0) || (n < unLen)) {
        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
        return NDK_ERR_TCP_SEND;
    }

    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    signal(SIGPIPE, SIG_DFL);

    if(punWriteLen != NULL) {
        *punWriteLen = n;
    }
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"%s succ,writelen=%d\n",__func__,n);
    return NDK_OK;
}


/**
 *@brief    接收数据
 *@param    unFd    TCP通道句柄
 *@param    unLen   接收数据的长度
 *@param    unTimeout   超时时间，单位为秒
 *@retvalpOutbuf    接收缓冲区的地址
 *@retvalpunReadlen 返回实际发送长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pOutbuf/punReadlen为NULL)
 *@li   NDK_ERR         操作失败NDK_ERR_TCP_TIMEOUT
 *@li   NDK_ERR_TCP_TIMEOUT         超时错误
 *@li   NDK_ERR_TCP_RECV            接收错误
*/
NEXPORT int NDK_TcpRead(uint unFd, void *pOutbuf, uint unLen, uint unTimeout, uint *punReadlen)
{
    int ret = -1;
    fd_set readfds;
    struct timeval tv;
    int n = -1;
    int read_bytes = 0;
    sigset_t newmask, oldmask;

    if ((NULL == pOutbuf) || (NULL == punReadlen)) {
        return NDK_ERR_PARA;
    }


    tv.tv_sec = unTimeout;
    tv.tv_usec= 0;


    while(read_bytes < unLen) {
		FD_ZERO(&readfds);
        FD_SET(unFd, &readfds);
        ret=select(unFd+1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            if (EINTR == errno) {
                continue;
            }
            return NDK_ERR;
        } else if (0 == ret) {
            if (0 == read_bytes) {
                return NDK_ERR_TCP_TIMEOUT;
            }
            *punReadlen = read_bytes;
            return NDK_OK;
        } else {
            /**
            *无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，
            *此处可能被该信号中断，因此需要暂时屏蔽
            */
            sigemptyset(&newmask);
            sigaddset(&newmask, SIGALRM);
            sigprocmask(SIG_BLOCK, &newmask, &oldmask);
            n = recv(unFd, pOutbuf+read_bytes, unLen-read_bytes, 0);
            sigprocmask(SIG_UNBLOCK, &newmask, NULL);
            if (n <= 0) {
                if(0==read_bytes) {
                    //perror("_______________recv error_____________________\n");
                    //fprintf(stderr,"");
                    return NDK_ERR_TCP_RECV;
                } else {
                    *punReadlen = read_bytes;
                    return NDK_OK;
                }
            }

            read_bytes += n;
        }
    }
    *punReadlen = read_bytes;
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"%s succ,readlen=%d\n",__func__,read_bytes);
    return NDK_OK;
}
/**
 *@brief    立即关闭TCP通讯链路（在链路异常情况下（如以太网拔线之后的）收发失败之后调用的彻底断开链接即立即清空发送缓存返回，调用TcpReset之后无需再调用TcpClose）
 *@param    unFd    要关闭的TCP通道句柄
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR     操作失败
*/
NEXPORT int NDK_TcpReset(uint unFd)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"call %s\n",__func__);
    struct linger ling = {1, 0};
    setsockopt(unFd, SOL_SOCKET, SO_LINGER, (void*)&ling, sizeof(ling));
    int ret = -1;
    if(unFd==0)
        return NDK_ERR;
    ret = shutdown(unFd, SHUT_RDWR);    /**<禁止收发,并不关闭套接口*/
    ret=close(unFd);
    if(ret<0)
        return NDK_ERR;
    else if(errno==EBADF)
        return NDK_ERR;
    return NDK_OK;
}


/**
 *@brief    打开UDP通讯通道
 *@retval   punFd   返回UDP通道句柄
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_UdpOpen(uint *punFd)
{
    int ret = -1;

    if (NULL == punFd) {
        return NDK_ERR_PARA;
    }

    ret = socket(PF_INET, SOCK_DGRAM, 0);
    if (ret < 0) {
        ret = getnetworkerrcode();
        return ret;
    }

    *punFd = ret;
    return NDK_OK;
}


/************************************************************************/

static int getnetworkerrcode()
{
    int ret = -1;

    switch (errno) {
        case ENOBUFS:
        case ENOMEM:
        case ENFILE:
        case EMFILE:
        case EAGAIN:
            ret = NDK_ERR_TCP_ALLOC;
            break;
        case EINVAL:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
        case EISCONN:
        case EAFNOSUPPORT:
            ret = NDK_ERR_TCP_PARAM;
            break;
        case ETIMEDOUT:
        case EINTR:
            ret = NDK_ERR_TCP_TIMEOUT;
            break;
        case EADDRINUSE:
            ret = NDK_ERR_TCP_INVADDR;
            break;
        case ECONNREFUSED:
            ret = NDK_ERR_TCP_CONNECT;
            break;
        case EPROTONOSUPPORT:
            ret = NDK_ERR_TCP_PROTOCOL;
            break;
        default:
            ret = NDK_ERR_TCP_NETWORK;
            break;
    }
    return ret;
}

