/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys20.c
* Author 			: chensj
* version			: 
* DATE				: 20130829
* directory 		: 
* description		: 测试NDK_InitStatisticsData,NDK_GetStatisticsData接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysInitStatisticsData,NDK_SysGetStatisticsData"

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
*			  		   chensj	          20130829	 	created
*****************************************************************/
#if !(defined ME50N||defined ME50C||defined ME50H||defined SP610||defined ME62||defined ME50NX||defined ME68)
void sys20(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int i = 0, nRet = 0;
	ulong stOutPut = 0;
#if OVERSEAS_ENABLE
	EM_SS_DEV_ID devid[] = {SS_KEYBOARD_ENTER, SS_KEYBOARD_BASP,SS_KEYBOARD_ESC,SS_KEYBOARD_ZMK,SS_KEYBOARD_ZERO,SS_KEYBOARD_DOT,\
		SS_KEYBOARD_NINE,SS_KEYBOARD_EIGHT,SS_KEYBOARD_SEVEN,SS_KEYBOARD_SIX,SS_KEYBOARD_FIVE,SS_KEYBOARD_FOUR,SS_KEYBOARD_THREE,\
		SS_KEYBOARD_TWO,SS_KEYBOARD_ONE,SS_KEYBOARD_F1,SS_KEYBOARD_F2,SS_KEYBOARD_F3,SS_KEYBOARD_F4,SS_KEYBOARD_0_ID,SS_KEYBOARD_1_ID,\
		SS_KEYBOARD_2_ID,SS_KEYBOARD_3_ID,/*SS_KEYBOARD_4_ID,SS_KEYBOARD_5_ID,SS_KEYBOARD_6_ID,SS_KEYBOARD_7_ID,SS_KEYBOARD_11_ID,\
		SS_KEYBOARD_15_ID,SS_KEYBOARD_19_ID,SS_KEYBOARD_27_ID,*/SS_KEYBOARD_TOTAL,SS_PRT_PAPER_ID,SS_PRT_HEAT_ID,SS_PRT_STITCH_ID,SS_MAG_TIMES_ID,\
		SS_ICCARD_BASE_ID,SS_RFID_TIMES_ID,SS_MODEM_TIMES_ID,SS_MODEM_FAILTIMES_ID,SS_MODEM_SDLCTIME_ID,SS_MODEM_ASYNTIME_ID,\
		SS_WLS_TIMES_ID,SS_WLS_FAILTIMES_ID,SS_WLS_PPPTIME_ID,SS_WIFI_TIMES_ID,SS_WIFI_TIME_ID,SS_POWER_TIMES_ID,SS_POWERUP_TIME_ID,\
		SS_BATTERY_CHARGE_ID,SS_BUTTON_CELL_VOLT};
#else
	EM_SS_DEV_ID devid[] = {SS_KEYBOARD_ENTER, SS_KEYBOARD_BASP,SS_KEYBOARD_ESC,SS_KEYBOARD_ZMK,SS_KEYBOARD_ZERO,SS_KEYBOARD_DOT,\
		SS_KEYBOARD_NINE,SS_KEYBOARD_EIGHT,SS_KEYBOARD_SEVEN,SS_KEYBOARD_SIX,SS_KEYBOARD_FIVE,SS_KEYBOARD_FOUR,SS_KEYBOARD_THREE,\
		SS_KEYBOARD_TWO,SS_KEYBOARD_ONE,SS_KEYBOARD_F1,SS_KEYBOARD_F2,SS_KEYBOARD_F3,SS_KEYBOARD_F4,SS_KEYBOARD_0_ID,SS_KEYBOARD_1_ID,\
		SS_KEYBOARD_2_ID,SS_KEYBOARD_3_ID,/*SS_KEYBOARD_4_ID,SS_KEYBOARD_5_ID,SS_KEYBOARD_6_ID,SS_KEYBOARD_7_ID,SS_KEYBOARD_11_ID,\
		SS_KEYBOARD_15_ID,SS_KEYBOARD_19_ID,SS_KEYBOARD_27_ID,*/SS_KEYBOARD_TOTAL,SS_PRT_PAPER_ID,SS_PRT_HEAT_ID,SS_PRT_STITCH_ID,SS_MAG_TIMES_ID,\
		SS_ICCARD_BASE_ID,SS_RFID_TIMES_ID,SS_MODEM_TIMES_ID,SS_MODEM_FAILTIMES_ID,SS_MODEM_SDLCTIME_ID,SS_MODEM_ASYNTIME_ID,\
		SS_WLS_TIMES_ID,SS_WLS_FAILTIMES_ID,SS_WLS_PPPTIME_ID,SS_WIFI_TIMES_ID,SS_WIFI_TIME_ID,SS_POWER_TIMES_ID,SS_POWERUP_TIME_ID};
#endif
#if OVERSEAS_ENABLE
	char *devstr[] = {"确认键","退格键","取消键","字母键","数字键0","小数点键",\
		"数字键9","数字键8","数字键7","数字键6","数字键5","数字键4","数字键3",\
		"数字键2","数字键1","方向键F1","方向键F2","方向键F3","方向键F4","未定义键0","未定义键1",\
		"未定义键2","未定义键3",/*"未定义键4","未定义键5","未定义键6","未定义键7","未定义键11",\
		"未定义键15","未定义键19","未定义键27",*/"所有键","打印毫米数","加热头加热豪秒数","针打出针数","刷卡次数",\
		"插卡次数","寻卡次数","MDM连接次数","MDM连接失败次数","同步MDM时长","异步MDM时长",\
		"无线连接次数","无线连接失败次数","无线连接毫秒数","WIFI连接次数","WIFI连接毫秒数","开关机次数","开机时间",\
		"电池充放电次数","纽扣电池电量"
	};
#else
	char *devstr[] = {"确认键","退格键","取消键","字母键","数字键0","小数点键",\
		"数字键9","数字键8","数字键7","数字键6","数字键5","数字键4","数字键3",\
		"数字键2","数字键1","方向键F1","方向键F2","方向键F3","方向键F4","未定义键0","未定义键1",\
		"未定义键2","未定义键3",/*"未定义键4","未定义键5","未定义键6","未定义键7","未定义键11",\
		"未定义键15","未定义键19","未定义键27",*/"所有键","打印毫米数","加热头加热豪秒数","针打出针数","刷卡次数",\
		"插卡次数","寻卡次数","MDM连接次数","MDM连接失败次数","同步MDM时长","异步MDM时长",\
		"无线连接次数","无线连接失败次数","无线连接毫秒数","WIFI连接次数","WIFI连接毫秒数","开关机次数","开机时间"};
#endif

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:正常测试功能
	for(i=0;i<(sizeof(devid)/sizeof(devid[0]));i++)
	{
		stOutPut = 0;
		if((nRet = NDK_SysGetStatisticsData(devid[i],&stOutPut))!=NDK_OK)
		{
			send_result("line %d:%s获取%s测试失败(%d)", __LINE__, TESTAPI, devstr[i], nRet);
			RETURN;
		}
		if(cls_show_msg("%s统计次数为%ld\n[确认]通过,[其他]失败", devstr[i], stOutPut) != ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}
	}

	//case2:异常测试
	if((nRet = NDK_SysGetStatisticsData(devid[0],NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}	
	if((nRet = NDK_SysGetStatisticsData(-1,&stOutPut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}

	//case 2.2 不在枚举范围内
	if((nRet = NDK_SysGetStatisticsData(SS_KEYBOARD_0_ID-1,&stOutPut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
#if OVERSEAS_ENABLE
	if((nRet = NDK_SysGetStatisticsData(SS_BUTTON_CELL_VOLT+1,&stOutPut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
#else
	if((nRet = NDK_SysGetStatisticsData(SS_POWERUP_TIME_ID+1,&stOutPut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
#endif
	
	//case3:测试统计清空功能(建议在统计类综合测试完成后在进行此测试)
	if(cls_show_msg("是否测试统计信息清空功能\n[确认]是  [其他]否")==ENTER)
	{
		//清之前先进行确认键、 所有键的统计存入数据库,然后进行清的动作,才不会影响清后的统计
		stOutPut = 0;
		if((nRet = NDK_SysGetStatisticsData(SS_KEYBOARD_ENTER,&stOutPut))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		stOutPut = 0;
		if((nRet = NDK_SysGetStatisticsData(SS_KEYBOARD_TOTAL,&stOutPut))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		cls_printf("正在验证是否清零,请勿按键");
		//进行清零
		if((nRet = NDK_SysInitStatisticsData())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		//判断是否清零
		for(i=0;i<(sizeof(devid)/sizeof(devid[0]));i++)
		{
			if(devid[i] == SS_POWERUP_TIME_ID)   /* 开机时间清空完后for循环还是会走*/
				continue ;
			
			stOutPut = 255;
			if((nRet = NDK_SysGetStatisticsData(devid[i],&stOutPut))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
				RETURN;
			}
			if(stOutPut!=0)
			{
				send_result("line %d:%s统计未清空(%ld)", __LINE__, devstr[i], stOutPut);
				RETURN;
			}
		}

		//开机统计的时间不应为0，即使清空后也应该马上开始计数
		stOutPut = 0;
		if((nRet = NDK_SysGetStatisticsData(SS_POWERUP_TIME_ID,&stOutPut))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		if(stOutPut==0)
		{
			send_result("line %d:%s测试失败(%ld)", __LINE__,  devstr[i], stOutPut);
			RETURN;
		}
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}
#else
void sys20(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int i = 0, ret = 0;
	gprs_erf_info_t Gprs_Info;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 
	
	memset(&Gprs_Info,0,sizeof(gprs_erf_info_t));
	//case1:异常测试
	if((ret = NDK_SysGetStatisticsData(SS_WLS_50N_GPRS, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret = NDK_SysGetStatisticsData(-1, &Gprs_Info))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case 1.2 不在枚举范围内
	if((ret = NDK_SysGetStatisticsData(SS_WLS_50N_GPRS-1, &Gprs_Info))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_SysGetStatisticsData(SS_WLS_50N_GPRS+1, &Gprs_Info))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常测试功能,应能显示相应的信息(6个项目分别为:RF射频,PO ppp open,SO sckt open,PC ppp close,SC sckt open,SS sckt send)
	if( (ret=NDK_SysGetStatisticsData(SS_WLS_50N_GPRS, &Gprs_Info)) != NDK_OK )
	{
		send_result("line %d:%s获取GPRS信息测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	for(i=0; i<6; i++)
	{
		if(cls_show_msg("%s总执行次数:%d\n总成功次数:%d\n总fail_1次数:%d\n总fail_2次数:%d\n[确认]通过,[其他]失败", &Gprs_Info.details[i].item, Gprs_Info.details[i].n_total, Gprs_Info.details[i].n_suc, Gprs_Info.details[i].n_fail_1, Gprs_Info.details[i].n_fail_2) != ENTER)
		{
			send_result("line %d:%s测试失败(item=%s, total=%d, succ=%d, fail1=%d, fail2=%d)", __LINE__, TESTAPI, &Gprs_Info.details[i].item, Gprs_Info.details[i].n_total, Gprs_Info.details[i].n_suc, Gprs_Info.details[i].n_fail_1, Gprs_Info.details[i].n_fail_2);
			RETURN;
		}
	}
		
	//case3:测试统计清空功能
	if(cls_show_msg("是否测试GPRS统计信息清空功能\n[确认]是  [其他]否")==ENTER)
	{
		cls_printf("正在验证是否清零");
		//进行清零
		if((ret = NDK_SysInitStatisticsData())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		memset(&Gprs_Info,0,sizeof(gprs_erf_info_t));
		//判断是否清零
		for(i=0; i<6; i++)
		{
			if( (ret=NDK_SysGetStatisticsData(SS_WLS_50N_GPRS, &Gprs_Info)) != NDK_OK )
			{
				send_result("line %d:%s获取GPRS信息测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((Gprs_Info.details[i].n_total!=0) || (Gprs_Info.details[i].n_suc!=0) || (Gprs_Info.details[i].n_fail_1!=0) || (Gprs_Info.details[i].n_fail_2!=0))
			{
				send_result("line %d:%s测试失败(item=%s, total=%d, succ=%d, fail1=%d, fail2=%d)", __LINE__, TESTAPI, &Gprs_Info.details[i].item, Gprs_Info.details[i].n_total, Gprs_Info.details[i].n_suc, Gprs_Info.details[i].n_fail_1, Gprs_Info.details[i].n_fail_2);
				RETURN;
			}
		}
	}
	
	//测试结束
	send_result("%s测试通过,请进行无线综合测试后执行本用例,应能获取到正确的信息.", TESTAPI);
	return;
}
#endif
