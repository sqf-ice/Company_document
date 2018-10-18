/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App9.c
* Author 			: huangjianb
* version			: 
* DATE			: 20140814
* directory 		: 
* description		: 测试 NDK_AppGetPathByAppName接口,获取应用程序安装目录
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AppGetPathByAppName"

#if K21_ENABLE
#define	TESTAPP1	"App_NULL"
#define	TESTAPP2	"自检1"
#define FILETEST1   "/appfs/App_NULL.NLD" 
#define FILETEST2   "/appfs/自检1.NLD" 
#elif OVERSEAS_ENABLE
#define	TESTAPP1	"typeAunMaster"
#define	FILETEST1 	"typeAunMaster.NLD"  //为了测试打包方便改为当前路径 20180423
#elif defined ME50S|| defined ME50NS
#define	TESTAPP1	"App_A_NonMaster"
#define	TESTAPP2	"a~!@#%^_=+,.[{]}丁"
#define FILETEST1   "/appfs/apps/download/App_A_NonMaster.NLD"
#define FILETEST2   "/appfs/apps/download/a~!@#%^_=+,.[{]}丁.NLD"
#else
#define	TESTAPP1	"App_A_NonMaster"
#define	TESTAPP2	"a~!@#%^_=+,.[{]}丁"
#define FILETEST1   "/appfs/download/App_A_NonMaster.NLD"
#define FILETEST2   "/appfs/download/a~!@#%^_=+,.[{]}丁.NLD"
#endif

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : app8
* functional description : 获取应用程序安装目录
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*										 huangjianb 20140814  created
*****************************************************************/
void app9(void)
{
	/*private & local definition*/
	int ret = 0;

#if K21_ENABLE
	char pszAppName1[64]={"App_NULL"}, sharepath1[64]={"/appfs/apps/App_NULL/"},pszAppPath[64]={"0"};
	char pszAppName2[64]={"自检1"}, sharepath2[64]={"/appfs/apps/自检1/"};
	char pszAppName3[64]={"NoExist"};//不需要下载，测试应用不存在
#elif defined SP630PG||defined SP830PG
	char pszAppName1[64]={"typeAunMaster"}, sharepath1[64]={"/app/typeAunMaster/"},pszAppPath[64]={"0"};
	char pszAppName3[64]={"NoExist"};//不需要下载，测试应用不存在
#elif OVERSEAS_ENABLE&&!defined SP630PG&&!defined SP830PG
	char pszAppName1[64]={"typeAunMaster"}, sharepath1[64]={"/appfs/apps/typeAunMaster/"},pszAppPath[64]={"0"};
	char pszAppName3[64]={"NoExist"};//不需要下载，测试应用不存在
#else
	char pszAppName1[64]={"App_A_NonMaster"}, sharepath1[64]={"/appfs/apps/App_A_NonMaster/"},pszAppPath[64]={"0"};
	char pszAppName2[64]={"a~!@#%^_=+,.[{]}丁"}, sharepath2[64]={"/appfs/apps/a~!@#%^_=+,.[{]}丁/"};
	char pszAppName3[64]={"NoExist"};//不需要下载，测试应用不存在
#endif

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); 
	
	if(( ret = NDK_AppLoad((uchar *)FILETEST1,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !OVERSEAS_ENABLE  //海外版本不支持特殊字符的NLD包
	if(( ret = NDK_AppLoad((uchar *)FILETEST2,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif		
	//case1:参数非法(pszAppName为NULL,pszAppPath为NULL)
	if(( ret = NDK_AppGetPathByAppName(NULL,pszAppPath))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(( ret = NDK_AppGetPathByAppName(pszAppName1,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:参数非法(应用不存在)
	if(( ret = NDK_AppGetPathByAppName(pszAppName3,pszAppPath))!=NDK_ERR_APP_FILE_NOT_EXIST)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:参数非法
	if(( ret = NDK_AppGetPathByAppName("",pszAppPath))!=NDK_ERR_APP_FILE_NOT_EXIST)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:参数正确,正常应用名
	memset(pszAppPath, 0, sizeof(pszAppPath));
	if(( ret = NDK_AppGetPathByAppName(pszAppName1,pszAppPath))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(pszAppPath, sharepath1))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, pszAppPath);
		goto ERR;
	}
#if !OVERSEAS_ENABLE  //海外版本不支持特殊字符的NLD包
	//case5:参数正确,应用名为特殊字符组成
	memset(pszAppPath, 0, sizeof(pszAppPath));
	if(( ret = NDK_AppGetPathByAppName(pszAppName2,pszAppPath))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(pszAppPath, sharepath2))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, pszAppPath);
		goto ERR;
	}
#endif 	
	//测试通过
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_AppDel(TESTAPP1);
#if !OVERSEAS_ENABLE 	
	NDK_AppDel(TESTAPP2);
#endif
	return;
}
