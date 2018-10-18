/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�	��Ʒ������
* ��    �ڣ�	2012-08-17
* ��	����	V1.00
* ����޸��ˣ�
* ����޸����ڣ�
*/
#ifndef __SOCKET__H
#define __SOCKET__H


int NDK_TcpOpen(uint *punFd);
int NDK_TcpClose(uint unFd);
int NDK_TcpWait(uint unFd);
int NDK_TcpBind(uint unFd, const char *pszMyIp, ushort usMyPort);
int NDK_TcpConnect(uint unFd, const char *pszRemoteIp, ushort usRemotePort, uint unTimeout);
int NDK_TcpListen(uint unFd, int nBacklog);
int NDK_TcpAccept(uint unFd, const char *pszPeerIp, ushort usPeerPort, uint *punNewFd);
int NDK_TcpWrite(uint unFd, const void *pInbuf, uint unLen, uint unTimeout, uint *punWriteLen);
int NDK_TcpRead(uint unFd, void *pOutbuf, uint unLen, uint unTimeout, uint *punReadlen);
int NDK_UdpOpen(uint *punFd);


#endif
/* End of this file */

