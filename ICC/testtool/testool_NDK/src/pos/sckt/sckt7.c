/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: socket模块
* file name		: sckt7.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_UdpOpen接口功能
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
#define 	TESTAPI		"NDK_UdpOpen"
#define		MAXNUM	4

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
void sckt7(void)
{
	//以下为局部变量
	int ret = -1, i = 0;
	uint unSckt[MAXNUM] = {0};
	
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:传入非法参数	
	if((ret=NDK_UdpOpen(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常使用测试，打开多个(MAXNUM个)socket测试：打开MAXNUM个UDP
	for(i=0; i<MAXNUM; i++)
	{
		if((ret=NDK_UdpOpen(&unSckt[i])) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
	}
	
	//case3:关闭socket
	for(i=0; i<MAXNUM; i++)//这里UDP的关闭也用TCP的函数？
	{
		if((ret=NDK_TcpClose(unSckt[i])) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}		
		if((ret=NDK_TcpWait(unSckt[i])) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			goto ERR;
		}
	}
		
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:	
	for(i=0; i<MAXNUM; i++)
	{
		NDK_TcpClose(unSckt[i]);
		NDK_TcpWait(unSckt[i]);
	}
	return;
}

