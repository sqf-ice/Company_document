/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描
* file name			: 7.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_ScanStart扫描头开始扫描、NDK_ScanState获取扫描状态、NDK_ScanStop停止扫描功能(低端非阻塞式)
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScanStart,NDK_ScanState,NDK_ScanStop"
#define 	MAXTIME     60
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scan6
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*							zhengry			20170221		created
*
*****************************************************************/
void scan7(void)
{
	/*private & local definition*/
	int ret=0, plen=0, poutlen = 0, i = 0;
	uchar pvalue[500]={0}, poutvalue[500]={0};
	char pvaluein[17] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x00,0x31,0x32,0x33,0x34,0x35,0x36,0x0d,0x0a};
#if defined ME66
	time_t oldtime = 0;
#endif
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	 //case1:未开启扫描头,ScanState读取扫描数据应该失败
#if CPU5810X_ENABLE	
	if((ret=NDK_ScanState(pvalue, &plen)) != NDK_ERR_READ)
#else
	if((ret=NDK_ScanState(pvalue, &plen)) != NDK_ERR)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:开始扫描后,若没待扫描图像,应返回失败
	if((ret=NDK_ScanStart()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if CPU5810X_ENABLE	
	if((ret=NDK_ScanState(pvalue, &plen)) != NDK_ERR_READ)
#else
	if((ret=NDK_ScanState(pvalue, &plen)) != NDK_ERR)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:若未停止扫描,再次调用开始扫描应返回成功
	if((ret=NDK_ScanStart()) != NDK_OK)
	{
		NDK_ScanStop();
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//GOTOERR;//实际scanstate扫描结果会出错
	}

	//case4:若未停止扫描,应能继续进行扫描
	cls_show_msg("请准备好待扫描的图形,任意键继续");
	memset(pvalue,0,sizeof(pvalue));
	memset(poutvalue,0,sizeof(poutvalue));
#if defined ME66
	oldtime = time(NULL);
#else
	lib_StartStopwatch();
#endif
	while(1)
	{	
#if defined ME66
		if((time(NULL)-oldtime)>MAXTIME)
#else
		if(lib_ReadStopwatch()>MAXTIME)
#endif
		{
			NDK_ScanStop();
			send_result("line %d:%s扫描超时", __LINE__, TESTAPI);
			break;
		}
		ret=NDK_ScanState(pvalue, &plen);
		switch(ret)
		{
#if CPU5810X_ENABLE
			case NDK_ERR_READ:
#else
			case NDK_ERR:
#endif
				continue;
				break;
			case NDK_OK:
				NDK_ScanStop();
#if defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME50NX||defined ME68
				if((ret=NDK_Utf8ToGbk(pvalue, plen, poutvalue, &poutlen)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					goto ERR;
				}
				if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", poutlen, poutvalue)!=ENTER)
#else
				if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
#endif
				{
					send_result("line %d:%s 测试扫描内容与预期不符", __LINE__, TESTAPI);
					goto ERR;
				}
				break;
			default:
				NDK_ScanStop();
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				goto ERR;
		}
		if(ret == NDK_OK)
			break;
	}

	//case7:重新开始扫描后，应能正常扫描
	cls_show_msg("请准备好另一个待扫描的图形,任意键继续");
	if((ret=NDK_ScanStart()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	plen=0;
	memset(pvalue,0,sizeof(pvalue));
	memset(poutvalue,0,sizeof(poutvalue));
#if defined ME66
	oldtime = time(NULL);
#else
	lib_StartStopwatch();
#endif
	while(1)
	{	
#if defined ME66
		if((time(NULL)-oldtime)>MAXTIME)
#else
		if(lib_ReadStopwatch()>MAXTIME)
#endif
		{
			NDK_ScanStop();
			send_result("line %d:%s扫描超时", __LINE__, TESTAPI);
			break;
		}
		ret=NDK_ScanState(pvalue, &plen);
		switch(ret)
		{
#if CPU5810X_ENABLE
			case NDK_ERR_READ:
#else
			case NDK_ERR:
#endif
				continue;
				break;
			case NDK_OK:
				NDK_ScanStop();
#if defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME50NX||defined ME68
				if((ret=NDK_Utf8ToGbk(pvalue, plen, poutvalue, &poutlen)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					goto ERR;
				}
				if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", poutlen, poutvalue)!=ENTER)
#else
				if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
#endif
				{
					send_result("line %d:%s 测试扫描内容与预期不符", __LINE__, TESTAPI);
					goto ERR;
				}
				break;
			default:
				NDK_ScanStop();
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				goto ERR;
		}
		if(ret == NDK_OK)
			break;
	}

	//case8:新增用例支持二维码数据内容中带0x00字节内容的扫描支持，0x00无法显示，所以采用内存比较的方法进行比较，需要扫描特定的二维码
	cls_show_msg("请扫描0x00字节的二维码，任意键继续");
	if((ret=NDK_ScanStart()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	plen=0;
	memset(pvalue,0,sizeof(pvalue));
	memset(poutvalue,0,sizeof(poutvalue));
#if defined ME66
	oldtime = time(NULL);
#else
	lib_StartStopwatch();
#endif
	while(1)
	{	
#if defined ME66
		if((time(NULL)-oldtime)>MAXTIME)
#else
		if(lib_ReadStopwatch()>MAXTIME)
#endif
		{
			NDK_ScanStop();
			send_result("line %d:%s扫描超时", __LINE__, TESTAPI);
			break;
		}
		ret=NDK_ScanState(pvalue, &plen);
		switch(ret)
		{
#if CPU5810X_ENABLE
			case NDK_ERR_READ:
#else
			case NDK_ERR:
#endif
				continue;
				break;
			case NDK_OK:
				NDK_ScanStop();
#if defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME50NX||defined ME68
				if((ret=NDK_Utf8ToGbk(pvalue, plen, poutvalue, &poutlen)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					goto ERR;
				}
				if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", poutlen, poutvalue)!=ENTER)
#else
				if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
#endif
				{
					send_result("line %d:%s 测试扫描内容与预期不符", __LINE__, TESTAPI);
					goto ERR;
				}
				break;
			default:
				NDK_ScanStop();
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				goto ERR;
		}
		if(ret == NDK_OK)
			break;
	}
	if((plen!=15&&plen!=17)||memcmp(pvaluein,pvalue,plen))
	{
		send_result("line %d:%s测试失败(plen=%d)", __LINE__, TESTAPI, plen);
		//for(i=0;i<15;i++)
			//send_result("0x%2x ", pvalue[i]);
		goto ERR;
	}
		
	//结束测试
	send_result("%s测试通过", TESTAPI);
ERR:
	return;
}


