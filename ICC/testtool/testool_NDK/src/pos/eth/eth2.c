/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 以太网模块
* file name		: eth2.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_EthGetMacAddr接口功能
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_EthGetMacAddr"

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
*			  						chensl		   20130105	 		created
*****************************************************************/
void eth2(void)
{
	//以下为局部变量
	int ret = -1;
	char MacAddr[18] = {0};

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:传入非法参数NULL
	if ((ret=NDK_EthGetMacAddr(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常使用测试
	memset(MacAddr, 0, sizeof(MacAddr));
	if ((ret=NDK_EthGetMacAddr(MacAddr)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (strlen(MacAddr) != 17)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, strlen(MacAddr));
		RETURN;
	}
	/*
	for(i=0;i<16;)
	{		
		macBuf[j++] = (MacAddr[i]-'0')*16 + (MacAddr[i+1]-'0');
		i+=3;
	}
	*/

	//测试结束
	//send_result("%s测试通过!(mac:%03d:%03d:%03d:%03d:%03d:%03d),请进[自检]核对是否一致!", TESTAPI, macBuf[0], macBuf[1], macBuf[2], macBuf[3], macBuf[4], macBuf[5]);
	send_result("%s测试通过!(mac:%s),请进[自检]核对是否一致!", TESTAPI, MacAddr);
	return;
}

