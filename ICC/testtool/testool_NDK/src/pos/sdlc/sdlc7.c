/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc7.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmClrbuf 能否清空同步通讯缓冲区
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmClrbuf"
#define	MAXWAITTIME	60	//60秒
#define PCKTMAXLEN 350

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	chenjings      20130108	created
*****************************************************************/
void sdlc7(void)
{
	//以下为局部变量
	int ret = 0;
	int j = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};// 1K是文档推荐的上限,2K是最大上限
	uint GetReadLen = -1;
	
	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用
	
	//测试前置:
	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		return;
	}
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
	update_SDLC_packet(buf, sizeof(buf));//添加TPDU包头
#if 0 //20141202 reset函数功能修改直接返回NDK_OK后屏蔽本用例 
	//case1:未初始化,应该返回NDK_ERR_MODEM_INIT_NOT
	if((ret=NDK_MdmClrbuf())!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s未初始化返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
#endif
	//case2:初始化应返回成功NDK_OK
	if((ret=NDK_MdmSdlcInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_MdmClrbuf())!=NDK_OK)
	{
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=0))//清后缓冲区内容为0
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
		GOTOERR;
	}
	
	//case3:拨号之后调用清缓冲区有效应该返回NDK_OK，并且不影响正常数据通信
	if((ret=LinkUp())!=NDK_OK)
	{
		send_result("line %d:%s创建链路失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(buf, 0, sizeof(buf));
	if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);		
		GOTOERR;
	}
	sleep(5);//延时
	//4.4.6NDK版本之后只要初始化后，都可以进行对应的同步、异步接收缓存清空  linwl20140304
	if((ret=NDK_MdmClrbuf())!=/*NDK_ERR_MODEM_SDLCCLRBUFFAIL*/NDK_OK)//同步MODEM清缓冲区在拨号之后无效，会返回失败 linwl20130710
	{
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	sleep(5);
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0	
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_ERR_TIMEOUT/*NDK_OK*/)//拨号之后清缓冲区有效，所以读数据应该超时
	{
		send_result("line %d:%s读数据失败(ret=%d,rlen=%d)", __LINE__, TESTAPI, ret, rlen);
		GOTOERR;
	}
	if((ret=NDK_MdmClrbuf())!=/*NDK_ERR_MODEM_SDLCCLRBUFFAIL*/NDK_OK)//同步MODEM清缓冲区在拨号之后无效，会返回失败 linwl20130710
	{
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:正常数据收发前，清空缓冲区，不影响正常数据收发
	if((ret=NDK_MdmClrbuf())!=NDK_OK)
	{
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);		
		GOTOERR;
	}
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0	
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)//拨号之后清缓冲区无效，所以读数据应该成功
	{
		send_result("line %d:%s读数据失败(ret=%d,rlen=%d)", __LINE__, TESTAPI, ret, rlen);
		GOTOERR;
	}
	if((ret=NDK_MdmClrbuf())!=NDK_OK)
	{
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}		
	//比较读取数据
	if((PCKTMAXLEN!=rlen)||(MemCmp(buf, rbuf, rlen)))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, PCKTMAXLEN, rlen);
		goto ERR;
	}
	
	//测试结束
	LinkDown();
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	LinkDown();
	return;
}

