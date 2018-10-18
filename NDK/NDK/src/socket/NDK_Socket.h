/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：	产品开发部
* 日    期：	2012-08-17
* 版	本：	V1.00
* 最后修改人：
* 最后修改日期：
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

