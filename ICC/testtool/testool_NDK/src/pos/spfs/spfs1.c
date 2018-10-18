/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name		: spfs1.c
* Author 			: linying
* version			: 
* DATE			: 20180828
* directory 		: 
* description		: 测试NDK_SP_FsOpen接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/

#define  FILETEST1	"/appfs/fsys1"  
#define  APPFSFILE	"/appfs/appfsfsys"	//绝对路径
#define	 NONLICET	"/"
#define 	TESTAPI		"NDK_SP_FsOpen"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 高端文件系统模块的1号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*                                 linying              20180828       created
*****************************************************************/
void spfs1(void)
{
	/*private & local definition*/
	int fp = -1, fp2 = -1;
	int writelen = 0, readlen = 0, ret = 0;
	uint punSize=0;
	char writebuf[210] = {0};
	char readbuf[210] = {0};

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例
	
	//测试前置
	NDK_SP_FsDel(FILETEST1);
	NDK_SP_FsDel(APPFSFILE);
	
	//case1:以只读的方式打开不存在的文件
	if ((fp=NDK_SP_FsOpen(FILETEST1, "r"))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}

	//case2:正常创建文件，写入文件，关闭文件，删除文件
	if ((fp=NDK_SP_FsOpen(FILETEST1, "w"))<0) // 关闭文件后以写的文件创建该文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETEST1);
	if ((fp=NDK_SP_FsOpen(FILETEST1, "w"))<0) // 以写的文件打开刚刚删除的文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x55, 200);
	if((writelen=NDK_SP_FsWrite(fp,writebuf, 200))!=200) // 写入一定数据(0x55)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	NDK_SP_FsClose(fp);
	if ((fp=NDK_SP_FsOpen(FILETEST1, "w"))<0) // 以写的文件打开刚刚创建的文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x88, 100);
	if((writelen=NDK_SP_FsWrite(fp,writebuf, 100))!=100) // 覆盖写入数据(0x88)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	NDK_SP_FsClose(fp);
	if ((fp=NDK_SP_FsOpen(FILETEST1, "r"))<0) // 检查是否能打开程序运行过程中创建的文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}

	//zhangnw20110420add 对NL的fopen("w")特性的测试:fopen("w")并不是新建文件
	if ((ret=NDK_SP_FsFileSize(FILETEST1,&punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret,punSize);
		GOTOERR;
	}
	memset(readbuf, 0, sizeof(readbuf));
	if((readlen=NDK_SP_FsRead(fp,readbuf, 200))!=200)//此处其实也验证了fopen("w")并不是新建文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;
	}
	else
	{
		if (memcmp(writebuf, readbuf, 200)) // 校验写入与读出数据的内容
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}		
	}
	NDK_SP_FsClose(fp);
	
	//case4:第2个参数非法的情况
	if ((fp=NDK_SP_FsOpen(FILETEST1, "abc"))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if ((fp=NDK_SP_FsOpen(FILETEST1, NULL))!=NDK_ERR_PARA)
	{	
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
		
	//case5:测试打开一个不合法文件名的文件
	if ((fp=NDK_SP_FsOpen(NONLICET, "w"))!=NDK_ERR_OPEN_DEV)
	{	
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		NDK_SP_FsClose(fp); //目前发现打开"/"文件会成功，需关闭该句柄，否则后面测试会受影响 20160729 linwl
		GOTOERR;
	}
	if ((fp=NDK_SP_FsOpen(NULL, "w"))!=NDK_ERR_PARA)
	{	
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if ((ret=NDK_SP_FsDel(FILETEST1))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:测试绝对路径文件名操作
	if ((fp2=NDK_SP_FsOpen(APPFSFILE, "w"))<0)
	{	
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp2);
		GOTOERR2;
	}
	memset(writebuf, 0x88, 100);
	if((writelen=NDK_SP_FsWrite(fp2,writebuf, 100))!=100) // 覆盖写入数据(0x88)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR2;
	}
	NDK_SP_FsClose(fp2);
	if ((fp2=NDK_SP_FsOpen(APPFSFILE, "r"))<0) // 检查是否能打开程序运行过程中创建的文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp2);
		GOTOERR2;
	}

	if ((ret=NDK_SP_FsFileSize(APPFSFILE,&punSize))!=NDK_OK||punSize!=100)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret,punSize);
		GOTOERR2;
	}
	memset(readbuf, 0, sizeof(readbuf));
	if((readlen=NDK_SP_FsRead(fp2,readbuf, 100))!=100)//此处其实也验证了fopen("w")并不是新建文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR2;	
	}
	else
	{
		if (memcmp(writebuf, readbuf, 100)) // 校验写入与读出数据的内容
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			GOTOERR2;
		}		
	}
	NDK_SP_FsClose(fp2);
	if ((ret=NDK_SP_FsDel(APPFSFILE))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}

	//case7:测试连续打开同一个文件,连续打开的文件句柄应该独立有效
	if ((fp=NDK_SP_FsOpen(FILETEST1, "w"))<0)//构造测试用文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((writelen=NDK_SP_FsWrite(fp,"0123456789", 10))!=10) // 写入一定数据"0123456789"
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;	
	}
	NDK_SP_FsClose(fp);
	
	if ((fp=NDK_SP_FsOpen(FILETEST1, "r"))<0)//第一次打开文件,句柄fp
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(readbuf,0,sizeof(readbuf));
	if((readlen=NDK_SP_FsRead(fp,readbuf, 5))!=5)//读取5个字节数据
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;		
	}
	if (memcmp(readbuf, "01234", 5)) //校验写入与读出数据的内容
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	if ((fp2=NDK_SP_FsOpen(FILETEST1, "r"))<0)//第二次打开文件,句柄fp2
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(readbuf,0,sizeof(readbuf));
	if((readlen=NDK_SP_FsRead(fp2,readbuf, 10))!=10)//读取10个字节数据
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;		
	}
	if (memcmp(readbuf, "0123456789", 10)) //校验写入与读出数据的内容
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp2))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	// 测试通过
	send_result("%s测试通过", TESTAPI);
	//return;//测试结束后进行环境清理 20171213 modify
ERR://出错清理
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETEST1);
	NDK_SP_FsDel(NONLICET);
	//return;
	goto ERR2;
ERR2:
	NDK_SP_FsClose(fp2);
	NDK_SP_FsDel(APPFSFILE);
	return;
}

