/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs11.c
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: 测试NDK_FsFormat接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_FsFormat" 
#if !K21_ENABLE || OVERSEAS_ENABLE || defined SP610
#define 	FILETEST111 	"FT111"
#define 	FILETEST112		"FT112"
#else
#define 	FILETEST111 	"/appfs/FT111"
#define 	FILETEST112		"/appfs/FT112"
#endif

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
void fs11(void)
{
	/*private & local definition*/
	int fp=0, fp1=0, writelen=0, ret=0;
	char writebuf[200]={0};

	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例
	
#if K21_ENABLE//低端平台格式化后会要求重启
	if(cls_show_msg("首次进入此案例,按[Enter]继续,再次进入时按[其他]")==ENTER)
#endif
	{	//测试前置
		NDK_FsDel(FILETEST111);
		NDK_FsDel(FILETEST112);

		//case1:正常调用format进行电子盘格式化,格式化后测试文件应该丢失
		//step1:创建两个文件，并写入一定数据
		cls_printf("正在创建测试文件...");
		if((fp=NDK_FsOpen(FILETEST111, "w"))<0)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
			GOTOERR2;
		}
		memset(writebuf, 0x05, sizeof(writebuf));
		if((writelen=NDK_FsWrite(fp, writebuf, 200))!=200)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
			GOTOERR2;
		}
		if((ret=NDK_FsClose(fp))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR2;
		}
		
		if((fp1=NDK_FsOpen(FILETEST112, "w"))<0)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp1);
			GOTOERR;
		}
		memset(writebuf, 0x06, sizeof(writebuf));
		if((writelen=NDK_FsWrite(fp1, writebuf, 200))!=200)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
			GOTOERR;
		}
		if((ret=NDK_FsClose(fp1))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//step2:调用NDK_FsFormat函数进行格式化
#if K21_ENABLE//低端平台格式化后会要求重启
		cls_show_msg("格式化后设备会自动重启,需要再进入此案例验证,任意键继续");
#endif
		cls_printf("正在格式化电子盘...");
		if((ret=NDK_FsFormat())!=NDK_OK)
		{
			send_result("line %d:格式化电子盘失败(ret=%d)", __LINE__, ret);
			GOTOERR;
		}
	}
	
	//step3:格式化后测试文件应该不存在
	cls_printf("\n格式化电子盘后验证文件...");
	if(NDK_FsExist(FILETEST111)==NDK_OK||NDK_FsExist(FILETEST112)==NDK_OK)
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}

	// 测试通过
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_FsClose(fp1);
	NDK_FsDel(FILETEST112);
	goto ERR2;
ERR2:
	NDK_FsClose(fp);
	NDK_FsDel(FILETEST111);
	return;
}

