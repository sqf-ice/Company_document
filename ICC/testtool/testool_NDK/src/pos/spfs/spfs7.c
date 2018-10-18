/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name			: spfs7.c
* Author 			: wangk
* version			: 
* DATE				: 20180829
* directory 		: 
* description		: 测试NDK_SP_FsTell接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SP_FsTell"
#define FILETEST7	"/appfs/FT7"


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
void spfs7(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, ret=0;
	ulong pulRet=0;
	char writebuf[200]={0};

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置，以"w"方式打开一个特定文件，并写入200字节数据0x09
	NDK_SP_FsDel(FILETEST7);
	if((fp=NDK_SP_FsOpen(FILETEST7, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}

	memset(writebuf, 0x09, sizeof(writebuf));
	if((writelen=NDK_SP_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1: 以"r"方式打开一个特定文件,调用NDK_SP_FsTell取得当前指针位置,当前位置应该为0
	if((fp=NDK_SP_FsOpen(FILETEST7, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}

	//case2: 移动文件指针到距SEEK_SET到-1的位置，再调用NDK_SP_FsTell取当前指针位置
	NDK_SP_FsSeek(fp, -1l, SEEK_SET);
	if((ret=NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}

	//case3: 移动文件指针到距SEEK_SET到200的位置，再调用NDK_SP_FsTell取当前指针位置,应在200的位置
	NDK_SP_FsSeek(fp, 200l, SEEK_SET);
	if((ret=NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}

	//case4: 移动文件指针到距SEEK_SET到201的位置，再调用ftell取当前指针位置,应在201的位置
	NDK_SP_FsSeek(fp, 201l, SEEK_SET);
	if((ret=NDK_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}

	//case5:异常测试
	if((ret=NDK_SP_FsTell(fp, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}	

	//case6:对一个已关闭的或不存的文件做NDK_SP_FsTell，应该失败返回。	
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsTell(fp, &pulRet))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}

	//测试后置
	if((ret=NDK_SP_FsDel(FILETEST7))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_SP_FsTell(fp, &pulRet))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		goto ERR;
	}


	send_result("%s测试通过", TESTAPI); // 测试通过
	return;
ERR:
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETEST7);
	return;
}