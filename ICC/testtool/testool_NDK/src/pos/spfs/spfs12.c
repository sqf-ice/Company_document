/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name			: spfs12.c
* Author 			: wangk
* version			: 
* DATE				: 20180829
* directory 		: 
* description		: 测试NDK_SP_FsFileSize接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI		"NDK_SP_FsFileSize"
#define FILETEST14	"/appfs/FT14"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的18号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*                          wangk          20180829        created
*****************************************************************/
void spfs12(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, ret=0;
	uint punSize=0;
	
	char writebuf[200]={0}/*, readbuf[200] = {0}*/;

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_SP_FsDel(FILETEST14);

	//case1: 以"w"方式打开一个特定文件，调用filelength取文件长度(应该返回0)
	if((fp=NDK_SP_FsOpen(FILETEST14, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_SP_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:关闭该文件后再以"w"方式打开，并写入200字节数据0x18,调用NDK_SP_FsFileSize计算文件大小，应能得到相应的大小200
	if((fp=NDK_SP_FsOpen(FILETEST14, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x18, sizeof(writebuf));
	if((writelen=NDK_SP_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_SP_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}

	//case3:调用fseek将文件指针移到100位置,调用NDK_SP_FsFileSize取该文件的长度(应该返回200)
	if((ret=NDK_SP_FsSeek(fp, 100l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:文件关闭后获取文件大小应该保持不变
	if((ret=NDK_SP_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}

	//case5:以"r"方式打开该文件,调用NDK_SP_FsFileSize取该文件的长度(应该返回200)
	if((fp=NDK_SP_FsOpen(FILETEST14, "r"))==NDK_ERR_OPEN_DEV)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_SP_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}


	//case7:异常测试
	if((ret=NDK_SP_FsFileSize(NULL, &punSize))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_SP_FsFileSize(FILETEST14, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试后置
	if((ret=NDK_SP_FsDel(FILETEST14))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	// 测试通过
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETEST14);
	return;
}