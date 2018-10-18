/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name		: spfs2.c
* Author 			: linying
* version			: 
* DATE			: 20180828
* directory 		: 
* description		: 测试NDK_SP_FsClose接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/

#define  FILETEST2 "/appfs/FT2"
#define 	TESTAPI		"NDK_SP_FsClose"

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
*                                 linying              20180828       created
*****************************************************************/
void spfs2(void)
{
	/*private & local definition*/
	volatile int countnum=0;
	int fp=0, writelen=0, ret=0;
	char writebuf[200]={0};

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_SP_FsDel(FILETEST2); // 确保打开的是一个空的测试文件

	//case1:创建一个空文件，删除一个刚创建的文件，应该成功返回。
	if((fp=NDK_SP_FsOpen(FILETEST2, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:以"w"方式打开刚创建的文件并写入一定数据，关闭非空文件应该成功返回。
	if((fp=NDK_SP_FsOpen(FILETEST2, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x22, sizeof(writebuf)); // 设置内容
	if((writelen=NDK_SP_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:以"r"方式打开刚创建的文件，关闭以"r"方式打开的文件，应成功返回。
	if((fp=NDK_SP_FsOpen(FILETEST2, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:关闭已经关闭的文件，应失败返回。
	if((ret=NDK_SP_FsClose(fp))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:关闭允许句柄范围外的描述符，应失败返回。
	if((ret=NDK_SP_FsClose(-1))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:关闭一个没有打开过的句柄，应失败返回。
	if((ret=NDK_SP_FsClose(113))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试清理
	if((ret=NDK_SP_FsDel(FILETEST2))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if((ret=NDK_SP_FsClose(MIN_SIGNINT))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_SP_FsClose(MAX_SIGNINT))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 1//!K21_ENABLE//低端平台编译器原因引起for循环卡死的BUG，目前无法解决
	for(countnum=0, fp=MIN_SIGNINT;countnum<21;fp+=MAX_SIGNINT/10, countnum++)
	{
		if((ret=NDK_SP_FsClose(fp))!=NDK_ERR)
		{
			send_result("line %d: %s测试失败(%d, %d)", __LINE__, TESTAPI, fp, ret);
			goto ERR;
		}
	}
#endif
	// 测试通过
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETEST2);
	return;
}

