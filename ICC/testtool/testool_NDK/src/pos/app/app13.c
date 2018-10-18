/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name			: App13.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description		: 测试 NDK_NlpVerify校验NLP 接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_NlpVerify"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的2号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*						jiangym           20170622          created
*****************************************************************/
void app13(void)
{
	/*private & local definition*/
	int ret = 0;
	int  errnum = 0;
	char nlpbuf[950] = {0}; //50*19

	/*process body*/
	if(cls_show_msg1(30, "测试%s...请确保测试NLP存在,ESC退出", TESTAPI)==ESC)
		return;
	
	cls_printf("正在校验NLP包...");
	//case1:参数异常
	if(( ret = NDK_NlpVerify(NULL,&errnum))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret = NDK_NlpVerify(nlpbuf,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:正常测试
	if(( ret = NDK_NlpVerify(nlpbuf,&errnum))!=NDK_OK)
		cls_show_msg1(30,"出错返回ret=%d,出错的NLP有%d个,为:%s,任意键继续", ret, errnum, nlpbuf);
	else
		cls_show_msg1(30,"没有出错的NLP包,任意键继续");
	
	send_result("%s测试通过", TESTAPI);
	return;
}

