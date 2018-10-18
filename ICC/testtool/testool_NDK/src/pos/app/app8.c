/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App8.c
* Author 			: huangjianb
* version			: 
* DATE			: 20140814
* directory 		: 
* description		: 测试 NDK_AppGetPathById接口,获取应用管理相关目录
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AppGetPathById"

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : app8
* functional description : 获取应用管理相关目录
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*										 huangjianb 20140814  created
*****************************************************************/
void app8(void)
{
	/*private & local definition*/
	int ret = 0;	
#if !(defined SP630PG||defined SP830PG)//SP630PG文件目录有修改linying 20180322 modified
	char emPathId1[64]={0}, pszPath1[64]={"/appfs/apps/share/"};
	char emPathId2[64]={0}, pszPath2[64]={"/appfs/apps/share/"};
#else 
    char emPathId1[64]={0}, pszPath1[64]={"/app/share/"};
	char emPathId2[64]={0}, pszPath2[64]={"/app/share/"};
#endif
	char emPathId3[64]={0}, pszPath3[64]={"/tmp/"};
	
	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); 
	
	//case1:参数非法 emPathId不在枚举范围内
	if(( ret = NDK_AppGetPathById(APPPATH_LIARARY-1,emPathId1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret = NDK_AppGetPathById(APPPATH_TMP+1,emPathId1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:参数非法 pszPath为NULL
	if(( ret = NDK_AppGetPathById(APPPATH_LIARARY,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	
	//case4：正确获取应用库存放目录应成功
	if(( ret = NDK_AppGetPathById(APPPATH_LIARARY,emPathId1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp(emPathId1, pszPath1))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, emPathId1);
		RETURN;
	}
	
	//case5:正确获取应用共享库目录应成功
	if(( ret = NDK_AppGetPathById(APPPATH_SHARA_LIBRARY,emPathId2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp(emPathId2, pszPath2))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, emPathId2);
		RETURN;
	}
	
	//case6:正确获取应用临时目录应成功
	if(( ret = NDK_AppGetPathById(APPPATH_TMP,emPathId3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp(emPathId3, pszPath3))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, emPathId3);
		RETURN;
	}
	
	//测试通过
	send_result("%s测试通过", TESTAPI);
	return;
}
