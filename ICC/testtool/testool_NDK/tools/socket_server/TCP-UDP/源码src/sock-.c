/*
[转]vc中socket编程步骤 
    sockets（套接字）编程有三种，流式套接字（SOCK_STREAM），数据报套接字（SOCK_DGRAM），原始套接字（SOCK_RAW）；基于TCP的socket编程是采用的流式套接字。在这个程序中，将两个工程添加到一个工作区。要链接一个ws2_32.lib的库文件。

服务器端编程的步骤：
1：加载套接字库，创建套接字(WSAStartup()/socket())；
2：绑定套接字到一个IP地址和一个端口上(bind())；
3：将套接字设置为监听模式等待连接请求(listen())；
4：请求到来后，接受连接请求，返回一个新的对应于此次连接的套接字(accept())；
5：用返回的套接字和客户端进行通信(send()/recv())；
6：返回，等待另一连接请求；
7：关闭套接字，关闭加载的套接字库(closesocket()/WSACleanup())。
*/
//服务器端代码如下：
#include <stdio.h>
#include <Winsock2.h>
void main()
{
 WORD wVersionRequested;
 WSADATA wsaData;
 int err;
 
 wVersionRequested = MAKEWORD( 1, 1 );
 
 err = WSAStartup( wVersionRequested, &wsaData );
 if ( err!=0 ) {
  return;
 }
 
 if ( LOBYTE( wsaData.wVersion )!=1 ||
        HIBYTE( wsaData.wVersion )!=1 ) {
  WSACleanup( );
  return;
 }
 SOCKET sockSrv=socket(AF_INET,SOCK_STREAM,0);

 SOCKADDR_IN addrSrv;
 addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
 addrSrv.sin_family=AF_INET;
 addrSrv.sin_port=htons(6000);
 
 bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));

 listen(sockSrv,5);

 SOCKADDR_IN addrClient;
 int len=sizeof(SOCKADDR);
 while(1)
 {
  SOCKET sockConn=accept(sockSrv,(SOCKADDR*)&addrClient,&len);
  char sendBuf[50];
  sprintf(sendBuf,"Welcome %s to here!",inet_ntoa(addrClient.sin_addr));
  send(sockConn,sendBuf,strlen(sendBuf)+1,0);
  char recvBuf[50];
  recv(sockConn,recvBuf,50,0);
  printf("%s\n",recvBuf);
  closesocket(sockConn);
 }

}
/*
客户端编程的步骤：
1：加载套接字库，创建套接字(WSAStartup()/socket())；
2：向服务器发出连接请求(connect())；
3：和服务器端进行通信(send()/recv())；
4：关闭套接字，关闭加载的套接字库(closesocket()/WSACleanup())。
*/
//客户端的代码如下：
#include <stdio.h>
#include <Winsock2.h>
void main()
{
 WORD wVersionRequested;
 WSADATA wsaData;
 int err;
 
 wVersionRequested = MAKEWORD( 1, 1 );
 
 err = WSAStartup( wVersionRequested, &wsaData );
 if ( err!=0 ) {
  return;
 }
 
 if ( LOBYTE( wsaData.wVersion )!=1 ||
        HIBYTE( wsaData.wVersion )!=1 ) {
  WSACleanup( );
  return;
 }
 SOCKET sockClient=socket(AF_INET,SOCK_STREAM,0);
 
 SOCKADDR_IN addrSrv;
 addrSrv.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
 addrSrv.sin_family=AF_INET;
 addrSrv.sin_port=htons(6000);
 connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
 send(sockClient,"hello",strlen("hello")+1,0);
 char recvBuf[50];
 recv(sockClient,recvBuf,50,0);
 printf("%s\n",recvBuf);
 
 closesocket(sockClient);
 WSACleanup();
}