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
#ifndef _WLMODEM_H_
#define _WLMODEM_H_

/** @addtogroup 无线模块
* @{
*/
#include "NDK.h"
#include "NDK_debug.h"

int NDK_WlModemReset(void);
int NDK_WlModemClose(void);
int NDK_WlInit(int nTimeout,const char *pszPINPassWord,EM_WLM_STATUS *pemStatus);
int NDK_WlModemGetSQ(int *pnSq);
int NDK_WlSendATCmd(const ST_ATCMD_PACK *pstATCmdPack,char *pszOutput,uint unMaxlen,uint unTimeout,EM_WLM_STATUS *pemStatus);
int NDK_WlSelSIM(uchar ucSimNo);
int NDK_WlCloseRF(void);


/** @} */ // 无线模块结束

#endif

/* End of this file */

