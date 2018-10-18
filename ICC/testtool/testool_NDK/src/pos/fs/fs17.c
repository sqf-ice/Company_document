/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs17.c
* Author 			: chensj
* version			: 
* DATE			: 20140218
* directory 		: 
* description		: 测试文件名最大稳定长度
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if defined SP610
#define  FILETEST17_1 "FT17+11111"
#define  FILETEST17_2 "FT17+2222222" //文件名长度为12
#define  FILETEST17_3 "FT17+33333333"  //文件名长度为13
#else
#define  FILETEST17_1 "/appfs/FT17+11111"
#define  FILETEST17_2 "/appfs/FT17+2222222" //文件名长度为12
#define  FILETEST17_3 "/appfs/FT17+33333333"  //文件名长度为13
#endif
#define	TESTAPI		"文件名最大稳定长度"
#define	MAXFD	5

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
*					chensj			20140218	created
*****************************************************************/
void fs17(void)
{
	/*private & local definition*/
	int fp = 0,writelen = 0,readlen = 0;
	char writebuf[101]={0},readbuf[101]={0};

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("%s测试中...", TESTAPI);//屏幕提示当前测试所在模块与用例

	memset(writebuf,'a',sizeof(writebuf)-1);

	//case 1:创建打开文件名长度为10的文件不应失败
	if ((fp=NDK_FsOpen(FILETEST17_1, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((writelen=NDK_FsWrite(fp,writebuf, 100))!=100)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	NDK_FsClose(fp);
	if ((fp=NDK_FsOpen(FILETEST17_1, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(readbuf,0,sizeof(readbuf));
	NDK_FsSeek(fp, 0, SEEK_SET);
	if((readlen=NDK_FsRead(fp,readbuf, 100))!=100)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;		
	}
	if(strcmp(writebuf,readbuf))
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, strlen(writebuf), strlen(readbuf));
		GOTOERR;	
	}
	NDK_FsClose(fp);

	//case 2:创建打开文件名长度为12的文件不应失败
	if ((fp=NDK_FsOpen(FILETEST17_2, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((writelen=NDK_FsWrite(fp,writebuf, 100))!=100)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	NDK_FsClose(fp);
	if ((fp=NDK_FsOpen(FILETEST17_2, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(readbuf,0,sizeof(readbuf));
	NDK_FsSeek(fp, 0, SEEK_SET);
	if((readlen=NDK_FsRead(fp,readbuf, 100))!=100)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;		
	}
	if(strcmp(writebuf,readbuf))
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, strlen(writebuf), strlen(readbuf));
		GOTOERR;	
	}
	NDK_FsClose(fp);

	//case 3:创建打开文件名长度为大于12的文件会失败
	if ((fp=NDK_FsOpen(FILETEST17_3, "w"))>=0) 
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		goto ERR;
	}
	NDK_FsClose(fp);
	
	send_result("%s测试通过", TESTAPI); // 测试通过
ERR:
	NDK_FsClose(fp);
	NDK_FsDel(FILETEST17_1);
	NDK_FsDel(FILETEST17_2);
	NDK_FsDel(FILETEST17_3);
	return;
}

