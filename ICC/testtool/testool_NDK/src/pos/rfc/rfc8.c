/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc8.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试NDK_GetRfidType接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_GetRfidType"

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
*			  		    chenfm	        20130108	 	created
*****************************************************************/
void rfc8(void)
{
	//以下为局部变量
	int ret= -1, pnRfidtype= -1;
	char *Em_type[]={"RFID_Autoscan", "RFID_RC531", "RFID_PN512", "RFID_AS3911", "RFID_RC663", "RFID_MH1608"};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:异常测试
	if((ret=NDK_GetRfidType(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	} 

	//case2:正常获取射频卡芯片类型
	if((ret=NDK_GetRfidType(&pnRfidtype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(pnRfidtype<0||pnRfidtype>SZ_ARRAY(Em_type)-1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pnRfidtype);
		return;	
	}
	if(ENTER!=cls_show_msg("当前射频芯片类型为:%s\n正确按[ENTER],否则按其他键", Em_type[pnRfidtype]))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, Em_type[pnRfidtype]);
		RETURN;
	}  

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

