/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 键盘
* file name		: 1.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_KbFlush能否清除键盘缓冲区。NDK_KbGetCode能够在超时时间内读取键盘按键值,NDK_KbHit能否获取缓冲区中的首个键盘键值，立即返回
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define  TESTAPI		"NDK_KbFlush,NDK_KbGetCode,NDK_KbHit"
#define  WUCHASEC 0.03//0.03 //这个值待定
#define	 MAXTIMEOUT	10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: kb1
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*****************************************************************/
void kb1(void)
{
	/*private & local definition*/
	int cnt = 0,ret = 0;
	float tmp = 0;
	int nKeyin = 0;
#if K21_ENABLE
	char str[32] = {0};
#endif
	
#include "kbtable.h"
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:不合理的参数:pnCode为NULL
#if !K21_ENABLE //K21允许参数为NULL
	if ((ret=NDK_KbHit(NULL))!=NDK_ERR_PARA)
#else
	if ((ret=NDK_KbHit(NULL))!=NDK_OK)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}
	
	//pnCode为NULL时,返回成功,因为只是在等待时间按键,并没有要获取当前的按键内容
	if ((ret=NDK_KbGetCode(2,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		RETURN;
	}	

	//case2:测试NDK_KbFlush"清除按键缓冲区"功能和NDK_KbHit"检查按键缓冲区"
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请在%ds内按下%s键,NDK_KbHit将捕获它\n", MAXWAITTIME, table[cnt].keyname);
	nKeyin = wait_anykey(MAXWAITTIME);
	if (nKeyin!=table[cnt].keycode && nKeyin!=0)
	{
		send_result("line %d: NDK_KbHit测试失败", __LINE__);
		RETURN;
	}

	//case3:测试NDK_KbGetCode(30)
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请在%ds内按下%s键,NDK_KbGetCode(30)将捕获它\n", MAXWAITTIME, table[cnt].keyname);
	nKeyin = lib_getkeycode(30);
	if (nKeyin!=table[cnt].keycode && nKeyin!=0)
	{
		send_result("line %d: NDK_KbGetCode测试失败", __LINE__);
		RETURN;
	}

	//case4:测试NDK_KbGetCode(0)
	cnt = rand()%(SZ_ARRAY(table));
	cls_printf("请按下%s键,NDK_KbGetCode(0)将捕获它\n", table[cnt].keyname);
	nKeyin = lib_getkeycode(0);
	if (nKeyin!=table[cnt].keycode && nKeyin!=0)
	{
		send_result("line %d: NDK_KbGetCode测试失败", __LINE__);
		RETURN;
	}

	//case5:超时测试
	cls_printf("请在%ds内不按下任何键,请耐心等待...", MAXTIMEOUT);
	lib_StartStopwatch();
	nKeyin = lib_getkeycode(MAXTIMEOUT);
#if defined ME30GX||defined ME32GX
	if (nKeyin!=0||(tmp=abs(INT_ReadStopwatch()-MAXTIMEOUT))>WUCHASEC)
#else
	if (nKeyin!=0||(tmp=fabs(lib_ReadStopwatch()-MAXTIMEOUT))>WUCHASEC)
#endif
	{
#if !K21_ENABLE
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, nKeyin, tmp);
#else
		memset(str,0,sizeof(str));
		ftos(tmp, str);
		send_result("line %d: %s测试失败(%d,%s)", __LINE__, TESTAPI, nKeyin, str);
#endif
		RETURN;
	}

#if !K21_ENABLE	//低端产品不支持自动休眠功能,且没有休眠开关,不测试20160729 linwl
#if !(defined SP60||defined ME31S||defined ME31SLS||defined SP33||defined SP600||defined SP60G||defined SP33G||defined SP600G||defined ME51||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG)
	//case6:按键唤醒休眠时，唤醒按键不应被应用获取到(应丢弃)
	if(cls_show_msg("本产品是否支持休眠并已打开休眠开关,是:按[ENTER],否:按其它")==ENTER)
	{
		if(cls_show_msg("请等待进入休眠后按键唤醒,唤醒后,仍在本界面!是:按[ENTER],否:按其它")!=ENTER)
		{
			send_result("line %d: NDK_KbGetCode测试失败", __LINE__);
			RETURN;
		}
	}
#endif

	//case7:休眠唤醒后，按键唤醒后紧接的下一个按键应被应用获取到
	if(cls_show_msg("本产品是否支持休眠并已打开休眠开关,是:按[ENTER],否:按其它")==ENTER)
	{
		cls_show_msg("请等待进入休眠后按键唤醒,唤醒后如果仍在本界面,按任意键继续");
		cnt = rand()%(SZ_ARRAY(table));
		cls_printf("请按下%s键,NDK_KbGetCode(0)将捕获它\n", table[cnt].keyname);
		nKeyin = lib_getkeycode(0);
		if (nKeyin!=table[cnt].keycode && nKeyin!=0)
		{
			send_result("line %d: NDK_KbGetCode测试失败", __LINE__);
			RETURN;
		}
	}
#endif

	send_result("%s测试通过", TESTAPI);
	return;
}

