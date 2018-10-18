/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name			: spfs9.c
* Author 			: wangk
* version			: 
* DATE				: 20180829
* directory 		: 
* description		: 测试NDK_SP_FsTruncate接口文件截短功能
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SP_FsTruncate"
#define  FILETEST10 "/appfs/FT10"
#define  FILETESTA  "/appfs/FTA"
#define  FILETESTB  "/appfs/FTB"


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
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*                          wangk          20180829        created
*****************************************************************/
void spfs9(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, ret=0, fp1=0, writelen1=0,readlen1=0;
	char writebuf[200]={0},writebuf1[200]={0},readbuf1[200]={0};
	uint punSize=0;
	

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//测试前置，并创建测试文件，长度200，内容0x12
	NDK_SP_FsDel(FILETEST10);
	if((fp=NDK_SP_FsOpen(FILETEST10, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x12, sizeof(writebuf));
	if((writelen=NDK_SP_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}

	//case1: 测试设置文件长度等于文件自己原来长度200的情况，设置应该成功
	if((ret=NDK_SP_FsTruncate(FILETEST10, 200))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsFileSize(FILETEST10, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}

	//case2: 测试设置该文件的长度为100(操作应该成功),再获取文件长度应相应的变为100
	if((ret=NDK_SP_FsTruncate(FILETEST10, 100))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsFileSize(FILETEST10, &punSize))!=NDK_OK||punSize!=100)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	
	//case4: 调用NDK_SP_FsTruncate设置该文件的长度为0(文件)
	if((ret=NDK_SP_FsTruncate(FILETEST10, 0l))!=NDK_OK) //GP730和E80这里设置成功 文件长度变为0
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsFileSize(FILETEST10, &punSize))!=NDK_OK||punSize!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试后置
	if((ret=NDK_SP_FsDel(FILETEST10))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:对一个不存在的文件做NDK_SP_FsTruncate,应失败返回
	if((ret=NDK_SP_FsTruncate(FILETEST10, 200l))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:文件路径为NULL
	if((ret=NDK_SP_FsTruncate(NULL, 200l))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:对一个不合法文件名的文件测试
	if((ret=NDK_SP_FsTruncate("/", 200l))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case8:新增测试:创建A 文件,往A写数据后进行截短,然后创建B文件,往B文件写数据,同时对A文件也写数据,然后读B文件数据进行比较应该与写入的内容一致(在sp610发现过B文件会被修改的现象然后新增此测试点) sull add 20180207
	if((fp = NDK_SP_FsOpen(FILETESTA, "w")) <0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x13, sizeof(writebuf));
	if((writelen = NDK_SP_FsWrite(fp, writebuf, 200)) != 200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_SP_FsTruncate(FILETESTA, 0l))!=NDK_OK) 
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsFileSize(FILETESTA, &punSize))!=NDK_OK||punSize!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((fp1 = NDK_SP_FsOpen(FILETESTB, "w")) <0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp1);
		GOTOERR;
	}
	memset(writebuf1, 0x11, sizeof(writebuf1));
	if((writelen1=NDK_SP_FsWrite(fp1, writebuf1, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen1);
		GOTOERR;
	}
	memset(writebuf, 0x12, sizeof(writebuf));
	if((writelen = NDK_SP_FsWrite(fp, writebuf, 200)) != 200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_SP_FsSeek(fp1, 0l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(readbuf1, 0, sizeof(readbuf1));
	if((readlen1=NDK_SP_FsRead(fp1, readbuf1, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen1);
		GOTOERR;
	}
	if((readlen1 != writelen1)|| (memcmp(writebuf1, readbuf1, readlen1)))
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, writelen1,readlen1);
		goto ERR;
	}
	
	// 测试通过
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_SP_FsClose(fp);
	NDK_SP_FsClose(fp1);
	NDK_SP_FsDel(FILETEST10);
	NDK_SP_FsDel(FILETESTA);
	NDK_SP_FsDel(FILETESTB);
	return;
}