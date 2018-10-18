/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示模块
* file name			: 30.c
* Author 			: zhengry
* version			: 
* DATE				: 20160510
* directory 			: 
* description			: 测试NDK_LedLight单个数码管全亮或全灭
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_LedLight"
#define MAXWAITTIME  15

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef struct _LEDLIGHT_ALG
{
	uchar nr;
	uint onoff;
}LEDLIGHT_ALG;
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scr30
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					    zhengry		    20160510		created
*****************************************************************/
void scr30(void)
{
	/*private & local definition*/
	int ret=0, i=0, nKeyin=0;
	LEDLIGHT_ALG abnormal[]={
		{0, 0},
		{8, 0},
		{0, 1},
		{8, 1},
		{1, -1},
		{1, 2},
	};
		
	/*process body*/
	cls_show_msg1(2,"测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_LedClr();
	if((ret=NDK_LedSetBright(3)) != NDK_OK)//设置正常亮度
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case1:异常参数测试:nr=0/8时
	for(i=0; i<SZ_ARRAY(abnormal); i++)
	{
		if((ret = NDK_LedLight(abnormal[i].nr, abnormal[i].onoff)) != NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
	}
	
	//case2:正常测试:数码管全部亮起后全部灭掉
	cls_printf("测试led全部亮起后灭掉中...");
	for(i=1; i<8; i++)
	{
		if((ret = NDK_LedLight(i, 1)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		NDK_SysMsDelay(200);
	}
	for(i=1; i<8; i++)
	{
		if((ret = NDK_LedLight(i, 0)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		NDK_SysMsDelay(200);
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "led数码管全亮起后再全灭掉.是[ENTER],否则[其他].");
	if( nKeyin != ENTER &&  nKeyin != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nKeyin);
		GOTOERR;
	}

	//case3:正常测试:单个数码管逐一亮灭
	cls_printf("测试led逐一亮灭中...");
	for(i=1; i<8; i++)
	{
		if((ret = NDK_LedLight(i, 1)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		NDK_SysMsDelay(200);
		if((ret = NDK_LedLight(i, 0)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		NDK_SysMsDelay(200);
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "led数码管逐一亮灭.是[ENTER],否则[其他].");
	if( nKeyin != ENTER &&  nKeyin != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nKeyin);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_LedClr();
	return;
}

