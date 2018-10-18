/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:触摸屏获取按键
* file name			: kb6.c
* Author 			:
* version			: 
* DATE				: 
* directory 			: 
* description			: 测试 NDK_TSKbd_Ctrl是否能正确控制由K21接管或者释放触摸屏以及
*					测试NDK_TSKbdGetXY是否正确获取触摸的横纵坐标
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  TESTAPI		"NDK_TSKbd_Ctrl,NDK_TSKbdGetXY"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: kb6
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        jiangym		20150513	created
*****************************************************************/
void kb6(void)
{
	/*private & local definition*/
	int ret = -1;
	uint x = 1, y = 1;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:当K21释放触摸屏的控制时，调用NDK_TSKbdGetXY会返回失败
	if((ret=NDK_TSKbd_Ctrl(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TSKbdGetXY(2,&x,&y))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:连续设置最后一次有效
	if((ret=NDK_TSKbd_Ctrl(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TSKbd_Ctrl(2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TSKbdGetXY(2,&x,&y))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:当k21接管触摸屏,x y参数为NULL时,返回成功,因为只是在等待时间按键,并没有要获取当前的按键内容
	if((ret=NDK_TSKbd_Ctrl(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TSKbdGetXY(2,NULL,&y))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TSKbdGetXY(2,&x,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TSKbdGetXY(2,NULL,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:当k21接管触摸屏,规定时间内未按触摸屏,x y返回值为0
	//再次设置有效
	if((ret=NDK_TSKbd_Ctrl(3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_printf("不要按触摸屏,等待2s");
	if((ret=NDK_TSKbdGetXY(2,&x,&y))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(x!=0 || y!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, x, y);
		RETURN;
	}

	//case5:k21接管触摸屏,规定时间内按触摸屏,判断获取的x y值应该正确	
	cls_printf("在5s内按触摸屏上任意一点后放开");
	if((ret=NDK_TSKbdGetXY(5,&x,&y))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(cls_show_msg("触碰点的坐标值为(%d,%d),[Enter]正确,[其他]错误", x, y)!=ENTER)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, x, y);
		RETURN;
	}

	//case6:k21接管触摸屏,死等按键,判断获取的x y值应该正确
	cls_printf("请在触摸屏上,下,左,右,中,左上,右上,左下,右下九个位置随意按下后弹起应可以获取到相应坐标,一分钟后退出测试");
	lib_StartStopwatch();
	while(1)
	{
		if (lib_ReadStopwatch()>60)//一分钟后退出测试
			break;
		if((ret=NDK_TSKbdGetXY(0,&x,&y))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
		cls_printf("触碰点的坐标值为(%d,%d),继续下一个位置的测试", x, y);
	}
	if(cls_show_msg("判断刚获取的各个位置的坐标值是否正确,[Enter]正确,[其他]错误")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}

