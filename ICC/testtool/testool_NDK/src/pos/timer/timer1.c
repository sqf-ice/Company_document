/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: timer模块
* file name		: timer1.c
* Author 			: huangbs
* version			: 
* DATE			: 20120905
* directory 		: 
* description		: 测试NDK_Wait接口功能
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_Wait"

/****************************************************************
* function name 	 	: void timer1(void)
* functional description 	: 延时函数
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  huangbs     20120905	created
*****************************************************************/
void timer1(void)
{
	time_t oldtime=0, diff=0;

	cls_printf("测试%s...请耐心等待30s", TESTAPI); 

	//case1 :测试延时
	oldtime=time(NULL);
	NDK_Wait(30*ONESECOND);
	if((diff=abs(time(NULL)-oldtime-30))>2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, diff);
		RETURN;
	}

	send_result("%s测试通过", TESTAPI);
	return;
}

