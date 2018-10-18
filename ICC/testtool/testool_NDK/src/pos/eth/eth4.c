/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 以太网模块
* file name		: eth4.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试NDK_NetPing接口功能
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
#define 	TESTAPI		"NDK_NetPing"
#define		TIMEOUT		3.0
#define		WUCHASEC	0.2 //这个值待定


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
void eth4(void)
{
	//以下为局部变量
	int ret = -1, ret1 = -1;
	float tmp = 0.0;
#if K21_ENABLE
	char str[32] = {0};
#endif

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//测试前置,设置以太网地址，加载以太网模块
	//if ((ret=NDK_EthSetAddress(netsetting.LocalIP.Str, netsetting.LocalMask.Str, netsetting.LocalGateway.Str, NULL)) != NDK_OK)
	//{
	//	send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
	//	return;
	//}
	if(NetUp()!=SUCC)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	
	//case1:传入非法参数，错误地址，不存在地址
	if(((ret=NDK_NetPing(NULL, 0)) != NDK_ERR_PARA)
		||((ret1=NDK_NetPing("192.168.114.2066", 0)) != NDK_ERR_PARA)){
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}
	
	//case2:ping服务器IP
	if ((ret=NDK_NetPing(netsetting.SvrIP.Str, TIMEOUT)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case3:ping局域网的一个ip
	if ((ret=NDK_NetPing(netsetting.LocalGateway.Str, TIMEOUT)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0	//II部郑锦东与严明确认:没有这个配置 可以不用ping这个
	//case4:ping轮询地址127.0.0.1
	if ((ret=NDK_NetPing("127.0.0.1", TIMEOUT)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	
	//case5:超时测试
	lib_StartStopwatch();
	if ((ret=NDK_NetPing("192.166.3.44", TIMEOUT)) != NDK_ERR || (tmp=fabs(lib_ReadStopwatch()-TIMEOUT))>WUCHASEC)//这里的返回值  应该要返回超时
	{
#if K21_ENABLE
		memset(str,0,sizeof(str));
		ftos(tmp,str);
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, str);
#else		
		send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
#endif
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NetDown();
	return;
}

