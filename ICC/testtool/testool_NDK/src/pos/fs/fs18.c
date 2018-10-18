/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name			: fs18.c
* Author 			: zhengry
* version			: 
* DATE				: 20150723
* directory 		: 
* description		: 本例测试NDK_FsClearAppFile文件系统格式化(仅K21支持)
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if defined SP610
#define  FILETEST18_S "fs18_s"
#define  FILETEST18_B "fs18_b"  
#define  FILE_TEST1 "ak_main.data" 
#define  FILE_TEST2 "ak_main.idx" 
#else
#define  FILETEST18_S "/appfs/fs18_s"  //相对路径
#define  FILETEST18_B "/appfs/fs18_b"  //相对路径
#define  FILE_TEST1 "/appfs/ak_main.data"  //相对路径
#define  FILE_TEST2 "/appfs/ak_main.idx"  //相对路径
#endif
#define TESTAPI	"NDK_FsClearAppFile"
#define MAX_SIZE 7168
#define MIN_SIZE 1024
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	  zhengry       20150723       created
*****************************************************************/
void fs18(void)
{
	/*private & local definition*/
	int ret=0, fp1=0, fp2=0, writelen=0, i=0;
	ulong size1=0, size2=0;
	char writebuf1[MAX_SIZE] = {0}, writebuf2[MIN_SIZE] = {0};
	
	/*process body*/
	if(auto_flag==1)//NDK_FsClearAppFile函数会删除paraconf文件，故不自动测试
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//测试前置:如果两个特殊文件不存在则创建文件
	for (i=0; i<sizeof(writebuf1); i++)
		writebuf1[i]=rand()%256;
	if(NDK_FsExist(FILE_TEST1)!=NDK_OK)
	{
		if ((fp1=NDK_FsOpen(FILE_TEST1, "w"))<0)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp1);
			return;
		}
		if((writelen=NDK_FsWrite(fp1,writebuf1, MAX_SIZE))!=sizeof(writebuf1))
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
			NDK_FsClose(fp1);
			return;
		}
		NDK_FsClose(fp1);
	}
	if(NDK_FsExist(FILE_TEST2)!=NDK_OK)
	{
		if ((fp1=NDK_FsOpen(FILE_TEST2, "w"))<0)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp1);
			return;
		}
		if((writelen=NDK_FsWrite(fp1,writebuf1, MAX_SIZE))!=sizeof(writebuf1))
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
			NDK_FsClose(fp1);
			return;
		}
		NDK_FsClose(fp1);
	}

	//测试前置 
	//case1:第一次格式化后，获取磁盘空间大小，创建大文件和小文件，ak_main.data，ak_main.idx文件应该存在。 
	if((ret=NDK_FsClearAppFile())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_FsGetDiskSpace(1, &size1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	//创建大文件
	if ((fp1=NDK_FsOpen(FILETEST18_B, "w"))<0) 
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp1);
		return;
	}
	for (i=0; i<sizeof(writebuf1); i++)
		writebuf1[i]=rand()%256;
	if((writelen=NDK_FsWrite(fp1,writebuf1, MAX_SIZE))!=sizeof(writebuf1))
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		goto ERR2;
	}
	NDK_FsClose(fp1);
	//创建小文件
	if ((fp2=NDK_FsOpen(FILETEST18_S, "w"))<0) 
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp2);
		return;
	}
	for (i=0; i<sizeof(writebuf2); i++)
		writebuf2[i]=rand()%256;
	if((writelen=NDK_FsWrite(fp2,writebuf2, MIN_SIZE))!=sizeof(writebuf2))
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		goto ERR;
	}
	NDK_FsClose(fp2);

	if((ret=NDK_FsExist(FILE_TEST1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if((ret=NDK_FsExist(FILE_TEST2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//case2:第二次格式化后，ak_main.data，ak_main.idx文件应该存在，此时获取到磁盘空间大小应与第一次的一致
	if((ret=NDK_FsClearAppFile( ))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if((ret=NDK_FsGetDiskSpace(1, &size2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if((ret=NDK_FsExist(FILETEST18_B))!=NDK_ERR )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if((ret=NDK_FsExist(FILETEST18_S))!=NDK_ERR )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if((ret=NDK_FsExist(FILE_TEST1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	if((ret=NDK_FsExist(FILE_TEST2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	if(size1!=size2)
	{	
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, size1, size2);
		goto ERR;
	}	

	//测试通过
	send_result("%s测试通过", TESTAPI);
	//因为ME32没有外部fresh空间，如果不删除这两份文件，会影响之后的测试20171212 linying add
#if defined ME32
    NDK_FsDel(FILE_TEST1);
    NDK_FsDel(FILE_TEST2);
#endif
ERR:
	NDK_FsClose(fp2);
	NDK_FsDel(FILETEST18_S);
ERR2:
	NDK_FsClose(fp1);
	NDK_FsDel(FILETEST18_B);
	return;
}

