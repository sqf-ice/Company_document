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
#ifndef _PPP_H_
#define _PPP_H_

/** @addtogroup PPP通讯
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


/** @} */ // PPP通讯模块结束

#endif

/* End of this file */

