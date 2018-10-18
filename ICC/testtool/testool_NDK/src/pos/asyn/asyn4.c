/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: asyn模块
* file name		: asyn3.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/asyn
* description		: 测试NDK_MdmWrite 能否进行异步通讯的发送数据操作，即asyn写操作
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmWrite"
#define MAXWAITTIME 60
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
*			  	  	chenjings      20130114	created
*****************************************************************/
void asyn4(void)
{
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[ASYNPCKTMAXLEN+1] = {0}, rbuf[512] = {0};// 1K是文档推荐的上限

	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);  //屏幕提示当前所在的测试用

	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		return;
	}
#if 0 //20141202 reset函数功能修改直接返回NDK_OK后屏蔽本用例 
	//case1,未初始化情况
	if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s写数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);		
		RETURN;
	}
#endif
	//拨号连接
	if((ret=LinkUp())!=NDK_OK)
	{
		send_result("line %d:%s创建链路失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:测试写正确长度
	//初始化buf
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;

#if 0		
	NDK_MdmClrbuf();//异步MODEM拨号后连接数据收发前如果不调用该函数，异步串口会有遗留数据影响测试结果所以暂时增加该函数
#endif
	
	//写数据
	if((ret=NDK_MdmWrite(buf,sizeof(rbuf)))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);		
		GOTOERR;
	}
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(rbuf)+1;//期望读取到的数据长度要大于等于实际能读取到的数据长度。
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较数据
	if((sizeof(rbuf)!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, sizeof(rbuf), rlen);
		GOTOERR;
	}
	
	//case3:写长度4096+1,应返回失败
	if((ret=NDK_MdmWrite(buf, ASYNPCKTMAXLEN+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s写数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:发送数据长度为0，也应该返回NDK_ERR_PARA
	memset(rbuf, 0, sizeof(rbuf));//清空rbuf,并借用一下rbuf发送空数据
	if((ret=NDK_MdmWrite(rbuf, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s写数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5,发送数据指针为NULL
	if((ret=NDK_MdmWrite(NULL, PCKTMAXLEN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s写数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:测试 异步链路已断时，是否放回未拨号。
	//挂机
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s挂机失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_OK/*NDK_ERR_MODEM_NOPREDIAL*/)//异步MDM读写时是写串口操作，所以仍然返回成功,linwl20130617
	{
		send_result("line %d:%s写数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	LinkDown();
	send_result("%s测试通过",TESTAPI);
	return;
ERR:
	LinkDown();
	return;
}

