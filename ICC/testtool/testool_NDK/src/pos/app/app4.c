/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App4.c
* Author 			: dait
* version			: 
* DATE			: 20130228
* directory 		: 
* description		: 测试 NDK_AppDel接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define		TESTAPI		"NDK_AppDel"
#if K21_ENABLE&&!CPU5810X_ENABLE&&!defined ME50NX&&!defined ME68
#define TESTAppName		"自检5"
#define	FILETEST4		"/appfs/自检5.NLP" 
#elif CPU5810X_ENABLE||defined ME50NX ||defined ME68
#define TESTAppName		"自检1"
#define	FILETEST4		"自检1.NLP" 
#define TESTAppName1 	"k#~a"
#define	FILETEST5		"k#~a.NLP"
#elif OVERSEAS_ENABLE
#define TESTAppName 	"typeAunMaster"
#define FILETEST4		"typeAunMaster.NLD" 
#if OVERSEAS_OS70_ENABLE  //7.0版本开始不支持root权限应用  20180420 modify
#define TESTAppName1 	"typeAunMaster1"
#define FILETEST5		"typeAunMaster1.NLD" 
#else
#define TESTAppName1 	"typeRunMaster"
#define FILETEST5		"typeRunMaster.NLD" 
#endif
#elif defined ME50S|| defined ME50NS
#define TESTAppName 	"App_A_NonMaster"
#define TESTAppName1 	"a~!@#%^_=+,.[{]}丁"
#define FILETEST4		"/appfs/apps/download/App_A_NonMaster.NLD" 
#define	FILETEST5		"/appfs/apps/download/a~!@#%^_=+,.[{]}丁.NLD"
#else
#define TESTAppName 	"App_A_NonMaster"
#define TESTAppName1 	"a~!@#%^_=+,.[{]}丁"
#define FILETEST4		"/appfs/download/App_A_NonMaster.NLD" 
#define	FILETEST5		"/appfs/download/a~!@#%^_=+,.[{]}丁.NLD"
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
void app4(void)
{
	/*private & local definition*/
	int ret = 0;
	ST_APPINFO UsrPrgInfo ;
#if CPU5810X_ENABLE||defined ME50NX||defined ME68
	int nFlag = 1, nPos = 0;
#endif	
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	memset(&UsrPrgInfo,0,sizeof(UsrPrgInfo));
	if(cls_show_msg1(30, "测试%s...请确保%s,%s存在,ESC退出", TESTAPI,FILETEST4,FILETEST5)==ESC)
		return;
	
	//case1:删除带“0xA1”不可显示字符应用,可以装载,并且重启后可以卸载
	if(cls_show_msg("第一次进此案例请选择装载(装载后将重启),再次进入不装载.[Enter]装载,[其他]跳过")==ENTER)
	{
		//20140603 added by huangjianb
		if(( ret = NDK_AppLoad((uchar *)FILETEST5,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#if !(defined ME50||defined ME50N ||defined ME50C ||defined ME50H||defined ME62||defined ME50NX||defined ME68)
		memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
		if((ret = NDK_AppGetInfo((uchar *)TESTAppName1,0,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#endif
		//End 20140603
		NDK_SysReboot();
	}
	//重启后删除应用应该能够成功,并且再获取应用信息失败
	//20140603 added by huangjianb
	if((ret=NDK_AppDel(TESTAppName1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !(defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME62||defined ME50NX||defined ME68)
	memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
	if((ret = NDK_AppGetInfo((uchar *)TESTAppName1,0,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_ERR_APP_NOT_EXIST)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	while (nFlag) 
	{
		memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
		ret = NDK_AppGetInfo(NULL,nPos++,&UsrPrgInfo,sizeof(ST_APPINFO));
		switch(ret)
		{
			case NDK_OK:
				//比较如果有查到此应用就出错,没有就继续直到最后一个
				if(!strcmp(UsrPrgInfo.szAppName,TESTAppName1))
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto ERR;
				}
				else
					continue;
				break;
			case NDK_ERR_APP_NOT_EXIST://当前没有应用信息 成功退出
				nFlag = 0;
				break;
			case NDK_ERR:
				//cls_show_msg("第%d个固件的文件类型为:file文件类型", nPos);
				break;
			default:
				send_result("line %d:%s测试失败(%d,npos=%d)", __LINE__, TESTAPI, ret, nPos);  
				goto ERR;
				break;	
		}
	}
#endif
	//End 20140603
	
	//case2:传递为空参数
	if((ret=NDK_AppDel(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:传递任意的不存在的应用名称,删除失败
	if((ret=NDK_AppDel("testNoneapp"))!=NDK_ERR_APP_NOT_EXIST)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:删除正确的应用,删除成功,删除后再获取信息应该提示应用不存在
	if(( ret = NDK_AppLoad((uchar *)FILETEST4,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_AppDel(TESTAppName))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !(defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME62||defined ME50NX||defined ME68)
	memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
	if((ret = NDK_AppGetInfo((uchar *)TESTAppName,0,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_ERR_APP_NOT_EXIST)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#else
	while (nFlag) 
	{
		memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
		ret = NDK_AppGetInfo(NULL,nPos++,&UsrPrgInfo,sizeof(ST_APPINFO));
		switch(ret)
		{
			case NDK_OK:
				//比较如果有查到此应用就出错,没有就继续直到最后一个
				if(!strcmp(UsrPrgInfo.szAppName,TESTAppName))
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto ERR;
				}
				else
					continue;
				break;
			case NDK_ERR_APP_NOT_EXIST://当前没有应用信息 成功退出
				nFlag = 0;
				break;
			case NDK_ERR:
				//cls_show_msg("第%d个固件的文件类型为:file文件类型", nPos);
				break;
			default:
				send_result("line %d:%s测试失败(%d,npos=%d)", __LINE__, TESTAPI, ret, nPos);  
				goto ERR;
				break;	
		}
	}
#endif
	// 测试通过
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_AppDel(TESTAppName);
	NDK_AppDel(TESTAppName1);
	return;
}
