/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App5.c
* Author 			: dait
* version			: 
* DATE			: 20130228
* directory 		: 
* description		: 测试 NDK_AppEXECV接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AppEXECV"  
#if OVERSEAS_ENABLE
#define TESTApp 	"typeAunMaster"
#if defined SP630PG||defined SP830PG
#define TESTAppName "typeAunMaster/main"
#else
#define TESTAppName "typeAunMaster/sp33_20160622_user_test"
#endif
#define	FILETEST1 	"typeAunMaster.NLD"  //为了测试打包方便直接改为放在当前路径下 20180423
#elif defined ME50S|| defined ME50NS
#define TESTApp 	"App_U_NonMaster"
#define TESTAppName "App_U_NonMaster/main"
#define	FILETEST1 	"/appfs/apps/download/App_U_NonMaster.NLD"
#else
#define TESTApp 	"App_U_NonMaster"
#define TESTAppName "App_U_NonMaster/main"
#define	FILETEST1 	"/appfs/download/App_U_NonMaster.NLD" 
#endif

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的2号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*
*****************************************************************/
void app5(void)
{
	/*private & local definition*/
	int ret = 0;
	char *exec_argv [4];
	exec_argv[0]="telnet";
	exec_argv[1]="ip";
	exec_argv[2]="port";
	exec_argv[3]=NULL;
	
	char *exec_argv2 [4];
	exec_argv[0]="telnet";
	exec_argv[1]="ip";
	exec_argv[2]="port";
	
	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保%s存在,ESC退出", TESTAPI,FILETEST1)==ESC)
		return;
	if(( ret = NDK_AppLoad((uchar *)FILETEST1,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
		
	//case1:应用名为NULL应该返回参数错误
	if((ret=NDK_AppEXECV (NULL,exec_argv,1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if 1
	//case2:第3个参数为文档规定以外的值，应该返回失败
	if((ret=NDK_AppEXECV ((uchar *)TESTAppName,exec_argv,2))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case3:应用运行参数为NULL，应该返回成功
	if((ret=NDK_AppEXECV ((uchar *)TESTAppName,NULL,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:正确执行可执行程序
	if((ret=NDK_AppEXECV ((uchar *)TESTAppName,exec_argv2,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:正确执行可执行程序
	if((ret=NDK_AppEXECV ((uchar *)TESTAppName,exec_argv,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if 1
	//case5:传递任意的应用名称 应该返回NDK_ERR_APP_NOT_EXIST  20140924 jiangym modify
	if((ret=NDK_AppEXECV ((uchar *)"testNoneApp",exec_argv,0))!=NDK_ERR_APP_NOT_EXIST/*NDK_OK*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif

	// 测试通过
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_AppDel(TESTApp);
	return;
}
