/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App2.c
* Author 			: dait
* version			: 
* DATE			: 20130228
* directory 		: 
* description		: 测试 NDK_AppGetInfo接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define		TESTAPI		"NDK_AppGetInfo"
#if K21_ENABLE&&!CPU5810X_ENABLE&&!defined ME50NX&&!defined ME68
#define	TESTAPP		"test1"
#define	TESTAPP2	"自检1"
#define	TESTAPP3	"k#~a"
#define	TESTNONAPP	"App_GetInfo"
#define	FILETEST	"/appfs/test1.NLP"
#define	FILETEST2	"/appfs/自检1.NLP"
#define	FILETEST3	"/appfs/k#~a.NLP"
#define	MAXPOS     11 
#elif CPU5810X_ENABLE ||defined ME50NX||defined ME68
#define	TESTAPP		"test1"
#define	TESTAPP2	"自检1"
#define	TESTAPP3	"k#~a"
#define	TESTNONAPP	"App_GetInfo"
#define	FILETEST	"test1.NLP"
#define	FILETEST2	"自检1.NLP"
#define	FILETEST3	"k#~a.NLP"
#define	MAXPOS     49 //ME50上限是50个0~49
#elif OVERSEAS_ENABLE
//海外产品7.0版本开始不支持root权限,将含有root权限测试的改为用普通用户权限测试  20180420 modify
#define	TESTAPP	    "typeAunMaster"
#define	TESTNONAPP	"App_GetInfo"
#define	FILETEST 	"typeAunMaster.NLD" 
#define	MAXPOS     64   //???
#if !OVERSEAS_OS70_ENABLE
#define	TESTAPP2	"typeRunMaster"
#define	FILETEST2	"typeRunMaster.NLD" 
#endif
#elif defined ME50S|| defined ME50NS
#define	TESTAPP		"App_U_NonMaster"
#define	TESTAPP2	"App_A_NonMaster"
#define	TESTAPP3	"a~!@#%^_=+,.[{]}丁"
#define	TESTNONAPP	"App_GetInfo"
#define	FILETEST	"/appfs/apps/download/App_U_NonMaster.NLD" 
#define	FILETEST2	"/appfs/apps/download/App_A_NonMaster.NLD" 
#define	FILETEST3	"/appfs/apps/download/a~!@#%^_=+,.[{]}丁.NLD"
#define	MAXPOS     64 
#else
#define	TESTAPP		"App_U_NonMaster"
#define	TESTAPP2	"App_A_NonMaster"
#define	TESTAPP3	"a~!@#%^_=+,.[{]}丁"
#define	TESTNONAPP	"App_GetInfo"
#define	FILETEST	"/appfs/download/App_U_NonMaster.NLD" 
#define	FILETEST2	"/appfs/download/App_A_NonMaster.NLD" 
#define	FILETEST3	"/appfs/download/a~!@#%^_=+,.[{]}丁.NLD"
#define	MAXPOS     64 
#endif

//由于新的防呆机制中，白名单必须经过签名才能成功下载，签名又不能包含appinfo和appver字段，故此处不测，在app1和app11更新应用时测
#if 0//defined SP610//用新xml打包的新应用
#define	TESTAPP4	"n新!%1" 
#define FILETEST4   "n新!%1.NLP" //不能超过12个字节
#define APPINFO		"new_新大陆!@#$%^&*(0"
#define APPVER      "20170412"
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
void app2(void)
{
	/*private & local definition*/
	ST_APPINFO UsrPrgInfo ;
	int ret = 0,  nPos = 0;
	int nFlag = 1;

	memset(&UsrPrgInfo,0,sizeof(UsrPrgInfo));
	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	//appfs/download/temp/App_A_Master.NLD 
	//appfs/download/temp/App_A_NonMaster.NLD 请确保证以上两个NLD包存在
	if(cls_show_msg1(30, "测试%s...请确保测试文件存在,ESC退出", TESTAPI)==ESC)
		return;

	if(( ret = NDK_AppLoad((uchar *)FILETEST,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !OVERSEAS_OS70_ENABLE//海外版本不支持root权限测试  20180420 modify
	if(( ret = NDK_AppLoad((uchar *)FILETEST2,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#if !OVERSEAS_ENABLE//海外版本UPT工具不支持特殊字符打包,因此不测试FILETEST3
	//20140603 added by huangjianb
	if(( ret = NDK_AppLoad((uchar *)FILETEST3,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//End 20140603
#endif
#if 0//defined SP610//用新xml(新增appinfo信息和appver信息)打包新的应用进行测试
	if(( ret = NDK_AppLoad((uchar *)FILETEST4,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	//case1:nPos参数为-1,应该返回参数错误
	if((ret = NDK_AppGetInfo(NULL,-1,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:应用信息接收缓冲区参数为NULL,应该返回参数错误
	if((ret = NDK_AppGetInfo(NULL,0,NULL,sizeof(ST_APPINFO)))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:超过最大应用数目64,应该返回参数错误，各个产品可能有差异
	if((ret = NDK_AppGetInfo(NULL,MAXPOS+1,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !K21_ENABLE || defined SP610
	//case4:获取不存在的应用信息，应该是获取不存在
	 if((ret = NDK_AppGetInfo((uchar *)TESTNONAPP,0,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_ERR_APP_NOT_EXIST)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif	

	//case5:正常测试:将所有POS端的应用信息获取到
	while (nFlag) 
	{
		memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
		ret = NDK_AppGetInfo(NULL,nPos++,&UsrPrgInfo,sizeof(ST_APPINFO));
		switch(ret)
		{
			case NDK_OK://取到应用信息 完成发送
#if !K21_ENABLE
					if(auto_flag==0)
					{
						cls_show_msg("用户程序名称:%s\n版本信息:%s\n编译时间:%s\n主控标志:%d", UsrPrgInfo.szAppName, UsrPrgInfo.szVerBuf, UsrPrgInfo.szBuildTime, UsrPrgInfo.nIsMaster);
						cls_show_msg("测试版本:%d\n应用编号:%d", UsrPrgInfo.sunReverse[0], UsrPrgInfo.nSeriNo);
					}
					else
					{
						send_result("用户程序名称:%s 版本信息:%s 编译时间:%s 主控标志:%d 测试版本标志:%d 应用编号:%d", UsrPrgInfo.szAppName, UsrPrgInfo.szVerBuf, UsrPrgInfo.szBuildTime, UsrPrgInfo.nIsMaster, UsrPrgInfo.sunReverse[0], UsrPrgInfo.nSeriNo);
					}
#if !OVERSEAS_ENABLE
					if(!strcmp((char *)UsrPrgInfo.szAppName,TESTAPP))
					{
						if(UsrPrgInfo.nSeriNo != 1)
						{
							send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, UsrPrgInfo.nSeriNo);
							GOTOERR;
						}
					}
					if(!strcmp((char *)UsrPrgInfo.szAppName,TESTAPP3))
					{
						if(UsrPrgInfo.nSeriNo != 2)
						{
							send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, UsrPrgInfo.nSeriNo);
							GOTOERR;
						}
					}
#else
					if(!strcmp((char *)UsrPrgInfo.szAppName,TESTAPP))
					{
						if(UsrPrgInfo.nSeriNo != 0)//海外版本UPT工具无法设置nSeriNo值,默认为0
						{
							send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, UsrPrgInfo.nSeriNo);
							GOTOERR;
						}
					}
#endif
#else			//K21产品只有szAppName有效,其它参数不判断
				if(auto_flag==0)
					cls_show_msg("第%d个固件的文件类型为:app文件类型,应用名为:%s", nPos, UsrPrgInfo.szAppName);
				else
					send_result("第%d个固件的文件类型为:app文件类型,应用名为:%s", nPos, UsrPrgInfo.szAppName);
#if 0//defined SP610 //SP610产品上对新打包的测试文件TESTAPP4的名字和版本信息进行比较
				if(!strcmp((char *)UsrPrgInfo.szAppName, APPINFO))
				{
					if(strcmp((char *)UsrPrgInfo.szVerBuf, APPVER))
					{
						send_result("line %d:%s测试失败(Name:%s ,VerBuf:%s)", __LINE__, TESTAPI, UsrPrgInfo.szAppName, UsrPrgInfo.szVerBuf);
						GOTOERR;
					}
				}					
#endif			
				break;
#endif
				continue;
				break;
			case NDK_ERR_APP_NOT_EXIST://当前没有应用信息 成功退出
#if defined SP610 || defined ME50H || defined ME50C	//会先获取到固件列表信息,导致应用信息获取不到,故加上此为了能够继续获取应用信息 20180714	
				send_result("第%d个固件的文件类型为:file文件类型,请在设置中查看固件列表判断获取信息的正确性", nPos);
				continue;
#else				
 				nFlag = 0;
				break;
#endif				
#if K21_ENABLE 
			case NDK_ERR://低端平台获取到的文件类型为file时直接返回NDK_ERR
#if !defined SP610
				if(auto_flag==0)
					cls_show_msg("第%d个固件的文件类型为:file文件类型,请在设置中查看固件列表判断获取信息的正确性", nPos);
				else
					send_result("第%d个固件的文件类型为:file文件类型,请在设置中查看固件列表判断获取信息的正确性", nPos);
#else
				send_result("查找结束，请确认查找到的文件数是否和实际一致");
				nFlag = 0;//SP610当所有APP和FILE都被遍历过后返回NDK_ERR查找结束
#endif				
				break;			
#endif
			default:
				send_result("line %d:%s测试失败(%d,npos=%d)", __LINE__, TESTAPI, ret, nPos);  
				goto ERR;
				break;
		}
	}
	
#if !K21_ENABLE || defined SP610//不支持应用名取app信息的功能不测试
	//case6:测试直接通过应用名获取应用信息应该成功
	memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
	 if((ret = NDK_AppGetInfo((uchar *)TESTAPP,0,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	 
#if !OVERSEAS_ENABLE
	//20140603 added by huangjianb
	//case7:特殊字符组成的应用名称获取应用信息应成功
	memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
	 if((ret = NDK_AppGetInfo((uchar *)TESTAPP3,0,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	//End 20140603
#endif
#endif
#if 0//defined SP610
	memset(&UsrPrgInfo, 0, sizeof(ST_APPINFO));
	 if((ret = NDK_AppGetInfo((uchar *)TESTAPP4,0,&UsrPrgInfo,sizeof(ST_APPINFO)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif
	//测试通过
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_AppDel(TESTAPP);
#if !OVERSEAS_OS70_ENABLE
	NDK_AppDel(TESTAPP2);
#endif
#if !OVERSEAS_ENABLE
	NDK_AppDel(TESTAPP3);
#endif
#if 0//defined SP610
	NDK_AppDel(TESTAPP4);
#endif
	return;
}

