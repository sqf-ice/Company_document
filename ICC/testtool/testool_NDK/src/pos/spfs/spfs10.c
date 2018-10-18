/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name			: spfs10.c
* Author 			: wangk
* version			: 
* DATE				: 20180829
* directory 		: 
* description		: 测试NDK_SP_FsFormat接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SP_FsFormat" 
#define 	FILETEST111 	"/appfs/FT111"
#define 	FILETEST112		"/appfs/FT112"



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
void spfs10(void)
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
	
	if(cls_show_msg("首次进入此案例,按[Enter]继续,再次进入时按[其他]")==ENTER)
	{
		//测试前置
		NDK_SP_FsDel(FILETEST111);
		NDK_SP_FsDel(FILETEST112);

		//case1:正常调用format进行电子盘格式化,格式化后测试文件应该丢失
		//step1:创建两个文件，并写入一定数据
		cls_printf("正在创建测试文件...");
		if((fp=NDK_SP_FsOpen(FILETEST111, "w"))<0)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
			GOTOERR2;
		}
		memset(writebuf, 0x05, sizeof(writebuf));
		if((writelen=NDK_SP_FsWrite(fp, writebuf, 200))!=200)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
			GOTOERR2;
		}
		if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR2;
		}
		
		if((fp1=NDK_SP_FsOpen(FILETEST112, "w"))<0)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp1);
			GOTOERR;
		}
		memset(writebuf, 0x06, sizeof(writebuf));
		if((writelen=NDK_SP_FsWrite(fp1, writebuf, 200))!=200)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
			GOTOERR;
		}
		if((ret=NDK_SP_FsClose(fp1))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//step2:调用NDK_SP_FsFormat函数进行格式化
		cls_show_msg("格式化后设备会自动重启,需要再进入此案例验证,任意键继续");
		cls_printf("正在格式化电子盘...");
		if((ret=NDK_SP_FsFormat())!=NDK_OK)
		{
			send_result("line %d:格式化电子盘失败(ret=%d)", __LINE__, ret);
			GOTOERR;
		}
	}

	//step3:格式化后测试文件应该不存在
	cls_printf("\n格式化电子盘后验证文件...");
	if(NDK_SP_FsExist(FILETEST111)==NDK_OK||NDK_SP_FsExist(FILETEST112)==NDK_OK)
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}

	// 测试通过
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_SP_FsClose(fp1);
	NDK_SP_FsDel(FILETEST112);
	goto ERR2;
ERR2:
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETEST111);
	return;
}