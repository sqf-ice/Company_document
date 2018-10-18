/************************************************************************
*
* copyright(c)2012-2015 Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc14.c
* Author 			:  zhengry
* version			: 
* DATE				:  20150811
* directory 			: 
* description			: 测试NDK_RfidFelicaApdu 对Felica卡的数据读写
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidFelicaApdu"

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
*			  		       zhengry	        20150811	 	created
*****************************************************************/
void rfc14(void)
{
	int ret=0, rec_len=0, pUIDlen=0;
	uchar rec_buf[300]={0}, send_buf[300]={0}, UID[300]={0};
	
	//构造send_buf
	send_buf[0] = 16;
	send_buf[1] = 0x06;
	send_buf[10] = 0x01;
	send_buf[11] = 0x09;
	send_buf[12] = 0x00;
	send_buf[13] = 0x01;
	send_buf[14] = 0x80;
	send_buf[15] = 0x00;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//寻卡
	cls_show_msg("请在感应区放置Felica卡，按任意键继续...");
	if((ret=NDK_RfidFelicaPoll(UID, &pUIDlen))!=NDK_OK)
	{
		send_result("line %d:%s寻卡失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case1:参数错误
	if((ret=NDK_RfidFelicaApdu(-1, send_buf, &rec_len, rec_buf))!=NDK_ERR_PARA)//-1引起程序跑飞
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_RfidFelicaApdu(2, send_buf, &rec_len, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_RfidFelicaApdu(send_buf[0], NULL, &rec_len, rec_buf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_RfidFelicaApdu(send_buf[0], send_buf, NULL, rec_buf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_RfidFelicaApdu(send_buf[0], send_buf, &rec_len, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:异常测试：IDM码未赋值，读写应该失败
	if((ret=NDK_RfidFelicaApdu(send_buf[0], send_buf, &rec_len, rec_buf))!=NDK_ERR_MI_NOTAGERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:正常测试:不需认证读
	memcpy(&send_buf[2], &UID[2], 8);
	if((ret=NDK_RfidFelicaApdu(send_buf[0], send_buf, &rec_len, rec_buf))!=NDK_OK||rec_len!=rec_buf[0])
	{
		send_result("line %d:%s不认证读失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(rec_buf[1]!=0x07||memcmp(send_buf+2, rec_buf+2, 8))
	{
		send_result("line %d:%s不认证读失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:异常测试：发送长度为3的时候，即便构造好了sendbuf，也应该失败
	if((ret=NDK_RfidFelicaApdu(3, send_buf, &rec_len, rec_buf))!=NDK_ERR_MI_NOTAGERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:异常测试：响应码出错，读写应该失败
	send_buf[1] = 0x20;
	if((ret=NDK_RfidFelicaApdu(send_buf[0], send_buf, &rec_len, rec_buf))!=NDK_ERR_MI_NOTAGERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//下电
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	//case6:在未寻卡的情况下，读写应该失败
	if((ret=NDK_RfidFelicaApdu(send_buf[0], send_buf, &rec_len, rec_buf))!=NDK_ERR_MI_WriteFifo)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;

}


