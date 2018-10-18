/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt29.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterClrBuf  能否清除指定蓝牙连接的接收缓冲区
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterClrBuf"	
#define MAX_SIZE 1024
#define TIME_OUT 30000

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 		: bt29
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150612		created
*****************************************************************/
//以下为本用例入口函数
void bt29(void)	//无参数无返回
{
	/*private & local definition*/
	int ret = 0, readlen = 0, len = 0, j = 0;
	char smac[6] = {0},smac1[6] = {0};
	char recbuf[MAX_SIZE] = {0}, sendbuf[MAX_SIZE] = {0};
	time_t oldtime = 0;

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
	cls_show_msg("请打开双模的蓝牙从设备1,按任意键开始扫描");  
	if(lib_btmaster_scan(smac, 0)!=NDK_OK)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg("请打开双模的蓝牙从设备2,按任意键开始扫描");  
	if(lib_btmaster_scan(smac1, 0)!=NDK_OK)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	//配对连接从设备1
	cls_show_msg("从设备1请[设置蓝牙配对模式]后进入[等待配对]后按任意键继续");
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
	cls_printf("连接从模式设备1中");
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

	//配对连接从设备2
	cls_show_msg("从设备2请[设置蓝牙配对模式]后进入[等待配对]后按任意键继续");
	if((ret=NDK_BTMasterBond(smac1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=lib_btmaster_pairing(1))!=2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("连接从模式设备2中");
	if((ret=NDK_BTMasterConnect(smac1,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=lib_btmaster_getconstatus(smac1, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//case1:参数非法测试
	if((ret=NDK_BTMasterClrBuf(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:清第一台缓冲后，再去读取数据，应读不到数据了，第二台不做清缓冲后，仍可读取到数据
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)	
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_BTMasterClrBuf(smac1)) != NDK_OK)	
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	cls_show_msg("两台从设备进入[数据收发]后按任意键继续");
	if((ret=NDK_BTMasterWrite(smac,MAX_SIZE,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterWrite(smac1,MAX_SIZE,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//判断接收缓冲区已经收到送回来的数据后
	oldtime=time(NULL);
	while(1)
	{	
		if((ret=NDK_BTMasterReadLen(smac,&len))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if(len==MAX_SIZE)
			break;
		if(time(NULL)-oldtime>60)
		{
			send_result("line %d:%s测试失败，数据传输超时(%d)", __LINE__, TESTAPI, len);
			goto ERR;
		}
	}
	//清除从设备1的缓冲区
	if((ret=NDK_BTMasterClrBuf(smac)) != NDK_OK)	
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recbuf,0,sizeof(recbuf));
	if((ret=NDK_BTMasterRead(smac,MAX_SIZE,recbuf,5000,&readlen))!=NDK_ERR_TIMEOUT||readlen!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, readlen);
		GOTOERR;	
	}

	//第二台应正常有数据
	memset(recbuf,0,sizeof(recbuf));
	if((ret=NDK_BTMasterRead(smac1,MAX_SIZE,recbuf,TIME_OUT,&readlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if(readlen!=MAX_SIZE||memcmp(sendbuf,recbuf,MAX_SIZE)) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;	
	}

	//case3:断开主动连接后，应返回NDK_ERR_BT_NOT_CONNECTED
	if((ret = NDK_BTMasterDisconnect(smac)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_BTMasterDisconnect(smac1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTMasterClrBuf(smac))!=NDK_ERR_BT_NOT_CONNECTED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:关闭蓝牙串口，应该返回NDK_ERR_OPEN_DEV
	if((NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}	
	if((ret=NDK_BTMasterClrBuf(smac))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTMasterDisconnect(smac);
	NDK_BTMasterDisconnect(smac1);
	NDK_PortClose(PORT_NUM_BT);
	return;
	
}

