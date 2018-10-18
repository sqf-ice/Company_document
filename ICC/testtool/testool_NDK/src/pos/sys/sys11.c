/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys11.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysGetPowerVol接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysGetPowerVol"

#define	MAXWAITTIME 30

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
*			  		      linwei	        20130121	 	created
*****************************************************************/
void sys11(void)
{
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
#if BATTERY_GETOUT_ENABLE	
	int i = 0;
#endif
	int ret = -1;
	unsigned int unVol = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//case1: 异常测试，错误参数
	if((ret=NDK_SysGetPowerVol(NULL))!= NDK_ERR_PARA)//应该返回错误
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if( cls_show_msg("是否支持使用电池?\n[确认]是[其它]否") == ENTER)
	{
#if BATTERY_GETOUT_ENABLE
		//case3: 有插电源，取电池电压
		cls_show_msg("确保已插电源并未装电池,按任意键开始");
		unVol = 0;
#if defined ME31 //由硬件人员确认ME31 插着电源是返回最大电压值20150604 modify 
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK || unVol == 0)
#else
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK || unVol != 0)
#endif
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,unVol);
			RETURN;
		}
#endif

		//case4: 有插电源，并且有电池，取电池电压//文档描述由 "如果有插电源则为0" 变成 "只插电源则为0" linwl20131216 NDK 4.4.3
		cls_show_msg("确保已插电源并装有电池,按任意键开始");
		unVol = 0;
//#if  !(defined ME31)//ME30插着电源，也有电池返回的unVol不为0
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK || unVol == 0)//相应的修改unVol != 0 成 unVol == 0 linwl20131216
//#else  //ME31插着电源且有电池返回的unVol不为0  modify 20150604
//		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK || unVol != 0)//相应的修改unVol != 0 成 unVol == 0 linwl20131216
//#endif
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,unVol);
			RETURN;
		}

#if (defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||defined ME66)//ME30 ME11插着usb 且有电池最高位为1
		//case6: 插电源,电池未满在充电
		//cls_show_msg("确保已插电源并装有电池,按任意键开始");
		unVol = 0;
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK || ((unVol >> 31)!=1))
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,unVol);
			RETURN;
		}
#endif

		//case5: 没插电源，取电池电压(若有电池)
		//me11需要插着手机音频口,然后使用app使得它使用电池而不使用usb供电
		cls_show_msg("确保未插电源并装有电池,按任意键开始");
		unVol = 0;
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK || unVol == 0)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,unVol);
			RETURN;
		}
	}
	else
	{
		//case2: 有插电源，无电池
		unVol = 0;
#if defined ME66 //me66最高位若有USB插入，代表1，故最高位为1不会为0，而是一个很大的数  20180816  modify
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK)
#else
  		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK || unVol != 0)
#endif
		{
			send_result("line %d:%s测试失败(%d,%u", __LINE__, TESTAPI, ret,unVol);
			RETURN;
		}
		send_result("%s测试通过", TESTAPI);
		return; //不支持电池下面无需测试
	}

#if BATTERY_GETOUT_ENABLE //电池可在不关机情况下拆卸的操作以下测试
	//case3:新增用例验证能够正确的识别3块不同电压的电池  20140702跟郑凯确认该函数返回的值约是电压的10倍 linwl
	if(cls_show_msg("接下来的测试需要3块不同电压的电池,跳过按ESC")!=ESC)
	{
		while(1)
		{
			if(i++)
				cls_show_msg("请连接电源后,换个电池,再把电源拔掉,任意键继续");
			else
				cls_show_msg("请把连接POS机的电源线USB线等供电设备移除,任意键继续");
			
			unVol = 0;
			if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK || unVol == 0)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,unVol);
				RETURN;
			}
			cls_show_msg("请记录,该块电池电压为%d",unVol);
			if(i == 3)
				break;
		}
		if(cls_show_msg("三块电池电压合理,[确认]是 [其他]否")!=ENTER)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,unVol);
			RETURN;
		}
	}
#endif

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

