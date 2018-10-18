/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: asyn模块
* file name		: asyn8.c
* Author 			: chenjings
* version			: 
* DATE			: 20130114
* directory 		: src/pos/asyn
* description		: 测试NDK_MdmGetreadlen 能否进行异步通讯时读取读缓冲区中的数据长度
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmGetreadlen"
#define MAXWAITTIME 60

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
void asyn8(void)
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
	//case1 : 未初始化情况
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

	//连接
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
	
	//case3:测试连接后读写之前，读缓冲区长度应该为0
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=0))
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
		GOTOERR;
	}
	
  //写数据
	if((ret=NDK_MdmWrite(buf, sizeof(buf)))!=NDK_OK)
	{		
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//延时
	sleep(10);
	
	//case4:正常写数据后判断modem长度是否正确
	GetReadLen=-1;
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=sizeof(buf)))
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
		GOTOERR;
	}
	
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(buf)+1;
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{		
		send_result("line %d:%s读数据失败(ret=%d,len=%d)", __LINE__, TESTAPI, ret, rlen);
		GOTOERR;
	}
	//比较数据长度
	if(sizeof(buf)!=rlen)
	{
		send_result("line %d:%s收发长度不一致(W=%d,R=%ld)", __LINE__, TESTAPI, sizeof(buf), rlen);
		GOTOERR;
	}
	
	//case5:正常读数据后判断modem长度应该为0
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=0))
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
		GOTOERR;
	}

	//写数据
	if((ret=NDK_MdmWrite(buf, 350))!=NDK_OK)
	{				
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//延时
	sleep(10);
	GetReadLen=-1;
	if((ret=NDK_MdmClrbuf())!=NDK_OK)//清接收缓冲
	{		
		send_result("line %d:%s清空缓冲失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//case6:测试调用NDK_MdmClrbuf判断modem长度应该为0
	if(((ret=NDK_MdmGetreadlen(&GetReadLen))!= NDK_OK)||(GetReadLen!=0))
	{
		send_result("line %d:%s获取modem长度错误(ret=%d,len=%d)", __LINE__, TESTAPI, ret, GetReadLen);
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

