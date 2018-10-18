/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs14.c
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: 测试NDK_FsFileSize接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if !K21_ENABLE || OVERSEAS_ENABLE || defined SP610
#define  FILETEST14 "FT14"
#else //K21平台只支持绝对路径
#define  FILETEST14 "/appfs/FT14"
#endif
#define 	TESTAPI		"NDK_FsFileSize"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的18号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*
*****************************************************************/
void fs14(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, ret=0;
	uint punSize=0;
#if !K21_ENABLE
	ulong pulRet=0;
#endif
	char writebuf[200]={0}/*, readbuf[200] = {0}*/;

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_FsDel(FILETEST14);

	//case1: 以"w"方式打开一个特定文件，调用filelength取文件长度(应该返回0)
	if((fp=NDK_FsOpen(FILETEST14, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:关闭该文件后再以"w"方式打开，并写入200字节数据0x18,调用NDK_FsFileSize计算文件大小，应能得到相应的大小200
	if((fp=NDK_FsOpen(FILETEST14, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x18, sizeof(writebuf));
	if((writelen=NDK_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}

	//case3:调用fseek将文件指针移到100位置,调用NDK_FsFileSize取该文件的长度(应该返回200)
	if((ret=NDK_FsSeek(fp, 100l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:文件关闭后获取文件大小应该保持不变
	if((ret=NDK_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}

	//case5:以"r"方式打开该文件,调用NDK_FsFileSize取该文件的长度(应该返回200)
	if((fp=NDK_FsOpen(FILETEST14, "r"))==NDK_ERR_OPEN_DEV)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=200)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if !K21_ENABLE  //K21平台不支持NDK_FsTruncate
	//case6:调用NDK_FsTruncate设置该文件的长度为100(应该成功),以"r"方式打开该文件,
	//调用NDK_FsSeek将文件指针移到300位置
	//然后使用NDK_FsTell取得文件长度(应该返回300),调用NDK_FsFileSize取该文件的长度(应也为300)
	if((ret=NDK_FsTruncate(FILETEST14, 100l))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((fp=NDK_FsOpen(FILETEST14, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	NDK_FsSeek(fp, 300l, SEEK_SET);
	if((ret=NDK_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=300)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=100)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:调用NDK_FsTruncate设置该文件的长度为0,以"r"方式打开该文件后调用NDK_FsFileSize取该文件的长度(应该返回0)
	if((ret=NDK_FsTruncate(FILETEST14, 0l))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((fp=NDK_FsOpen(FILETEST14, "r"))<0) // 以"r"方式打开该文件失败
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_FsFileSize(FILETEST14, &punSize))!=NDK_OK||punSize!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punSize);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case7:异常测试
	if((ret=NDK_FsFileSize(NULL, &punSize))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_FsFileSize(FILETEST14, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试后置
	if((ret=NDK_FsDel(FILETEST14))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	// 测试通过
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_FsClose(fp);
	NDK_FsDel(FILETEST14);
	return;
}

