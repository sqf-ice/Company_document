/**
*@file		dns.c
*@brief		dns功能接口函数
*@version	1.0.0
*@author	yanm
*@date		2009-04-15
*@section 	history 	修改历史
	     	\<author\>\<time\>\<version\>\<desc\>
*/


#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "debug.h"
#include "NDK.h"
#include "NDK_debug.h"


#define MAX_DNS_SERVER_NUM      3
#define MAX_IP_LEN              16

#define CFG_FILE_NAME           "/etc/resolv.conf"
#define CFG_SERVER_STR          "nameserver"

extern int NDK_FsOpen(const char *pszName,const char *pszMode);
extern int NDK_FsClose(int nHandle);
extern int NDK_FsRead(int nHandle, char *psBuffer, uint unLength );
extern int NDK_FsWrite(int nHandle,const char * psBuffer,uint unLength);
extern int NDK_FsDel(const char * pszName);
extern int ndk_file_touch(char *filename);


int ndk_Add_DNS_IP(char* DNSBuf)
{
	int dns_fd = 0, count = 0;
	char Tmpbuf[128], szWbuf[256] = {0};
	char *pTmp1, *pTmp2;
	
	pTmp1 = DNSBuf;
	if (strlen(DNSBuf) == 0) 
	{
		return NDK_ERR;
    }

	memset(szWbuf, 0, sizeof(szWbuf));
	while((pTmp2 = strchr(pTmp1, ';')) != NULL)
	{
		count++;
		if(count > 2)		/**<最多3个DNS地址*/
			return NDK_ERR;
		memset(Tmpbuf, 0, sizeof(Tmpbuf));
		memcpy(Tmpbuf, pTmp1, pTmp2-pTmp1);
		if(inet_addr(Tmpbuf) == -1)	/**<判断IP是否合法*/
			return NDK_ERR_NET_ADDRILLEGAL;		
		sprintf(szWbuf+strlen(szWbuf), "%s %s\n", CFG_SERVER_STR, Tmpbuf);
		pTmp1 = pTmp2+1;
	}
	memset(Tmpbuf, 0, sizeof(Tmpbuf));
	strcpy(Tmpbuf, pTmp1);
	if(inet_addr(Tmpbuf) == -1)		/**<最后一个IP*/	
		return NDK_ERR_NET_ADDRILLEGAL;	
	sprintf(szWbuf+strlen(szWbuf), "%s %s\n", CFG_SERVER_STR, Tmpbuf);

	/**<清空文件*/	
	if (ndk_file_touch("/etc/resolv.conf")!=0) 	{
        DPRINTF("clean DNS FAIL\n");
        return NDK_ERR;
	}
	if ((dns_fd=NDK_FsOpen(CFG_FILE_NAME, "w")) < 0) {
        DPRINTF("open %s err!\n", CFG_FILE_NAME);
        return NDK_ERR;
    }
	if (NDK_FsWrite(dns_fd, szWbuf, strlen(szWbuf)) < strlen(szWbuf)){
		NDK_FsClose(dns_fd);
        DPRINTF("wtire %s err!\n", CFG_FILE_NAME);
        return NDK_ERR;
    }	

	NDK_FsClose(dns_fd);
	return NDK_OK;
}

int ndk_Add_DNS_IP_For_CommType(EM_COMM_TYPE emCommType,char* DNSBuf)
{
	int dns_fd = 0, count = 0;
	char Tmpbuf[128], szWbuf[256] = {0};
	char *pTmp1, *pTmp2;
	char resolve_path[64]={0};

	switch(emCommType){
            case COMM_TYPE_ETH:
                strcpy(resolve_path,"/tmp/eth_resolv.conf");
                break;
            case COMM_TYPE_WIFI:
                strcpy(resolve_path,"/tmp/wifi_resolv.conf");
                break;
            case COMM_TYPE_PPP:
                strcpy(resolve_path,"/tmp/wls_resolv.conf");
                break;
            default:
                return NDK_ERR_NET_UNKNOWN_COMMTYPE;
	}

	pTmp1 = DNSBuf;
	if (strlen(DNSBuf) == 0) 
	{
		return NDK_ERR;
        }

	memset(szWbuf, 0, sizeof(szWbuf));
	while((pTmp2 = strchr(pTmp1, ';')) != NULL)
	{
		count++;
		if(count > 2)		/**<最多3个DNS地址*/
			return NDK_ERR;
		memset(Tmpbuf, 0, sizeof(Tmpbuf));
		memcpy(Tmpbuf, pTmp1, pTmp2-pTmp1);
		if(inet_addr(Tmpbuf) == -1)	/**<判断IP是否合法*/
			return NDK_ERR_NET_ADDRILLEGAL;		
		sprintf(szWbuf+strlen(szWbuf), "%s %s\n", CFG_SERVER_STR, Tmpbuf);
		pTmp1 = pTmp2+1;
	}
	memset(Tmpbuf, 0, sizeof(Tmpbuf));
	strcpy(Tmpbuf, pTmp1);
	if(inet_addr(Tmpbuf) == -1)		/**<最后一个IP*/	
		return NDK_ERR_NET_ADDRILLEGAL;	
	sprintf(szWbuf+strlen(szWbuf), "%s %s\n", CFG_SERVER_STR, Tmpbuf);

	/**<清空文件*/	
	if (ndk_file_touch(resolve_path)!=0) 	{
        DPRINTF("clean DNS FAIL\n");
        return NDK_ERR;
	}
	if ((dns_fd=NDK_FsOpen(resolve_path, "w")) < 0) {
        DPRINTF("open %s err!\n", resolve_path);
        return NDK_ERR;
    }
	if (NDK_FsWrite(dns_fd, szWbuf, strlen(szWbuf)) < strlen(szWbuf)){
		NDK_FsClose(dns_fd);
        DPRINTF("wtire %s err!\n", resolve_path);
        return NDK_ERR;
    }	

	NDK_FsClose(dns_fd);
	return NDK_OK;
}

int ndk_Get_DNS_IP(char *DNSBuf)
{
	FILE *dns_fd;
	char Tmpbuf[128] = {0}, szRbuf[128] = {0};
	char *pTmp;
	
	if ((dns_fd = fopen(CFG_FILE_NAME, "r")) == NULL)
	{
        DPRINTF("open %s err!\n", CFG_FILE_NAME);
        return NDK_ERR; 
    }	
	
	memset(szRbuf, 0, sizeof(szRbuf));
	while(fgets(Tmpbuf, sizeof(Tmpbuf), dns_fd) != NULL)
	{
		if (strlen(Tmpbuf) != 0) 			
			Tmpbuf[strlen(Tmpbuf)-1] = 0;
    	if ((pTmp = strstr(Tmpbuf, CFG_SERVER_STR)) != NULL) 
		{
			strcat(szRbuf, Tmpbuf+strlen(CFG_SERVER_STR)+1);
			strcat(szRbuf, ";");
    	}
		memset(Tmpbuf, 0, sizeof(Tmpbuf));
	}

	fclose(dns_fd);
	if(strlen(szRbuf) == 0)
		return NDK_ERR;
	szRbuf[strlen(szRbuf)-1] = 0;	//去掉;
	strcpy(DNSBuf, szRbuf);

	return NDK_OK;
}

