/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：    产品开发部
* 日    期：    2012-08-17
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/sockios.h>

#include "Ethernet.h"
#include "../public/config.h"
#include "hwcfginfo.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_Ethernet.h"
#include "../public/delay.h"
#include "devmgr.h"


#define DEV_CLASS_MACHINE       "machine"
#define DHCP_INFO   "/tmp/udhcpc_info"
static char g_interface[4+1];
static int first_eth_open = 1;  //0:不是;1:是
static int if_mac_set = 0;
const char *ETHERNET_UP_INFO = "UP BROADCAST RUNNING MULTICAST";

extern int NDK_FsDel(const char * pszName);
extern int NDK_FsExist(const char * pszName);
extern int ndk_ifunloaddrv(char *driver);
extern int __getNetAddr(char *devstr, char *szAddrName, char *szAddrInfo);


/**
*@fn        int if_pid_exist(char *p_pidname)
*@brief     判断pid是否存在
*@param     @li p_pidname   pid进程名，最好带执行路径和参数。
                            说明：该函数不能获取grep进程是否存在。
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_if_pid_exist(char *p_pidname)
{
    FILE *fd;
    char szLineBuf[1024], cmd[256];

    if(NULL == p_pidname) {
        return NDK_ERR_PARA;
    }

    sprintf(cmd, "ps |grep %s", p_pidname);
    TRACE_ETH("eth0_cmd：%s\n", cmd);

    if ((fd=popen(cmd, "r")) != NULL) {
        while (fgets(szLineBuf, sizeof(szLineBuf)-1, fd) != NULL) {
            //能找到输入的进程名称，但又不是由grep命令获取到的。
            if (strstr(szLineBuf, p_pidname) && !strstr(szLineBuf, "grep")) {
                pclose(fd);
                return 1;
            }
        }
    }

    pclose(fd);
    return NDK_OK;
}


/**
*@fn        static int __ReadDhcpAddr(char *szIPAddr, char *szMask, char *szGatewayAddr)
*@brief     将静态地址从临时文件中读出
*@param     @li szIPAddr        本地IP地址
            @li szMask          子网掩码
            @li szGatewayAddr   网关
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_getNetAddrCfg(char *pszIPAddr, char *pszMask, char *pszGatewayAddr)
{
    int ret;
    char *pszIpTmp;
    char *pszMaskTmp;
    char *pszGwTmp;

    ret = ndk_getconfig("eth","ipaddr", CFG_STRING, &pszIpTmp);
    ret += ndk_getconfig("eth","netmask", CFG_STRING, &pszMaskTmp);
    ret += ndk_getconfig("eth","gateway", CFG_STRING, &pszGwTmp);
    if (ret>=0) {
        TRACE_ETH("szIPAddr=%s szMask=%s szgw=%s\n", pszIpTmp, pszMaskTmp, pszGwTmp);
        if ((!strlen(pszIpTmp)) || (!strlen(pszMaskTmp)) || (!strlen(pszGwTmp))) {
            return NDK_ERR_PARA;
        } else {
            strcpy(pszIPAddr, pszIpTmp);
            strcpy(pszMask, pszMaskTmp);
            strcpy(pszGatewayAddr, pszGwTmp);
        }
    }

    TRACE_ETH("__getNetAddrCfg ret=%d\n", ret);
    return ret;
}


/**
*@fn        static int __SetNetAddrCfg(const char *szIPAddr, const char *szMask, const char *szGatewayAddr)
*@brief     将静态地址写入硬件配置文件
*@param     @li szIPAddr        本地IP地址
            @li szMask          子网掩码
            @li szGatewayAddr   网关
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_SetNetAddrCfg(const char *szIPAddr, const char *szMask, const char *szGatewayAddr)
{
    int ret = 0;

    if (NULL != szIPAddr) {
        ret += ndk_setconfig("eth","ipaddr", CFG_STRING, szIPAddr);
    }

    if (NULL != szMask) {
        ret +=ndk_setconfig("eth","netmask", CFG_STRING, szMask);
    }

    if (NULL != szGatewayAddr) {
        ret +=ndk_setconfig("eth","gateway", CFG_STRING, szGatewayAddr);
    }

    return ret;
}


/**
*@fn        static int __is_load_eth0(void)
*@brief     判断是否加载以太网驱动
*@param     @li 无
*@return    @li TRUE    已加装
            @li FALSE   未加装?
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_is_load_eth0(void)
{
    TRACE_ETH("eth0_cmd: lsmod | grep dm9000\n");

    /*   sp60 bcm5892 cpu 走不同分支       */
    if(ndk_cpu_type() == 1) {
        if (ndk_ifunloaddrv("bcm589X_mac")==0) {
            return TRUE;
        }
        return FALSE;

    } else {
        if (ndk_ifunloaddrv("dm9000")==0) {
            return TRUE;
        }
        return FALSE;
    }
}


/**
*@fn        static int __GetEth0NetAddr(char *szIPAddr, char *szMask, char *szGatewayAddr)
*@brief     将地址从以太网设备中读出
*@param     @li szIPAddr        本地IP地址
            @li szMask          子网掩码
            @li szGatewayAddr   网关
*@return    @li 0       成功
            @li -4      获取本地地址或子网掩码失败
            @li -5      获取网关地址失败
            @li -6      获取地址格式错误
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
static int ndk_GetEth0NetAddr(char *szIPAddr, char *szMask, char *szGatewayAddr)
{
    char dev_name[20];
    memset(dev_name,0x00,sizeof(dev_name));
    ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
    if (NULL != szIPAddr) {
        if (__getNetAddr(dev_name, "inet addr:", szIPAddr)!=0) {
            return NDK_ERR_NET_GETADDR;
        }
        if (strlen(szIPAddr)==0) {
            return NDK_ERR_NET_ADDRILLEGAL;
        }
    }

    if (NULL != szMask) {
        if (__getNetAddr(dev_name, "Mask:", szMask)!=0) {
            return NDK_ERR_NET_GETADDR;
        }
        if (strlen(szMask)==0) {
            return NDK_ERR_NET_ADDRILLEGAL;
        }
    }

    if (NULL != szGatewayAddr) {
        if (ndk_NetGetGateWay(dev_name,szGatewayAddr)!=0) {
            return NDK_ERR_NET_GATEWAY;
        }
        if (strlen(szGatewayAddr)==0) {
            return NDK_ERR_NET_ADDRILLEGAL;
        }
    }

    return NDK_OK;
}

/**
*@fn        static int unload_eth_mod(void)
*@brief     卸载以太网驱动
*@param     @li 无
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int unload_eth_mod(void)
{
    TRACE_ETH("eth0_cmd: modprobe -r ETH\n");

    unload_driver_by_class(DEV_CLASS_ETHERNET);

    first_eth_open = 1;
    //同时删除dhcp的临时文件
    remove(DHCP_ADDR_FILE);
    return NDK_OK;
}

/**
*@fn        int exec_dhcp(void)
*@brief     使用dhcp获取网络地址
*@param     @li 无
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_exec_dhcp(void)
{
    char szLineBuf[256];
    int ret = NDK_OK;
    int i;
    char cmd[256];
    char Ne2k_MacAddress[18];
    char dev_name[20];
    int eth_fd;
    char szTempIP[30],szServerIP[30];

    //如果驱动未加载，直接加载
    if (!ndk_is_load_eth0()) {
        if (ndk_load_eth_mod()<0) {
            return NDK_ERR;
        }
        memset(dev_name,0x00,sizeof(dev_name));
        ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
        //获取MAC参数
        if(ndk_getMACAddrCfg(Ne2k_MacAddress)<0) {
            //打开失败后，卸载驱动
            unload_eth_mod();
            return NDK_ERR;
        }

        //设置mac地址
        if (first_eth_open) {
            sprintf(cmd, "sudo /sbin/ifconfig %s hw ether %s", dev_name,Ne2k_MacAddress);
            TRACE_ETH("eth0_cmd: %s\n", cmd);
            if (system(cmd)) {
                unload_eth_mod();
                return NDK_ERR;
            }
            if_mac_set = 1;
            first_eth_open = 0;
        }
    } else {
        memset(dev_name,0x00,sizeof(dev_name));
        ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
        if (if_mac_set == 0) {
            if(ndk_getMACAddrCfg(Ne2k_MacAddress)<0) {
                //打开失败后，卸载驱动
                unload_eth_mod();
                return FALSE;
            }
            sprintf(cmd, "sudo /sbin/ifconfig %s hw ether %s",dev_name, Ne2k_MacAddress);
            TRACE_ETH("eth0_cmd: %s\n", cmd);
            system(cmd);
            if_mac_set = 1;
        }
    }

    //启动dhcp服务，并判断服务是否成功
    for (i = 1; i > 0; i--) {
        if (ndk_if_pid_exist("udhcpc")) {
            TRACE_ETH("eth0_cmd：sudo /usr/bin/killall udhcpc\n");
            system("sudo /usr/bin/killall udhcpc");
        }

        sprintf(szLineBuf, "sudo /sbin/udhcpc -i %s -s /usr/share/udhcpc/eth.script -n -b > /tmp/udhcpc_info",dev_name);
        system(szLineBuf);
        if ((!system("grep 'Sending select for' /tmp/udhcpc_info > /dev/null"))
            && (!system("grep 'obtained' /tmp/udhcpc_info > /dev/null"))) {
            break;
        }
    }
    //system("sudo rm /tmp/udhcpc_info"); //判断完删除
    if(remove("/tmp/udhcpc_info") != 0)
        fprintf(stderr,"ndk_exec_dhcp remove udhcpc_info err\n");
    if (0==i) {
        fprintf(stderr,"dhcp err now \n");
        //服务启动失败，退出
        NDK_LOG_ERR(NDK_LOG_MODULE_ETH,"dhcp fail.\n");
        TRACE_ETH("dhcp fail.\n");
        if (ndk_if_pid_exist("udhcpc")) {
            TRACE_ETH("eth0_cmd：sudo /usr/bin/killall udhcpc\n");
            system("sudo /usr/bin/killall udhcpc");
        }

        return NDK_ERR;
    }
    if(access(ETH_ROUTE_TABLE_CONFIG_FILE,F_OK)==0) {
        eth_fd=fopen(ETH_ROUTE_TABLE_CONFIG_FILE,"r");
        memset(szTempIP,0,sizeof(szTempIP));
        while (fgets(szTempIP, sizeof(szTempIP)-1, eth_fd) != NULL) {
            memset(szServerIP,0,sizeof(szServerIP));
            strncpy(szServerIP,szTempIP,strlen(szTempIP)-2);
            ndk_NetAddRouter(COMM_TYPE_ETH,szServerIP);
        }
        fclose(eth_fd);
    }
    return NDK_OK;
    //将动态地址写入临时文件，供后续查询使用,该功能关闭，后期有需要再开放
    //    ret = __WriteDhcpAddr(sIPaddr, smask, sgateway);
}


/**
*@fn        static int __getMACAddrCfg(char *szMac)
*@brief     将MAC地址从配置文件中读出
*@param     @li szMac   地址缓冲区
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_getMACAddrCfg(char *szMac)
{
    char *szBuf;
    if(ndk_getconfig("eth", "mac", CFG_STRING, &szBuf) < 0) {
        return NDK_ERR;
    } else {
        strcpy(szMac, szBuf);
        return NDK_OK;
    }
}
/**
*@fn        static int __getMACAddrCfg(char *szMac)
*@brief     设置MAC地址
*@param     @li szMac   地址缓冲区
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_setMACAddrCfg(char *szMac)
{
    char *szBuf,cmd[256] = {0};
    char dev_name[20];

    memset(dev_name,0x00,sizeof(dev_name));
    if(szMac==NULL)
        return NDK_ERR;
    if(ndk_load_eth_mod()<0)
        return NDK_ERR;
    ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
    sprintf(cmd, "sudo /sbin/ifconfig %s hw ether %s", dev_name,szMac);
    if(system(cmd))
        return NDK_ERR;
    if(ndk_getconfig("eth", "mac", CFG_STRING, &szBuf)==0&&strcmp(szBuf,szMac)==0) {
        return NDK_OK;
    }
    if(ndk_setconfig("eth", "mac", CFG_STRING, szMac) < 0) {
        return NDK_ERR;
    }
    return NDK_OK;
}


/**
*@fn        int GetETHAddress(char *ip, char *netmask, char *gw)
*@brief     获取网络地址
*@param     @li ip      IP地址
            @li netmask 映射地址
            @li gw      网关地址
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_GetETHAddress(char *ip, char *netmask, char *gw)
{
    return ndk_GetEth0NetAddr(ip, netmask, gw);
}


/**
*@fn        static int load_eth_mod(void)
*@brief     加载以太网驱动
*@param     @li 无
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_load_eth_mod(void)
{
    TRACE_ETH("eth0_cmd: modprobe ETH\n");

    load_driver_by_class(DEV_CLASS_ETHERNET);
    first_eth_open = 1;
    return NDK_OK;

}


/**
*@fn        static int __eth_open(void)
*@brief     打开以太网设备
*@param     @li 无
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
static int __eth_open(void)
{
    //如果驱动已加载，直接返回成功
    if (ndk_is_load_eth0()==FALSE) {
        //加载驱动
        if (ndk_load_eth_mod()<0) {
            return NDK_ERR_OPEN_DEV;
        }
    }

    return NDK_OK;
}


/**
*@fn        int _EthOpen(void)
*@brief     打开以太网设备
*@param     @li 无
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_EthOpen(void)
{
    return __eth_open();
}


/**
*@fn        int EthClose(void)
*@brief     关闭以太网设备
*@param     @li 无
*@return    @li 0       成功
            @li -1      失败
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int _EthClose(void)
{
    if (!ndk_is_load_eth0()) {
        return TRUE;
    }

    //加载驱动
    if (unload_eth_mod()<0) {
        return FALSE;
    }

    return TRUE;
}

/**
*@fn    int ndk_if_mac_set(void)
*@brief     判断是否做过MAC地址设置
*@param     @li 无
*@return    @li     1   未设
        @li   0 已设
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_if_mac_set(void)
{
    return first_eth_open;
}

/*设置MAC地址标志*/
void ndk_mac_set(void)
{
    first_eth_open = 0;
}

int ndk_if_dhcp_open(void)
{
    if(NDK_FsExist(DHCP_INFO) == NDK_OK)
        return 1;
    else
        return 0;
}

void ndk_dhcp_kill(void)
{
    NDK_FsDel(DHCP_INFO);
}

bool ndk_isEthUp()
{
    FILE *fd;
    char cmd[256] = {0},dev_name[20] = {0},ethstatefile[256] = {0},szLineBuf[256] = {0};
    int flag = 0;

    ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
    sprintf(ethstatefile,"/sys/class/net/%s/operstate",dev_name);
    if(access(ethstatefile,F_OK)!=0)
        return false;
    if ((fd=fopen(ethstatefile, "r")) != NULL) {
        memset(szLineBuf, 0x00, sizeof(szLineBuf));
        while (fgets(szLineBuf, sizeof(szLineBuf)-1, fd) != NULL) {
            if (strstr(szLineBuf, "down") != NULL) {
                flag = 0;
                break;
            } else {
                flag = 1;
                break;
            }
            memset(szLineBuf, 0x00, sizeof(szLineBuf));
        }
        fclose(fd);
    } else {
        return false;
    }
    if (flag)
        return true;
    return false;
}
typedef enum {
    Destination,
    Gateway,
    Genmask
} ROWID;
char *getAddrByRowId(ROWID rowid,const char *AddrStr)
{
    char addrstr[256] = {0},tmpstr[256] = {0};
    char *delim=" ";
    char *p = addrstr;
    int count = 0;

    if(AddrStr==NULL||rowid<0||rowid>2)
        return NULL;
    strcpy(tmpstr,AddrStr);
    p=strtok(tmpstr,delim);
    if(rowid==Destination) {
        return p;
    }
    count++;
    while((p=strtok(NULL,delim))) {
        if(rowid==count) {
            break;
        }
        count++;
    }

    return p;
}

#define ETHTOOL_GLINK 0x0000000a /* Get link status (ethtool_value) */
/* for passing single values */
struct ethtool_value {
    uint  cmd;
    uint  data;
};
/**
 *@brief    判断以太网的连接状态
 *@return
 *@li   1 -- interface link up   0 -- interface link down        -1 --ioctl err(驱动未加载)
*/
int ndk_EthGetLink(void)
{
    int skfd;
    struct ifreq ifr;
    struct ethtool_value edata;
    char dev_name[20] = {0};

    edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;
    ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
    memset(&ifr, 0,sizeof(ifr));

    strncpy(ifr.ifr_name, dev_name,sizeof(ifr.ifr_name)- 1);
    ifr.ifr_data =(char *) &edata;

    if (( skfd= socket(AF_INET, SOCK_DGRAM, 0 )) < 0)
        return -1;
    if(ioctl( skfd, SIOCETHTOOL,&ifr ) == -1) {
        close(skfd);
        return -2;
    }

    close(skfd);
    return edata.data;
}

int Is_Eth_PullOut(const char *hostaddr)
{
    FILE *fd;
    char cmd[256] = {0},dev_name[20] = {0},szLineBuf[256] = {0},ethstatefile[256] = {0},tmpipstr[INET_ADDRSTRLEN],tmpipstr1[INET_ADDRSTRLEN];
    int flag = 0,firsttime = 0;
    unsigned long test_ip,test_ip1;
    int test_netmask;
    char *p=NULL;


    if(hostaddr==NULL)
        return -1;

    ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
    sprintf(ethstatefile,"/sys/class/net/%s/operstate",dev_name);
    if(access(ethstatefile,F_OK)!=0)
        return -2;

    sprintf(cmd,"%s > /tmp/.tmpinfo","sudo /sbin/route -n");
    system(cmd);
    if(access("/tmp/.tmpinfo",F_OK)==-1) {
        return -3;
    }
    if ((fd=fopen("/tmp/.tmpinfo", "r")) != NULL) {
        while(fgets(szLineBuf, sizeof(szLineBuf)-1, fd) != NULL) {
            if(strchr(szLineBuf,'.')!=NULL) {//筛选路由表信息，去掉头两行
                if(strstr(szLineBuf,hostaddr)!=NULL) {//首先判断主机地址是否有相关的路由信息设置。
                    if(strstr(szLineBuf,dev_name)!=NULL) {//判断是否是以太网对应的路由信息
                        if(!ndk_isEthUp()) {//判断以太网是否down状态
                            flag=1;
                        }
                    } else {
                        break;
                    }
                } else {
                    if(strcmp(getAddrByRowId(Genmask,szLineBuf),"0.0.0.0")!=0) {//判断是否默认路由
                        inet_pton(AF_INET,hostaddr,(void *)&test_ip);
                        inet_pton(AF_INET,getAddrByRowId(Destination,szLineBuf),(void *)&test_ip1);
                        inet_pton(AF_INET,getAddrByRowId(Genmask,szLineBuf),(void *)&test_netmask);
                        test_ip=test_ip&test_netmask;
                        test_ip1=test_ip1&test_netmask;
                        inet_ntop(AF_INET, (void *)&test_ip, tmpipstr, INET_ADDRSTRLEN);
                        if((p=strrchr(tmpipstr,'.'))!=NULL)
                            *p='\0';
                        inet_ntop(AF_INET, (void *)&test_ip1, tmpipstr1, INET_ADDRSTRLEN);
                        if((p=strrchr(tmpipstr1,'.'))!=NULL)
                            *p='\0';
                        if(strcmp(tmpipstr,tmpipstr1)==0) {//比较路由信息所在网段和主机地址(hostaddr)所在的网段，看是否属于同一网段。
                            if(strstr(szLineBuf,dev_name)!=NULL) {
                                if(!ndk_isEthUp()) {
                                    flag=1;
                                }
                            } else {
                                break;
                            }
                        }
                    } else {
                        if(!firsttime&&strstr(szLineBuf,dev_name)!=NULL) {//如果是默认路由信息，并且有多条默认路由时，则按默认路由顺序，如果以太网顺序在最前面则判断路由走以太网。
                            if(!ndk_isEthUp()) {
                                flag=1;
                            }
                        }
                        firsttime++;
                    }
                }
            }
            memset(szLineBuf, 0x00, sizeof(szLineBuf));
        }
    }
    fclose(fd);
    return flag;
}


