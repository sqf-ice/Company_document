/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App6.c
* Author 			: dait
* version			: 
* DATE			: 20130228
* directory 		: 
* description		: 测试 NDK_AppDoEvent接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AppDoEvent"
#define	APP_NAME	"app_event"
#if defined ME50S|| defined ME50NS
#define	FILETEST	"/appfs/apps/download/app_event.NLD" 
#elif OVERSEAS_ENABLE
#define	FILETEST	"app_event.NLD" //为了测试方便打包把海外产品改为放在当前路径下  20180420 modify
#else
#define	FILETEST	"/appfs/download/app_event.NLD" 
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
void app6(void)
{
	/*private & local definition*/
	int ret = 0, nLen = 0, nOutLen = 0;
	int nModule = 1;
	char szEventMsg[256]="Hello world";
	char szOut[256]={0};
	char strresult[256] = "返回输出";
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保%s存在,ESC退出", TESTAPI,FILETEST)==ESC)
		return;
	if(( ret = NDK_AppLoad((uchar *)FILETEST,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1:应用名为NULL应该返回参数错误
	nLen = strlen(szEventMsg);
	if((ret=NDK_AppDoEvent (NULL,nModule,szEventMsg,nLen,szOut,256,&nOutLen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2:输入事件为NULL应该返回参数错误
	if((ret=NDK_AppDoEvent ((uchar *)APP_NAME,nModule,NULL,nLen,szOut,256,&nOutLen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case3:输出事件长度NULL应该返回参数错误
	if((ret=NDK_AppDoEvent ((uchar *)APP_NAME,nModule,szEventMsg,nLen,szOut,256,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case4:传递不存在的应用应返回失败
	if((ret=NDK_AppDoEvent ((uchar *)"App_NULL",nModule,szEventMsg,nLen,szOut,256,&nOutLen))!=NDK_ERR_APP_NOT_EXIST)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case5:传递正确参数应返回成功
	if((ret=NDK_AppDoEvent ((uchar *)APP_NAME,nModule,szEventMsg,nLen,szOut,256,&nOutLen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(strcmp(strresult,szOut))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, szOut);
		goto ERR;
	}

	//测试后置,删除相关程序
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_AppDel((char *)APP_NAME);
	return;
}
