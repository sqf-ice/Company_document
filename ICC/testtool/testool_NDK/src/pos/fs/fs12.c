/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs12.c
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: 测试NDK_FsDir接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if OVERSEAS_ENABLE || defined SP610
#define  FILETEST12 "DIRFILE"
#else
#define  FILETEST12 "/appfs/DIRFILE"
#endif
#define 	TESTAPI		"NDK_FsDir"
#define	MAXWAITTIME	(30)

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的12号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*
*****************************************************************/
void fs12(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int ret= -1, fp=0, i=0, nKeyin=0;
	char s[3000]={0}, discfilesdir[100][21];
	uint dirnum=0;

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	NDK_FsDel(FILETEST12);
	if((fp=NDK_FsOpen(FILETEST12, "w"))<0)
	{
		send_result("line %d:创建文件失败", __LINE__);
		GOTOERR;
	}
	NDK_FsClose(fp);

	//case1:异常测试:参数为NULL情况
	if((ret=NDK_FsDir(NULL, s, &dirnum))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsDir("./", NULL, &dirnum))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsDir("./", s, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//增加一个不存在的路径,应返回非法路径 20140923 jiangym add
	if((ret=NDK_FsDir("./non", s, &dirnum))!=NDK_ERR_PATH)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:正常获取当前目录下所有文件，并逐个显示
	//每20个字节存储一个文件名 前19 为文件名，超过自动截短。第20字节如果为1则表示该文件为文件夹，0为普通文件 要在这里验证
	memset(s, 0, sizeof(s));
	memset(discfilesdir, 0, sizeof(discfilesdir));
#if !(K21_ENABLE||OVERSEAS_ENABLE)
	if((ret=NDK_FsDir("/appfs", s, &dirnum))!=NDK_OK)
#else
	if((ret=NDK_FsDir("./", s, &dirnum))!=NDK_OK)
#endif
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for(i=0;i<dirnum;i++)
	{
		memcpy(discfilesdir[i], s+i*20, 20); // 存储当前文件目录
		cls_show_msg1(5, "dir结果显示:\n%s %-s\n", s+i*20, s[i*20+19]==1?"文件夹":(s[i*20+19]==0?"普通文件":"测试失败:获取到错误的文件类型"));
	}

	//case3:验证创建的文件存在
	for(i=0;i<dirnum;i++)
	{
#if !K21_ENABLE
		if(!memcmp(&discfilesdir[i], "DIRFILE", 7))
#elif defined SP610
		if(!memcmp(&discfilesdir[i], "/appfs/DIRFILE", 14))
#else
		if(!memcmp(&discfilesdir[i], FILETEST12, sizeof(FILETEST12)))
#endif
			break;
	}
	if(i==dirnum)
	{
		send_result("line %d:%s测试失败,测试文件不存在", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4:验证文件类型的正确性。
	if(discfilesdir[i][19]!=0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, discfilesdir[i][19]);
		goto ERR;
	}
	//case5:验证文件总数的正确性
	nKeyin=cls_show_msg1(MAXWAITTIME, "共获取到%d个文件和文件夹\n[ENTER]通过,[其它]不通过", dirnum);
	if(nKeyin==0||nKeyin==ENTER)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);

	//测试后置:删除文件
	NDK_FsDel(FILETEST12);
	return;
ERR:
	NDK_FsDel(FILETEST12);
	return;
}

