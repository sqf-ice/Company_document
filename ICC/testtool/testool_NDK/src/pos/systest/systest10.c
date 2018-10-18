/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: RTC综合测试
* file name			: 30.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: RTC综合测试
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"RTC综合"

#define	BASE_YEAR		(1900)
#define	BASE_MON		(1)

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
* history 		 		: author			date			remarks
*			  		      linwei	        20130121	 	created
*****************************************************************/
void systest10(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	//以下为局部变量
	int ret = -1, keyin = 0;
	struct tm stOldPosTime;
	struct tm stExpect;
	struct tm stNew_time;

	//以下为函数体
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	if(cls_show_msg("此用例需要在root权限下进行,[Enter]继续,其他返回" )!=ENTER)
	{
		cls_show_msg("line %d:%s测试中断", __LINE__, TESTITEM);
		return;
	}
	memset(&stOldPosTime, 0, sizeof(stOldPosTime));
	memset(&stExpect, 0, sizeof(stExpect));
	memset(&stNew_time, 0, sizeof(stNew_time));

	//设置合理时间(非BCD码了,int型)
	//E90支持的年份:2010-2099; E90G:2000-2030
	//综合各方面,选择2025年12月31日作为设置值
	stNew_time.tm_year = stExpect.tm_year = 2025 - BASE_YEAR;
	stNew_time.tm_mon = stExpect.tm_mon = 12 - BASE_MON;
	stNew_time.tm_mday = stExpect.tm_mday = 31;
	stNew_time.tm_wday = stExpect.tm_wday = 3;
	stNew_time.tm_hour = stExpect.tm_hour = 0x17;
	stNew_time.tm_min = stExpect.tm_min = 0x37;
	stNew_time.tm_sec = stExpect.tm_sec = 0x03;
	if((ret=NDK_SysGetPosTime(&stOldPosTime)) != NDK_OK)
	{
		cls_show_msg("line %d:获取时间失败", __LINE__);
		return;
	}
	if (stOldPosTime.tm_year == stNew_time.tm_year)
	{
		cls_show_msg("请先将系统时间改为非%04d年,任意键退出...", stNew_time.tm_year+BASE_YEAR);
		return;
	}
	if((ret=NDK_SysSetPosTime(stNew_time)) != NDK_OK)
	{
		cls_show_msg("line %d:设置时间失败", __LINE__);
		return;
	}
	cls_show_msg("时间应已调整为%04d年%02d月%02d日%02d:%02d,任意键继续...", stExpect.tm_year+BASE_YEAR, stExpect.tm_mon+BASE_MON, stExpect.tm_mday, stExpect.tm_hour, stExpect.tm_min);
	
	while(1)
	{
		keyin = cls_show_msg("请选择动作:\n"
							"1.退出程序\n"
							"2.关机后再重启机器\n"
							"3.重启机器\n");

		switch(keyin) 
		{
		case '1':
			cls_show_msg1(3, "手动退出程序后,请人工校验所设时间!\n");
			return;
			break;
		case '2':
#if !(ANDIROD_ENABLE)
			cls_show_msg("按任意键关机，重启机器后,请人工校验所设时间!\n");
			NDK_SysShutDown();
#else
			cls_printf("本产品不支持自动关机函数,请手动关机重启机器后,再人工校验所设时间!\n");
			while(1);
#endif	
			break;
		case '3':
#if defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||defined ME66||CPU5810X_ENABLE||defined ME50NX||defined ME68||defined ME50S||defined ME50NS //ME30需要插着电源线才能重启，故增加提示
			cls_show_msg("请插上电源线后，按任意键继续\n");
#endif
			cls_show_msg1(3, "重启机器后,请人工校验所设时间!\n");
			NDK_SysReboot();
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

