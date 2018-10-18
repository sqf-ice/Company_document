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
#ifndef _WLMODEM_H_
#define _WLMODEM_H_

/** @addtogroup ����ģ��
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


/** @} */ // ����ģ�����

#endif

/* End of this file */

