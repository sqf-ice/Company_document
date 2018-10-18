/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描
* file name			: 3.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_ScanDoScan是否可以正常进行扫描头/摄像头阻塞式扫描功能(阻塞式)
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScanDoScan"
#define		MAXTIMEOUT 10
#define		WUCHASEC	0.5//0.03 5810x平台产品计时前还做了初始化需要340ms，中端产品耗时会更久,这边暂时先改成0.5s  20180516 modify

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scan3
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*										jiangym			20130917		created
*
*****************************************************************/
void scan3(void)
{
	/*private & local definition*/
	int ret = 0, plen = 0, poutlen = 0;
	char pvalue[500] = {0}, poutvalue[500] = {0};//50改成500,避免扫回来的数据超过50字节引起跑飞
	char pvaluein[17] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x00,0x31,0x32,0x33,0x34,0x35,0x36,0x0d,0x0a};
	float tmp = 0.0;
#if K21_ENABLE
	char str[32] = {0};
#endif

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置模块初始化
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case1:参数错误 nTimeOut=-1,pszValue,pnLen=NULL
	if((ret=NDK_ScanDoScan(-1,pvalue,&plen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScanDoScan(MAXTIMEOUT,NULL,&plen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScanDoScan(MAXTIMEOUT,pvalue,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2: 参数ntimeout=0,返回扫描超时
	if((ret=NDK_ScanDoScan(0,pvalue,&plen))!=NDK_ERR_TIMEOUT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:测试超时时间结束未扫描到数据退出,且超时参数误差在允许范围内
	cls_printf("请勿扫描,等待超时退出");
	lib_StartStopwatch();
	if((ret=NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen))!=NDK_ERR_TIMEOUT||((tmp = lib_ReadStopwatch()-MAXTIMEOUT)>WUCHASEC))
	{
#if K21_ENABLE
		memset(str,0,sizeof(str));
		ftos(tmp,str);
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, str);
#else	
		send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
#endif
		GOTOERR;
	}

	//case5:新增用例支持二维码数据内容中带0x00字节内容的扫描支持，0x00无法显示，所以采用内存比较的方法进行比较，需要扫描特定的二维码  20150317 linwl
	cls_printf("请在20秒内进行扫描带0x00字节的二维码");
	plen=0;
	memset(pvalue,0,sizeof(pvalue));
	if((ret=NDK_ScanDoScan(MAXTIMEOUT*2,pvalue,&plen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((plen!=15&&plen!=17)||memcmp(pvaluein,pvalue,plen))
	{
		send_result("line %d:%s测试失败(plen=%d)", __LINE__, TESTAPI, plen);
		//for(i=0;i<plen;i++)
		//	cls_printf("0x%2x ", pvalue[i]);
		//wait_anykey(0);
		GOTOERR;
	}
	
	//case4:正常测试,应该能够获取到扫描内容和扫描数据长度
	//此处结合评测体系要求，需要对测试部收集到的各种类型的条码都已经扫描验证
	while(1)
	{
		if(cls_show_msg1(2,"请在20秒内进行扫描,取消键退出")==ESC)
			break;
		
		plen=0;
		memset(pvalue,0,sizeof(pvalue));
		memset(poutvalue,0,sizeof(poutvalue));
		if((ret=NDK_ScanDoScan(MAXTIMEOUT*2,pvalue,&plen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#if defined ME50||defined ME50N||defined ME50C||defined ME50H		
		if((ret=NDK_Utf8ToGbk(pvalue, plen, poutvalue, &poutlen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", poutlen, poutvalue)!=ENTER)
#else
		if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
#endif
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	
	//结束测试
	send_result("%s测试通过", TESTAPI);
ERR:
#if SCAN_SOFT_ENABLE
	NDK_ScanExit();
#endif
	return;
}

