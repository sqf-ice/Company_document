/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt28.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterReadLen能否正确读从设备蓝牙缓冲区字节
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterReadLen"	
#define MAX_SIZE 4096
#define TIME_OUT 30000

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 		: bt28
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150612		created
*****************************************************************/
//以下为本用例入口函数
void bt28(void)	//无参数无返回
{
	/*private & local definition*/
	int ret=0,readlen=0,j=0;
	char smac[6]={0};
	char recbuf[MAX_SIZE]={0},sendbuf[MAX_SIZE]={0};

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
	cls_show_msg("请打开双模的蓝牙从设备,按任意键继续");  
	if(lib_btmaster_scan(smac, 0)!=NDK_OK)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	//配对主从设备
	cls_show_msg("请配对蓝牙从设备，并选择好相应的配对模式完成配对，任意键继续");
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
	
	//case1:参数非法测试
	if((ret=NDK_BTMasterReadLen(NULL,&readlen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTMasterReadLen(smac,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:正常测试:向从设备写一定字节的数据，再调用读取缓冲区字节的函数，读到的字节数应与写的字节数一致
	cls_show_msg("进行读缓冲区,从设备进入[数据收发]后按任意键继续");
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_BTMasterWrite(smac,MAX_SIZE,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//从设备应该要读取数据，并且发送数据给主设备
	
	//主设备读取从缓冲区的数据长度
	sleep(5);//原本没有5S延时，读到的长度为0
	if((ret=NDK_BTMasterReadLen(smac,&readlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if(readlen!=sizeof(sendbuf)) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, readlen);//len=0
		GOTOERR;	
	}

	//case3:清缓冲后在调用，读取到的数据长度应该为0
	cls_show_msg("清缓冲区后调用,从设备进入[数据收发]后按任意键继续");
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)	
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTMasterReadLen(smac,&readlen))!=NDK_OK || readlen!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, readlen);
		GOTOERR;	
	}
	
	//case4:向从设备写数据，主设备读取数据后，接收数据的缓冲区清0，则读字节数为0
	cls_show_msg("读取数据后，缓冲区清0.从设备进入[数据收发]后按任意键继续");
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_BTMasterWrite(smac,MAX_SIZE,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//从设备应该要读取数据，并且发送数据给主设备
	
	//主设备读取从缓冲区的数据长度
	memset(recbuf,0,sizeof(recbuf));
	if((ret=NDK_BTMasterRead(smac,MAX_SIZE,recbuf,TIME_OUT,&readlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((readlen!=sizeof(sendbuf))||memcmp(sendbuf,recbuf,readlen))	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterReadLen(smac,&readlen))!=NDK_OK || readlen!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, readlen);
		GOTOERR;	
	}
	
	//case5:断开主动连接后，应返回未连接到该设备
	if((ret = NDK_BTMasterDisconnect(smac)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTMasterReadLen(smac,&readlen))!=NDK_ERR_BT_NOT_CONNECTED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:关闭蓝牙串口，应该失败
	if((NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}	
	if((ret=NDK_BTMasterReadLen(smac,&readlen))!=NDK_ERR_OPEN_DEV)
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


