/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: asyn模块
* file name		: asyn7.c
* Author 			: chenjings
* version			: 
* DATE			: 20130114
* directory 		: src/pos/asyn
* description		: 测试NDK_MdmClrbuf 能否清空异步通讯缓冲区
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmClrbuf"
#define	MAXWAITTIME	60	//60秒

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
void asyn7(void)
{
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};// 1K是文档推荐的上限,2K是最大上限

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
	//case1:未初始化
	if((ret=NDK_MdmClrbuf())!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s未初始化返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
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

	//写数据
	if((ret=NDK_MdmWrite(buf,sizeof(buf)))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);		
		GOTOERR;
	}
	//延时
	sleep(10);
	
	//case2:测试清空读缓冲区是否有效(清空后应该不能read到数据)
	if((ret=NDK_MdmClrbuf())!=NDK_OK)
	{
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(rbuf)+1;
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_ERR_TIMEOUT)	//应该不能读取到数据
	{
		send_result("line %d:%s读数据超时返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:测试正确清空缓冲区，数据通讯应成功
	if((ret=NDK_MdmClrbuf())!=NDK_OK)
	{
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//写数据
	if((ret=NDK_MdmWrite(buf, sizeof(buf)))!=NDK_OK)
	{		
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(buf)+1;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较数据
	if((sizeof(buf)!=rlen)||(MemCmp(buf, rbuf, rlen)))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, sizeof(buf), rlen);
		goto ERR;
	}

	//case4:清写缓冲待定
	
	//测试结束
	LinkDown();
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	LinkDown();
	return;


}

