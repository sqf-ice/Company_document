/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc4.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmWrite 能否进行同步通讯的发送数据操作，即SDLC写操作
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmWrite"
#define MAX_SDLC_WRITEDATALEN 2048//1024=>2048用户需要底层缓冲区开大到2048
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
*			  	  	chenjings      20130108	created
*****************************************************************/
void sdlc4(void)
{
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[2048+1] = {0}, rbuf[512] = {0};// 1K是文档推荐的上限

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

	//case2 ,写正确长度
	//case2.1 初始化buf
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
	update_SDLC_packet(buf, sizeof(buf)); //添加TPDU包头
	//case2.2,正确写长度
	if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);		
		GOTOERR;
	}
	//case2.3:是否能正常读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2.4:比较读写数据是否一致
	if((PCKTMAXLEN!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, PCKTMAXLEN, rlen);
		GOTOERR;
	}

	//case3:写数据长度为2048，应该成功，底层要至少支持2K数据缓冲区//由于测试环境限制，不做读和比较 20130724
	if((ret=NDK_MdmWrite(buf, MAX_SDLC_WRITEDATALEN))!=NDK_OK)
	{
		send_result("line %d:%s写数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if 0	//linwl20130606,客户需求需要支持大于1K以上的数据收发，故屏蔽该用例，由外部验证反馈对大于1K数据收发的支持 改由上面的case3验证
	//case3,写长度1024+1,应返回失败
	if((ret=NDK_MdmWrite(buf, MAX_SDLC_WRITEDATALEN+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s写数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	//case4,发送数据长度为0，也应该返回NDK_ERR_PARA
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
	
	//case6:测试 同步链路已断时是否返回未拨号。
	//挂机
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s挂机失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(5);
	if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_ERR_MODEM_NOPREDIAL)//放宽限制==NDK_OK)
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

