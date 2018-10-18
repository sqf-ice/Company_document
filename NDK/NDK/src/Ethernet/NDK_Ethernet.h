/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�	��Ʒ������
* ��    �ڣ�	2012-08-17
* ��	����	V1.00
* ����޸��ˣ�csl
* ����޸����ڣ�
*/
#ifndef _ETHERNET_H_
#define _ETHERNET_H_

/** @addtogroup ��̫��ͨѶ
* @{
*/

#define FALSE 0	/**<�߼���*/
#define TRUE 1	/**<�߼���*/


int NDK_EthSetAddress(const char *pszIp, const char *pszMask, const char *pszGateway, const char *pszDns);
int NDK_EthGetMacAddr(char *pszMacAddr);
int NDK_EthGetNetAddr(char *pszIp, char *pszMask, char *pszGateway, char *pszDns);
int NDK_NetDHCP(void);



/** @} */ // ��̫��ͨѶģ�����

#endif

/* End of this file */
