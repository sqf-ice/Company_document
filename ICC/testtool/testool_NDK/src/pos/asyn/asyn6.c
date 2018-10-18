/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: asyn模块
* file name		: asyn6.c
* Author 			: chenjings
* version			: 
* DATE			: 20130114
* directory 		: src/pos/asyn
* description		: 测试NDK_MdmHangup 能否断开MODEM的异步通讯链路，即asyn挂机操作
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmHangup"
#define	MAXWAITTIME 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
//自定义的拨号连接函数，由于linkup使用linkdown 来关闭通讯，如果使用NDK_MdmHangup会出错。
static int AsynModemLink(void)
{
	int ret =0 ;
	
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=SUCC)
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
void asyn6(void)
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
	if(AsynModemLink()!=SUCC)
	{
		send_result("line %d:%s创建链路失败", __LINE__,  TESTAPI);
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
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, sizeof(buf), rlen);
		GOTOERR;
	}
	
	//挂起
	if(NDK_MdmHangup()!=NDK_OK)
	{
		send_result("line %d:%s挂起测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:挂断之后，MODEM状态应该是未连接状态
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_NOPREDIAL))
	{
		send_result("line %d:%s猫异常状态(ret=%d,status=%d)", __LINE__, TESTAPI, ret, MdmStatus);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_MdmHangup();
	return;
}

