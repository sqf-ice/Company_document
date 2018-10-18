/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys5.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysShutDown接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysShutDown"

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
*			  		        linwei	           20130121	 	created
*****************************************************************/
void sys5(void)
{
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
#if STABAR_LED_ENABLE
	NDK_ScrStatusbar(STATUSBAR_DISP_LED|STATUSBAR_POSITION_TOP);
#endif
	NDK_LedStatus(LED_RFID_RED_ON);
	#if (defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME50||(CPU5810X_ENABLE&&!defined ME62)||defined ME50NX||defined ME68||defined ME50NS||defined ME31||ANDIROD_ENABLE)//||defined IM81||defined N900
		if(cls_show_msg("——警告——\n进行软关机吗?\n是:拔掉USB线后按[ENTER] 否[其他]")==ENTER)//K21硬件设计USB供电情况下不能关机，调用函数后拔线会马上关机
			NDK_SysShutDown();
		goto end;
	#elif  defined ME11||defined ME15||defined ME20||defined ME15C  //在只有电池供电情况下,电池供电需使用手机APP操作
		if(cls_show_msg("进行软关机吗?(只有电池供电时才能关机)\n是:按[ENTER] 否[其他]")==ENTER)
		{
			cls_show_msg1(5,"5秒内拔掉外接电源线,观察LED灯应该灭掉");//如果不拔线则由于USB或者串口线供电，将不会关机
			NDK_SysShutDown();
			return;
		}
		goto end;
	#else	
		send_result("请在测试主菜单按[.]进行%s验证", TESTAPI);
	#endif
end:
	NDK_LedStatus(LED_RFID_RED_OFF);
#if STABAR_LED_ENABLE
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	return;
}

