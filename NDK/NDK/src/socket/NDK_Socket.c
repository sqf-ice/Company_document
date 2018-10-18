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

/**<�ڲ���������*/
static int getnetworkerrcode();

/**<�⺯������*/
extern void notifier_start();
extern int Is_Eth_PullOut(const char *hostaddr);

/**<�ڲ�����*/


/**
 *@brief    ��TCPͨѶͨ��
 *@retval   punFd   ����TCPͨ�����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(punFdΪNULL)
 *@li   NDK_ERR_TCP_ALLOC       �޷�����
 *@li   NDK_ERR_TCP_PARAM       ��Ч����
 *@li   NDK_ERR_TCP_TIMEOUT     ���䳬ʱ
 *@li   NDK_ERR_TCP_INVADDR     ��Ч��ַ
 *@li   NDK_ERR_TCP_CONNECT     û������
 *@li   NDK_ERR_TCP_PROTOCOL        Э�����
 *@li   NDK_ERR_TCP_NETWORK     �������
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
 *@brief    �ر�TCPͨѶͨ��
 *@param    unFd    Ҫ�رյ�TCPͨ�����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR     ����ʧ��
*/
NEXPORT int NDK_TcpClose(uint unFd)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"call %s\n",__func__);
    int ret = -1;
    if(unFd==0)
        return NDK_ERR;
    ret = shutdown(unFd, SHUT_RDWR);    /**<��ֹ�շ�,�����ر��׽ӿ�*/
	errno =0;
    ret=close(unFd);
    if(ret<0)
        return NDK_ERR;
    else if(errno==EBADF)
        return NDK_ERR;

    return NDK_OK;
}


/**
 *@brief    �ȴ�TCP�رճɹ���һ���رռ�ʱ�˳�������NDK_TcpClose()�󣬱���������øú���ȷ��TCP��·��ȫ�ر�
 *@param    unFd    TCPͨ�����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR     ����ʧ��(unFd�Ƿ���)
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
    fcntl(unFd, F_SETFL, val | O_NONBLOCK); /**<����Ϊ������*/

    /**
    *�ȴ��Է�����FIN�����ܼ����Ҷ�TCP���������ɰ�����
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
        if(ret==0) { //��·�Ѿ�����
            break;
        } else if((ret<0)&&(errno == EAGAIN)) { //����td ���noblock ���ܳ���EAGAIN����
            continue;
        } else if(ret<0) { //��������,�˳�ѭ��
            break;
        }
    }
    close(unFd);
    return NDK_OK;
}


/**
 *@brief    �󶨱��˵�IP��ַ�Ͷ˿ں�
 *@param    unFd    TCPͨ�����
 *@param    pszMyIp Դ��ַ
 *@param    usMyPort    Դ�˿�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA    �����Ƿ�(pszMyIpΪNULL)
 *@li   NDK_ERR_TCP_PARAM   ��Ч����(unFd�Ƿ���Դ��ַ���Ϸ�)
 *@li   NDK_ERR_TCP_ALLOC       �޷�����
 *@li   NDK_ERR_TCP_TIMEOUT     ���䳬ʱ
 *@li   NDK_ERR_TCP_INVADDR     ��Ч��ַ
 *@li   NDK_ERR_TCP_CONNECT     û������
 *@li   NDK_ERR_TCP_PROTOCOL        Э�����
 *@li   NDK_ERR_TCP_NETWORK     �������
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

    if(inet_addr(pszMyIp) == -1) {  /**<�ж�IP�Ƿ�Ϸ�*/
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
 *@brief    ���ӷ�����
 *@param    unFd    TCPͨ�����
 *@param    pszRemoteIp Զ�̵�ַ
 *@param    usRemotePort    Զ�̶˿�
 *@param    unTimeout   Զ�����ӳ�ʱʱ�䣬��λΪ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA    �����Ƿ�(pszRemoteIpΪNULL)
 *@li   NDK_ERR             ����ʧ��
 *@li   NDK_ERR_TCP_TIMEOUT             ��ʱ����
 *@li   NDK_ERR_LINUX_TCP_TIMEOUT         TCPԶ�̶˿ڴ���
 *@li   NDK_ERR_LINUX_TCP_REFUSE         TCPԶ�̶˿ڱ��ܾ�
 *@li   NDK_ERR_LINUX_TCP_NOT_OPEN         TCP���δ�򿪴���
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

    /**<���÷�����*/
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
                    iTempRet=errno;//����ȡerrno,perror ����ȡ������ɴ���
                    perror("[NDK] NDK_TcpConnect ERROR:");
                    //TRACE_SOCKET("connect select return ret=%d,errno=%d\n",ret,iTempRet);
                    shutdown(unFd, SHUT_RDWR);  /**<�ͷ�����*/
                    NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%d\n",__func__,__LINE__,NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                    return (NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                } else if (ret > 0) {
                    /**<Socket selected for write*/
                    lon = sizeof(int);
                    if (getsockopt(unFd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
                        iTempRet=errno;//����ȡerrno,perror ����ȡ������ɴ���
                        perror("[NDK] NDK_TcpConnect ERROR:");
                        //TRACE_SOCKET("connect getsockopt return error,errno=%d\n",iTempRet);
                        shutdown(unFd, SHUT_RDWR);  /**<�ͷ�����*/
                        //return NDK_ERR_TCP_NETWORK;
                        return (NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                    }
                    if (valopt) {
                        iTempRet=valopt;//����ȡerrno,perror ����ȡ������ɴ���
                        perror("[NDK] NDK_TcpConnect ERROR:");
                        TRACE_SOCKET("connect getsockopt valopt=%d,errno=%d\n",valopt,iTempRet);
                        shutdown(unFd, SHUT_RDWR);  /**<�ͷ�����*/
                        //return NDK_ERR_TCP_NETWORK;
                        NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%d\n",__func__,__LINE__,NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                        return (NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
                    }
                    break;
                } else {
                    //TRACE_SOCKET("connect select ret=%d\n",ret);
                    shutdown(unFd, SHUT_RDWR);  /**<�ͷ�����*/
                    NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%s\n",__func__,__LINE__,"NDK_ERR_TCP_TIMEOUT");
                    return NDK_ERR_TCP_TIMEOUT;
                }
            } while (1);
        } else {
            iTempRet=errno;//����ȡerrno,perror ����ȡ������ɴ���
            perror("[NDK] NDK_TcpConnect ERROR:");
            TRACE_SOCKET("connect return ret!=EINPROGRESS,errno=%d\n",iTempRet);
            shutdown(unFd, SHUT_RDWR);  /**<�ͷ�����*/
            //return NDK_ERR_TCP_NETWORK;
            NDK_LOG_ERR(NDK_LOG_MODULE_SOCKET,"%s fail line[%d],error return-%d\n",__func__,__LINE__,NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
            return (NDK_ERR_LINUX_ERRNO_BASE-iTempRet);
        }
    }

    /**<���û�ԭ��״̬*/
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
 *@brief    �������������
 *@param    unFd    TCPͨ�����
 *@param    nBacklog    �ȴ����Ӷ��е���󳤶�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_TCP_ALLOC       �޷�����
 *@li   NDK_ERR_TCP_PARAM       ��Ч����
 *@li   NDK_ERR_TCP_TIMEOUT     ���䳬ʱ
 *@li   NDK_ERR_TCP_INVADDR     ��Ч��ַ
 *@li   NDK_ERR_TCP_CONNECT     û������
 *@li   NDK_ERR_TCP_PROTOCOL        Э�����
 *@li   NDK_ERR_TCP_NETWORK     �������
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
 *@brief    ������������
 *@param    unFd    TCPͨ�����
 *@param    pszPeerIp   ��������ʵ��ĵ�ַ
 *@param    usPeerPort  ��������ʵ��Ķ˿�
 *@retval       punNewFd    ����TCPͨ�����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszPeerIp/punNewFdΪNULL)
 *@li   NDK_ERR     ����ʧ��(����accept()ʧ�ܷ���)
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

    *punNewFd = ret;    /**<���յ��µ�socket���,���ڷ��ͺͽ�������*/
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"%s succ,current accept ip=%s,port=%d\n",__func__,pszPeerIp,usPeerPort);
    return NDK_OK;
}


/**
*@brief ��������
*@param unFd    TCPͨ�����
*@param pInbuf  ���ͻ������ĵ�ַ
*@param unLen   �������ݵĳ���
*@param unTimeout   ��ʱʱ�䣬��λΪ��
*@retval    punWriteLen ����ʵ�ʷ��ͳ���,���ΪNULL�򲻽���
*@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pInbufΪNULL)
 *@li   NDK_ERR_TCP_SEND        ���ʹ���(����send()ʧ�ܷ���)
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
    *�����źš���Դ״̬ˢ����Ҫʹ��SIGALRM�ź�(widget/notifier.c)��
    *�˴����ܱ����ź��жϣ������Ҫ��ʱ����
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
 *@brief    ��������
 *@param    unFd    TCPͨ�����
 *@param    unLen   �������ݵĳ���
 *@param    unTimeout   ��ʱʱ�䣬��λΪ��
 *@retvalpOutbuf    ���ջ������ĵ�ַ
 *@retvalpunReadlen ����ʵ�ʷ��ͳ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pOutbuf/punReadlenΪNULL)
 *@li   NDK_ERR         ����ʧ��NDK_ERR_TCP_TIMEOUT
 *@li   NDK_ERR_TCP_TIMEOUT         ��ʱ����
 *@li   NDK_ERR_TCP_RECV            ���մ���
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
            *�����źš���Դ״̬ˢ����Ҫʹ��SIGALRM�ź�(widget/notifier.c)��
            *�˴����ܱ����ź��жϣ������Ҫ��ʱ����
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
 *@brief    �����ر�TCPͨѶ��·������·�쳣����£�����̫������֮��ģ��շ�ʧ��֮����õĳ��׶Ͽ����Ӽ�������շ��ͻ��淵�أ�����TcpReset֮�������ٵ���TcpClose��
 *@param    unFd    Ҫ�رյ�TCPͨ�����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR     ����ʧ��
*/
NEXPORT int NDK_TcpReset(uint unFd)
{
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_SOCKET,"call %s\n",__func__);
    struct linger ling = {1, 0};
    setsockopt(unFd, SOL_SOCKET, SO_LINGER, (void*)&ling, sizeof(ling));
    int ret = -1;
    if(unFd==0)
        return NDK_ERR;
    ret = shutdown(unFd, SHUT_RDWR);    /**<��ֹ�շ�,�����ر��׽ӿ�*/
    ret=close(unFd);
    if(ret<0)
        return NDK_ERR;
    else if(errno==EBADF)
        return NDK_ERR;
    return NDK_OK;
}


/**
 *@brief    ��UDPͨѶͨ��
 *@retval   punFd   ����UDPͨ�����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERRCODE      ����ʧ��
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

