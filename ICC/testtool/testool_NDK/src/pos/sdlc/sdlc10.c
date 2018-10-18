/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc1.c
* Author 			: chenjings
* version			: 
* DATE			: 20130106
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmReset能否对有线MODEM进行有效复位
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmReset"
#define	MAXWAITTIME	60	//60秒
#define	PCKTMAXLEN	350

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
void sdlc10(void)
{
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};
	EM_MDMSTATUS MdmStatus;	//mdm状态
	
	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); 
	
	//case1 : 验证复位后MODEM处于未初始化状态,检测状态应该失败
	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_MdmCheck(&MdmStatus))!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s未初始化情况返回错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:验证拨号连接后复位,应该成功,且复位后操作应该失败
	if((ret=NDK_MdmSdlcInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号(%s)失败(ret=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s未能接通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	//复位
	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MdmCheck(&MdmStatus))!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s未初始化情况返回错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MdmWrite(buf, PCKTMAXLEN))!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//读                                     		
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:验证case2中的复位操作不影响后续MDM的正常使用
	if((ret=NDK_MdmSdlcInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号(%s)失败(ret=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s未能接通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	//初始化发送缓冲区数据
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
	update_SDLC_packet(buf, sizeof(buf)); //添加TPDU包头		
	//写
	if((ret=NDK_MdmWrite(buf, PCKTMAXLEN))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//读                                     		
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较
	if((PCKTMAXLEN!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, PCKTMAXLEN, rlen);
		GOTOERR;
	}
	//挂起
	if(NDK_MdmHangup()!=NDK_OK)
	{
		send_result("line %d:%s挂起测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_MdmHangup();
	return;
}

