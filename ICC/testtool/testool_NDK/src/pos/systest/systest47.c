/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 蜂鸣器压力测试
* file name			: systest44.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 蜂鸣器压力测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20130221  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"蜂鸣器压力测试"

#define	DEFAULT_CNT_STR	"1500"
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)
#define	MAXWAITTIME	(30)	

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
* history 				: author			date			remarks

*****************************************************************/
void SysBeep_press(void)
{	
	int ret = 0;
	int  i = 0, cnt = DEFAULT_CNT_VLE, succ = 0;
	char CntStr[8] = DEFAULT_CNT_STR; 
	uint unLen = 0;
	
	//测试前置
#if !K21_ENABLE
	NDK_SysSetBeepVol(5);
#endif
	
	// 测试蜂鸣器响连续响
	cls_printf("默认测试次数:\n");
	//为了兼容无键盘的产品使用封装的函数20141031 jiangym modify
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 5,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)   
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	while(1)
	{
		cls_printf("NDK_SysBeep压力测试中\n总共:%d次,已进行:%d次",cnt,i);
		if(i++>=cnt)//达到测试次数后退出
			break;
		if((ret=NDK_SysBeep())!=NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:SysBeep压力测试失败(%d)", __LINE__, ret);
			return;
		}
		succ++;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"NDK_SysBeep压力测试完成,执行次数:%d次,成功%d次", i-1, succ);	
	return;
}
	
void SysTimeBeep_press(void)
{	
	int ret = 0;
	int  i = 0, cnt = DEFAULT_CNT_VLE, succ = 0;
	char CntStr[8] = DEFAULT_CNT_STR; 
	uint unLen = 0;
	
	//测试前置
#if !K21_ENABLE
	NDK_SysSetBeepVol(5);
#endif
	
	//测试在给定时间间隔内发出一定频率的声音
	cls_printf("默认测试次数:\n");
	//为了兼容无键盘的产品使用封装的函数20141031 jiangym modify
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 5,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	while(1)
	{
		cls_printf("NDK_SysTimeBeep压力测试中\n总共:%d次,已进行:%d次",cnt,i);
		if(i++>=cnt)//达到测试次数后退出
			break;
		if((ret=NDK_SysTimeBeep(1000,500)) != NDK_OK)//频率1000,间隔0.5s
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:SysTimeBeep压力测试失败(%d)", __LINE__, ret);
			return;
		}
		succ++;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"SysTimeBeep压力测试完成,执行次数:%d次,成功%d次", i-1, succ);	
	return;
}

#if 0//!K21_ENABLE
void SysTimeBeepUnblock_press(void)
{
	int ret = 0;
	int  i = 0, cnt = DEFAULT_CNT_VLE, succ = 0;
	char CntStr[8] = DEFAULT_CNT_STR; 
	uint unLen = 0;
	
	//测试前置
#if !K21_ENABLE
	NDK_SysSetBeepVol(5);
#endif
	
	//测试在给定时间间隔内发出一定频率的声音
	cls_printf("默认测试次数:\n");
	//为了兼容无键盘的产品使用封装的函数20141031 jiangym modify
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 5,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	while(1)
	{
		cls_printf("SysTimeBeepUnblock压力测试中\n总共:%d次,已进行:%d次",cnt,i);
		if(i++>=cnt)//达到测试次数后退出
			break;
		if((ret=NDK_SysTimeBeepUnblock(1000,500)) != NDK_OK)//频率1000,间隔0.5s
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:SysTimeBeep压力测试失败(%d)", __LINE__, ret);
			return;
		}
		sleep(0.5);
		succ++;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"SysTimeBeep压力测试完成,执行次数:%d次,成功%d次", i-1, succ); 
	return;
}
#else
void SysTimeBeepUnblock_press(void){cls_show_msg1(5, "低端产品不支持SysTimeBeepUnblock,如果已经支持,请联系测试部增加用例");return;}
#endif

void systest47(void)
{	
	/*private & local definition*/
	int keyin = 0;
		
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	if(auto_flag==1)//autotest_flag
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行SysBeep压力测试
		SysBeep_press();

		//执行SysTimeBeep压力测试
		SysTimeBeep_press();
		
		//执行SysTimeBeepUnblock压力测试
		//SysTimeBeepUnblock_press();
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		keyin = cls_show_msg("1.SysBeep压力测试\n"
							 "2.SysTimeBeep压力测试\n"
							 "3.SysTimeBeepUnblock压力测试");
		switch(keyin)
		{
			case '1':
				SysBeep_press();
				break;
			case '2':
				SysTimeBeep_press();
				break;	
			case '3':
				SysTimeBeepUnblock_press();
				break;	
			case ESC:
				return;
				break;
			default:
				continue;
				break;
		}
	}
	return; 
}


