/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs10.c
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: 测试NDK_FsTruncate接口文件截短功能
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if !K21_ENABLE || OVERSEAS_ENABLE || defined SP610
#define  FILETEST10 "FT10"
#define  FILETESTA  "FTA" 
#define  FILETESTB  "FTB"
#else
#define  FILETEST10 "/appfs/FT10"
#define  FILETESTA  "/appfs/FTA"
#define  FILETESTB  "/appfs/FTB"
#endif
#define 	TESTAPI		"NDK_FsTruncate"

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
*
*****************************************************************/
void fs10(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, ret=0, fp1=0, writelen1=0,readlen1=0;
	char writebuf[200]={0},writebuf1[200]={0},readbuf1[200]={0};
	uint punSize=0;
#if !K21_ENABLE
	int readlen = 0;
	char readbuf[200]={0};
#endif

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//测试前置，并创建测试文件，长度200，内容0x12
	NDK_FsDel(FILETEST10);
	if((fp=NDK_FsOpen(FILETEST10, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x12, sizeof(writebuf));
	if((writelen=NDK_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}

	//case1: 测试设置文件长度等于文件自己原来长度200的情况，设置应该成功
	if((ret=NDK_FsTruncate(FILETEST10, 200))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST10, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}

	//case2: 测试设置该文件的长度为100(操作应该成功),再获取文件长度应相应的变为100
	if((ret=NDK_FsTruncate(FILETEST10, 100))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST10, &punSize))!=NDK_OK||punSize!=100)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
#if !K21_ENABLE
	//case3:测试设置该文件的长度为200大于100(操作应该成功),再获取文件长度应相应的变为200，且会补充0xff
	if((ret=NDK_FsTruncate(FILETEST10, 200))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST10, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//验证补充0xff
	if((fp=NDK_FsOpen(FILETEST10, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x12, sizeof(writebuf));//构造前100字节0x12后100字节0xff
	memset(writebuf+100, 0xff, 100);
	memset(readbuf, 0, sizeof(readbuf));
	if((readlen=NDK_FsRead(fp, readbuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;
	}
	if(memcmp(writebuf, readbuf, 200))
	{
		send_result("line %d:校验内容出错", __LINE__);
		GOTOERR;
	}
#endif
	//case4: 调用NDK_FsTruncate设置该文件的长度为0(文件)
	if((ret=NDK_FsTruncate(FILETEST10, 0l))!=NDK_OK) //GP730和E80这里设置成功 文件长度变为0
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST10, &punSize))!=NDK_OK||punSize!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试后置
	if((ret=NDK_FsDel(FILETEST10))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:对一个不存在的文件做NDK_FsTruncate,应失败返回
	if((ret=NDK_FsTruncate(FILETEST10, 200l))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:文件路径为NULL
	if((ret=NDK_FsTruncate(NULL, 200l))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:对一个不合法文件名的文件测试
#if CPU5810X_ENABLE/*defined SP610*///5810平台统一返回-4 张兆鑫解释 20180427 modify
	if((ret=NDK_FsTruncate("/", 200l))!=NDK_ERR_OPEN_DEV)
/*#elif defined ME50S //凯哥解释50s对/是只可读，原来的是可读写，返回值不一样了
	if((ret=NDK_FsTruncate("/", 200l))!=NDK_ERR_PATH)*///50S也是返回-1
#else
	if((ret=NDK_FsTruncate("/", 200l))!=NDK_ERR)
#endif
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case8:新增测试:创建A 文件,往A写数据后进行截短,然后创建B文件,往B文件写数据,同时对A文件也写数据,然后读B文件数据进行比较应该与写入的内容一致(在sp610发现过B文件会被修改的现象然后新增此测试点) sull add 20180207
	if((fp = NDK_FsOpen(FILETESTA, "w")) <0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x13, sizeof(writebuf));
	if((writelen = NDK_FsWrite(fp, writebuf, 200)) != 200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_FsTruncate(FILETESTA, 0l))!=NDK_OK) 
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETESTA, &punSize))!=NDK_OK||punSize!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((fp1 = NDK_FsOpen(FILETESTB, "w")) <0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp1);
		GOTOERR;
	}
	memset(writebuf1, 0x11, sizeof(writebuf1));
	if((writelen1=NDK_FsWrite(fp1, writebuf1, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen1);
		GOTOERR;
	}
	memset(writebuf, 0x12, sizeof(writebuf));
	if((writelen = NDK_FsWrite(fp, writebuf, 200)) != 200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_FsSeek(fp1, 0l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(readbuf1, 0, sizeof(readbuf1));
	if((readlen1=NDK_FsRead(fp1, readbuf1, 200))!=200)
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
	NDK_FsClose(fp);
	NDK_FsClose(fp1);
	NDK_FsDel(FILETEST10);
	NDK_FsDel(FILETESTA);
	NDK_FsDel(FILETESTB);
	return;
}

