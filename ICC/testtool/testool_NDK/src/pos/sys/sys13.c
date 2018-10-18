/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys13.c
* Author 			: linw
* version			: 
* DATE				: 20130109
* directory 		: 
* description		: 本例测试NDK_SysGetPosTime,NDK_SysSetPosTime接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysGetPosTime,NDK_SysSetPosTime"

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
*			  	  	      linw          20130109	created
*****************************************************************/
void sys13(void)
{
	//以下为局部变量
	int ret = -1, i = 0;
	struct tm stOldPosTime;
	struct tm stGetPosTime;
	struct tm stSetPosTime, stSetPosTime0;
	char tmp[512] = {0};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);
	if(cls_show_msg1(5,"此用例需要在root权限下进行,[Enter]继续,其他返回" )!=ENTER && auto_flag!=1)
	{
		send_result("line %d:%s测试中断", __LINE__, TESTAPI);
		return;
	}

	//case1: 取POS当前时间
	if((ret=NDK_SysGetPosTime(&stOldPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2: 先设置一个时间，再设置个时间,最后获取是否第二次设置的 20140603 added by jiangym
	//设置POS时间2010.10.10 10:10:10
	memset(&stSetPosTime0, 0, sizeof(stSetPosTime0));
	stSetPosTime0.tm_year = 2010 - 1900;
	stSetPosTime0.tm_mon = 10 - 1;
	stSetPosTime0.tm_mday = 10;
	stSetPosTime0.tm_hour = 10;
	stSetPosTime0.tm_min = 10;
	stSetPosTime0.tm_sec = 10;
	if((ret=NDK_SysSetPosTime(stSetPosTime0)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//设置POS时间2011.11.11 11:11:11
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 11 - 1;
	stSetPosTime.tm_mday = 11;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&stGetPosTime, 0, sizeof(stGetPosTime));
	NDK_SysGetPosTime(&stGetPosTime);
	if(stSetPosTime.tm_year != stGetPosTime.tm_year 
		|| stSetPosTime.tm_mon != stGetPosTime.tm_mon
		|| stSetPosTime.tm_mday != stGetPosTime.tm_mday
		|| stSetPosTime.tm_hour != stGetPosTime.tm_hour
		|| stSetPosTime.tm_min != stGetPosTime.tm_min
		/*|| stSetPosTime.tm_sec != stGetPosTime.tm_sec*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:测试润年，2012.2.29
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2012 - 1900;
	stSetPosTime.tm_mon = 2 - 1;
	stSetPosTime.tm_mday = 29;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&stGetPosTime, 0, sizeof(stGetPosTime));
	NDK_SysGetPosTime(&stGetPosTime);
	if(stSetPosTime.tm_year != stGetPosTime.tm_year 
		|| stSetPosTime.tm_mon != stGetPosTime.tm_mon
		|| stSetPosTime.tm_mday != stGetPosTime.tm_mday
		|| stSetPosTime.tm_hour != stGetPosTime.tm_hour
		|| stSetPosTime.tm_min != stGetPosTime.tm_min
		/*|| stSetPosTime.tm_sec != stGetPosTime.tm_sec*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:异常测试，错误参数
	if((ret=NDK_SysGetPosTime(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case5: 异常测试，错误参数，年份1899
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 1899 - 1900;
	stSetPosTime.tm_mon = 11 - 1;
	stSetPosTime.tm_mday = 11;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6: 异常测试，错误参数，月份13
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 13 - 1;
	stSetPosTime.tm_mday = 11;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7: 异常测试，错误参数，月份0
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 0 - 1;
	stSetPosTime.tm_mday = 11;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case8: 异常测试，错误参数，2011.02.29
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 2 - 1;
	stSetPosTime.tm_mday = 29;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case9:异常测试，参数错误，2011.11.00
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 11 - 1;
	stSetPosTime.tm_mday = 00;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case10:异常测试，参数错误，2011.10.32
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 10 - 1;
	stSetPosTime.tm_mday = 32;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case11: 异常测试，错误参数，2011.11.31
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 11 - 1;
	stSetPosTime.tm_mday = 31;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case12: 异常测试，错误参数，小时24
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 11 - 1;
	stSetPosTime.tm_mday = 11;
	stSetPosTime.tm_hour = 24;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case13: 异常测试，错误参数，分钟60
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 11 - 1;
	stSetPosTime.tm_mday = 11;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 61;
	stSetPosTime.tm_sec = 11;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case14: 异常测试，错误参数，数秒60
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2011 - 1900;
	stSetPosTime.tm_mon = 11 - 1;
	stSetPosTime.tm_mday = 11;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 11;
	stSetPosTime.tm_sec = 60;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case15:异常测试，错误参数，闰年2月30
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 1904 - 1900;
	stSetPosTime.tm_mon = 2 - 1;
	stSetPosTime.tm_mday = 30;
	stSetPosTime.tm_hour = 11;
	stSetPosTime.tm_min = 35;
	stSetPosTime.tm_sec = 00;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

if(auto_flag==0 || auto_flag==2)
{
	//case16:时间完整性测试,验证时钟在变化过程中不会缺分少秒
	//例如：
	//XX分59秒之后应该是XX+1分00秒,不是01秒;
	//59分59秒之后应该是00分00秒，不是01分01秒
#if !(K21_ENABLE||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG||defined ME50S||defined ME50NS)//K21没有串口，故将测试结果由USB串口上送
	cls_show_msg("按任意键开始时钟完整性测试,请查看串口1连续输出的时间是否会缺分少秒");
#elif ANDIROD_ENABLE
	cls_printf("时钟完整性测试,请查看连续输出的时间是否会缺分少秒");
#else
	cls_show_msg("请用USB线将机器跟电脑相连,打开AccessPort软件,任意键继续");
#endif
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2013 - 1900;
	stSetPosTime.tm_mon = 5 - 1;
	stSetPosTime.tm_mday = 7;
	stSetPosTime.tm_hour = 10;
	stSetPosTime.tm_min = 58;
	stSetPosTime.tm_sec = 57;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !(K21_ENABLE||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG||defined ME50S||defined ME50NS)||defined IM81||defined ME20//有串口1
	NDK_PortClose(PORT_NUM_COM1);
	NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1");
#elif ANDIROD_ENABLE
	//NDK_AZPortClose(PORT_NUM_COM2);
	//NDK_AZPortOpen(PORT_NUM_COM2, "115200,8,N,1");
#elif defined ME15C//defined N900||
	NDK_PortClose(PORT_NUM_COM2);
	NDK_PortOpen(PORT_NUM_COM2, "115200,8,N,1");
#else //仅有USB口
	NDK_PortClose(PORT_NUM_USB);
	NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1");
	
	cls_show_msg("请连接上AccessPort,任意键继续");
	cls_printf("观察AccessPort输出数据");
#endif
	for(i=0; i< 100; i++)
	{	
		ret=NDK_SysGetPosTime(&stGetPosTime);
		/*fprintf(stderr," stGetPosTime=%d/%d/%d %d:%d:%d \n", stGetPosTime.tm_year+1900,stGetPosTime.tm_mon+1,\
			stGetPosTime.tm_mday,stGetPosTime.tm_hour,stGetPosTime.tm_min,stGetPosTime.tm_sec);*/
		//K21平台不支持控制台,修改信息从串口输出 20130831 linwl
		sprintf(tmp, "stGetPosTime=%04d/%02d/%02d %02d:%02d:%02d \n", stGetPosTime.tm_year+1900,stGetPosTime.tm_mon+1,\
				stGetPosTime.tm_mday,stGetPosTime.tm_hour,stGetPosTime.tm_min,stGetPosTime.tm_sec);
#if !(K21_ENABLE||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG||defined ME50S||defined ME50NS)||defined IM81||defined ME20
		NDK_PortWrite(PORT_NUM_COM1, strlen(tmp), tmp);
#elif ANDIROD_ENABLE
		//NDK_AZPortWrite(PORT_NUM_COM2, strlen(tmp), tmp);
		cls_printf("%s",tmp); //直接在pos界面上输出
#elif defined ME15C//defined N900||
		NDK_PortWrite(PORT_NUM_COM2, strlen(tmp), tmp);
#else
		NDK_PortWrite(PORT_NUM_USB, strlen(tmp), tmp);
#endif
		sleep(1);
	}

#if !(K21_ENABLE||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG||defined ME50S||defined ME50NS)||defined IM81||defined ME20
	NDK_PortClose(PORT_NUM_COM1);
#elif 0//ANDIROD_ENABLE
	NDK_AZPortClose(PORT_NUM_COM2);
#elif defined ME15C//defined N900||
	NDK_PortClose(PORT_NUM_COM2);
#else
	NDK_PortClose(PORT_NUM_USB);
#endif

	if (g_com!=INVALID_COM)
	{
#if 0//ANDIROD_ENABLE
		NDK_AZPortOpen(g_com, "115200,8,N,1");
#else
		NDK_PortOpen(g_com, "115200,8,N,1");
#endif
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
	NDK_SysSetPosTime(stOldPosTime);//设置回原来的时间，此处由于测试用例需要执行约3分钟，因此会引起测试前后系统时间慢约3分钟，不是BUG
	return;
}

