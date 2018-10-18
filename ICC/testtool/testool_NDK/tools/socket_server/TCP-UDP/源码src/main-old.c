#include <stdio.h>
#include <iostream.h>
#include <Winsock.h>
#include <time.h>

#pragma comment(lib, "wsock32")

#define MAX_NUM 4096+512;
#define RECEIVE 1
#define SEND 2
#define TYPE RECEIVE
//#define TYPE SEND

void do_echo(int sockfd, struct sockaddr *pcliaddr, int clilen) 
{ 
	int n; 
	int len; 
	char mesg[5960]; 
	
	for(;;) 
	{ 
		len = clilen; 
		/* waiting for receive data */ 
		n = recvfrom(sockfd, mesg, 5960, 0, pcliaddr, &len); 
		/* sent data back to client */ 
		sendto(sockfd, mesg, n, 0, pcliaddr, len); 
	} 
} 

//int main(int argc,char *argv[])

int main()
{	struct sockaddr_in server,cliaddr;
	SOCKET s,rs;
	int len,pn,n;
	FILE *fd=NULL;
	char buf[5960],ip[20],key;
	u_short IpPortnum;
	char IpPort[10];
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 0);
	int ilen;

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
#if TYPE==RECEIVE
	{
		struct tm *t;
		time_t timer;
		while(1)
		{
			if (WSAStartup(version, &wsaData)!=0)                             
			{                                                                  
				printf("WSAStartup Err!");
				return -1;                                                     
			}
			
			if ((s=socket(AF_INET, SOCK_STREAM, 0))==-1)                         
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
			if (listen(s, 1) ==-1 )                  
			{	
				printf("listen error\n");                                        
				closesocket(s);                                                
				return -1;                                                     
			}   
                              
			key = 0;
			fd = fopen("netlog.txt","a+");
			printf("waiting for connecting....................\n"); 
			ilen = fprintf(fd, "waiting for connecting....................\n");
			rs = accept(s, (struct sockaddr *)&server, &len);                      
			printf("some one is connected\n");
			fprintf(fd, "some one is connected\n");
			
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
//					n = recv(rs, buf, 1024, 0);                              
					n = recv(rs, buf, 5000, 0);                              
					if (n>0)                                          
					{
						int i=0;
						buf[n] = 0x00;                                           
						printf("receive data:%d+%d  number:%d\n", buf[0], n);                            
						fprintf(fd, "receive data:%d  number:%d\n", buf[0], n);                                 
						printf("Send the data !\n");
						fprintf(fd, "Send the data :\n");
						fwrite(buf, 1, n, fd);
						fprintf(fd, "\n");
						printf("server port number: %d\n",IpPortnum);
#if 0
						IpPortnum ++;	
						buf[n] =(IpPortnum>>8) ;
						buf[n+1] = IpPortnum;
						send(rs, buf, n+2, 0);   //为什么要加n+2 bcat
//						send(rs, buf, n+2, 0); 
						for(i =0;i<n+2;i++)
						printf("%02x " ,buf[i]);
						printf("\n");
#else
						send(rs, buf, n, 0);   //为什么要加n+2 bcat
						for(i =0;i<n;i++)
						printf("%02x " ,buf[i]);
						printf("\n");
#endif
#if 1
						while(1)
						{
							if(send(rs, buf, n, 0)!=n)
							{ 
								printf("send error....");
							}
							printf("send data:\n");
							for(i =0;i<n;i++)
								printf("%02x " ,buf[i]);
							    printf("\n");
							Sleep(1200); //毫秒单位
 						}
#endif
					}
					else if (n==0)                        
					{                               
						printf("disconnect\n");
						fprintf(fd, "disconnect\n");
						key = 'e';
						break;   		            
					}   
				}
				fclose(fd);
				closesocket(rs); 
				closesocket(s);

		}
	}
#else
	{
		int i;
		while(1)
		{
			if (WSAStartup(version, &wsaData)!=0)                             
			{                                                                  
				printf("WSAStartup Err!");
				return -1;                                                     
			}
			
			if ((s=socket(AF_INET, SOCK_STREAM, 0))==-1)                         
			{	
				printf("socket error\n");                                      
				return -1;                                                     
			}   
			server.sin_family		= AF_INET;                                  
 			server.sin_addr.s_addr  = (232<<24)|(2<<16)|(168<<8)|(192); //bcat 232            
// 			server.sin_addr.s_addr  = (160<<24)|(2<<16)|(168<<8)|(192);             
			server.sin_port			= htons(IpPortnum);                               
			len = sizeof(server); 
			if ((connect(s, (struct sockaddr *)&server, len) )==-1)    
			{	
				
				printf("connect error\n");                                        
				closesocket(s);        
				 getchar();     
				return -1;                                              
			} 
			else
			{
				printf("server port number:%d\n",IpPortnum);
			}
			memset(buf,8,8);
			n=8;
			printf("Send the data !\n");
			getchar();
			if((send(s, buf, 8, 0) )==-1)
				printf("send error"); 
			n = recv(s, buf, 1024, 0);                              
			if (n>0)                                          
			{	
				buf[n] = 0x00;                                           
				printf("receive data:%d  number:%d\n", buf[0], n);                            
			}
			else         
			{                               
				printf("disconnect\n");
			}  
			IpPortnum = ((buf[n-2]<<8) |(0x000000ff& (buf[n-1])));
			
			for(i =0;i<n;i++)
				printf("%02x " ,buf[i]);
			printf("\n");
			closesocket(s);  
		}
	}
#endif
	return 0;
}

