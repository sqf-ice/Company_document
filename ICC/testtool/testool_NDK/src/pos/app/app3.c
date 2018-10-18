/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: App3.c
* Author 			: dait
* version			: 
* DATE			: 20130228
* directory 		: 
* description		: 测试 NDK_AppRun接口能否运行相应的应用
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AppRun"
#if K21_ENABLE&&!CPU5810X_ENABLE&&!defined ME50NX&&!defined ME68
#define	TESTAppName		"自检"
#define	FILETEST1		"/appfs/自检.NLP" 
//由于低端产品的NLP命名字符不能超过12位,故分成三个
#define	TESTAppName2	"a~!@#%"
#define	FILETEST2		"/appfs/a~!@#%.NLP"
#define	TESTAppName3	"^_=+.,"
#define	FILETEST3		"/appfs/^_=+.,.NLP"
#define	TESTAppName4	"[{]}丁"
#define	FILETEST4		"/appfs/[{]}丁.NLP"
#elif CPU5810X_ENABLE ||defined ME50NX||defined ME68
#define	TESTAppName		"自检"
#define	FILETEST1		"自检.NLP" 
//由于低端产品的NLP命名字符不能超过12位,故分成三个
#define	TESTAppName2	"a~!@#%"
#define	FILETEST2		"a~!@#%.NLP"
#define	TESTAppName3	"^_=+.,"
#define	FILETEST3		"^_=+.,.NLP"
#define	TESTAppName4	"[{]}丁"
#define	FILETEST4		"[{]}丁.NLP"
#elif OVERSEAS_ENABLE
//7.0版本开始不支持root权限应用包，相关子用例不测  20180420 modify
#define	TESTAppName		"typeAunMaster"
#define	FILETEST1		"typeAunMaster.NLD"
#if !OVERSEAS_OS70_ENABLE
#define	TESTAppName2	"typeRunMaster" 
#define	FILETEST2		"typeRunMaster.NLD"
#endif
#elif defined ME50S|| defined ME50NS
#define	TESTAppName		"App_U_NonMaster"
#define	TESTAppName2	"a~!@#%^_=+,.[{]}丁"
#define	FILETEST2		"/appfs/apps/download/a~!@#%^_=+,.[{]}丁.NLD"
#define	FILETEST1		"/appfs/apps/download/App_U_NonMaster.NLD"
#else
#define	TESTAppName		"App_U_NonMaster"
#define	TESTAppName2	"a~!@#%^_=+,.[{]}丁"
#define	FILETEST2		"/appfs/download/a~!@#%^_=+,.[{]}丁.NLD"
#define	FILETEST1		"/appfs/download/App_U_NonMaster.NLD"
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
void app3(void)
{
	/*private & local definition*/
	int ret = 0;
	float appruntime = 0;
#if K21_ENABLE
	char str[32] = {0};
	int fd = 0;
#endif
#if CPU5810X_ENABLE
	int i = 0;
	char *filename[] = {
							"selftest.NLP",
							"font24_h.NLP",
							"font_h.NLP",
							"font32_h.NLP",
							"statusic.NLP",
							"asc.NLP",
							"gb18030.NLP",
							"wifi-bin.NLP",
							"xui.NLP",
							"factory.NLP",
							"kla.NLP",
#if defined SP610   //ME62 并不会把debugapp当做白名单20171010 目前只有610有此功能 20180529 modify
							"debugapp.NLP",//SP610新增
#endif							
						};
	char *appname[] = {
							"selftest",
							"font24_h",
							"font_h",
							"font32_h",
							"statusicon",
							"asc",
							"gb18030",
							"wifi-bin",
							"xui",
							"factory",
							"kla",
#if defined SP610   //目前只有610有此功能 20180529 modify
							"debugapppk",
#endif							
						};
#endif
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保测试文件存在,ESC退出", TESTAPI)==ESC)
		return;//屏幕提示当前测试所在模块与用例

	//case2:传递参数为空的
	if((ret=NDK_AppRun(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:传递不存在的应用名称
#if (defined ME30MH || defined ME30GX ||defined ME32 || defined ME32GX||defined ME68||defined ME50NX||defined ME65)  //ME30MH返回值不一样
	if((ret=NDK_AppRun((const char *)"test_Noneapp"))!=NDK_ERR)
#else
	if((ret=NDK_AppRun((const char *)"test_Noneapp"))!=NDK_ERR_APP_NOT_EXIST)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//第一次执行装载完后run,之后重启不进行装载直接run
	if(cls_show_msg("第一次进此案例请选择装载(装载后选择重启),再次进入不装载不重启.[Enter]装载,[其他]跳过")==ENTER)
	{
#if defined ME68 || defined ME50NX || defined ME32 || defined ME65
		if((fd = NDK_FsOpen(FILETEST1, "w")) <0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
			RETURN;
		}
		if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
#endif
		if(( ret = NDK_AppLoad((const char *)FILETEST1,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#if !OVERSEAS_OS70_ENABLE
		//20140603 added by huangjianb
#if defined ME68 || defined ME50NX || defined ME32 || defined ME65
		if((fd = NDK_FsOpen(FILETEST2, "w")) <0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
			RETURN;
		}
		if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
#endif
		if(( ret = NDK_AppLoad((const char *)FILETEST2,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#endif
#if K21_ENABLE
		//End 2014/6/3
		//jiangym add 20140708  要支持不同的字符,拆分成小于12位字符的NLP分别验证
#if defined ME68 || defined ME50NX || defined ME32 || defined ME65
		if((fd = NDK_FsOpen(FILETEST3, "w")) <0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
			RETURN;
		}
		if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
#endif
		if(( ret = NDK_AppLoad((const char *)FILETEST3,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#if defined ME68 || defined ME50NX || defined ME32 || defined ME65
		if((fd = NDK_FsOpen(FILETEST4, "w")) <0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
			RETURN;
		}
		if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
#endif
		if(( ret = NDK_AppLoad((const char *)FILETEST4,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#endif
	}
	
	//case1:应用名称存在的情况下调用
	lib_StartStopwatch();
	if(( ret = NDK_AppRun((const char *)TESTAppName))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	appruntime = lib_ReadStopwatch();
	//20140603 added by huangjianb
	//case2:特殊字符应用名称装载应成功
#if !OVERSEAS_OS70_ENABLE
	if(( ret = NDK_AppRun((const char *)TESTAppName2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#if K21_ENABLE
	//End 20140603
	//jiangym add 20140708
	if(( ret = NDK_AppRun((const char *)TESTAppName3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(( ret = NDK_AppRun((const char *)TESTAppName4))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#if 0//(defined IM81||defined N900||ANDIROD_ENABLE)//无屏幕 通过串口打印信息来判断 同时能够调用重启函数
	if(cls_show_msg("是否在控制台上连续打印出数字:4 5 6 7,是:按[Enter]继续,否:按[其他]")!=ENTER)
	{//上面四个子应用是分别连续打印出数字4,5,6,7，在此进行判断是否正常运行子应用
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

#if !(defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B)
	if(cls_show_msg("是否重启(重启过请跳过),[Enter]重启,[其他]跳过")==ENTER)
	{
#if (defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME31||defined SP10||defined ME30GX||defined ME32||defined ME32GX||defined ME32THM)//K21_ENABLE	目前只有30 31 SP10需要插着usb重启
		cls_show_msg("确保机器插着usb"); //硬件设计需要插着usb才能重启
#endif
		NDK_SysReboot();
	}
#endif

#if (defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B)//由于重启会和控制台冲突，无法看到后面的信息,可采取手动重启测试
	if(cls_show_msg("是否正常连续蜂鸣,并在控制台上连续打印出数字:4 5 6 7,是:按[Enter]继续,否:按[其他]")!=ENTER)
	{//上面四个子应用是蜂鸣并分别连续打印出数字4,5,6,7，在此进行判断是否正常运行子应用
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if(cls_show_msg("是否首次进入此案例,是[Enter]继续,[其他]跳过")==ENTER)
	{
		cls_show_msg("重启后选择不装载再测试一次,任意键继续");
		return;
	}
#endif

#if CPU5810X_ENABLE
	//新增白名单过滤测试: 装载使用白名单打包的nlp，运行应该失败
	for(i = 0;i<SZ_ARRAY(appname);i++)
	{
		if(( ret = NDK_AppLoad((const char *)filename[i],0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,第%d个装载失败)", __LINE__, TESTAPI, ret, i+1);
			GOTOERR;
		}
		if(( ret = NDK_AppRun((const char *)appname[i]))!=NDK_ERR)
		{
			send_result("line %d:%s测试失败(%d,第%d个运行应失败)", __LINE__, TESTAPI, ret, i+1);
			GOTOERR;
		}
	}
	cls_show_msg("注意:此用例测试完后需要重新下载固件...\nSP610,ME62需重新下载字库、自检、烤机、statusicon.NLP");
#endif

#if !K21_ENABLE	
	if((ret=NDK_AppDel((const char *)TESTAppName)!=NDK_OK))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !OVERSEAS_OS70_ENABLE
	//20140603 added by huangjianb
	if((ret=NDK_AppDel((const char *)TESTAppName2)!=NDK_OK))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#if  0//K21_ENABLE
	//End 20140603
	//jiangym add 20140708
	if((ret=NDK_AppDel((const char *)TESTAppName3)!=NDK_OK))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_AppDel((const char *)TESTAppName4)!=NDK_OK))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#endif	
	
	// 测试通过
#if !K21_ENABLE
	send_result("%s测试通过(运行时间:%f)", TESTAPI, appruntime);
#else
	memset(str,0,sizeof(str));
	ftos(appruntime,str);
	send_result("%s测试通过(运行时间:%s)", TESTAPI, str);
#endif
goto ERR;
ERR:
#if !K21_ENABLE	
	NDK_AppDel(TESTAppName);
#if !OVERSEAS_OS70_ENABLE
	NDK_AppDel(TESTAppName2);
#endif
#if 0//K21_ENABLE
	NDK_AppDel(TESTAppName3);
	NDK_AppDel(TESTAppName4);
#endif
#endif
	return;
}
