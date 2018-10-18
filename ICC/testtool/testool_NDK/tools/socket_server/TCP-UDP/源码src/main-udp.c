#include <stdio.h>
#include <iostream.h>
#include <Winsock.h>
#pragma comment(lib, "wsock32")
int main()
{
	struct sockaddr_in server;
	SOCKET s;
	int len,n;
	char buf[5960],key;
	u_short IpPortnum=5000;
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 0);
	int ret=0;
	
	if (WSAStartup(version, &wsaData)!=0)                             
	{                                                                  
		printf("WSAStartup Err!");
		return -1;                                                     
	}
	
	if ((s=socket(AF_INET, SOCK_DGRAM, 0))==-1)                         
	{	
		printf("socket error\n");                                      
		return -1;                                                     
	}   
	
	server.sin_family		= AF_INET;                                  
	server.sin_addr.s_addr  = htonl(INADDR_ANY);
	server.sin_port			= htons(IpPortnum);                               
	len = sizeof(server);
	if (bind(s, (struct sockaddr *)&server, len)==-1)                     
	{	
		printf("bind error\n");                                        
		closesocket(s);                                                
		return -1;                                                     
	}  
	
	while(1)
	{
		printf("\nreceive data");
		n = recvfrom(s,buf,5000,0,(struct sockaddr *)&server,&len);
		printf("(%d)...",n);

		if (n>0)                                          
		{
			buf[n] = 0x00;                                           
			printf("receive data:%d  number:%d\n", buf[0], n);                            
			sendto(s,buf,n,0,(struct sockaddr *)&server,len);
		}
		else if (n==0)                        
		{                               
			printf("disconnect\n");
			key = 'e';
			break;   		            
		}   
	}
	closesocket(s);
	return 0;
}
