/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc8.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmGetreadlen 能否获取modem长度
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmGetreadlen"
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
void sdlc8(void)
{
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};
	uint GetReadLen = -1;

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
	if((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s获取modem未初始化返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	//case2:NDK_MdmGetreadlen返回的长度值为NULL时是否返回错误 
	if((ret=NDK_MdmGetreadlen(NULL))!= NDK_ERR_PARA)
	{
		send_result("line %d:%s获取modem长度返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if((ret=LinkUp())!=NDK_OK)
	{
		send_result("line %d:%s创建链路失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
	update_SDLC_packet(buf, sizeof(buf)); //添加TPDU包头
	
#if 1
	//case3:刚拨号成功modem缓冲区长度应该没有数据
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=0))
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
		GOTOERR;
	}
	
	//case4:正常写数据后判断modem长度是否正确，应该跟写入的一致
	//写数据
	if((ret=NDK_MdmWrite(buf, PCKTMAXLEN))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(10);
	GetReadLen=-1;
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=PCKTMAXLEN))
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
		GOTOERR;
	}
	
	//case5:读取数据后缓冲区应该要被清空GetReadLen应该为0
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{		
		send_result("line %d:%s读数据失败(ret=%d,len=%d)", __LINE__, TESTAPI, ret, rlen);
		GOTOERR;
	}
	//比较读取数据长度
	if(PCKTMAXLEN!=rlen)
	{		
		send_result("line %d:%s收发长度不一致(W=%d,R=%ld)", __LINE__, TESTAPI, PCKTMAXLEN, rlen);
		GOTOERR;
	}
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=0))
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
		GOTOERR;
	}
#endif

#if 1
	//case6:同步MODEM清缓冲区在拨号之后有效，所以NDK_MdmGetreadlen返回0
	//写数据
	if((ret=NDK_MdmWrite(buf, PCKTMAXLEN))!=NDK_OK)
	{				
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//延时
	sleep(10);
	if((ret=NDK_MdmClrbuf())!=/*NDK_ERR_MODEM_SDLCCLRBUFFAIL*/NDK_OK)//同步MODEM清缓冲区在拨号之后无效，会返回失败 l20130710->20140304 新版本中调整后清缓冲有效
	{		
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(10);
	GetReadLen=-1;
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=/*350*/0))
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
		goto ERR;
	}
#endif

	//测试结束
	LinkDown();
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	LinkDown();
	return;
}

