/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys35.c
* Author 			: 
* version			: 
* DATE				: 
* directory 		: 
* description		: 本例测试NDK_SysGetRtcTime,NDK_SysSetRtcTime接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysGetRtcTime,NDK_SysSetRtcTime"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	       sull       20171031      created 
*****************************************************************/
void sys35(void)
{
	//以下为局部变量
	int ret = -1, i = 0;
	struct tm stOldRtcTime;
	struct tm stOldPosTime;
	struct tm stGetRtcTime;
	struct tm stGetPosTime;
	struct tm stSetRtcTime, stSetRtcTime0;
	struct tm stSetPosTime;
	char tmp[512] = {0};
	unsigned int unTime1 = 0,unTime2 = 0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);
	if(cls_show_msg1(5,"此用例需要在root权限下进行,[Enter]继续,其他返回" )!=ENTER && auto_flag!=1)
	{
		send_result("line %d:%s测试中断", __LINE__, TESTAPI);
		return;
	}
	//cls_show_msg("请在控制台输入命令echo  -n  ""  > /mnt/hwinfo/.delta_time清除软硬件时间差，任意键继续");  //调用NDK接口设置时间会产生软硬件时间差,清除可以方便定位
	//其它相关控制台命令:
	//date -s "2017-12-13 15:35:50"  设置系统软件时间，控制台输入
	//hwclock  -w                    把系统软件时间写入rtc时间
	//hwclock  -r                    查看rtc时间

	//case1: 取POS当前硬件和软件时间
	if((ret=NDK_SysGetRtcTime(&stOldRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
		if((ret=NDK_SysGetPosTime(&stOldPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//应开发要求，增加软硬时间相等的测试前置，方便定位 20171213 sull
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = stOldPosTime.tm_year;
	stSetRtcTime.tm_mon = stOldPosTime.tm_mon;
	stSetRtcTime.tm_mday = stOldPosTime.tm_mday;
	stSetRtcTime.tm_hour = stOldPosTime.tm_hour;
	stSetRtcTime.tm_min = stOldPosTime.tm_min;
	stSetRtcTime.tm_sec = stOldPosTime.tm_sec;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2: 先设置一个时间，再设置个时间,最后获取是否第二次设置的 
	//设置RTC时间2010.10.10 10:10:10
	memset(&stSetRtcTime0, 0, sizeof(stSetRtcTime0));
	stSetRtcTime0.tm_year = 2010 - 1900;
	stSetRtcTime0.tm_mon = 10 - 1;
	stSetRtcTime0.tm_mday = 10;
	stSetRtcTime0.tm_hour = 10;
	stSetRtcTime0.tm_min = 10;
	stSetRtcTime0.tm_sec = 10;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime0)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//设置RTC时间2011.11.11 11:11:11
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = 11;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&stGetRtcTime, 0, sizeof(stGetRtcTime));
	NDK_SysGetRtcTime(&stGetRtcTime);
	if(stSetRtcTime.tm_year != stGetRtcTime.tm_year 
		|| stSetRtcTime.tm_mon != stGetRtcTime.tm_mon
		|| stSetRtcTime.tm_mday != stGetRtcTime.tm_mday
		|| stSetRtcTime.tm_hour != stGetRtcTime.tm_hour
		|| stSetRtcTime.tm_min != stGetRtcTime.tm_min
		/*|| stSetRtcTime.tm_sec != stGetRtcTime.tm_sec*/)
	{
		send_result("line %d:%s测试失败(stGetRtcTime = %04d/%02d/%02d %02d:%02d:%02d)", __LINE__, TESTAPI, 
			stGetRtcTime.tm_year+1900, stGetRtcTime.tm_mon+1, stGetRtcTime.tm_mday,stGetRtcTime.tm_hour, stGetRtcTime.tm_min, stGetRtcTime.tm_sec);
		GOTOERR;
	}

	//case3:测试润年，2012.2.29
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2012 - 1900;
	stSetRtcTime.tm_mon = 2 - 1;
	stSetRtcTime.tm_mday = 29;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&stGetRtcTime, 0, sizeof(stGetRtcTime));
	NDK_SysGetRtcTime(&stGetRtcTime);
	if(stSetRtcTime.tm_year != stGetRtcTime.tm_year 
		|| stSetRtcTime.tm_mon != stGetRtcTime.tm_mon
		|| stSetRtcTime.tm_mday != stGetRtcTime.tm_mday
		|| stSetRtcTime.tm_hour != stGetRtcTime.tm_hour
		|| stSetRtcTime.tm_min != stGetRtcTime.tm_min
		/*|| stSetRtcTime.tm_sec != stGetRtcTime.tm_sec*/)
	{
		send_result("line %d:%s测试失败(stGetRtcTime = %04d/%02d/%02d %02d:%02d:%02d)", __LINE__, TESTAPI,
			stGetRtcTime.tm_year+1900, stGetRtcTime.tm_mon+1, stGetRtcTime.tm_mday,stGetRtcTime.tm_hour, stGetRtcTime.tm_min, stGetRtcTime.tm_sec);
		GOTOERR;
	}

	//case4:异常测试，错误参数
	if((ret=NDK_SysGetRtcTime(NULL)) != NDK_ERR/*目前代码设计为改成NDK_ERR,而非NDK_ERR_PARA*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case5: 异常测试，错误参数，年份1899
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 1899 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = 11;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6: 异常测试，错误参数，月份13
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 13 - 1;
	stSetRtcTime.tm_mday = 11;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7: 异常测试，错误参数，月份0
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 0 - 1;
	stSetRtcTime.tm_mday = 11;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case8: 异常测试，错误参数，2011.02.29
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 2 - 1;
	stSetRtcTime.tm_mday = 29;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case9:异常测试，参数错误，2011.11.00
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = 00;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case10:异常测试，参数错误，2011.10.32
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 10 - 1;
	stSetRtcTime.tm_mday = 32;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case11: 异常测试，错误参数，2011.11.31
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = 31;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case12: 异常测试，错误参数，小时24
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = 11;
	stSetRtcTime.tm_hour = 24;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case13: 异常测试，错误参数，分钟60
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = 11;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 61;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case14: 异常测试，错误参数，数秒60
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = 11;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 60;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case15:异常测试，错误参数，闰年2月30
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 1904 - 1900;
	stSetRtcTime.tm_mon = 2 - 1;
	stSetRtcTime.tm_mday = 30;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 35;
	stSetRtcTime.tm_sec = 00;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case16:异常参数，错误参数，参数为负数
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = -11;
	stSetRtcTime.tm_hour = -11;
	stSetRtcTime.tm_min = -11;
	stSetRtcTime.tm_sec = -11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case17:设置软件时间，硬件时间不应跟着改变
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2011 - 1900;
	stSetRtcTime.tm_mon = 11 - 1;
	stSetRtcTime.tm_mday = 11;
	stSetRtcTime.tm_hour = 11;
	stSetRtcTime.tm_min = 11;
	stSetRtcTime.tm_sec = 11;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2012 - 1900;
	stSetPosTime.tm_mon = 12 - 1;
	stSetPosTime.tm_mday = 12;
	stSetPosTime.tm_hour = 12;
	stSetPosTime.tm_min = 12;
	stSetPosTime.tm_sec = 12;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&stGetRtcTime, 0, sizeof(stGetRtcTime));
	NDK_SysGetRtcTime(&stGetRtcTime);
	memset(&stGetPosTime, 0, sizeof(stGetPosTime));
	NDK_SysGetPosTime(&stGetPosTime);
	if(stSetRtcTime.tm_year != stGetRtcTime.tm_year 
		|| stSetRtcTime.tm_mon != stGetRtcTime.tm_mon
		|| stSetRtcTime.tm_mday != stGetRtcTime.tm_mday 
		|| stSetRtcTime.tm_hour != stGetRtcTime.tm_hour
		|| stSetRtcTime.tm_min != stGetRtcTime.tm_min )
	{
		send_result("line %d:%s测试失败(stGetRtcTime = %04d/%02d/%02d %02d:%02d:%02d",__LINE__, TESTAPI,
			stGetRtcTime.tm_year+1900, stGetRtcTime.tm_mon+1, stGetRtcTime.tm_mday, stGetRtcTime.tm_hour, stGetRtcTime.tm_min, stGetRtcTime.tm_sec);
		GOTOERR;
	}

	if(stSetPosTime.tm_year != stGetPosTime.tm_year 
		|| stSetPosTime.tm_mon != stGetPosTime.tm_mon
		|| stSetPosTime.tm_mday != stGetPosTime.tm_mday 
		|| stSetPosTime.tm_hour != stGetPosTime.tm_hour
		|| stSetPosTime.tm_min != stGetPosTime.tm_min)
	{
		send_result("line %d:%s测试失败(stGetPosTime = %04d/%02d/%02d %02d:%02d:%02d)",__LINE__, TESTAPI,
			stGetPosTime.tm_year+1900, stGetPosTime.tm_mon+1, stGetPosTime.tm_mday, stGetPosTime.tm_hour, stGetPosTime.tm_min, stGetPosTime.tm_sec);
		GOTOERR;
	}

	//case18:设置硬件时间，软件时间应该跟着改变
	//case18:需求变更为，设置硬件时间，软件时间不应该跟着改变  20171213 sull
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2010 - 1900;
	stSetRtcTime.tm_mon = 10 - 1;
	stSetRtcTime.tm_mday = 10;
	stSetRtcTime.tm_hour = 10;
	stSetRtcTime.tm_min = 10;
	stSetRtcTime.tm_sec = 10;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&stGetRtcTime, 0, sizeof(stGetRtcTime));
	NDK_SysGetRtcTime(&stGetRtcTime);
	memset(&stGetPosTime, 0, sizeof(stGetPosTime));
	NDK_SysGetPosTime(&stGetPosTime);
	if(stGetRtcTime.tm_year == stGetPosTime.tm_year 
		&& stGetRtcTime.tm_mon == stGetPosTime.tm_mon
		&& stGetRtcTime.tm_mday == stGetPosTime.tm_mday 
		&& stGetRtcTime.tm_hour == stGetPosTime.tm_hour
		&& stGetRtcTime.tm_min == stGetPosTime.tm_min)
	{
		send_result("line %d:%s测试失败(stGetRtcTime = %04d/%02d/%02d %02d:%02d:%02d, stGetPosTime = %04d/%02d/%02d %02d:%02d:%02d)",__LINE__, TESTAPI,
			stGetRtcTime.tm_year+1900, stGetRtcTime.tm_mon+1, stGetRtcTime.tm_mday, stGetRtcTime.tm_hour, stGetRtcTime.tm_min, stGetRtcTime.tm_sec,
			stGetPosTime.tm_year+1900, stGetPosTime.tm_mon+1, stGetPosTime.tm_mday, stGetPosTime.tm_hour, stGetPosTime.tm_min, stGetPosTime.tm_sec);
		GOTOERR;
	}
			
	//case19:同时设置软硬时间,观察误差值是否保持稳定
	memset(&stGetRtcTime, 0, sizeof(stGetRtcTime));
	memset(&stGetPosTime, 0, sizeof(stGetPosTime));
	NDK_SysGetRtcTime(&stGetRtcTime);
	NDK_SysGetPosTime(&stGetPosTime);
	unTime1 = abs(stGetRtcTime.tm_sec - stGetPosTime.tm_sec);
	sleep(60);
	memset(&stGetRtcTime, 0, sizeof(stGetRtcTime));
	memset(&stGetPosTime, 0, sizeof(stGetPosTime));
	NDK_SysGetRtcTime(&stGetRtcTime);
	NDK_SysGetPosTime(&stGetPosTime);
	unTime2 = abs(stGetRtcTime.tm_sec - stGetPosTime.tm_sec);
	if((ret = abs(unTime1 - unTime2)) != 0)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__, TESTAPI, ret);
	}
if(auto_flag==0 || auto_flag==2)
{
	//case20:时间完整性测试,验证时钟在变化过程中不会缺分少秒
	//例如：
	//XX分59秒之后应该是XX+1分00秒,不是01秒;
	//59分59秒之后应该是00分00秒，不是01分01秒
	cls_show_msg("请用USB线将机器跟电脑相连,打开AccessPort软件,任意键继续");
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2013 - 1900;
	stSetRtcTime.tm_mon = 5 - 1;
	stSetRtcTime.tm_mday = 7;
	stSetRtcTime.tm_hour = 10;
	stSetRtcTime.tm_min = 58;
	stSetRtcTime.tm_sec = 57;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PortClose(PORT_NUM_USB);
	NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1");
	
	cls_show_msg("请连接上AccessPort,任意键继续");
	cls_printf("观察AccessPort输出数据");

	for(i=0; i< 100; i++)
	{
		ret=NDK_SysGetRtcTime(&stGetRtcTime);
		sprintf(tmp, "stGetRtcTime=%04d/%02d/%02d %02d:%02d:%02d \n", stGetRtcTime.tm_year+1900,stGetRtcTime.tm_mon+1,\
				stGetRtcTime.tm_mday,stGetRtcTime.tm_hour,stGetRtcTime.tm_min,stGetRtcTime.tm_sec);
		NDK_PortWrite(PORT_NUM_USB, strlen(tmp), tmp);
		sleep(1);
	}

	NDK_PortClose(PORT_NUM_USB);

	if (g_com!=INVALID_COM)
	{
		NDK_PortOpen(g_com, "115200,8,N,1");
	}
		
	if(cls_show_msg("检查时钟是否缺分少秒,[确认]是,[其它]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
}
else
	send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);	
	//NDK_SysSetPosTime(stOldPosTime);//设置回原来的时间
	//测试结束
	send_result("%s测试通过", TESTAPI);
	//return;
	goto ERR;
ERR:
	//错误处理
	NDK_SysSetRtcTime(stOldRtcTime);//设置回原来的时间，此处由于测试用例需要执行约3分钟，因此会引起测试前后系统时间慢约3分钟，不是BUG
	NDK_SysSetPosTime(stOldPosTime);
	return;
}

