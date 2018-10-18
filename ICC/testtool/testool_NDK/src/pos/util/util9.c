/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util9.c
* Author 			: jiangym
* version			:
* DATE				: 20130108
* directory 	:
* description	: 测试 NDK_CalcLRC    接口功能
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
#define 	TESTAPI		"NDK_CalcLRC"

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
* history 		 		:
*****************************************************************/
void util9(void)
{
	//以下为局部变量
	uchar sbuf[4] = {0x31,0x15,0x14,0x21};
	int len = 4, ret = 0;
	uchar lrc = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:各个参数为NULL情况，应返回失败
	if((ret=NDK_CalcLRC(NULL,len,&lrc))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_CalcLRC(sbuf,-1,&lrc))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_CalcLRC(sbuf,len,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:正常测试:psBuf[4]="0x31 0x15 0x14 0x21"，长度为4，输出结果为0x11,按位异或运算不进位
	if((ret=NDK_CalcLRC(sbuf,4,&lrc))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(lrc!=0x11)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

