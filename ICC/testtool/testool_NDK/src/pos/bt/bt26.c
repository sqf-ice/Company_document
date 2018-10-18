/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt26.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterWrite能否给指定蓝牙设备送指定长度的数据
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterWrite"	
#define MASTER_MAX_SIZE 4096//没有最大值
#define TIME_OUT 30000
#define EXPMAXNUM 3
#define 	SNDCNT			(71)
#define 	SNDCNT1			(128)

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt26
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150611		created
*****************************************************************/
//以下为本用例入口函数
void bt26(void)	//无参数无返回
{
	/*private & local definition*/
	int ret=0,j=0,real_len=0;
	char smac[6]={0};
	char sendbuf[MASTER_MAX_SIZE]={0},recbuf[MASTER_MAX_SIZE]={0};

	
	/*process body*/
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用例

	//测试前置
	NDK_PortClose(PORT_NUM_BT);
	
	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	
	
	//主设备设置连接方式为默认，从设备为双模设备，主从设备配对并主动连接
	//扫描双模测试样机
	cls_show_msg("请打开双模的蓝牙从设备,按任意键继续");  
	if(lib_btmaster_scan(smac, 0)!=NDK_OK)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	//配对主从设备
	cls_show_msg("从设备请[设置蓝牙配对模式]后进入[等待配对]后按任意键继续");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//主动连接从设备
	cls_printf("连接从模式设备中");
	if((ret=NDK_BTMasterConnect(smac,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=lib_btmaster_getconstatus(smac, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)	/*清从设备的接收缓冲区*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case1:	参数非法测试
	if((ret=NDK_BTMasterWrite(NULL,sizeof(sendbuf),sendbuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterWrite(smac,sizeof(sendbuf),NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//case2:应能向从设备写指定长度的数据（非整8字节数据），再将从设备读到的数据，发送给主设备，校验主设备发送的数据与主设备读取到的是否一致
	cls_show_msg("进行非整8字节数据读写,从设备进入[数据收发]后按任意键继续");
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)	/*清从设备的接收缓冲区*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_BTMasterWrite(smac,SNDCNT,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//从设备应该要读取数据，并且发送数据给主设备
	
	//主设备读取从设备发来的数据
	memset(recbuf,0,sizeof(recbuf));
	if((ret=NDK_BTMasterRead(smac,SNDCNT, recbuf,TIME_OUT,&real_len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((real_len!=SNDCNT)||memcmp(sendbuf, recbuf, real_len))	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//case2.1:应能向从设备写指定长度的数据（发送64整数倍但又不超过4K的数据），再将从设备读到的数据，发送给主设备，校验主设备发送的数据与主设备读取到的是否一致
	cls_show_msg("进行64整数倍但又不超过4K的数据读写,从设备进入[数据收发]后按任意键继续");
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)	/*清从设备的接收缓冲区*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_BTMasterWrite(smac,SNDCNT1,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//从设备应该要读取数据，并且发送数据给主设备
	
	//主设备读取从设备发来的数据
	memset(recbuf,0,sizeof(recbuf));
	if((ret=NDK_BTMasterRead(smac,SNDCNT1, recbuf,TIME_OUT,&real_len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((real_len!=SNDCNT1)||memcmp(sendbuf, recbuf, real_len))	
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, real_len, SNDCNT1);
		GOTOERR;	
	}

//#if 0
	//case3:主设备往从设备写数据 然后主设备断开连接不应出现异常
	cls_show_msg("进行写完断开蓝牙从设备连接,从设备进入[数据收发]后任意键继续");
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)	/*清从设备的接收缓冲区*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_BTMasterWrite(smac,MASTER_MAX_SIZE,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}	
//#endif
	if((ret = NDK_BTMasterDisconnect(smac)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}


	//case4:重新进行主设备与从设备的配对连接应该能正常进行数据收发
	cls_show_msg("从设备请[设置蓝牙配对模式]后进入[等待配对]后按任意键继续");
	if((ret=NDK_BTMasterBond(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("连接从模式设备中");
	if((ret=NDK_BTMasterConnect(smac,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}	
	if((ret=lib_btmaster_getconstatus(smac, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	cls_show_msg("进行4K的数据读写,从设备进入[数据收发]后按任意键继续");
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)	/*清从设备的接收缓冲区*/
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_BTMasterWrite(smac,MASTER_MAX_SIZE,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//从设备应该要读取数据，并且发送数据给主设备
	
	//主设备读取从设备发来的数据
	memset(recbuf,0,sizeof(recbuf));
	if((ret=NDK_BTMasterRead(smac,MASTER_MAX_SIZE, recbuf,TIME_OUT,&real_len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((real_len!=MASTER_MAX_SIZE)||memcmp(sendbuf, recbuf, real_len))	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//case5:断开主动连接，应返回未连接到该设备
	if((ret=NDK_BTMasterDisconnect(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d),连接超时", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterWrite(smac,sizeof(sendbuf),sendbuf))!=NDK_ERR_BT_NOT_CONNECTED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	//case6:蓝牙串口未打开，给指定蓝牙设备送指定长度的数据应该失败
	if((NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterWrite(smac,sizeof(sendbuf),sendbuf))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTMasterDisconnect(smac);
	NDK_PortClose(PORT_NUM_BT);
	return;
	
}

