/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc3.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmCheck 能否正确检测检测modem状态
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmCheck"
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
void sdlc3(void)
{
	//以下为局部变量
	int ret = -1, j = 0;
	EM_MDMSTATUS MdmStatus;	//mdm状态
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};

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
	//case1 : 未初始化失败情况
	if((ret=NDK_MdmCheck(&MdmStatus))!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s未初始化情况返回错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	//case2:测试modem为-11 MDMSTATUS_NOPREDIAL (并未拨号,或SDLC链接已终止,期待再次拨号)状态的情况
	if((ret=NDK_MdmSdlcInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	sleep(5);
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_NOPREDIAL))//未能正确获取modem状态
	{
		send_result("line %d:%s检测到异常状态(ret=%d,status=%d)", __LINE__, TESTAPI, ret, MdmStatus);
		RETURN;
	}
	
	//case3:测试modem为-4 MDMSTATUS_MS_BUSY
	//拨号
	if((ret=NDK_MdmDial("123456"))!=NDK_OK)//case2.1: 判断拨号是否成功
	{
		send_result("line %d:%s拨号失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//判断状态
	lib_StartStopwatch();
	while(1)
	{									
		if((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)//未能正确获取modem状态
		{
			send_result("line %d:%s检测状态失败(ret=%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		else if(MdmStatus!=MDMSTATUS_NORETURN_AFTERPREDIAL)// 判断modem状态是否改变
		{
			if((MdmStatus==MDMSTATUS_MS_BUSY )){// 判断modem状态是否为忙音
				NDK_MdmHangup();   
				break;
			}
			else {
				send_result("line %d:%s猫状态异常(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, MdmStatus);
				goto ERR;
			}
		}
		else if(lib_ReadStopwatch()>MAXWAITTIME)//60 s 超时未检测到拨通
		{
			send_result("line %d:%s未能拨通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, MdmStatus);
			goto ERR;
		}									
	}		

	//case4:测试modem为2 MDMSTATUS_CONNECT_AFTERPREDIAL
	//拨号
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号失败(DialNum=%s,ret=%d)", __LINE__, TESTAPI, MDMDialStr ,ret);
		GOTOERR;
	}					
	//判断modem状态
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s未能接通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}

	//读写后在判断状态是否正常
	//初始化缓冲区
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;	
	update_SDLC_packet(buf, sizeof(buf)); //添加TPDU包头
	//写
	if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_OK)
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
	//判断状态
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_CONNECT_AFTERPREDIAL ))
	{
		send_result("line %d:%s猫状态异常(ret=%d,status=%d)", __LINE__, TESTAPI, ret, MdmStatus);
		GOTOERR;
	}
		
	//case5:挂机后取状态为MDMSTATUS_NOPREDIAL (-11)
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s挂起失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(5);
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_NOPREDIAL))
	{
		send_result("line %d:%s猫异常状态(ret=%d,status=%d)", __LINE__, TESTAPI, ret, MdmStatus);
		GOTOERR;
	}
	
	//case6: 错误参数NULL
	if((ret=NDK_MdmCheck(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//MDMSTATUS_OK_AFTERPREDIAL 
	//MDMSTATUS_MS_NODIALTONE  
	//MDMSTATUS_MS_ERROR  
	//MDMSTATUS_MS_NOCARRIER
	//MDMSTATUS_LINE_VOL_TOO_LOW
	
	//测试结束
	NDK_MdmHangup();
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_MdmHangup();
	return;


}

