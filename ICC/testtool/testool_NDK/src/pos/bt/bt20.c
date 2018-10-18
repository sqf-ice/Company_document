/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt20.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterScan,NDK_BTMasterStopScan 是否可以正常扫描得到扫描结果，并且根据需要停止扫描
* related document	: NDK.chm
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterScan,NDK_BTMasterStopScan"	
#define EXPMAXNUM 10
#define MAXWAITTIME 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt20
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150529		created
*****************************************************************/
void bt20(void)	//无参数无返回
{
	/*private & local definition*/
	ST_BT_DEV sresult[50],sresult1[50];
	int ret=0,snum=0,j=0,i=0,snum1=0;
	time_t oldtime=0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//case1：蓝牙串口未打开，扫描或获取扫描结果或停止扫描都应该失败，返回值为“NDK_ERR_OPEN_DEV”
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;	
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;	
	}

	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//case2：参数异常应返回参数错误
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_BREDR-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	//case3：正常测试:分别扫描单模BREDR、单模BLE、双模，正常调用扫描结果里的蓝牙设备类型应与之一致	
	for(j=BT_DEVICE_TYPE_BREDR;j<BT_DEVICE_TYPE_DUMO+1;j++)
	{
		cls_printf("正在测试%d模式中...", j);
		if((ret=NDK_BTMasterScan(j))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,j);
			GOTOERR; 
		}
		oldtime=time(NULL);	
		while(1)
		{
			sleep(5);
			memset(sresult, 0, sizeof(sresult));
			if((ret=NDK_BTMasterGetScanResults(NULL,sresult,EXPMAXNUM,&snum))!=NDK_OK)//应该能够成功获取到扫描信息
			{
				send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, snum,j);
				GOTOERR;
			}
			if(snum>0)//扫描到至少一个后退出扫描
				break;
			if((time(NULL)-oldtime)>MAXWAITTIME)
			{
				send_result("line %d:%s长时间未扫描到蓝牙信息(%d,%d)", __LINE__, TESTAPI, snum,j);		
				break;
			}
		}
		if(j!=BT_DEVICE_TYPE_DUMO)//扫描双模会返回3种类型的设备，不对设备类型进行比较
		{
			for(i=0;i<snum;i++)
			{
				//cls_show_msg1(1,"snum=%d,j=%d,i=%d", snum, j, i);				
				if(sresult[i].emBtType!=j&&sresult[i].emBtType!=BT_DEVICE_TYPE_DUMO)//扫描单模时应返回要扫描的单模类型或者双模
				{					
					send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, sresult[i].emBtType,j);					
					GOTOERR;
				}
			}
		}
		//测试过程中出现过停止扫描失败的情况
		if((ret=NDK_BTMasterStopScan())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,j);
			GOTOERR; 
		}
	}

	//case4:连续两次调用NDK_BTMasterScan，预期应该是第一次生效，获取到的扫描模式应该和预期一致
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_BREDR))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//实际测试:连续两次调用，中间如果没有调用NDK_BTMasterStopScan，第2次NDK_BTMasterScan返回-1  该方案已经被取消 20151009 涂志广
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_BLE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(10);
	memset(sresult, 0, sizeof(sresult));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult,EXPMAXNUM,&snum))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	for(i=0;i<snum;i++)
	{				
		if(sresult[i].emBtType!=BT_DEVICE_TYPE_BREDR&&sresult[i].emBtType!=BT_DEVICE_TYPE_DUMO)
		{					
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, sresult[i].emBtType);					
			GOTOERR;
		}
	}
	//设计要求成对使用
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR; 
	}
	
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_BLE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(10);
	memset(sresult, 0, sizeof(sresult));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult,EXPMAXNUM,&snum))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	for(i=0;i<snum;i++)
	{				
		if(sresult[i].emBtType!=BT_DEVICE_TYPE_BLE&&sresult[i].emBtType!=BT_DEVICE_TYPE_DUMO)
		{					
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, sresult[i].emBtType);					
			GOTOERR;
		}
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR; 
	}
	
	//case5：正常测试:扫描双模，当停止扫描后，增加几台设备，扫描的数量与之前的做对比	
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(10);
	memset(sresult, 0, sizeof(sresult));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult,EXPMAXNUM,&snum))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR; 
	}
	cls_show_msg("请再添加一台或多台蓝牙从设备，添加后按任意键继续。");
	sleep(10);
	memset(sresult1, 0, sizeof(sresult1));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult1,EXPMAXNUM,&snum1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR; 
	}
	if(snum!=snum1)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, snum,snum1);
		goto ERR;
	}
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTMasterStopScan();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

