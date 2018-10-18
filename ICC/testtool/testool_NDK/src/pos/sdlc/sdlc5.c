/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc5.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmRead 能否进行同步通讯的接收数据操作，即SDLC读操作
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmRead"
#define MAXWAITTIME 60
#define	WUCHASEC	0.03 //这个值待定
#define PCKTMAXLEN	350

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
void sdlc5(void)
{
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};// 1K是文档推荐的上限,2K是最大上限
	float tmp = 0.0;
	
	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用
	
	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		return;
	}
#if 0 //20141202 reset函数功能修改直接返回NDK_OK后屏蔽本用例 
	//case1:未初始化情况
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s读数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);		
		RETURN;
	}
#endif
	//拨号连接	
	if((ret=LinkUp())!=NDK_OK)
	{
		send_result("line %d:%s创建链路失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//初始化发送缓冲区数据
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
	update_SDLC_packet(buf, sizeof(buf)); //添加TPDU包头
	//case2:正确写长度
	//case2.1:读等待时间为60秒
	if((ret=NDK_MdmWrite(buf, PCKTMAXLEN))!=NDK_OK)
	{		
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);
	//读
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较读取数据
	if((PCKTMAXLEN!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__,TESTAPI,  PCKTMAXLEN, rlen);
		GOTOERR;
	}
	//case2.2:读等待时间为0	
	if((ret=NDK_MdmWrite(buf, PCKTMAXLEN))!=NDK_OK)
	{		
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__,TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);
	//读
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, 0))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较读取数据
	if((PCKTMAXLEN!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, PCKTMAXLEN, rlen);
		GOTOERR;
	}
	
	//case3:错误参数情况
	if((ret=NDK_MdmRead(NULL, &rlen, MAXWAITTIME))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MdmRead(rbuf, NULL, MAXWAITTIME))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s读数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MdmRead(rbuf, &rlen, -1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s读数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case4: 等待超时
	// 此时读缓冲区没有数据,应返回读超时
	lib_StartStopwatch();
	if(((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_ERR_TIMEOUT)||((tmp=(lib_ReadStopwatch()-MAXWAITTIME))>WUCHASEC))
	{
		send_result("line %d:%s读数据测试超时失败(ret=%d,tm=%f)", __LINE__, TESTAPI, ret, tmp);
		GOTOERR;
	}
	
	//case5:读取的等待时间为0时且没有数据应返回无数据
	if((ret=NDK_MdmRead(rbuf, &rlen, 0))!=NDK_ERR_TIMEOUT)
	{
		send_result("line %d:%s读数据测试超时失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:测试 同步链路已断时是否返回未拨号。
	if((ret=NDK_MdmHangup())!=NDK_OK )
	{
		send_result("line %d:%s挂机失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(5);
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_ERR_MODEM_NOPREDIAL)
	{
		send_result("line %d:%s读数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	LinkDown();
	send_result("%s测试通过", TESTAPI);//或还可输出其它信息,如：send_result("%s测试通过(耗时%f)", TESTAPI, fTimeElapsed);
	return;
ERR:
	LinkDown();
	return;
}

