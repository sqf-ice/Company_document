/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			:    spfs模块
* file name			:  
* Author 			: linying
* version			: 
* DATE				: 20180829
* directory 		: 
* description		: 本例测试NDK_CopyFileToSecMod将pad端的指令文件拷贝到k21端/appfs/目录下
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_CopyFileToSecMod"	 
#define DESTFILE "/appfs/test"
#define DESTFILE1 "/appfs/test111111111"//名字大于12个字节
#define DESTFILE2 "/data/test"//不在APPS下的
#define DESTFILE3 "/appfs/a"//有同名文件
#define SOURCEFILE  "/mnt/sdcard/sourcefile"//"/data/sourcefile"  //需本地创建 //原来的路径无权限故修改 20171101
#define SOURCEFILE1 "/appfs/sourcefile" //不在pad端下的源文件，需本地创建
#define SOURCEFILE4000   "/mnt/sdcard/sourcefile2"//"/data/sourcefile2"  //本文件内容是4000字节的A


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
*			  	  	      linying        20180829       created
*****************************************************************/
void spfs15(void)//无参数无返回
{
	/*private & local definition*/
	int ret=0, fp=0, i=0, writelen=0, readlen=0, size=0, size1=0;
	char writebuf[4000]={0}, readbuf[4000]={0};//传建的文件大小由原来的1024增加到4000
	char str[]="新12345`~!\"@$#[^%]*() 大-_=+|\\&{}:;?/><.,陆", str2[4000];
	
	/*process body*/
	if(auto_flag==1)
	{
		//send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);  
	NDK_SP_FsDel(DESTFILE);
	NDK_SP_FsDel(DESTFILE3);
	NDK_SP_FsDel(SOURCEFILE1);

	//在pad端创建源文件
	cls_show_msg("请在pad端/mnt/sdcard/路径下创建一个名为sourcefile,内容为%s的源文件，创建完后按任意键继续...", str);
	cls_show_msg("请在pad端/mnt/sdcard/路径下创建一个名为sourcefile2,内容4000个A的源文件，创建完后按任意键继续...");
	
	//case1:异常测试：目标文件名长度超过12个字节，应返回失败
	if((ret=NDK_CopyFileToSecMod(SOURCEFILE, DESTFILE1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:异常测试：目标文件不是在/appfs/路径下，则拷贝应该失败。
	if((ret=NDK_CopyFileToSecMod(SOURCEFILE, DESTFILE2))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:异常测试：源文件不是在pad端中，则拷贝应该失败。
	//创建非pad端创建源文件
	if((fp=NDK_SP_FsOpen(SOURCEFILE1, "w"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		RETURN;
	}
	NDK_SP_FsClose(fp);
	if((ret=NDK_CopyFileToSecMod(SOURCEFILE1, DESTFILE))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_SP_FsDel(SOURCEFILE1);
		RETURN;
	}
	NDK_SP_FsDel(SOURCEFILE1);
	
	//case4:正常测试：pad中的指令文件可以正常拷贝到k21端/appfs/目录下（源文件仍应该存在）
	if((ret=NDK_CopyFileToSecMod(SOURCEFILE, DESTFILE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//pad端下的源文件应存在
	if(cls_show_msg("请查看pad端底下的源文件%s是否还存在?存在[ENTER],不存在[其它]。", SOURCEFILE)!=ENTER)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI);
		RETURN;
	}
	//判断两个文件内容是否一致
	if((ret=NDK_SP_FsFileSize(DESTFILE, &size))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, size);
		RETURN;
	}
	if((fp=NDK_SP_FsOpen(DESTFILE, "r"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		RETURN;
	}
	if((readlen=NDK_SP_FsRead(fp, readbuf, size))!=size)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, readlen);
		NDK_SP_FsClose(fp);
		NDK_SP_FsDel(DESTFILE);
		RETURN;
	}
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(DESTFILE);
	if(memcmp(str, readbuf, strlen(str)))
	{	
		send_result("line %d:%s测试失败(str=%s, readbuf=%s)", __LINE__, TESTAPI, str, readbuf);
		RETURN;
	}

  	//case5:正常测试：在/appfs/中存放一个"a"文件，将pad中的指令文件拷贝到k21端/appfs/目录下时，目标文件也为"a"文件，则原先文件将被覆盖
	if((fp=NDK_SP_FsOpen(DESTFILE3, "w"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		RETURN;
	}
	for (i=0; i<sizeof(writebuf); i++)
		writebuf[i]=rand()%256;
	if((writelen=NDK_SP_FsWrite(fp, writebuf, 1024))!=1024)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		NDK_SP_FsClose(fp);
		NDK_SP_FsDel(DESTFILE3);
		RETURN;
	}
	NDK_SP_FsClose(fp);
	if((ret=NDK_CopyFileToSecMod(SOURCEFILE, DESTFILE3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_SP_FsDel(DESTFILE3);
		RETURN;
	}
	if((ret=NDK_SP_FsFileSize(DESTFILE3, &size1))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, size1);
		NDK_SP_FsDel(DESTFILE3);
		RETURN;
	}
	NDK_SP_FsDel(DESTFILE3);
 	if(size1!=size)
 	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, size, size1);
		RETURN;
 	}

	//case6:正常测试：pad中的指令文件可以正常拷贝到k21端/appfs/目录下（源文件仍应该存在）,大文件超过4000字节
	if((ret=NDK_CopyFileToSecMod(SOURCEFILE4000, DESTFILE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//判断两个文件内容是否一致
	if((ret=NDK_SP_FsFileSize(DESTFILE, &size))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, size);
		RETURN;
	}
	if((fp=NDK_SP_FsOpen(DESTFILE, "r"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		RETURN;
	}
	if((readlen=NDK_SP_FsRead(fp, readbuf, size))!=sizeof(str2))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, readlen);
		NDK_SP_FsClose(fp);
		NDK_SP_FsDel(DESTFILE);
		RETURN;
	}
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(DESTFILE);
	memset(str2,'A',sizeof(str2));
	if(memcmp(str2, readbuf, sizeof(str2)))
	{	
		send_result("line %d:%s测试失败,请比较两个文件", __LINE__, TESTAPI);
		RETURN;
	}

	//测试后置	
	send_result("%s测试通过", TESTAPI);
	return;
}


