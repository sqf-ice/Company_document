/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc6.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmHangup 能否断开MODEM的同步通讯链路，即SDLC挂机操作
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmHangup"
#define	MAXWAITTIME 60
#define PCKTMAXLEN 350

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
//自定义的拨号连接函数，由于linkup使用linkdown 来关闭通讯，如果使用NDK_MdmHangup会出错。
static int SdlcModemLink(void)
{
	int ret =0 ;
	
	if((ret=NDK_MdmSdlcInit(MDMPatchType))!=SUCC)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号(%s)失败(ret=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		goto END;
	}
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s未能接通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		goto END;
	}
	return SUCC;
END:
	NDK_MdmHangup();
	return FAIL;
}
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	chenjings      20130108	created
*****************************************************************/
void sdlc6(void)
{
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};// 1K是文档推荐的上限,2K是最大上限
	EM_MDMSTATUS MdmStatus;	//mdm状态

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
	//case1,未初始化情况
	if((ret=NDK_MdmHangup())!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s未初始化情况返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	//case2:正常读写后挂断
	//测试前置
	if(SdlcModemLink()!=SUCC)
	{
		send_result("line %d:%s创建链路失败", __LINE__,  TESTAPI);
		RETURN;
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
		RETURN;
	}

	//case3:挂断成功后判断MDM状态应该是挂断状态,或者未拨号状态
	sleep(5);
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_NOPREDIAL))
	{
		send_result("line %d:%s猫异常状态(ret=%d,status=%d)", __LINE__, TESTAPI, ret, MdmStatus);
		goto ERR;
	}
	
	//测试结束
	NDK_MdmHangup();
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_MdmHangup();
	return;
}

