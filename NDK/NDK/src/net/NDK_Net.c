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
#include <resolv.h>

#include "NDK.h"
#include "NDK_Net.h"
#include "NDK_debug.h"

char ptmpNetDnsBuf[3][30];

extern int ndk_NetAddRouter(EM_COMM_TYPE emCommType,char *szDstIP);
extern int ndk_NetDelRouter(EM_COMM_TYPE emCommType,char *szDstIP);
extern int ndk_NetGetResolv(void);
extern int ndk_NetGetInterface(EM_COMM_TYPE emCommType,char *pstbuf);
extern int ndk_GetNetAddr(char *interf,char *szIPAddr, char *szMask, char *szGatewayAddr);



/**
 *@brief ��������ת��ʹ�õ�ͨѶ�ӿ�
 *@param     emCommType     ͨѶ��ʽ
 *@param     pszDestIP      ���÷������ĵ�ַ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"        �����Ƿ�(pszDestIPΪNULL)
 *@li   \ref NDK_ERR_NET_UNKNOWN_COMMTYPE "NDK_ERR_NET_UNKNOWN_COMMTYPE"                δ֪ͨѶ��ʽ����
 *@li   \ref NDK_ERR_NET_INVALIDIPSTR "NDK_ERR_NET_INVALIDIPSTR"    ��ЧIP�ַ���
 *@li   \ref NDK_ERR_NET_UNSUPPORT_COMMTYPE "NDK_ERR_NET_UNSUPPORT_COMMTYPE"    ��֧�ֵ�ͨ������
  */
NEXPORT int NDK_NetAddRouterTable(EM_COMM_TYPE emCommType,char *pszDestIP)
{
	int ret;
	FILE *fd;
	char path[100];
	struct in_addr s; // IPv4��ַ�ṹ��
	memset(path,0x00,sizeof(path));

	if(pszDestIP == NULL)
		return NDK_ERR_PARA;
	
    ret=inet_pton(AF_INET, pszDestIP, (void *)&s);
    if(ret==0) {
        return NDK_ERR_NET_INVALIDIPSTR;
    }
	switch(emCommType){
		case COMM_TYPE_ETH:
			strcpy(path,PATH_ROUTER_ETH);
            break;
        case COMM_TYPE_PPP:
			strcpy(path,PATH_ROUTER_WLS);
            break;
        case COMM_TYPE_WIFI:
           	strcpy(path,PATH_ROUTER_WLAN);
            break;
        default:
            return NDK_ERR_NET_UNKNOWN_COMMTYPE;
			break;
		}
	fd=fopen(path,"a+");
	if(fd==NULL) 
		return NDK_ERR;
    fputs(pszDestIP,fd);
    fputs("\r\n",fd);
    fclose(fd);	
	ndk_NetAddRouter(emCommType,pszDestIP);
	return NDK_OK;	 
}

/**
 *@brief DNS��������ͨѶ�ӿ�
 *@param	 emCommType 	ͨѶ��ʽ(ppp��wifi��eth�Ȳ�����ϸ�ο��ýṹ�塢Ŀǰֻ֧���ᵽ��3��ͨѶ��ʽ)
 *@param	 pszDnsIp		�������֮��õ���ip��ַ
 *@param	 pszDnsName 	��������������
 *@return
 *@li	NDK_OK				�����ɹ�
 *@li	NDK_ERR 			����ʧ��
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"		�����Ƿ�
 *@li	\ref NDK_ERR_NET_UNKNOWN_COMMTYPE "NDK_ERR_NET_UNKNOWN_COMMTYPE"				δ֪ͨѶ��ʽ����
 */
NEXPORT int NDK_NetDnsResolv(EM_COMM_TYPE emCommType,char *pszDnsIp,char *pszDnsName)
{
	int ret,i,cnt = 0;
    struct hostent *pstHost;
	
	if((emCommType>=COMM_TYPE_UNKNOW)||(emCommType<COMM_TYPE_ETH))
        return NDK_ERR_NET_UNKNOWN_COMMTYPE;
		
    if ((NULL == pszDnsIp) || (NULL == pszDnsName)) {
        return NDK_ERR_PARA;
    }
	switch(emCommType){
		case COMM_TYPE_ETH:
			ret = ndk_NetLink(PATH_RESOLV_ETH,PATH_RESOLV_TMP);
			if(ret < 0)
				return NDK_ERR;
            break;
        case COMM_TYPE_PPP:
			ret = ndk_NetLink(PATH_RESOLV_WLS,PATH_RESOLV_TMP);
			if(ret < 0)
				return NDK_ERR;
            break;
        case COMM_TYPE_WIFI:
			ret = ndk_NetLink(PATH_RESOLV_WLAN,PATH_RESOLV_TMP);
			if(ret < 0)
				return NDK_ERR;
            break;
        default:
            ret = NDK_ERR;
			break;
		}	
	memset(ptmpNetDnsBuf,0x00,sizeof(ptmpNetDnsBuf));
	cnt = ndk_NetGetResolv();


	if(cnt <= 0 )
		return NDK_ERR;
	for(i=0;i<cnt;i++){
		ret = ndk_NetAddRouter(emCommType,ptmpNetDnsBuf[i]);
		if(ret !=0 ){
			return ret;
		}
	}
	res_init();	
    pstHost = gethostbyname(pszDnsName);
    if (pstHost == NULL) {
		for(i=0;i<cnt;i++){
			ret = ndk_NetDelRouter(emCommType,ptmpNetDnsBuf[i]);
			if(ret != 0)
				return ret;
		}
        return NDK_ERR;
    }

    for (i = 0; (pstHost->h_addr_list)[i] != NULL; i++) {
        inet_ntop(AF_INET, (pstHost->h_addr_list)[i], pszDnsIp, 16);
    }
	for(i=0;i<cnt;i++){
		ret = ndk_NetDelRouter(emCommType,ptmpNetDnsBuf[i]);
		if(ret != 0)
			return ret;
	}
    return NDK_OK;
}

/**
 *@brief    �������ߡ���̫����WIFI��ͨѶģʽ�µ�DNS��������ַ
 *@param	 emCommType 	ͨѶ��ʽ(ppp��wifi��eth�Ȳ�����ϸ�ο��ýṹ�塢Ŀǰֻ֧���ᵽ��3��ͨѶ��ʽ)
 *@param	 pszDns		       Ҫ���õ�DNS������,���3��DNS,ÿ��NDS֮����";"����
 *@return
 *@li	NDK_OK				�����ɹ�
 *@li	NDK_ERR 			����ʧ��
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA"		�����Ƿ�
 *@li	\ref NDK_ERR_NET_UNKNOWN_COMMTYPE "NDK_ERR_NET_UNKNOWN_COMMTYPE"				δ֪ͨѶ��ʽ����
 *@li	\ref NDK_ERR_NET_ADDRILLEGAL "NDK_ERR_NET_ADDRILLEGAL"	��ȡ��ַ��ʽ����
 */
NEXPORT int NDK_NetSetDns(EM_COMM_TYPE emCommType,const char *pszDns)
{
    if(pszDns==NULL){
        return NDK_ERR_PARA;
    }
    return ndk_Add_DNS_IP_For_CommType(emCommType, pszDns);
}

/**
 *@brief    �����ṩ���������ͻ�ȡ�����ַ,�����ַ������ΪNULL���ز�������
 *@param emComtype  Ҫ��ȡ�����ַ����������
 *@retval   pszIp   ����IP��ַ,ΪNULL��ȡIP��ַ
 *@retval   pszGateway  �������ص�ַ,ΪNULL��ȡGateway��ַ
 *@retval   pszMask ������������,ΪNULL��ȡMask��ַ
 *@retval   pszDns  DNS������IP��ַ,ΪNULL��ȡDNS��ַ,һ����ȡ������DNS,֮���Էֺŷ�';'����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"        �����Ƿ�(��ַȫΪNULL����emComtype����)
 *@li   \ref NDK_ERR_GET_NETADDR "NDK_ERR_GET_NETADDR"      ��ȡ���ص�ַ���������������ʧ��
 *@li   \ref NDK_ERR "NDK_ERR"      ����ʧ��(��ȡDNSʧ��)
*/
NEXPORT int NDK_NetGetAddr(EM_COMM_TYPE emComtype,char *pszIp, char *pszMask, char *pszGateway, char *pszDns)
{

	char szIp[16] = {0}, szMask[16] = {0}, szGateway[16] = {0}, inter[10]= {0};
    int ret=-1;
	char dev_name[20];
    if ((NULL == pszIp) && (NULL == pszGateway) && (NULL == pszMask) && (NULL == pszDns)) {
        return NDK_ERR_PARA;
    }
	
    switch(emComtype) {
        case COMM_TYPE_ETH:
			memset(dev_name,0x00,sizeof(dev_name));
			ndk_NetGetInterface(COMM_TYPE_ETH,dev_name);
            ret=ndk_GetNetAddr(dev_name,szIp, szMask, szGateway);
            break;
        case COMM_TYPE_WIFI:
            if(access("/sys/class/net/wlan0",0)==0)
                ret=ndk_GetNetAddr("wlan0",szIp, szMask, szGateway);
            else if(access("/proc/net/wlan/info",0)==0) {
                if(ndk_NetGetInterface(COMM_TYPE_WIFI,inter)==0)
                    ret=ndk_GetNetAddr(inter,szIp, szMask, szGateway);
                else
                    ret=-1;
            }
            break;
        case COMM_TYPE_PPP:
            ret=ndk_GetNetAddr("ppp0",szIp, szMask, szGateway);
            break;
        default:
            return NDK_ERR_PARA;
    }
    if(ret!=0)
        return NDK_ERR_NET_GETADDR;

    if (NULL != pszDns) {
		switch(emComtype){
			case COMM_TYPE_ETH:
				ret = ndk_NetLink(PATH_RESOLV_ETH,PATH_RESOLV_TMP);
				if(ret < 0)
					return NDK_ERR;
	            break;
	        case COMM_TYPE_PPP:
				ret = ndk_NetLink(PATH_RESOLV_WLS,PATH_RESOLV_TMP);
				if(ret < 0)
					return NDK_ERR;
	            break;
	        case COMM_TYPE_WIFI:
				ret = ndk_NetLink(PATH_RESOLV_WLAN,PATH_RESOLV_TMP);
				if(ret < 0)
					return NDK_ERR;
	            break;
	        default:
	            ret = NDK_ERR;
				break;
		}	
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
 *@brief    ����PING����
 *@param    pszIp   ����IP��ַ�ַ�������ָ��,����Ϊ��ָ��.��֧��IPV4Э��
 *@param    nTimeout    ��ʱʱ��,��λΪ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszIpΪNULL��pszIp���ȴ���15)
 *@li   NDK_ERR             ����ʧ��
*/
NEXPORT int NDK_NetPing(char *pszIp, uint nTimeout)
{
    char ping_data[40];
    int nStatus = -1;

    if ((NULL == pszIp) || (strlen(pszIp) > 15)) {
        return NDK_ERR_PARA;
    }

    sprintf(ping_data, "ping %s -c 1 -w %d",pszIp,nTimeout);
    nStatus = system(ping_data);
    if ((-1 != nStatus) && WIFEXITED(nStatus) && (0 == WEXITSTATUS(nStatus))) {
        return NDK_OK;
    } else {
        return NDK_ERR;
    }
}

