/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name		: spfs5.c
* Author 			: chenfm
* version			: 
* DATE			: 20180828
* directory 		: 
* description		: 测试NDK_SP_FsDel接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/

#define  FILETEST5 "/appfs/FT5"
#define	TESTAPI		"NDK_SP_FsDel"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的N号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 : 测试fdel删除文件的功能
* history 		 		 : author			date	 	  remarks
*                                linying                20180828     created
*****************************************************************/
void spfs5(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, readlen=0, ret=0;
	char writebuf[200]={0}, readbuf[200]={0};

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//测试前置	
	NDK_SP_FsDel(FILETEST5);
#if 0
	//case1: 以"w"方式打开一个特定文件,调用NDK_FsDel删除以"w"方式打开并且未关闭的空文件，应该失败返回。
	if((fp=NDK_FsOpen(FILETEST5, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if ((ret=NDK_FsDel(FILETEST5))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if 0
	//case2:以"r"方式打开该空文件,调用NDK_FsDel删除以"r"方式打开并且未关闭的空文件，应该失败返回。
	if((fp=NDK_FsOpen(FILETEST5, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if ((ret=NDK_FsDel(FILETEST5))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if 0
	//case3: 再用"w"方式打开该文件并写入200字节数据0x05,调用fdel删除以"w"方式打开且该未关闭的非空文件(应该出错)
	if((fp=NDK_FsOpen(FILETEST5, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x05, sizeof(writebuf));
	if((writelen=NDK_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if ((ret=NDK_FsDel(FILETEST5))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if 0
	//case4:以"r"打开文件,调用fdel删除以"r"方式打开且该未关闭的非空文件(应该出错)
	if((fp=NDK_FsOpen(FILETEST5, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if ((ret=NDK_FsDel(FILETEST5))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//测试前置	
	NDK_SP_FsDel(FILETEST5);
	if((fp=NDK_SP_FsOpen(FILETEST5, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x05, sizeof(writebuf));
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
	//case5: 以"r"方式打开该文件，读取200字节的数据并进行校验
	if((fp=NDK_SP_FsOpen(FILETEST5, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(readbuf, 0, sizeof(readbuf));
	if((readlen=NDK_SP_FsRead(fp, readbuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;
	}
	if(memcmp(writebuf, readbuf, 200))
	{
		send_result("line %d:校验内容出错", __LINE__);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6: 调用fdel删除该文件(删除存在的文件),刚删除的文件应该不存在了
	if((ret=NDK_SP_FsDel(FILETEST5))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsExist(FILETEST5))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7: 再次调用NDK_FsDel去删除在步骤6中已经删除了的文件(此次删除的是不存在的文件,应该失败)
	if((ret=NDK_SP_FsDel(FILETEST5))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case8:测试NDK_FsDel一个不合法的文件名,应该出错
	if((ret=NDK_SP_FsDel("/"))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case9:异常测试
	if((ret=NDK_SP_FsDel(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	

	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETEST5);
	return;
}

