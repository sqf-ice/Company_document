/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App10.c
* Author 			:
* version			: 
* DATE			:
* directory 		: 
* description		: 测试NDK_AppLoad接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#define  TESTAPI	  "NDK_AppLoad"
#if CPU5810X_ENABLE ||defined ME50NX||defined ME68
#define  FILETEST1   "mapp.NLP"
#else
#define  FILETEST1   "/appfs/mapp.NLP" 
#endif

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的10号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*
*****************************************************************/
void app10(void)
{
	/*private & local definition*/
	int  ret = 0;
	
	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保%s测试文件存在,ESC退出", TESTAPI,FILETEST1)==ESC)
		return;//屏幕提示当前测试所在模块与用例
	
	//case1:正常装载
	if(( ret = NDK_AppLoad((char *)FILETEST1,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
		RETURN;
	}
	
	//测试通过
	send_result("%s测试通过", TESTAPI);
	return;
}

