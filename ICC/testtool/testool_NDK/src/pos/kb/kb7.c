/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 键盘
* file name			: 7.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			:测试NDK_PS2GetCode超时时间内读取PS2键盘按键值、NDK_PS2Hit非阻塞获取PS2键盘键值
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_PS2GetCode,NDK_PS2Hit"
#define  MAXWAITTIME 30
#define  WUCHASEC 0.03//0.03 //这个值待定
#define	 MAXTIMEOUT	10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: kb7.c
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*							sull			20180522		created
*
*****************************************************************/
void kb7(void)
{
	/*private & local definition*/
	int ret = 0, pnCode = -1, key = -1, cnt = 0;
	time_t oldtime = 0, diff = 0;
	char str[32] = {0};
	float tmp = 0;

#include "kbtable.h"
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	cls_show_msg("请将POS机与PS2键盘相连，按任意键继续测试");

	//测试前置:使能PS2键盘
	if((ret = NDK_PS2Start()) != NDK_OK)
	 {
		 send_result("line %d:%s使能PS2键盘失败(%d)", __LINE__, TESTAPI,ret);
		 return;
	 }

	//case1:pnCode为NULL
	if((ret = NDK_PS2Hit(NULL)) != NDK_OK)
	 {
		 send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		 GOTOERR;
	 }
		 
	 //pnCode为NULL时,返回成功,因为只是在等待时间按键,并没有要获取当前的按键内容
	 if ((ret=NDK_PS2GetCode(2,NULL))!=NDK_OK)
	 {
		 send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
		 GOTOERR;
	 }	 
	 
	 //case2:测试NDK_PS2GetCode(30)，键盘左边数字部分
	 cnt = rand()%(SZ_ARRAY(ps2_num));
	 cls_printf("请在%ds内按下键盘左边的%s键,NDK_PS2GetCode(30)将捕获它\n", MAXWAITTIME, ps2_num[cnt].keyname);
	 if((ret = NDK_PS2GetCode(30,&key)) != NDK_OK)
	 {
		 send_result("line %d: NDK_PS2GetCode测试失败(%d)", __LINE__, ret);
		 GOTOERR;
	 }
	if(key!= ps2_num[cnt].keycode)
	 {
		 send_result("line %d: NDK_PS2GetCode测试失败(0x%02x)", __LINE__, key);
		 GOTOERR;
	}
	cls_show_msg1(1,"子用例1测试通过");
		
	 //case3:测试NDK_PS2GetCode(0)，键盘F 键部分
	 cnt = rand()%(SZ_ARRAY(ps2_f));
	 cls_printf("请按下%s键,NDK_PS2GetCode(0)将捕获它\n", ps2_f[cnt].keyname);
	 if((ret = NDK_PS2GetCode(0, &key)) != NDK_OK)
	  {
		 send_result("line %d: NDK_PS2GetCode测试失败(%d)", __LINE__, ret);
		 GOTOERR;
	 }
	 if (key!=ps2_f[cnt].keycode)
	 {
		 send_result("line %d: NDK_PS2GetCode测试失败(0x%02x)", __LINE__, key);
		 GOTOERR;
	 }
	 cls_show_msg1(1,"子用例2测试通过");
	 
	 //case4:超时测试
	 cls_printf("请在%ds内不按下任何键,请耐心等待...", MAXTIMEOUT);
	 lib_StartStopwatch();
	 if((ret = NDK_PS2GetCode(MAXTIMEOUT, &key)) != NDK_OK)
	 {
		 send_result("line %d: NDK_PS2GetCode测试失败(%d)", __LINE__, ret);
		 GOTOERR;
	 }
	 if (key!=0||(tmp=fabs(lib_ReadStopwatch()-MAXTIMEOUT))>WUCHASEC)
	 {
		 memset(str,0,sizeof(str));
		 ftos(tmp, str);
		 send_result("line %d: %s测试失败(0x%02x,%s)", __LINE__, TESTAPI, key, str);
		 GOTOERR;
	 }

	 //case5:测试NDK_PS2Hit没有键按下立即返回0
	 if(((ret = NDK_PS2Hit(&pnCode)) != NDK_OK) || (pnCode != 0))
	 {
		 send_result("line %d: NDK_PS2GetCode测试失败(%d,0x%02x)", __LINE__, ret, pnCode);
		 GOTOERR;
	 }

	 //case6:测试有按键，NDK_PS2Hit正确捕获按键，小写字母部分
	 cnt = rand()%(SZ_ARRAY(ps2_small_letter));
	 cls_printf("请在%ds内按下%s键,NDK_PS2Hit将捕获它\n", MAXWAITTIME, ps2_small_letter[cnt].keyname);
	 oldtime = time(NULL);
	 while(1)
	 {
	 	if((time(NULL)-oldtime) >= MAXWAITTIME)
	 	{
	 		send_result("line %d:用户30s内未按键，超时退出", __LINE__);
	 		GOTOERR;
			break;
	 	}
		 NDK_PS2Hit(&pnCode);
		 if(pnCode==0)
			continue;
		 else if(pnCode!=ps2_small_letter[cnt].keycode)
		 {
			send_result("line %d: NDK_PS2GetCode测试失败(0x%02x)", __LINE__, pnCode);
		 	GOTOERR;
			break;
		 }
		 else
			break;	
	 }
	 cls_show_msg1(1,"子用例3测试通过");

	 //case7:测试标点符号部分
	 cnt = rand()%(SZ_ARRAY(ps2_char));
	 cls_printf("请在%ds内按下键盘左边的%s键,NDK_PS2GetCode(30)将捕获它，若在键盘上方的，先按shift键，再按对应按键\n", MAXWAITTIME, ps2_char[cnt].keyname);
	 if(((ret = NDK_PS2GetCode(30,&key)) != NDK_OK) ||(key!= ps2_char[cnt].keycode))
	 {
		 send_result("line %d: NDK_PS2GetCode测试失败(%d,0x%02x)", __LINE__, ret, key);
		 GOTOERR;
	 }
	 cls_show_msg1(1,"子用例4测试通过");

	//case8:测试大写字母
	 cnt = rand()%(SZ_ARRAY(ps2_big_letter));
	 cls_printf("请在%ds内按下%s键,NDK_PS2Hit将捕获它[先按shift再按字母键]\n", MAXWAITTIME, ps2_big_letter[cnt].keyname);
	 oldtime = time(NULL);
	 while(1)
	 {
	 	if((time(NULL)-oldtime) >= MAXWAITTIME)
	 	{
	 		send_result("line %d:用户30s内未按键，超时退出, __LINE__");
	 		GOTOERR;
			break;
	 	}
		NDK_PS2Hit(&pnCode);
		if(pnCode==0)
			continue;
		else if(pnCode!=ps2_big_letter[cnt].keycode)
		{
			send_result("line %d: NDK_PS2GetCode测试失败(0x%02x)", __LINE__, pnCode);
			GOTOERR;
			break;
		}
		else
			break;	
	 }
	 cls_show_msg1(1,"子用例5测试通过");

	 //case9:测试右边键盘部分
	 cnt = rand()%(SZ_ARRAY(ps2_right));
	 cls_printf("请按下右边的%s键,NDK_PS2GetCode(0)将捕获它\n", ps2_right[cnt].keyname);
	 if(((ret = NDK_PS2GetCode(0, &key)) != NDK_OK) || (key!=ps2_right[cnt].keycode))
	  {
		 send_result("line %d: NDK_PS2GetCode测试失败(%d,0x%02x)", __LINE__, ret, key);
		 GOTOERR;
	 }
	 cls_show_msg1(1,"子用例6测试通过");
 	
	 //case10:测试按了Num Lock之后，右边小键盘按键应无效  //底层未实现这种用法，由应用自行实现
	
	//case11:测试特殊按键部分
	 cnt = rand()%(SZ_ARRAY(ps2_other));
	 cls_printf("请按下%s键,NDK_PS2GetCode(0)将捕获它\n", ps2_other[cnt].keyname);
	 if(((ret = NDK_PS2GetCode(0, &key)) != NDK_OK) || (key!=ps2_other[cnt].keycode))
	  {
		 send_result("line %d: NDK_PS2GetCode测试失败(%d,0x%02x)", __LINE__, ret, key);
		 GOTOERR;
	 }

	 //测试后置:关闭PS2键盘
	 if((ret = NDK_PS2Stop()) != NDK_OK)
	  {
		 send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		 goto ERR;
	 }	

	//结束测试
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_PS2Stop();
	return;
}



