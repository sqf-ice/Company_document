/*************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: general
* file name		: general21
* Author 			: linwl
* version			: V1.0
* DATE			: 20080324
* directory 		: .\SRC\pos\general
* description		: 测试计算菲波纳契数列的效率
* related document	: NL_POS程序员参考手册(2007-05-21)
*					 
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM		"CPU性能"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static unsigned int fibonach(unsigned int n)
{
	if(n>1) 
		return fibonach(n-1)+fibonach(n-2);
	else    
		return n;
}

static int recursiveTest(void)
{
	unsigned int i = 0;
	int f[30] = {0}, nTmp = 0;
	float fTimeElapsed=0.0;
#if K21_ENABLE
	char str[32] = {0};
#endif	
	//计算出菲波纳契数列的正确值存放在f[30]数组中
	f[0] = 0;
	f[1] = 1;
	for(i=2; i<28; i++)
		f[i] = f[i-1]+f[i-2];

	cls_printf("计算菲波纳契数列中...");
	lib_StartStopwatch();
	for(i=1; i<28; i++)
	{
		nTmp = fibonach(i);//调用菲波纳契的递归函数
		if(nTmp==f[i])
			;//cls_printf("菲波纳契数列:\nf(%02u)=%u\n", i, nTmp);
		else
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"计算菲波纳契数列第%d个元素时失败!(%d, %d)", f[i], nTmp);
			return FAIL;
		}
	}
	fTimeElapsed = lib_ReadStopwatch();
#if !K21_ENABLE
	cls_show_msg_record(FILENAME ,FUNCNAME ,"计算菲波纳契数列前%d个元素所用时间为%.2f秒。", i-1, fTimeElapsed);
#else
	memset(str,0,sizeof(str));
	ftos(fTimeElapsed,str);
	cls_show_msg_record(FILENAME ,FUNCNAME ,"计算菲波纳契数列前%d个元素所用时间为%s秒。", i-1, str);
#endif
	return SUCC;
}

/****************************************************************
* function name 	 	 : 
* functional description   : 
* input parameter	 	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date		  remarks
*					  lwl				20080324	  created
*****************************************************************/
void systest12(void)
{
	/*private & local definition*/
	int key = 0;
	
	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	if(auto_flag==1)
	{
		//自动化测试内容
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		
		//执行压力测试
		recursiveTest();

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;

	}
	while (1)
	{
		key = cls_show_msg("_CPU性能测试_\n"
							"1.菲波纳契数列测速\n");
		switch(key)
		{
		case '1':
			recursiveTest();
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

