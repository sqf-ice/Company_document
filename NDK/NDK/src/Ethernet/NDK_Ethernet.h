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
#ifndef _ETHERNET_H_
#define _ETHERNET_H_

/** @addtogroup 以太网通讯
* @{
*/

#define FALSE 0	/**<逻辑假*/
#define TRUE 1	/**<逻辑真*/


int NDK_EthSetAddress(const char *pszIp, const char *pszMask, const char *pszGateway, const char *pszDns);
int NDK_EthGetMacAddr(char *pszMacAddr);
int NDK_EthGetNetAddr(char *pszIp, char *pszMask, char *pszGateway, char *pszDns);
int NDK_NetDHCP(void);



/** @} */ // 以太网通讯模块结束

#endif

/* End of this file */
