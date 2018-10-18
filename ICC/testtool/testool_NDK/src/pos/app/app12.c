/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: app模块
* file name		: App12.c
* Author 		:
* version		: 
* DATE			:
* directory 		: 
* description	: 测试NDK_AppStartChildProcess能否正常运行后台服务程序 并读取返回值
				以及与NDK_AppGetDatafromParentProcess,NDK_AppChildProcessExit之间能否正常通讯
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_AppStartChildProcess,NDK_AppGetDatafromParentProcess,NDK_AppChildProcessExit"
#define   SUBAPP1          "subapp1"
#define   SUBAPP2         "subapp2"
#define   ERRPATH         "err"
#define   APPNAME       "SUBAPP"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 模块的12号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*						  jiangym			20170616     created
*****************************************************************/
void app12(void)
{
	/*private & local definition*/
	int  ret = -1, len = 0, data = 0;
	AppExitStatusT  *appstatus;
	char text[] = "这是子进程2";
		
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}

	if(cls_show_msg1(30, "测试%s...请确保测试文件%s,%s存在,%s.NLD已下载,ESC退出", TESTAPI,SUBAPP1,SUBAPP2,APPNAME)==ESC)
		return;//屏幕提示当前测试所在模块与用例
		
	//case1:参数异常测试
	if(( ret =NDK_AppStartChildProcess(NULL,NULL,0,&appstatus))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret =NDK_AppStartChildProcess(SUBAPP1,NULL,0,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret =NDK_AppGetDatafromParentProcess(NULL,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret =NDK_AppGetDatafromParentProcess(&data,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret =NDK_AppChildProcessExit(0,NULL,len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case2:子应用不存在时应该返回失败 开发已确认此测试点测试不了

	//case3:正常测试:启动的子应用无参数 子应用返回的参数中第三个参数为0
	cls_show_msg("请确保当前目录下的子应用%s和%s已修改为可执行文件",SUBAPP1,SUBAPP2);
	if(( ret =NDK_AppStartChildProcess(SUBAPP1,NULL,0,&appstatus))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//判断子应用获取到父进程的参数是否正确
	if(cls_show_msg("已回到父进程,子进程是否有显示:收到父进程传来的数据为0,正确按[Enter],错误按[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//父进程判断子应用返回的参数:成功退出返回空参数以及数据为0
	if(appstatus->code != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, appstatus->code);
		RETURN;
	}
	if(appstatus->cbData != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, appstatus->cbData);
		RETURN;
	}

	//case4:启动的子应用有带参数, 父进程传参数给子应用
	data = 10;
	if(( ret =NDK_AppStartChildProcess(SUBAPP2,&data,sizeof(int),&appstatus))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//判断子应用获取到父进程的参数是否正确
	if(cls_show_msg("已回到父进程,子进程是否有显示:收到父进程传来的数据为%d,正确按[Enter],错误按[其他]",data)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//父进程判断子应用返回的参数:成功退出并返回有效的参数
	if(appstatus->code != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, appstatus->code);
		RETURN;
	}
	if(strcmp(appstatus->data,text))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, appstatus->data);
		RETURN;
	}
	if(appstatus->cbData != 12)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, appstatus->cbData);
		RETURN;
	}

	//case5:测试启动的子应用路径传的是应用名称文件夹,父进程有传参数给子应用,子应用返回空的参数
	data = 20;
	if(( ret =NDK_AppStartChildProcess(APPNAME,&data,sizeof(int),&appstatus))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//判断获取到的子应用的参数是否正确
	if(cls_show_msg("已回到父进程,子进程是否有显示:收到父进程传来的数据为%d,正确按[Enter],错误按[其他]",data)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//父进程判断子应用返回的参数:成功退出并返回有效的参数
	if(appstatus->code != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, appstatus->code);
		RETURN;
	}
	if(appstatus->cbData != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, appstatus->cbData);
		RETURN;
	}

	//测试通过
	send_result("%s测试通过", TESTAPI);	
	return;
}
