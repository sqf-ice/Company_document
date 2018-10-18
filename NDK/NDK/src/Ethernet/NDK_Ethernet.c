/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：
* 日    期：    2012-08-27
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
*/
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

#include "NDK.h"
#include "NDK_Net.h"
#include "NDK_debug.h"
#include "Ethernet.h"
#include "../public/config.h"
#include "NDK_Ethernet.h"


/**
 *@brief    设置网络地址,参数都为NULL返回参数错误(设置后如果原已动态分配的地址,也会被修改)
 *@param    pszIp   本地IP 地址字符串的首指针,为NULL则不设IP地址.仅支持IPV4协议
 *@param    pszMask 本地子网掩码字符串的首指针,为NULL则不设Mask地址.仅支持IPV4协议
 *@param    pszGateway  本地网关地址字符串的首指针,为NULL则不设Gateway地址.仅支持IPV4协议
 *@param    pszDns  本地DNS服务器IP地址,为NULL则不设DNS地址,最多可设置3个DNS,之间以分号符';'隔开
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(地址为NULL)
 *@li   NDK_ERR_OPEN_DEV    打开设备文件错误
 *@li   NDK_ERR 操作失败
 *@li   NDK_ERR_ADDR_ILLEGAL    获取地址格式错误
*/
NEXPORT int NDK_EthSetAddress(const char *pszIp, const char *pszMask, const char *pszGateway, const char *pszDns)
{
    char sIPaddr[16];
    char smask[16];
    char sgateway[16];
    char cmd[512] = {0}, szDNS[16*3] = {0}, szMac[18] = {0};
    int ret;
    int eth_fd;
    char szTempIP[30],szServerIP[30],dev_name[20];

    if ((NULL == pszIp) && (NULL == pszGateway) && (NULL == pszMask) && (NULL == pszDns)) {
        return NDK_ERR_PARA;
    }
    if ((NULL != pszIp) && ((strlen(pszIp)>=16) || (inet_pton(AF_INET, pszIp, cmd) == 0))) {
        return NDK_ERR_PARA;
    }
    if ((NULL != pszMask) && ((strlen(pszMask)>=16) || (inet_pton(AF_INET, pszMask, cmd) == 0))) {
        return NDK_ERR_PARA;
    }
    if ((NULL != pszGateway) && ((strlen(pszGateway)>=16) || (inet_pton(AF_INET, pszGateway, cmd) == 0))) {
        return NDK_ERR_PARA;
    }

    //system("sudo /usr/bin/killall udhcpc");     /**<关闭DHCP*/

    if (ndk_getNetAddrCfg(sIPaddr, smask, sgateway)>=0) {
        /**<判断地址是否一致，与原保存地址一致，则不更新地址*/
        if (NULL != pszIp) {
            if (strcmp(sIPaddr, pszIp)) {
                ndk_setconfig("eth","ipaddr", CFG_STRING, pszIp);
            }
        }

        if (NULL != pszMask) {
            if (strcmp(smask, pszMask)) {
                ndk_setconfig("eth","netmask", CFG_STRING, pszMask);
            }
        }

        if (NULL != pszGateway) {
            if (strcmp(sgateway, pszGateway)) {
                ndk_setconfig("eth","gateway", CFG_STRING, pszGateway);
            }
        }
    } else {
        ndk_SetNetAddrCfg(pszIp, pszMask, pszGateway);
    }

    /**<打开以太网*/
    ret = ndk_EthOpen();
    if (ret < 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_ETH,"eth open fail.\n");
        return ret;
    }
    memset(dev_name,0x00,sizeof(dev_name));
    ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
    if ((NULL != pszIp) && (NULL != pszMask) && (NULL != pszGateway)) {
        if (ndk_if_mac_set() == 1) {
            if(ndk_getMACAddrCfg(szMac)<0) {
                //打开失败后，卸载驱动
                unload_eth_mod();
                return NDK_ERR;
            }
            sprintf(cmd, "sudo /sbin/ifconfig %s hw ether %s %s netmask %s\n", dev_name,szMac, pszIp, pszMask);
            ndk_mac_set();
        } else {
            sprintf(cmd, "sudo /sbin/ifconfig %s %s netmask %s\n",dev_name, pszIp,pszMask);
        }
        //strcat(cmd,"sudo /sbin/route del default dev %s\n",dev_name);
     
        sprintf(cmd+strlen(cmd), "sudo /sbin/route del default dev %s\n",dev_name);
        if((inet_addr(pszIp)&inet_addr(pszMask))!=(inet_addr(pszMask)&inet_addr(pszGateway)))
            sprintf(cmd+strlen(cmd), "sudo /sbin/route add %s netmask 0.0.0.0 dev %s\n",pszGateway,dev_name);
        sprintf(cmd+strlen(cmd), "sudo /sbin/route add default gw %s dev %s\n", pszGateway,dev_name);
        TRACE_ETH("eth0_cmd1: %s\n", cmd);
        if (system(cmd)) {
            unload_eth_mod();
            return NDK_ERR;
        }
    } else {
        /**<设置IP地址*/
        if (NULL != pszIp) {
            sprintf(cmd, "sudo /sbin/ifconfig %s %s", dev_name,pszIp);
            TRACE_ETH("eth0_cmd1: %s\n", cmd);
            if (system(cmd)) {
                unload_eth_mod();
                return NDK_ERR;
            }
        }

        /**<设置子网掩码*/
        if (NULL != pszMask) {
            sprintf(cmd, "sudo /sbin/ifconfig %s netmask %s", dev_name,pszMask);
            TRACE_ETH("eth0_cmd1: %s\n", cmd);
            if (system(cmd)) {
                unload_eth_mod();
                return NDK_ERR;
            }
        }

        /**<设置路由*/
        if (NULL != pszGateway) {
            TRACE_ETH("eth0_cmd1: sudo /sbin/route del default dev %s\n",dev_name);
            sprintf(cmd, "sudo /sbin/route del default dev %s",dev_name);
            system(cmd);
            if(NULL!=pszIp){
                if((inet_addr(pszIp)&inet_addr(pszMask))!=(inet_addr(pszMask)&inet_addr(pszGateway))){
                    sprintf(cmd, "sudo /sbin/route add %s netmask 0.0.0.0 dev %s", pszGateway,dev_name);
                    if (system(cmd)) {
                        unload_eth_mod();
                        return NDK_ERR;
                    }
                }
            }
            sprintf(cmd, "sudo /sbin/route add default gw %s dev %s", pszGateway,dev_name);
            TRACE_ETH("eth0_cmd1: %s\n", cmd);
            if (system(cmd)) {
                unload_eth_mod();
                return NDK_ERR;
            }
        }
    }

    if (NULL != pszDns) {
        strcpy(szDNS, pszDns);
        ret = ndk_NetLink(PATH_RESOLV_ETH,PATH_RESOLV_TMP);
        if(ret < 0)
            return NDK_ERR;
        ret = ndk_Add_DNS_IP(szDNS);    /**<增加DNS服务器*/
        if (ret < 0) {
            return ret;
        }
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
    NDK_COMM_LOG_INFO(NDK_LOG_MODULE_ETH,"%s succ,current eth ip=%s,mask=%s,gateway=%s,dns=%s\n",__func__,pszIp,pszMask,pszGateway,pszDns);
    return NDK_OK;
}


/**
 *@brief    获取网络MAC地址
 *@retval   pszMacAddr  返回MAC地址
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszMacAddr为NULL)
 *@li   NDK_ERR             操作失败(获取mac地址失败)
*/
NEXPORT int NDK_EthGetMacAddr(char *pszMacAddr)
{
    int ret;
    if ((NULL == pszMacAddr)) {
        return NDK_ERR_PARA;
    }
    ret=ndk_getMACAddrCfg(pszMacAddr);
    if(ret==NDK_OK)
        NDK_COMM_LOG_INFO(NDK_LOG_MODULE_ETH,"%s succ,set mac=%s\n",__func__,pszMacAddr);
    return ret;
}
/**
 *@brief    设置网络MAC地址
 *@param    pszMacAddr  要设置的MAC地址
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszMacAddr为NULL)
*/
NEXPORT int NDK_EthSetMacAddr(char *pszMacAddr)
{
    if ((NULL == pszMacAddr)) {
        return NDK_ERR_PARA;
    }

    return ndk_setMACAddrCfg(pszMacAddr);
}


/**
 *@brief    获取网络地址,参数都为NULL返回参数错误
 *@retval   pszIp   返回IP地址,为NULL则不取IP地址
 *@retval   pszGateway  返回网关地址,为NULL则不取Gateway地址
 *@retval   pszMask 返回子网掩码,为NULL则不取Mask地址
 *@retval   pszDns  DNS服务器IP地址,为NULL则不取DNS地址,一次性取出所以DNS,之间以分号符';'隔开
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_GET_NETADDR     获取本地地址或子网掩码失败
 *@li   NDK_ERR     操作失败(获取DNS失败)
*/
NEXPORT int NDK_EthGetNetAddr(char *pszIp, char *pszMask, char *pszGateway, char *pszDns)
{
    char szIp[16] = {0}, szMask[16] = {0}, szGateway[16] = {0};
    int ret = -1;

    if ((NULL == pszIp) && (NULL == pszGateway) && (NULL == pszMask) && (NULL == pszDns)) {
        return NDK_ERR_PARA;
    }

    if (ndk_GetETHAddress(szIp, szMask, szGateway) != 0) {
        return NDK_ERR_NET_GETADDR;
    }

    if (NULL != pszDns) {
        ret = ndk_NetLink("/tmp/eth_resolv.conf","/tmp/resolv.conf");
        if(ret < 0)
            return NDK_ERR;
        ret = ndk_Get_DNS_IP(pszDns);
        if (ret < 0) {
            return ret;
        }
    }

    if(NULL != pszIp)
        strncpy(pszIp, szIp, sizeof(szIp));
    if(NULL != pszMask)
        strncpy(pszMask, szMask, sizeof(szMask));
    if(NULL != pszGateway)
        strncpy(pszGateway, szGateway, sizeof(szGateway));

    return NDK_OK;
}


/**
 *@brief    使用dhcp获取网络地址
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR     操作失败(以太网驱动加载失败等)
 *@li   NDK_ERR_GET_NETADDR     获取本地地址或子网掩码失败
 *@li   NDK_ERR_ADDR_ILLEGAL    获取地址格式错误
 *@li   NDK_ERR_GET_GATEWAY     获取网关地址失败
*/
NEXPORT int NDK_NetDHCP(void)
{
    return ndk_exec_dhcp();
}

/**
 *@brief    域名解析
 *@param    pszDnsIp    解析得到的IP地址
 *@param    pszDnsName  域名
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszDnsIp/pszDnsName为NULL)
 *@li   NDK_ERR         操作失败(调用gethostbyname()失败返回)
*/
NEXPORT int NDK_GetDnsIp(char *pszDnsIp,const char *pszDnsName)
{
    return NDK_NetDnsResolv(COMM_TYPE_ETH,pszDnsIp,pszDnsName);

}
/**
   *@brief     以太网断开连接(此函数会删除用户添加的路由信息，后续重新连需要用户重新添加路由)
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"        参数非法(地址全为NULL或者emComtype错误)
*/
NEXPORT int NDK_EthDisConnect(void)
{
    int ret;
    /*-----------卸载驱动-----*/
	remove("/tmp/eth_route");
    ret = _EthClose();
	if(ret == 0)
		return NDK_ERR;
	else
    	return NDK_OK;
}

