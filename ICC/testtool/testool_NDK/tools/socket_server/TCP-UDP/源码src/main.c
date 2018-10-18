/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* Author 			: bcat
* version			: V1.0
* DATE			    : 20090311
* description		: 以太网测试后台-TCP&UDP
*				  
************************************************************************/
#include <stdio.h>
#include <iostream.h>
#include <Winsock.h>
#include <time.h>

#pragma comment(lib, "wsock32")

#define MAX_NUM 4096+512;

int main(int argc,char *argv[])
{
	struct sockaddr_in server;
	SOCKET s,rs;
	int len,pn,n;
	FILE *fd=NULL;
	char buf[5960],ip[20],key;
	u_short IpPortnum=5000;
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 0);
	int ilen;
	int protoType=0;
	int commuType=0;
	int connectpress=0;
	int portadd=0;
	int ret=0,ret1;
	long Rtimes;
	char tempbuf[5960];
	struct tm *t;
	time_t timer;
	char IpPort[11];

	if(argc==1)
	{
		printf("\n命令格式1：直接输入命令，如：nettest");
		printf("\n命令格式2：输入全部参数，如下：");
		printf("\n命令 [端口号][协议类型][链接压力][端口号累加][通讯类型]\n\n");
		Sleep(800); //毫秒单位
		printf("\n执行命令格式1\n");
		printf("请输入端口号:");
		if (fgets(IpPort,10,stdin)!=NULL)
		{	
			printf("IpPort:%s ",IpPort);
			if (!(IpPortnum = atoi(IpPort)))
			{	
				IpPortnum = 3456;
			}
		}
		else 
		{	
			IpPortnum = 3456;
		}
	}else if(argc<6)
	{
		printf("\n参数输入错误......\n请参考命令参数格式说明：");
		printf("\n命令 [端口号][协议类型][链接压力][端口号累加][通讯类型]");
		printf("\n端口号－以太网的端口号");
		printf("\n协议类型：0－TCP，1－UDP");
		printf("\n链接压力：0－非，1－压力(每次通讯都要重新链接)");
		printf("\n端口号累加：0－非，1－累加（每次链接端口号都改变）");
		printf("\n通讯类型：0－双工，1－发送（服务器），2－接收");
		printf("\n\n默认：\n端口号[5000]，TCP[0]，链接非压力[0]，端口号不变[0]，双工[0]\n");
        printf("\n\n例：命令 5000 0 0 0 0\n");

		Sleep(4300); //毫秒单位
		return 0;
	}
	if(argc==1)
	{
		protoType=SOCK_STREAM;
		connectpress=0;
		portadd=0;
		commuType=0;
	}
	else
	{

#if 0
	//输入端口号
	printf("please input the port number:");
	if (fgets(IpPort,10,stdin)!=NULL)
	{	
		printf("IpPort:%s ",IpPort);
		if (!(IpPortnum = atoi(IpPort)))
		{	
			IpPortnum = 3456;
		}
	}
	else 
	{	
		IpPortnum = 3456;
	}
#else
	if (!(IpPortnum = atoi(argv[1])))
	{	
		IpPortnum = 5000;
	}
#endif

		if(atoi(argv[2])==1)
			protoType=SOCK_DGRAM;
		else
			protoType=SOCK_STREAM;
		
		if((connectpress=atoi(argv[3]))!=1)
			connectpress=0;
		
		if((portadd=atoi(argv[4]))!=1)
			portadd=0;
		
		switch(atoi(argv[5]))
		{
		case 0:commuType=0;
			break;
		case 1:commuType=1;
			break;
		case 2:commuType=2;
			break;
		default:commuType=0;
			break;
		}
	}
	
	while(1)
	{
		if (WSAStartup(version, &wsaData)!=0)                             
		{                                                                  
			printf("WSAStartup Err!");
			return -1;                                                     
		}
		
		if ((s=socket(AF_INET, protoType, 0))==-1)                         
		{	
			printf("socket error\n");                                      
			return -1;                                                     
		}   
		
		server.sin_family		= AF_INET;                                  
		server.sin_addr.s_addr  = INADDR_ANY;                               
		server.sin_port			= htons(IpPortnum);                               
		len = sizeof(server);
		if (bind(s, (struct sockaddr *)&server, len)==-1)                     
		{	
			printf("bind error\n");                                        
			closesocket(s);                                                
			return -1;                                                     
		}  
		if(	protoType==SOCK_STREAM)
		{
			if (listen(s, 1) ==-1 )                  
			{	
				printf("listen error\n");                                        
				closesocket(s);                                                
				return -1;                                                     
			}
		}
		
		key = 0;
		fd = fopen("netlog.txt","a+");
		printf("waiting for connecting....................\n"); 
		ilen = fprintf(fd, "waiting for connecting....................\n");
		if(	protoType==SOCK_STREAM)
		{
			rs = accept(s, (struct sockaddr *)&server, &len);                      
			printf("some one is connected\n");
			fprintf(fd, "some one is connected\n");
		}
		
		//记录时间
		timer = time(NULL);
		t = localtime(&timer);
		printf("time: %s", asctime(t));
		fprintf(fd, "time: %s", asctime(t));
		
		//记录客户端地址，端口
		strcpy(ip, inet_ntoa(server.sin_addr));                             
		pn = (int) ntohs(server.sin_port);                                   
		printf("Client Ip Address=%s Port Numbdr =%d\n", ip, pn);            
		fprintf(fd, "Client Ip Address=%s Port Numbdr =%d\n", ip, pn);  
		while(1)
		{
			Rtimes=0;
			if(	protoType==SOCK_STREAM)
				n = recv(rs, buf, 5000, 0);  
			else
				n = recvfrom(s,buf,5000,0,(struct sockaddr *)&server,&len);
			
			if (n>0)                                          
			{
				int i=0;
				buf[n] = 0x00;                                           
				printf("receive data:%d  number:%d\n", buf[0], n);                            
				fprintf(fd, "receive data:%d  number:%d\n", buf[0], n);                                 
				printf("Send the data !\n");
				fprintf(fd, "Send the data :\n");
				fwrite(buf, 1, n, fd);
				fprintf(fd, "\n");
				printf("server port number: %d\n",IpPortnum);
				
				if(portadd==1)
				{
					IpPortnum ++;	
					if(IpPortnum>8000)
						IpPortnum=5000;
					buf[n] =(IpPortnum>>8) ;
					buf[n+1] = IpPortnum;
					if(	protoType==SOCK_STREAM)
						send(rs, buf, n+2, 0);   
					else
						sendto(s,buf,n+2,0,(struct sockaddr *)&server,len);
					
					//							send(rs, buf, n+2, 0); 
#if 0
					for(i =0;i<n+2;i++)
						printf("%02x " ,buf[i]);
					printf("\n");
#endif
				}else{
					if(	protoType==SOCK_STREAM)
						send(rs, buf, n, 0);   
					else
						sendto(s,buf,n,0,(struct sockaddr *)&server,len);
#if 0
					for(i =0;i<n;i++)
						printf("%02x " ,buf[i]);
					printf("\n");
#endif
				}
				
				if((commuType==1)&&(connectpress==0))
				{
					while(1)
					{
#if 1
						if(	protoType==SOCK_STREAM)
							ret1=send(rs, buf, n, 0);   
						else
							ret1=sendto(s,buf,n,0,(struct sockaddr *)&server,len);
#else
						if(send(rs, buf, n, 0)!=n)
						{ 
							printf("send error....");
						}
#endif
						if(ret1!=n)
							printf("send error....");
						printf("\nsend data:(%d),lenth(%d)",buf[0],n);
#if 0
						for(i =0;i<n;i++)
							printf("%02x " ,buf[i]);
						printf("\n");
#endif
#if 0	//测试单向性能时,这里的超时会影响,故屏蔽 20180714					
						if(	protoType==SOCK_STREAM)
							Sleep(1300); //毫秒单位
						else
							Sleep(1900); //毫秒单位
#endif
					}
				}
				if((commuType==2)&&(connectpress==0))
				{
					printf("\n第[1]次接收数据[长度:%d] 成功",n);
					memcpy(tempbuf,buf,n);
					Rtimes++;
				}
				
				if((commuType==2)&&(connectpress==0))
				{
					while(1)
					{
						memset(buf,0x00,sizeof(buf));
#if 0
						ret=recv(rs, buf, 5000, 0); 
#else
						if(	protoType==SOCK_STREAM)
							ret = recv(rs, buf, 5000, 0);  
						else
							ret = recvfrom(s,buf,5000,0,(struct sockaddr *)&server,&len);
						
#endif
						if((ret>0)&&(Rtimes>0))
						{
							printf("\n第(%d)次接收数据[长度:%d]",Rtimes+1,ret);
							if (memcmp(tempbuf,buf,ret)!=0) 
								printf("  失败....");
							else
								printf("  成功");
							Rtimes++;
						}
						Sleep(100);
					} 
				}
				}
				else if (n==0)                         
				{                               
					printf("disconnect\n");
					fprintf(fd, "disconnect\n");
					key = 'e';
					break;   		            
				}   
				else if (n<0) //bcat 2009.04.21 
				{
					printf("SOCKET_ERROR\ndisconnect\n");
					fprintf(fd, "SOCKET_ERROR\ndisconnect\n");
					key = 'e';
					break;   		            
				}
				if(connectpress==1)
					break;
				}
				fclose(fd);
				closesocket(rs); 
				closesocket(s);
				
		}
	return 0;
}

