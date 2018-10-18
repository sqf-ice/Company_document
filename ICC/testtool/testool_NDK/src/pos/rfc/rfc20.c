/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc20.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description		: 测试NDK_M0Authen认证接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_M0Authen"

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
*			  		     jiangym			    20180110		created
*****************************************************************/
void rfc20(void)
{
	//以下为局部变量
	int ret = -1;
	uchar  UID[LEN_UID] = {0}, UIDLen = 0, sak;
	uchar errkey[16] = {"\x11\x22\x33\x44\x55\x11\x22\x33\x44\x55\x11\x22\x33\x44\x55\x46"};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);
	//说明:带C的M0卡可以进行认证,认证后权限比较大;不带C的M0卡不需要认证
	//case1:未进行寻卡防冲突就进行认证应该失败
	cls_show_msg("请在感应区放置1张带C的M0卡,任意键继续...");
	if((ret=NDK_M0Authen(AUTHKEY_M0))!=NDK_ERR_MI_WriteFifo)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//寻卡防冲突
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2:异常测试:错误参数	     
	if((ret=NDK_M0Authen(NULL))!=NDK_ERR_PARA)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//这边卡片使用Mifare UltraLight C卡验证,注意的是正确的密钥是初始密钥,没有改过
	//如果使用不带C的那种卡片进行正确的密钥验证也是失败的,不带C的卡不需要验证
	//case3:使用错误的密码进行认证会失败
	if((ret=NDK_M0Authen(errkey))!=NDK_ERR_MI_CRCERR) //-2031 crc校验错
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:使用正确的密码进行认证(注意:失败后需要重新进行寻卡防冲突)
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Authen(AUTHKEY_M0))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//下电	
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:使用不带C的M0卡进行认证预期失败
	cls_show_msg("请在感应区放置1张不带C的M0卡,任意键继续...");
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Authen(AUTHKEY_M0))!=NDK_ERR_MI_NOTAGERR) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//case5:验证认证密钥可以改成自己的,暂时不验证

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}

