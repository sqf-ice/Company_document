/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs15.c
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: 测试NDK_Directory函数族
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_FsCreateDirectory,NDK_FsRemoveDirectory"

#define TESTDIR     "./test1"
#define TESTSUBDIR  "./test1/test2"

/****************************************************************
* function name 	 	: void fs15(void)
* functional description 	: 测试NDK_Directory函数族
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  chenfm     20121217	created
*****************************************************************/
void fs15(void)
{
	/*private & local definition*/
	int ret= -1;

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); 

	//测试前置：
	NDK_FsRemoveDirectory(TESTSUBDIR);	
	NDK_FsRemoveDirectory(TESTDIR);

	//case1 :非法参数
	if((ret=NDK_FsCreateDirectory(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsRemoveDirectory(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	 
	//case2:创建父目录不存在的子目录应该失败
	if((ret=NDK_FsCreateDirectory(TESTSUBDIR))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:删除.、..等特殊目录,应不成功
	if((ret=NDK_FsRemoveDirectory("."))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_FsRemoveDirectory(".."))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:创建当前目录，应不成功;        
	if((ret=NDK_FsCreateDirectory("./"))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:创建不存在的子目录应成功
	if((ret=NDK_FsCreateDirectory(TESTDIR))!=NDK_OK) //应成功
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:创建已存在的目录，应不成功; 
	if((ret=NDK_FsCreateDirectory(TESTDIR))!=NDK_ERR) //应失败
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:创建父目录已存在的子目录应该成功
	if((ret=NDK_FsCreateDirectory(TESTSUBDIR))!=NDK_OK) //应成功
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	
	//case8:正常删除一个二级子目录，应成功返回。
	if((ret=NDK_FsRemoveDirectory(TESTSUBDIR))!=NDK_OK) //应成功
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}   

	//case9:正常删除一个目录，应失败返回
	if((ret=NDK_FsRemoveDirectory(TESTDIR))!=NDK_OK) //应成功
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}   
	     
	//case10:删除不存在的目录应失败
	if((ret=NDK_FsRemoveDirectory(TESTDIR))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//test over
	send_result("%s测试通过", TESTAPI);
	return;	
ERR:
	//出错处理及测试后置：  
	NDK_FsRemoveDirectory(TESTSUBDIR);    
	NDK_FsRemoveDirectory(TESTDIR);	
	return;
}

