/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs13.c
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: 测试NDK_FsExist接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if !K21_ENABLE || OVERSEAS_ENABLE || defined SP610
#define  FILETEST13 "FT13"
#else //M31只支持绝对路径
#define  FILETEST13 "/appfs/FT13"
#endif
#define 	TESTAPI		"NDK_FsExist"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的17号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*
*****************************************************************/
void fs13(void)
{
	/*private & local definition*/
	int fp=0, ret=0;

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_FsDel(FILETEST13);

	// case1: 以"w"方式打开一个特定文件,调用NDK_FsExist判断该文件是否存在(应该存在)
	if((fp=NDK_FsOpen(FILETEST13, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_FsExist(FILETEST13))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2:关闭文件后，再判断文件是否存在(应该存在)
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsExist(FILETEST13))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3: 删除该文件,调用NDK_FsExist判断该文件是否存在(应该不存在)
	if((ret=NDK_FsDel(FILETEST13))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsExist(FILETEST13))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:非法参数测试
	if((ret=NDK_FsExist(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	// 测试通过
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_FsClose(fp);
	NDK_FsDel(FILETEST13);
	return;
}

