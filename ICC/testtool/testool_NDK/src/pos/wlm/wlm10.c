/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm10c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_WlSetSimSlot选择sim卡和NDK_WlGetSimSlot获取当前设置的sim卡接口功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_WlSetSimSlot、NDK_WlGetSimSlot"
 #define	WLM_TIMEOUT	2000

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm10void)
* functional description 			:   
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			zhengry     20161207	created
*****************************************************************/
void wlm10(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = 0, ret1 = 0, ret2 = 0, sq = 0, i=0, nKeyin = 0;
	float time1=0, time2=0;
	EM_WLM_SIM_SLOT get_simslot;
	EM_WLM_STATUS emStatus = -1;
	
	/*process body*/
	//case2:参数异常：NDK_WlSetSimSlot中的simSlot为SimSlot_1-1/SIM_SLOT_UNKNOW+1,应返回NDK_ERR_PARA；
	if( ((ret=NDK_WlSetSimSlot(SIM_SLOT1-1)) != NDK_ERR_PARA) || 
		((ret1=NDK_WlSetSimSlot(SIM_SLOT_UNKNOW+1)) != NDK_ERR_PARA) ||
		((ret2=NDK_WlGetSimSlot(NULL)) != NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败ret=%d,ret1=%d, ret2=%d", __LINE__, TESTAPI, ret, ret1, ret2);
		RETURN;
	}
	
	//安装sim卡1，未安装sim卡2时
	if(cls_show_msg("请开机后,未设置过SIM卡槽,并且SIM1插卡,SIM2未插卡后,执行本用例,退出[ESC],其它键开始测试")!=ESC)
	{
		//case1:开机后未调用set函数，NDK_WlGetSimSlot获取应该失败,且获取到的卡座应该是unknow
		//开发改成了未调用set函数，获取返回成功，且获取到的为SIM1  20180424 sull modify
		//if((ret=NDK_WlGetSimSlot(&get_simslot)) != NDK_ERR || get_simslot !=SIM_SLOT_UNKNOW)
		if((ret=NDK_WlGetSimSlot(&get_simslot)) != NDK_OK || get_simslot !=SIM_SLOT1)
		{
			send_result("line %d:%s测试失败ret=%d, get_simslot=%d", __LINE__, TESTAPI, ret, get_simslot);
			RETURN;
		}
		//case3:如果开机没有选择工作sim卡，系统默认是以sim卡槽1的作为当前工作sim卡,初始化应该成功
		if ((ret = NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus))!=NDK_OK||emStatus!=WLM_STATUS_OK)
		{
			send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, emStatus);
			RETURN;
		}
		sleep(5);//设置完SIM卡1后，模块复位，需要时间，才能取到信号
		if((ret = NDK_WlModemGetSQ(&sq))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, sq);
			RETURN;
		}
		if( (nKeyin=cls_show_msg("sim1应能成功取到信号,取到的信号为%d，通过[ENTER],失败[其他]", sq)) != ENTER)
		{
			send_result("line %d: %s测试失败(nKeyin=0x2%x, sq=%d)", __LINE__, TESTAPI, nKeyin, sq);
			RETURN;
		}
		
		//case4:NDK_WlSetSimSlot设置SIM2卡底座,NDK_WlGetSimSlot获取到为sim2卡,但由于SIM2未插卡初始化预期返回无卡,说明切换成功了
		if((ret=NDK_WlSetSimSlot(SIM_SLOT2)) != NDK_OK)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret=NDK_WlGetSimSlot(&get_simslot)) != NDK_OK || get_simslot != SIM_SLOT2)
		{
			send_result("line %d:%s测试失败ret=%d, get_simslot=%d", __LINE__, TESTAPI, ret, get_simslot);
			RETURN;
		}
		if ((ret = NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus))!=NDK_ERR_NO_SIMCARD)
		{
			send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, emStatus);
			RETURN;
		}
		send_result("子用例测试通过");
	}
	
	//case5:关机，取出sim卡1时，放置sim卡2时，NDK_WlSetSimSlot设置SIM2卡底座，NDK_WlGetSimSlot获取到为sim2，无线取信号应能成功；
	if(cls_show_msg("请关机,取出SIM1卡槽上的sim卡，并安装到SIM2卡槽上后,执行本用例,退出[ESC],其它键开始测试")!=ESC)
	{
		if((ret=NDK_WlSetSimSlot(SIM_SLOT2)) != NDK_OK)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret=NDK_WlGetSimSlot(&get_simslot)) != NDK_OK || get_simslot != SIM_SLOT2)
		{
			send_result("line %d:%s测试失败ret=%d, get_simslot=%d", __LINE__, TESTAPI, ret, get_simslot);
			RETURN;
		}
		if ((ret = NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus))!=NDK_OK||emStatus!=WLM_STATUS_OK)
		{
			send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, emStatus);
			RETURN;
		}
		
		//case6:NDK_WlSetSimSlot设置SIM1卡底座,NDK_WlGetSimSlot获取到为sim1卡,但由于SIM1未插卡初始化预期返回无卡,说明切换成功了
		if((ret=NDK_WlSetSimSlot(SIM_SLOT1)) != NDK_OK)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret=NDK_WlGetSimSlot(&get_simslot)) != NDK_OK || get_simslot != SIM_SLOT1)
		{
			send_result("line %d:%s测试失败ret=%d, get_simslot=%d", __LINE__, TESTAPI, ret, get_simslot);
			RETURN;
		}
		if ((ret = NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus))!=NDK_ERR_NO_SIMCARD)
		{
			send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, emStatus);
			RETURN;
		}
		send_result("子用例测试通过");
	}
	
	//case6:关机，放置sim卡1时，放置sim卡2时，NDK_WlSetSimSlot设置SIM1卡底座，NDK_WlGetSimSlot获取到为sim卡1，无线取信号应能成功
	//NDK_WlSetSimSlot设置SIM2卡底座，NDK_WlGetSimSlot获取到为sim2，无线取信号应能成功；
	if(cls_show_msg("请关机,在将两个卡槽都插上SIM卡后,执行本用例,退出[ESC],其它键开始测试")!=ESC)
	{
		for(i=SIM_SLOT1;i<SIM_SLOT_UNKNOW;i++)
		{
			cls_printf("正在进行sim%d测试",i+1);
			if(i==1)
				lib_StartStopwatch();
			if((ret=NDK_WlSetSimSlot(i)) != NDK_OK)
			{
				send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if(i==1)
				time1=lib_ReadStopwatch();
			if((ret=NDK_WlGetSimSlot(&get_simslot)) != NDK_OK || get_simslot != i)
			{
				send_result("line %d:%s测试失败ret=%d, get_simslot=%d", __LINE__, TESTAPI, ret, get_simslot);
				RETURN;
			}			
			if ((ret = NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus))!=NDK_OK||emStatus!=WLM_STATUS_OK)
			{
				send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, emStatus);
				RETURN;
			}
		}
		
		//case8:连续设置同一个卡槽,预期第2次设置的时候函数要比较快的返回
		lib_StartStopwatch();
		if((ret=NDK_WlSetSimSlot(SIM_SLOT2)) != NDK_OK)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			RETURN;
		}	
		time2=lib_ReadStopwatch();
		if(time2 > time1)
		{
			send_result("line %d :%s测试失败time1=%f, time2=%f", __LINE__, TESTAPI, time1, time2);
			RETURN;
		}
		send_result("子用例测试通过");
	}

#if 0	
	//case7:测试异常测试,在不支持双sim卡硬件的机子上，调用set sim2卡，
	if(cls_show_msg("请取不支持双sim卡硬件的机子上继续测试，退出[ESC],其它键开始测试")!=ESC)
	{
		if((ret=NDK_WlSetSimSlot(SIM_SLOT2)) != NDK_ERR_NOT_SUPPORT)
		{
			send_result("line %d:%s测试失败ret=%d", __LINE__, TESTAPI, ret);
			RETURN;
		}
		send_result("子用例测试通过");
	}
#endif

	send_result("%s测试通过", TESTAPI);
	return;
}

