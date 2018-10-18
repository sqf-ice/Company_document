/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt27.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterRead能否在设定的时间内从指定的蓝牙设备中读取指定长度的数据
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterRead"	
#define READ_MAX_SIZE 4096
#define SEND_SIZE 6000//实际不限
#define TIME_OUT 30000
#define WUCHASEC 0.03 //误差值，待定

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt27
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150611		created
*****************************************************************/
void bt27(void)
{
	/*private & local definition*/
	int ret=0, ret1=0, ret2=0, ret3=0, ret4=0,readlen=0,readlen1=0,j=0;
	char smac[6]={0};
	char recbuf[SEND_SIZE]={0},sendbuf[SEND_SIZE]={0};
	float tmp = 0.0;

	
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
	if(((ret=NDK_BTMasterRead(NULL,sizeof(recbuf),recbuf,TIME_OUT,&readlen))!=NDK_ERR_PARA)||
		((ret1=NDK_BTMasterRead(smac,sizeof(recbuf),NULL,TIME_OUT,&readlen))!=NDK_ERR_PARA)||
		((ret2=NDK_BTMasterRead(smac,sizeof(recbuf),recbuf,-1,&readlen))!=NDK_ERR_PARA)||
		((ret3=NDK_BTMasterRead(smac,0,recbuf,TIME_OUT,&readlen))!=NDK_ERR_PARA)||
		((ret4=NDK_BTMasterRead(smac,READ_MAX_SIZE+1,recbuf,TIME_OUT,&readlen))!=NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d,%d,%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2, ret3, ret4);//-6,-6,-6,0,0
		GOTOERR;
	}
	
	//case2:向从设备写指定长度的数据（大于4K的数据），从设备将读到的数据发送给主设备（发送两次），校验主设备两次读到的数据与主设备发送的数据是否一致
	cls_show_msg("进行大数据读写,从设备进入[数据收发]后按任意键继续");
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<sizeof(sendbuf); j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_BTMasterWrite(smac,SEND_SIZE,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//从设备应该要读取数据，并且发送数据给主设备
	
	//主设备读取从设备发来的数据
	memset(recbuf,0,sizeof(recbuf));
	if((ret=NDK_BTMasterRead(smac,READ_MAX_SIZE,recbuf,TIME_OUT,&readlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterRead(smac,SEND_SIZE-READ_MAX_SIZE,recbuf+READ_MAX_SIZE,TIME_OUT,&readlen1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if(((readlen+readlen1)!=SEND_SIZE)||memcmp(sendbuf, recbuf, SEND_SIZE))	
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, readlen, readlen1);
		GOTOERR;	
	}

	//case3:读超时测试:应该返回超时,并且超时时间在误差范围内
	cls_show_msg("读超时测试，请从设备先退出[数据收发],后按任意键继续");
	if((ret=NDK_BTMasterClrBuf(smac))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	lib_StartStopwatch();
	if(((ret=NDK_BTMasterRead(smac,READ_MAX_SIZE,recbuf,TIME_OUT,&readlen))!=NDK_ERR_TIMEOUT)|| (tmp=fabs(lib_ReadStopwatch()-TIME_OUT/1000))>WUCHASEC)
	{
		send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
		GOTOERR;
	}

	//case4:读完马上断开连接应该没问题 
	cls_show_msg("进行写完断开蓝牙从设备连接,从设备进入[数据收发]后按任意键继续");
	if((ret = NDK_BTMasterClrBuf(smac)) != NDK_OK)
	{				
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for (j=0; j<READ_MAX_SIZE; j++)
		sendbuf[j]=rand()%256;
	if((ret=NDK_BTMasterWrite(smac,READ_MAX_SIZE,sendbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	//从设备应该要读取数据，并且发送数据给主设备
	
	//主设备读取从设备发来的数据
	memset(recbuf,0,sizeof(recbuf));
	if((ret=NDK_BTMasterRead(smac,READ_MAX_SIZE,recbuf,TIME_OUT,&readlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret = NDK_BTMasterDisconnect(smac)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:断开主动连接后，读数据应返回未连接到该设备
	if((ret=NDK_BTMasterRead(smac,READ_MAX_SIZE,recbuf,TIME_OUT,&readlen))!=NDK_ERR_BT_NOT_CONNECTED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//GOTOERR;
	}

	//case6:	关闭蓝牙串口，读取数据应该失败
	if((NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}	
	if((ret=NDK_BTMasterRead(smac,READ_MAX_SIZE,recbuf,TIME_OUT,&readlen))!=NDK_ERR_OPEN_DEV)
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


