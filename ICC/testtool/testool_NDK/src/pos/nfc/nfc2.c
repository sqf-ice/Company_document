/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: nfc模块
* file name			: nfc2.c
* Author 			:  huangjianbin
* version			: 
* DATE				:  20141215
* directory 			: 
* description			: 测试NDK_Nfc_Activate函数
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_Nfc_Activate"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		       huangjianb	  20141215	created
*****************************************************************/

void nfc2(void)
{
	int ret=0, pnRecvlen=0, nSeekCnt=3;
	uchar psRecebuf[64] = {0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);
	
	//参数错误
	cls_show_msg("请打开手机NFC测试工具并贴近pos...,任意键继续");
	if((ret=NDK_Nfc_Activate(NULL,&pnRecvlen,nSeekCnt)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_Nfc_Activate(psRecebuf,NULL,nSeekCnt)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if((ret=NDK_Nfc_Activate(psRecebuf,&pnRecvlen,0)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if((ret=NDK_Nfc_Activate(psRecebuf,&pnRecvlen,11)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//NFC正常参数测试
	if((ret=NDK_Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
	{
		send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
		RETURN;
	}

	/*
	//循环扫描五次
	for (j=0; j<5; j++) 
	{
		if ((NDK_Nfc_Activate(psRecebuf,&pnRecvlen)) == NDK_OK) 
			break;
	 
		sleep(1);
	}
	
	if (j == 5) 
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	*/
	
	send_result("%s测试通过", TESTAPI);
	return;
}
