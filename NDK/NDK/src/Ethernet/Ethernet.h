/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：	产品开发部
* 日    期：	2012-08-17
* 版	本：	V1.00
* 最后修改人：csl
* 最后修改日期：
*/
#ifndef _ETHERNET_H_H_
#define _ETHERNET_H_H_

#include "NDK.h"
#include "NDK_debug.h"

#ifdef NDK_DEBUG
#define NDK_DEBUG_ETH
#endif

#ifdef NDK_DEBUG_ETH
#define TRACE_ETH(fmt, args...) NDK_LOG_DEBUG(NDK_LOG_MODULE_ETH,fmt, ##args)
#else
#define TRACE_ETH(fmt, args...)
#endif

#define DHCP_ADDR_FILE		"/tmp/dhcp_addr"
#define ETH_ROUTE_TABLE_CONFIG_FILE  "/tmp/eth_route"



int ndk_if_pid_exist(char *p_pidname);
int ndk_getNetAddrCfg(char *pszIPAddr, char *pszMask, char *pszGatewayAddr);
int ndk_SetNetAddrCfg(const char *szIPAddr, const char *szMask, const char *szGatewayAddr);
int ndk_exec_dhcp(void);
int ndk_GetETHAddress(char *ip, char *netmask, char *gw);
int ndk_EthOpen(void);
int ndk_getNetAddrCfg(char *pszIPAddr, char *pszMask, char *pszGatewayAddr);
int ndk_SetNetAddrCfg(const char *szIPAddr, const char *szMask, const char *szGatewayAddr);
int ndk_is_load_eth0(void);
int ndk_getMACAddrCfg(char *szMac);
int ndk_load_eth_mod(void);
int unload_eth_mod(void);
int ndk_if_mac_set(void);
void ndk_mac_set(void);
int ndk_if_dhcp_open(void);
void ndk_dhcp_kill(void);
void SetRouteTable2Eth0(void);
extern int ndk_Add_DNS_IP(char* DNSBuf);
extern int ndk_Get_DNS_IP(char *DNSBuf);


#endif
