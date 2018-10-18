/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描
* file name			: 2.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_ScanSet是否可以正常设置扫描头相关的参数
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define    TESTAPI		"NDK_ScanSet"
#define    MAXTIMEOUT  10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scan2
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*									jiangym			20130917		created
*
*****************************************************************/
void scan2(void)
{
	/*private & local definition*/
	int ret = 0, plen = 0, ret2 = 0, i=0;
	char pvalue[500] = {0};
	char* displaystr[] = {"闪烁","无照明","读码长亮",};
	char* displaystr2[] = {"闪烁,扫描结束后灯熄灭","始终都是熄灭的","熄灭,扫描结束后对焦灯开启"};
	char* displaystr3[] = {"闪烁","无对焦","感应",};
	char sBuf[128]={0};
	uint len = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置模块初始化
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO,&len,sBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((sBuf[3]==0x03) || (sBuf[3]==0x04) || (sBuf[3]==0x05) || (sBuf[3]==0x06))
	{
		//case1:参数错误,emScanSet 不在其范围内,不支持的硬件预期返回不支持
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT-1,1))!=NDK_ERR_NOT_SUPPORT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT+1,1))!=NDK_ERR_NOT_SUPPORT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	else
	{
		//case1:参数错误,emScanSet 不在其范围内,支持的硬件预期返回参数错误
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT-1,1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT+1,1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	
	//先进行一次设置，判断扫描头是否支持该测试
	if((ret=NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT,1))!=NDK_ERR_NOT_SUPPORT)
	{
		//case1:照明灯unSetValue的值不在0-2范围 
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT,-1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT,3))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case2:照明灯正常测试设置的值为0--闪烁,1--无照明,2--读码长亮
		for(i=0;i<3;i++)
		{
			if((ret=NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT,i))==NDK_OK)
			{
				cls_printf("请在10秒内进行扫描");
				if((ret2=NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret2);
					GOTOERR;
				}
				if(cls_show_msg("扫描过程中照明灯%s,扫描结束后灯熄灭,[Enter]通过,其他失败", displaystr[i])!=ENTER)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					GOTOERR;
				}
			}
			else if(ret==NDK_ERR_NOT_SUPPORT)
			{
				if(cls_show_msg("扫描头是否不支持照明灯设置%s,是按[确认],否则按[其它]", displaystr[i])!=ENTER)
				{
					send_result("line %d: %s测试用户取消了测试", __LINE__, TESTAPI);
					GOTOERR;
				}
			}
			else
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
		}
	}
	else
	{
		if(cls_show_msg("扫描头是否不支持照明灯设置,是按[确认],否则按[其它]")!=ENTER)
		{
			send_result("line %d: %s测试用户取消了测试", __LINE__, TESTAPI);
			GOTOERR;
		}
	}

	//先进行一次设置，判断扫描头是否支持该测试
	if((ret=NDK_ScanSet(SCAN_SETTYPE_FOCUSLIGHT,1))!=NDK_ERR_NOT_SUPPORT)
	{
		//case3:对焦灯unSetValue的值不在0-2范围 
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FOCUSLIGHT,-1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FOCUSLIGHT,3))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case4:照明灯正常测试设置的值为0--闪烁,1--无对焦,2--感应
		for(i=0;i<3;i++)
		{
			if((ret=NDK_ScanSet(SCAN_SETTYPE_FOCUSLIGHT,i))==NDK_OK)
			{
				cls_printf("请在10秒内进行扫描");
				if((ret2=NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret2);
					GOTOERR;
				}
				if(cls_show_msg("扫描过程中对焦灯%s,[Enter]通过,其他失败", displaystr2[i])!=ENTER)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					GOTOERR;
				}
			}
			else if(ret==NDK_ERR_NOT_SUPPORT)
			{
				if(cls_show_msg("扫描头是否不支持对焦灯设置%s,是按[确认],否则按[其它]", displaystr3[i])!=ENTER)
				{
					send_result("line %d: %s测试用户取消了测试", __LINE__, TESTAPI);
					GOTOERR;
				}
			}
			else
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
		}
	}
	else
	{
		if(cls_show_msg("扫描头是否不支持对焦灯设置,是按[确认],否则按[其它]")!=ENTER)
		{
			send_result("line %d: %s测试用户取消了测试", __LINE__, TESTAPI);
			goto ERR;
		}
	}

	//先进行一次设置，判断扫描头是否支持该测试
	if((ret=NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY,1))!=NDK_ERR_NOT_SUPPORT)
	{
		//case5:灵敏度设置unSetValue的值不在1-20范围
		if((ret=NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY,0))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY,21))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case6:灵敏度正常测试,设置的值分别为1和20,进行扫描速度的比较
		//数值越低灵敏度越高
		if((ret=NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY,20))==NDK_OK)
		{
			cls_printf("请进行扫描");
			if((ret=NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
		}
		else
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret2=NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY,1))==NDK_OK)
		{
			cls_printf("请进行扫描");
			if((ret=NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret2);
				GOTOERR;
			}
		}
		else
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret2);
			GOTOERR;
		}
		if(ret==NDK_OK&&ret2==NDK_OK)
		{
			if(cls_show_msg("第二次扫描感觉比第一次扫描快,[Enter]通过,其他失败")!=ENTER)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI);
				GOTOERR;
			}
		}
	}
	else
	{
		if(cls_show_msg("扫描头是否不支持灵敏度设置,是按[确认],否则按[其它]")!=ENTER)
		{
			send_result("line %d: %s测试用户取消了测试", __LINE__, TESTAPI);
			goto ERR;
		}
	}

	//先进行一次设置，判断扫描头是否支持该测试
	if((ret=NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,1))!=NDK_ERR_NOT_SUPPORT)
	{
		//case7:灵敏度设置unSetValue的值不在1-20范围
		if((ret=NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY,0))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY,21))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case8:恢复出厂设置应该成功
		if((ret=NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,11))==NDK_OK)
		{
			cls_printf("请进行扫描");
			if((ret=NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			//出厂默认设置(照明对焦为闪烁、灵敏度为11))
			if(cls_show_msg("扫描过程中照明对焦灯为闪烁,[Enter]通过,其他失败")!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
			}
		}
		else
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	else
	{
		if(cls_show_msg("扫描头是否不支持出厂默认设置,是按[确认],否则按[其它]")!=ENTER)
		{
			send_result("line %d: %s测试用户取消了测试", __LINE__, TESTAPI);
			GOTOERR;
		}
	}

	//结束测试
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,11);
#if SCAN_SOFT_ENABLE	
	NDK_ScanExit();
#endif
	return;
}

