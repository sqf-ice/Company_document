#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include<unistd.h>
#include <net/if.h>
#include<sys/ioctl.h>
#include <errno.h>




#include "NDK.h"
#include "NDK_Net.h"
#include "NDK_debug.h"
#include "../Ethernet/NDK_Ethernet.h"
#include "../Ethernet/Ethernet.h"
#define WIFI_IF_INFO_FILE_8686 "/proc/net/wlan/info"


extern char ptmpNetDnsBuf[3][30];
extern int ndk_cpu_type(void);


int SetEthRoute(char *szDstIP);
int SetWLSRoute(char *szDstIP);
int SetWIFIRoute(char *szDstIP);


int ndk_NetLink(const char * oldpath, const char * newpath)
{
	remove(newpath);
	if(symlink(oldpath,newpath) != 0){
		return -1;
	}
	return 0;
}



int ndk_NetGetResolv(void)
{
	FILE *dns_fd;
	char Tmpbuf[128];
	char *pTmp;
	int ret = 0;
	
	if ((dns_fd = fopen("/etc/resolv.conf", "r")) == NULL)
        return NDK_ERR; 
	memset(Tmpbuf,0x0,sizeof(Tmpbuf));
	while(fgets(Tmpbuf, sizeof(Tmpbuf), dns_fd) != NULL)
	{
		if (strlen(Tmpbuf) != 0) 			
			Tmpbuf[strlen(Tmpbuf)-1] = 0;
    	if ((pTmp = strstr(Tmpbuf, "nameserver")) != NULL) 
		{
			if(ret == 3){
				return -1;
			}
			strcpy(ptmpNetDnsBuf[ret],Tmpbuf+strlen("nameserver")+1);
			ret++;
			
    	}
		memset(Tmpbuf, 0x00, sizeof(Tmpbuf));
	}
	fclose(dns_fd);
	return ret;
}

int WiFiGetInfo(char *pszInfoName,int iNameLen,char *pszInfoValue)
{
	FILE *fd;
    char buf[256], szTemp[30];
	char szCmd[128];
	memset(szCmd,0,sizeof(szCmd));
	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szCmd,"cat %s",WIFI_IF_INFO_FILE_8686);
	if ((fd=popen(szCmd, "r")) != NULL) 
	{
	     memset(buf, 0x00, sizeof(buf));
            while (fgets(buf, sizeof(buf)-1, fd) != NULL) {
            if (strncmp(buf, pszInfoName, iNameLen) == 0) {
                strcpy(szTemp, buf+iNameLen+1);
                szTemp[strlen(szTemp)-2] = '\0';
                strcpy(pszInfoValue, szTemp);
                break;
            }
            memset(buf, 0x00, sizeof(buf));
        }
        pclose(fd);
    } 
	else 
	{
        return -3;
    }
    return 0;
}

int ndk_NetGetInterface(EM_COMM_TYPE emCommType,char *pstbuf)
{
	int ret;
	char tmp_dev_name[64];

	ret = NDK_OK;
	memset(tmp_dev_name,0x00,sizeof(tmp_dev_name));

	switch(emCommType){
		case COMM_TYPE_ETH:
			if(ndk_cpu_type()==1) {
		         if(access("/proc/sys/net/ipv4/conf/eth0",F_OK)==0){ 
				 	strcpy(pstbuf,"eth0");
		         }
		    } else {
		    	if(access("/proc/net/wlan",F_OK)==0) {
			   		if((access("/proc/sys/net/ipv4/conf/eth0",F_OK)==0) && (access("/proc/sys/net/ipv4/conf/eth1",F_OK)==0)){
						WiFiGetInfo("InterfaceName=", 14, tmp_dev_name);
						if(strcmp(tmp_dev_name,"eth1") == 0)
							strcpy(pstbuf,"eth0");
						else
							strcpy(pstbuf,"eth1");
			   		}
		   		}else
			   		if(access("/proc/sys/net/ipv4/conf/eth0",F_OK)==0){
						strcpy(pstbuf,"eth0");
			   		}
   			 }
			if((strcmp(pstbuf,"eth0")!=0) && (strcmp(pstbuf,"eth1")!= 0))
				strcpy(pstbuf,"eth0");
			break;
        case COMM_TYPE_PPP:
			strcpy(pstbuf,"ppp0");
            break;
        case COMM_TYPE_WIFI:
			 if(ndk_cpu_type() == 1) {
		        strcpy(pstbuf,"wlan0");
		    } else {
		    	ret = WiFiGetInfo("InterfaceName=", 14, pstbuf);
		    }
            break;
        default:
            ret = NDK_ERR_NET_UNSUPPORT_COMMTYPE;
			break;
		}
	return ret;
}



int ndk_NetAddHostRoute(char *dev,char *ip,char *gw)
{
	char cmd[100];
	memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"route del -net %s gw %s netmask 255.255.255.255 dev %s ",ip,gw,dev);
    system(cmd);
    sprintf(cmd,"route add -net %s gw %s netmask 255.255.255.255 dev %s ",ip,gw,dev);
    system(cmd);
	return 0;
}



int ndk_NetAddRouter(EM_COMM_TYPE emCommType,char *szDstIP)
{
	int ret;
	switch(emCommType){
		case COMM_TYPE_ETH:
			ret = SetEthRoute(szDstIP);
            break;
        case COMM_TYPE_PPP:
			ret = SetWLSRoute(szDstIP);
            break;
        case COMM_TYPE_WIFI:
           	ret = SetWIFIRoute(szDstIP);
            break;
        default:
            ret = NDK_ERR;
			break;
		}	
	return ret;
}
int ndk_NetDelRouter(EM_COMM_TYPE emCommType,char *szDstIP)
{
	char dev_name[30];
	char szCmd[128];
	char dev_name_tmp[20];

	memset(dev_name_tmp,0x00,sizeof(dev_name_tmp));
	memset(dev_name,0x00,sizeof(dev_name));
	memset(szCmd,0x00,sizeof(szCmd));
	switch(emCommType){
		case COMM_TYPE_ETH:
			ndk_NetGetInterface(COMM_TYPE_ETH,dev_name_tmp);
			strcpy(dev_name,dev_name_tmp);
            break;
        case COMM_TYPE_PPP:
			strcpy(dev_name,"ppp0");
            break;
        case COMM_TYPE_WIFI:
			ndk_NetGetInterface(COMM_TYPE_WIFI,dev_name_tmp);
           	strcpy(dev_name,dev_name_tmp);
            break;
        default:
            return NDK_ERR_NET_UNSUPPORT_COMMTYPE;
			break;
		}
  	sprintf(szCmd,"route del -net %s netmask 255.255.255.255 dev %s ",szDstIP,dev_name);
	system(szCmd);
	return NDK_OK;
}
int ndk_NetGetGateWay(char *iface,char *szGateWay)
{
    FILE *fd;
    char szLineBuf[256];
    char *pszStart, *pszEnd;

    //使用ifconfig命令获得网络配置
    if ((fd=popen("route -n", "r")) != NULL) {
        while (fgets(szLineBuf, sizeof(szLineBuf)-1, fd)!=NULL) {
            if (((pszStart=strstr(szLineBuf, "0.0.0.0"))==szLineBuf)&&(strstr(szLineBuf, iface))!=NULL) {
                pszStart += strlen("0.0.0.0");
                while(*pszStart==' ') pszStart++;
                if ((pszEnd=strchr(pszStart, ' '))!=NULL) {
                    *pszEnd = '\0';
                }

                strcpy(szGateWay, pszStart);
                pclose(fd);
                return NDK_OK;
            }
        }
        pclose(fd);
    }

    return NDK_ERR;
}
static int ndk_NetGetPPP0GateWay(char *szGateWay)
{
    int inet_sock;
    struct ifreq ifr;

    memset(&ifr,0,sizeof(struct ifreq));
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    //eth0为接口到名称
    strcpy(ifr.ifr_name, "ppp0");
    //SIOCGIFADDR标志代表获取接口地址
    if (ioctl(inet_sock, SIOCGIFDSTADDR, &ifr)!=0) {
        //perror("ioctl SIOCGIFADDR");
        close(inet_sock);
        return -1;
    }
    //fprintf(stderr,"%s\n",inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_netmask))->sin_addr));
    sprintf(szGateWay,"%s", inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr));
    if(strstr(szGateWay,"0.0.0.0")) {
        close(inet_sock);
        return -1;
    }
    close(inet_sock);

    return 0;
}
/**
*@fn        int __getNetAddr(char *devstr, char *szAddrName, char *szAddrInfo)
*@brief     通过ifconfig命令获取网络地址
*@param     @li devstr      设备字符串信息  例如："eth0","ppp0","lo"等
            @li szAddrName  地址信息名称 ；例如："inet addr:"
            @li szAddrInfo  地址信息字符串：例如："192.168.1.5"
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int __getNetAddr(char *devstr, char *szAddrName, char *szAddrInfo)
{
    FILE *fd;
    char szLineBuf[256];
    char *pszStart, *pszEnd;

    //使用ifconfig命令获得网络配置
    sprintf(szLineBuf, "sudo /sbin/ifconfig %s", devstr);
    TRACE_ETH("eth0_cmd：%s\n", szLineBuf);
    if ((fd=popen(szLineBuf, "r")) != NULL) {
        while (fgets(szLineBuf, sizeof(szLineBuf)-1, fd)!=NULL) {
            if ((pszStart=strstr(szLineBuf, szAddrName))!=NULL) {
                pszStart += strlen(szAddrName);
                if ((pszEnd=strchr(pszStart, ' '))!=NULL) { //字段结束符
                    *pszEnd = '\0';
                } else if ((pszEnd=strchr(pszStart, '\n'))!=NULL) { //行结束符
                    *pszEnd = '\0';
                }

                strcpy(szAddrInfo, pszStart);
                pclose(fd);
                return NDK_OK;
            }
        }
    }

    pclose(fd);
    return NDK_ERR;
}


int SetWIFIRoute(char *szDestIP)
{
    char gateway[16];
    char szCmd[128];
    char sBuf[128]= {0};
    int ret;
	char eth_ip[64],eth_mask[64];
	char dev_name[20];

	memset(dev_name,0x00,sizeof(dev_name));
	memset(eth_ip,0x00,sizeof(eth_ip));
	memset(eth_mask,0x00,sizeof(eth_mask));
    memset(szCmd,0,sizeof(szCmd));
    memset(sBuf,0,sizeof(sBuf));

    if(ndk_cpu_type()==1) {
        strcpy(dev_name,"wlan0");
    } else {
    	WiFiGetInfo("InterfaceName=", 14, dev_name);
    }
    ret=NDK_WiFiIsConnected();
    if(ret==0) {
        ret=ndk_NetGetGateWay(dev_name,gateway);
        if(ret<0) {
            return NDK_ERR_NET_GATEWAY;
        }
		ret = __getNetAddr(dev_name,"inet addr:",eth_ip);
		if(ret < 0)
			return NDK_ERR_NET_GETADDR;
		
		ret = __getNetAddr(dev_name,"Mask:",eth_mask);
		if(ret < 0)
			return NDK_ERR_NET_GETADDR;
		if((inet_addr(eth_ip) & inet_addr(eth_mask)) == (inet_addr(szDestIP) & inet_addr(eth_mask)))
			return 0;
        sprintf(szCmd,"route del -net %s gw %s netmask 255.255.255.255 dev %s ",szDestIP,gateway,dev_name);
        system(szCmd);
        sprintf(szCmd,"route add -net %s gw %s netmask 255.255.255.255 dev %s ",szDestIP,gateway,dev_name);
        system(szCmd);
    }
    return 0;
}

int SetEthRoute(char *szDestIP)
{
    char gateway[16];
    char szCmd[128];
    char sBuf[128]= {0};
	char dev_name[64] = {0};
	char tmp_dev_name[64] = {0};
    int ret,eth_flag;
	char eth_ip[64],eth_mask[64];
	
    memset(szCmd,0,sizeof(szCmd));
    memset(sBuf,0,sizeof(sBuf));
	memset(eth_ip,0x00,sizeof(eth_ip));
	memset(eth_mask,0x00,sizeof(eth_mask));

	eth_flag = -1;
	memset(dev_name,0x00,sizeof(dev_name));
	memset(tmp_dev_name,0x00,sizeof(tmp_dev_name));
    if(ndk_cpu_type()==1) {
         if(access("/proc/sys/net/ipv4/conf/eth0",F_OK)==0){ 
		 	strcpy(dev_name,"eth0");
		 	eth_flag = 0;
         }
    } else {
    	if(access("/proc/net/wlan",F_OK)==0) {
	   		if((access("/proc/sys/net/ipv4/conf/eth0",F_OK)==0) && (access("/proc/sys/net/ipv4/conf/eth1",F_OK)==0))
				WiFiGetInfo("InterfaceName=", 14, tmp_dev_name);
			if(strcmp(tmp_dev_name,"eth1") == 0)
				strcpy(dev_name,"eth1");
			else
				strcpy(dev_name,"eth0");
			eth_flag = 0;
   		}else
	   		if(access("/proc/sys/net/ipv4/conf/eth0",F_OK)==0){
				strcpy(dev_name,"eth0");
				eth_flag = 0;
	   		}
    }
    if((eth_flag == 0)) {
        ret=ndk_NetGetGateWay(dev_name,gateway);
        if(ret<0) {
            return NDK_ERR_NET_GATEWAY;
        }
		ret = __getNetAddr(dev_name,"inet addr:",eth_ip);
		if(ret < 0)
			return NDK_ERR_NET_GETADDR;
		
		ret = __getNetAddr(dev_name,"Mask:",eth_mask);
		if(ret < 0)
			return NDK_ERR_NET_GETADDR;
		if((inet_addr(eth_ip) & inet_addr(eth_mask)) == (inet_addr(szDestIP) & inet_addr(eth_mask))){
			return 0;
		}
		memset(szCmd,0,sizeof(szCmd));
        sprintf(szCmd,"route del -net %s gw %s netmask 255.255.255.255 dev %s ",szDestIP,gateway,dev_name);
        system(szCmd);
		memset(szCmd,0,sizeof(szCmd));
        sprintf(szCmd,"route add -net %s gw %s netmask 255.255.255.255 dev %s ",szDestIP,gateway,dev_name);
        system(szCmd);
    }
    return 0;
}
int SetWLSRoute(char *szDestIP)
{
	int ret;
    char gateway[16];
    char szCmd[128];
    memset(szCmd,0,sizeof(szCmd));

    if(access("/proc/sys/net/ipv4/conf/ppp0",F_OK)==0) {

        ret=ndk_NetGetPPP0GateWay(gateway);
        if(ret<0) {
            return NDK_ERR_NET_GATEWAY;
        }
        sprintf(szCmd,"route del -net %s gw %s netmask 255.255.255.255 dev ppp0 ",szDestIP,gateway);
        system(szCmd);
        sprintf(szCmd,"route add -net %s gw %s netmask 255.255.255.255 dev ppp0 ",szDestIP,gateway);
        system(szCmd);
    }
    return 0;
}

int ndk_GetNetAddr(char *interf,char *szIPAddr, char *szMask, char *szGatewayAddr)
{
    if (NULL != szIPAddr) {
        if (__getNetAddr(interf, "inet addr:", szIPAddr)!=0) {
            return NDK_ERR_NET_GETADDR;
        }
        if (strlen(szIPAddr)==0) {
            return NDK_ERR_NET_ADDRILLEGAL;
        }
    }

    if (NULL != szMask) {
        if (__getNetAddr(interf, "Mask:", szMask)!=0) {
            return NDK_ERR_NET_GETADDR;
        }
        if (strlen(szMask)==0) {
            return NDK_ERR_NET_ADDRILLEGAL;
        }
    }

    if (NULL != szGatewayAddr) {
        if(strstr(interf,"ppp0")!=NULL) {
            if (__getNetAddr(interf, "P-t-P:", szGatewayAddr)!=0) {
                return NDK_ERR_NET_GATEWAY;
            }
            if (strlen(szMask)==0) {
                return NDK_ERR_NET_ADDRILLEGAL;
            }
        } else {
            if (ndk_NetGetGateWay(interf,szGatewayAddr)!=0) {
                return NDK_ERR_NET_GATEWAY;
            }
            if (strlen(szGatewayAddr)==0) {
                return NDK_ERR_NET_ADDRILLEGAL;
            }
        }
    }

    return NDK_OK;
}

