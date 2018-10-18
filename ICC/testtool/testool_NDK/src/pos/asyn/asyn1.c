/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: asyn模块
* file name		: asyn1.c
* Author 			: chenjings
* version			: 
* DATE			: 20130106
* directory 		: src/pos/asyn
* description		: 测试NDK_MdmAsynInit 能否对有线MODEM进行异步方式的初始化
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmAsynInit"
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
*			  	  	chenjings      20130106	created
*****************************************************************/
void asyn1(void)

{
	//以下为局部变量
	int i = -1, j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};
	
	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); 

	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case1: 测试i为任意值都应该成功	
	i = rand()%256;     
	if((ret = NDK_MdmAsynInit(i))!=NDK_OK)
	{
		send_result("line %d:%s失败(i=%d,ret=%d)", __LINE__, TESTAPI, i, ret);
		RETURN;
	}
	//拨号
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号失败(DialNum=%s,ret=%d)", __LINE__, TESTAPI,  MDMDialStr,ret);
		GOTOERR;
	}
	//判断modem状态，是否连接成功
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s modem状态未接通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
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
		send_result("line %d:%s 写数据失败(ret=%d)", __LINE__, TESTAPI,  ret);
		GOTOERR;
	}
	//读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(buf)+1;//期望读取到的数据长度要大于等于实际能读取到的数据长度。
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s 读数据失败(ret=%d)", __LINE__, TESTAPI,  ret);
		GOTOERR;
	}
	//比较数据		
	if((sizeof(buf)!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, sizeof(buf), rlen);
		GOTOERR;
	}
	//挂机
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s挂断失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
	goto ERR;
ERR:
	NDK_MdmHangup();
	return;
}

