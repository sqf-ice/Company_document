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
#ifndef _PPP_H_
#define _PPP_H_

/** @addtogroup PPPͨѶ
* @{
*/
#include "NDK.h"
#include "NDK_debug.h"

int NDK_PppSetCfg(ST_PPP_CFG *pstPPPCfg, uint nuValidLen);
int NDK_PppDial(const char *pszUsername,const char *pszPassword);
int NDK_PppHangup(int nHangupType);
int NDK_PppCheck(EM_PPP_STATUS *pemStatus, int *pnErrCode);
int NDK_PppGetAddr(ulong *pulLocalAddr, ulong *pulHostAddr);
int NDK_PppAddrChange(register const char *pszIp, ulong *pulIpAddr);


/** @} */ // PPPͨѶģ�����

#endif

/* End of this file */

