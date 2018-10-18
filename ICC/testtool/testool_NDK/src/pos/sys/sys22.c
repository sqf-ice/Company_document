/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys22.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 测试NDK_SysGetPowerInfo是否正确获取电源信息
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysGetPowerInfo"

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
*			  		     
*****************************************************************/
void sys22(void)
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
	ST_POWER_INFO power_info, power_info1;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:参数异常测试:为NULL 返回NDK_ERR_PARA   
	if((ret=NDK_SysGetPowerInfo(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if( cls_show_msg("是否支持使用电池?\n[确认]是[其它]否") == ENTER)
	{
		//有电池情况
		if(cls_show_msg("该测试项需分别准备已/未充满电的电池,跳过按ESC")==ESC)
		{
			send_result("line %d:%s测试中止(请准备好电池重新测试)", __LINE__, TESTAPI);
			RETURN;
		}
		
		//case2:插电源情况,充满电的不在充电,未充满电的在充电,百分比为0,电源类型为3或6
		if(cls_show_msg("确保已插外接电源并装有充满电的电池,按任意键开始测试,[取消]键跳过测试")!=ESC)
		{
			memset(&power_info,0,sizeof(power_info));
			if((ret=NDK_SysGetPowerInfo(&power_info))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((power_info.unIsCharging!=0)||(power_info.unBattryPercent!=0)||((power_info.unPowerType!=3)&&(power_info.unPowerType!=6)))
			{
				send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, power_info.unIsCharging, power_info.unBattryPercent, power_info.unPowerType);
				RETURN;
			}
			if(cls_show_msg("获取到的外接电源类型为:%s,[确认]键通过,其它键失败",(power_info.unPowerType==3)?"适配器":"USB供电")!=ENTER)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, power_info.unPowerType);
				RETURN;
			}
		}
		else
			send_result("line %d:用户取消(外电+满电电池)用例测试", __LINE__);
		
		if(cls_show_msg("确保已插外接电源并装未充满电的电池,按任意键开始测试,[取消]键跳过测试")!=ESC)
		{
			if((ret=NDK_SysGetPowerInfo(&power_info))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((power_info.unIsCharging!=1)||(power_info.unBattryPercent!=0)||((power_info.unPowerType!=3)&&(power_info.unPowerType!=6)))
			{
				send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, power_info.unIsCharging, power_info.unBattryPercent, power_info.unPowerType);
				RETURN;
			}
			if(cls_show_msg("获取到的外接电源类型为:%s,[确认]键通过,其它键失败",(power_info.unPowerType==3)?"适配器":"USB供电")!=ENTER)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, power_info.unPowerType);
				RETURN;
			}
		}
		else
			send_result("line %d:用户取消(外电+不满电电池)用例测试", __LINE__);
		
		//case3:拔掉电源情况:不在充电,百分比在0-100间,电源类型为2
		if(cls_show_msg("确保不插外接电源并装有电池,按任意键开始测试,[取消]键跳过测试")!=ESC)
		{
			memset(&power_info,0,sizeof(power_info));
			if((ret=NDK_SysGetPowerInfo(&power_info))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((power_info.unIsCharging!=0)||((power_info.unBattryPercent<=0||power_info.unBattryPercent>100))||(power_info.unPowerType!=2))
			{
				send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, power_info.unIsCharging, power_info.unBattryPercent, power_info.unPowerType);
				RETURN;
			}
#if BATTERY_GETOUT_ENABLE//电池可在不关机情况下拆卸的操作以下测试
			//case4:不同电量电池获取电量百分比的测试
			if(cls_show_msg("接下来的测试需要3块不同电量的电池,跳过按ESC")!=ESC)
			{
				while(1)
				{
					if(i++)
						cls_show_msg("请连接电源后,换个电池,再把电源拔掉,任意键继续");
					else
						cls_show_msg("请把连接POS机的电源线USB线等供电设备移除,任意键继续");
					memset(&power_info,0,sizeof(power_info));
					if((ret=NDK_SysGetPowerInfo(&power_info))!=NDK_OK)
					{
						send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
						RETURN;
					}
					cls_show_msg("请记录,该块电池电量百分比为%d", power_info.unBattryPercent);
					if(i == 3)
						break;
				}
				if(cls_show_msg("三块电池电量百分比合理,[确认]是 [其他]否")!=ENTER)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					RETURN;
				}
			}
#endif	
		}
		else
			send_result("line %d:用户取消(不接外电+电池)用例测试", __LINE__);
		
		//case5:充电电量变化测试
		if(cls_show_msg("将进行较长时间的充电测试,跳过按ESC")!=ESC)
		{
			cls_show_msg("确保装有一个电量不满的电池,按任意键继续");
			memset(&power_info,0,sizeof(power_info));
			if((ret=NDK_SysGetPowerInfo(&power_info))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			cls_show_msg1(300,"请插入电源并等待5分钟");
			cls_show_msg("请拔下外电后按任意键继续");
			if((ret=NDK_SysGetPowerInfo(&power_info1))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if(power_info1.unBattryPercent<=power_info.unBattryPercent)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, power_info.unBattryPercent, power_info1.unBattryPercent);
				RETURN;
			}
		}
		else
			send_result("line %d:用户取消(长时间充电)用例测试", __LINE__);
	}
	
#if BATTERY_GETOUT_ENABLE//电池可在不关机情况下拆卸的操作以下测试
	//case6:没有电池情况:不在充电,百分比为0,电源类型为1或4
	cls_show_msg("确保已插外接电源并且没有装电池,按任意键继续");
	memset(&power_info,0,sizeof(power_info));
	if((ret=NDK_SysGetPowerInfo(&power_info))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((power_info.unIsCharging!=0)||(power_info.unBattryPercent!=0)||((power_info.unPowerType!=1)&&(power_info.unPowerType!=4)))
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, power_info.unIsCharging, power_info.unBattryPercent, power_info.unPowerType);
		RETURN;
	}
	if(cls_show_msg("获取到的外接电源类型为:%s,[确认]键通过,其它键失败",(power_info.unPowerType==1)?"适配器":"USB供电")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, power_info.unPowerType);
		RETURN;
	}
#endif

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

