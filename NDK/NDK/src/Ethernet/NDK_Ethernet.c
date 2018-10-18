/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�
* ��    �ڣ�    2012-08-27
* ��    ����    V1.00
* ����޸��ˣ�
* ����޸����ڣ�
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
 *@brief    ���������ַ,������ΪNULL���ز�������(���ú����ԭ�Ѷ�̬����ĵ�ַ,Ҳ�ᱻ�޸�)
 *@param    pszIp   ����IP ��ַ�ַ�������ָ��,ΪNULL����IP��ַ.��֧��IPV4Э��
 *@param    pszMask �������������ַ�������ָ��,ΪNULL����Mask��ַ.��֧��IPV4Э��
 *@param    pszGateway  �������ص�ַ�ַ�������ָ��,ΪNULL����Gateway��ַ.��֧��IPV4Э��
 *@param    pszDns  ����DNS������IP��ַ,ΪNULL����DNS��ַ,��������3��DNS,֮���Էֺŷ�';'����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(��ַΪNULL)
 *@li   NDK_ERR_OPEN_DEV    ���豸�ļ�����
 *@li   NDK_ERR ����ʧ��
 *@li   NDK_ERR_ADDR_ILLEGAL    ��ȡ��ַ��ʽ����
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

    //system("sudo /usr/bin/killall udhcpc");     /**<�ر�DHCP*/

    if (ndk_getNetAddrCfg(sIPaddr, smask, sgateway)>=0) {
        /**<�жϵ�ַ�Ƿ�һ�£���ԭ�����ַһ�£��򲻸��µ�ַ*/
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

    /**<����̫��*/
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
                //��ʧ�ܺ�ж������
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
        /**<����IP��ַ*/
        if (NULL != pszIp) {
            sprintf(cmd, "sudo /sbin/ifconfig %s %s", dev_name,pszIp);
            TRACE_ETH("eth0_cmd1: %s\n", cmd);
            if (system(cmd)) {
                unload_eth_mod();
                return NDK_ERR;
            }
        }

        /**<������������*/
        if (NULL != pszMask) {
            sprintf(cmd, "sudo /sbin/ifconfig %s netmask %s", dev_name,pszMask);
            TRACE_ETH("eth0_cmd1: %s\n", cmd);
            if (system(cmd)) {
                unload_eth_mod();
                return NDK_ERR;
            }
        }

        /**<����·��*/
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
        ret = ndk_Add_DNS_IP(szDNS);    /**<����DNS������*/
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
 *@brief    ��ȡ����MAC��ַ
 *@retval   pszMacAddr  ����MAC��ַ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszMacAddrΪNULL)
 *@li   NDK_ERR             ����ʧ��(��ȡmac��ַʧ��)
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
 *@brief    ��������MAC��ַ
 *@param    pszMacAddr  Ҫ���õ�MAC��ַ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszMacAddrΪNULL)
*/
NEXPORT int NDK_EthSetMacAddr(char *pszMacAddr)
{
    if ((NULL == pszMacAddr)) {
        return NDK_ERR_PARA;
    }

    return ndk_setMACAddrCfg(pszMacAddr);
}


/**
 *@brief    ��ȡ�����ַ,������ΪNULL���ز�������
 *@retval   pszIp   ����IP��ַ,ΪNULL��ȡIP��ַ
 *@retval   pszGateway  �������ص�ַ,ΪNULL��ȡGateway��ַ
 *@retval   pszMask ������������,ΪNULL��ȡMask��ַ
 *@retval   pszDns  DNS������IP��ַ,ΪNULL��ȡDNS��ַ,һ����ȡ������DNS,֮���Էֺŷ�';'����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR_GET_NETADDR     ��ȡ���ص�ַ����������ʧ��
 *@li   NDK_ERR     ����ʧ��(��ȡDNSʧ��)
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
 *@brief    ʹ��dhcp��ȡ�����ַ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR     ����ʧ��(��̫����������ʧ�ܵ�)
 *@li   NDK_ERR_GET_NETADDR     ��ȡ���ص�ַ����������ʧ��
 *@li   NDK_ERR_ADDR_ILLEGAL    ��ȡ��ַ��ʽ����
 *@li   NDK_ERR_GET_GATEWAY     ��ȡ���ص�ַʧ��
*/
NEXPORT int NDK_NetDHCP(void)
{
    return ndk_exec_dhcp();
}

/**
 *@brief    ��������
 *@param    pszDnsIp    �����õ���IP��ַ
 *@param    pszDnsName  ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszDnsIp/pszDnsNameΪNULL)
 *@li   NDK_ERR         ����ʧ��(����gethostbyname()ʧ�ܷ���)
*/
NEXPORT int NDK_GetDnsIp(char *pszDnsIp,const char *pszDnsName)
{
    return NDK_NetDnsResolv(COMM_TYPE_ETH,pszDnsIp,pszDnsName);

}
/**
   *@brief     ��̫���Ͽ�����(�˺�����ɾ���û���ӵ�·����Ϣ��������������Ҫ�û��������·��)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"        �����Ƿ�(��ַȫΪNULL����emComtype����)
*/
NEXPORT int NDK_EthDisConnect(void)
{
    int ret;
    /*-----------ж������-----*/
	remove("/tmp/eth_route");
    ret = _EthClose();
	if(ret == 0)
		return NDK_ERR;
	else
    	return NDK_OK;
}

