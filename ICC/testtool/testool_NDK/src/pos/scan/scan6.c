/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描
* file name			: 6.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_ScanStart开始摄像头扫描、NDK_ScanState获取摄像头扫描状态、NDK_ScanStop停止摄像头扫描功能(非阻塞式)
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
*							zhengry			20160525		created
*
*****************************************************************/
void scan6(void)
{
	/*private & local definition*/
	int ret=0, plen=0;
	uchar pvalue[500]={0};
	uchar pvaluein[35] = {0x3a, 0x37, 0x37, 0x3a, 0x74, 0x30, 0x3a, 0x2f, 0x3a, 0x30, 0x30, 0x2e, 0x2e, 0x74, 0x30, 0x3a, 0x2e, 0x74, 0x3a, 0x33, 0x74, 0x30, 0x2e, 0x33, 0x2e, 0x2e, 0x2e, 0x3a, 0x30, 0x30, 0x3a, 0x2e, 0x33, 0x37, 0x33};
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	 //case1:未初始化摄像头,应返回摄像头未初始化
	if((ret=NDK_ScanStart()) != NDK_ERR_CAMERA_INIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_ScanState(pvalue, &plen)) != NDK_ERR_CAMERA_INIT )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_ScanStop()) != NDK_ERR_CAMERA_INIT )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	
	//case2:未开始摄像头扫描,获取扫描状态应返回未处于异步扫描状态NDK_ERR_CAMERA_NOT_ASYNC
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_ScanState(pvalue, &plen)) != NDK_ERR_CAMERA_NOT_ASYNC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:开始扫描后,若没待扫描图像,应返回正在扫描NDK_ERR_CAMERA_SCANNING
	if((ret=NDK_ScanStart()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScanState(pvalue, &plen)) != NDK_ERR_CAMERA_SCANNING)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:若未停止摄像头扫描,摄像头重新开始扫描应返回设备忙
	if((ret=NDK_ScanStart()) != NDK_ERR_CAMERA_BUSY)
	{
		NDK_ScanStop();
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:若未停止摄像头扫描,摄像头直接关闭应返回设备忙
	if((ret=NDK_ScanClose()) != NDK_ERR_CAMERA_BUSY )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:若未停止摄像头扫描,应能继续进行扫描
	cls_show_msg("请准备好待扫描的图形,任意键继续");
	memset(pvalue,0,sizeof(pvalue));
	lib_StartStopwatch();
	while(1)
	{	
		if(lib_ReadStopwatch()>MAXTIME)
		{
			NDK_ScanStop();
			send_result("line %d:%s扫描超时", __LINE__, TESTAPI);
			break;
		}
		ret=NDK_ScanState(pvalue, &plen);
		switch(ret)
		{
			case NDK_ERR_CAMERA_SCANNING:
				continue;
				break;
			case NDK_OK:
				NDK_ScanStop();//如果要在屏幕上显示应用的界面，都要先把扫描停掉
				if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
				{
					send_result("line %d:%s 测试扫描内容与预期不符", __LINE__, TESTAPI);
					goto ERR;
				}
				break;
				/*if((plen!=35)||!memcmp(pvaluein,pvalue,plen))
				{
					send_result("line %d:%s解码失败(plen=%d, pvalue=%s)", __LINE__, TESTAPI, plen, pvalue);
					goto ERR2;
				}
				break;
				*///目前对一个二维码每次解码的结果都是不同的
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
	lib_StartStopwatch();
	while(1)
	{	
		if(lib_ReadStopwatch()>MAXTIME)
		{
			NDK_ScanStop();
			send_result("line %d:%s扫描超时", __LINE__, TESTAPI);
			break;
		}
		ret=NDK_ScanState(pvalue, &plen);
		switch(ret)
		{
			case NDK_ERR_CAMERA_SCANNING:
				continue;
				break;
			case NDK_OK:
				NDK_ScanStop();
				if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
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
	
	//NDK_ScanStart: NDK_ERR_CAMERA_THREAD   创建线程失败
	//NDK_ScanStop: NDK_ERR_CAMERA_SYNC   处于同步扫描状态  在同步出现异常，状态无法恢复的时候用
	//NDK_ScanStop: NDK_ERR_CAMERA_STOP   线程无法停止 
	//NDK_ScanState: NDK_ERR_CAMERA_READY_STOP   线程将被终止
	//NDK_ScanState: NDK_ERR_CAMERA_STATE     未知状态
	
	//结束测试
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_ScanClose();
	return;
}

