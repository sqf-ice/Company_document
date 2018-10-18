/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: asyn模块
* file name		: asyn5.c
* Author 			: chenjings
* version			: 
* DATE			: 20130114
* directory 		: src/pos/asyn
* description		: 测试NDK_MdmRead 能否进行异步通讯的接收数据操作，即asyn读操作
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmRead"
#define MAXWAITTIME 60
#define MAXTIMEOUT  10
#define	WUCHASEC	0.03 //这个值待定

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
void asyn5(void)
{
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0, rlenother = 0;
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
	//初始化buf
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
	
#if 0		
	NDK_MdmClrbuf();//异步MODEM拨号后连接数据收发前如果不调用该函数，异步串口会有遗留数据影响测试结果所以暂时增加该函数
#endif

	//case2:读取正确写长度
	//写数据
	if((ret=NDK_MdmWrite(buf, sizeof(buf)))!=NDK_OK)
	{		
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(buf)+1;//期望读取到的数据长度要大于等于实际能读取到的数据长度。
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较数据
	if((sizeof(buf)!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__,TESTAPI, sizeof(buf), rlen);
		GOTOERR;
	}
	
#if 0
	NDK_MdmClrbuf();//异步MODEM拨号后连接数据收发前如果不调用该函数，异步串口会有遗留数据影响测试结果所以暂时增加该函数
#endif

	//case3:测试读等待时间为0，会进行一次读操作
	//写数据
	if((ret=NDK_MdmWrite(buf, sizeof(buf)))!=NDK_OK)
	{		
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__,TESTAPI, ret);
		GOTOERR;
	}
	sleep(10);
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(buf)+1;
	if((ret=NDK_MdmRead(rbuf, &rlen, 0))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较读取数据
	if((sizeof(buf)!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, sizeof(buf), rlen);
		GOTOERR;
	}
	
#if 0
	NDK_MdmClrbuf();//异步MODEM拨号后连接数据收发前如果不调用该函数，异步串口会有遗留数据影响测试结果所以暂时增加该函数
#endif	
	//cas4:测试希望读取长度小于发送的数据长度
	//写数据
	if((ret=NDK_MdmWrite(buf, sizeof(buf)))!=NDK_OK)
	{		
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__,TESTAPI, ret);
		GOTOERR;
	}
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(buf)-1;
	if ((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较数据
	if(((sizeof(buf)-1)!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, sizeof(buf)-1, rlen);
		GOTOERR;
	}
	//读取剩余的数据
	memset(rbuf, 0, sizeof(rbuf));
	rlenother=sizeof(buf)-rlen;
	if ((ret=NDK_MdmRead(rbuf, &rlenother, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较数据
	if(((sizeof(buf)-rlen)!=rlenother)||memcmp((buf+rlen), rbuf, rlenother))
	{
		send_result("line %d:%s收发数据不一致(W=%d,%x,R=%ld,%x)", __LINE__, TESTAPI, sizeof(buf)-rlen, rlenother);
		GOTOERR;
	}
	//case5:错误参数情况
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
	rlen=0;
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s读数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	rlen=-1;
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s读数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MdmRead(rbuf, &rlen, -1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s读数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

#if 0
	NDK_MdmClrbuf();//异步MODEM拨号后连接数据收发前如果不调用该函数，异步串口会有遗留数据影响测试结果所以暂时增加该函数
#endif	

	//case6: 测试读等待超时
	//此时读缓冲区没有数据,应返回读超时
	rlen=sizeof(buf);
	lib_StartStopwatch();
	if(((ret=NDK_MdmRead(rbuf, &rlen, MAXTIMEOUT))!=NDK_ERR_TIMEOUT)||((tmp=(lib_ReadStopwatch()-MAXTIMEOUT))>WUCHASEC))
	{
		send_result("line %d:%s读数据测试超时失败(ret=%d,tm=%f)", __LINE__, TESTAPI, ret, tmp);
		GOTOERR;
	}
	
	//case7:测试读取的等待时间为0时且没有数据应返回无数据
	if((ret=NDK_MdmRead(rbuf, &rlen, 0))!=NDK_ERR_TIMEOUT)
	{
		send_result("line %d:%s读数据测试超时失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8:测试链路已断时是否返回未拨号。
	if((ret=NDK_MdmHangup())!=NDK_OK )
	{
		send_result("line %d:%s挂机失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	rlen=sizeof(buf);
	if((ret=NDK_MdmRead(rbuf, &rlen, 3))!=NDK_ERR_TIMEOUT/*NDK_ERR_MODEM_NOPREDIAL*/)//异步MDM读写时是读串口操作，返回超时,linwl20130617
	{
		send_result("line %d:%s读数据返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
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

