/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc3.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/asyn
* description		: 测试NDK_MdmCheck 能否正确检测检测modem状态
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmCheck"
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
*			  	  	chenjings      20130108	created
*****************************************************************/
void asyn3(void)
{
	//以下为局部变量
	int ret = -1, j = 0;
	EM_MDMSTATUS MdmStatus;	//mdm状态
	char buf[512] = {0}, rbuf[512] = {0};
	uint rlen = 0;

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
	//case2:测试modem为-11 MDMSTATUS_NOPREDIAL 
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_NOPREDIAL))//未能正确获取modem状态
	{
		send_result("line %d:检测到异常状态(ret=%d,status=%d)", __LINE__, ret, MdmStatus);
		RETURN;
	}
	
	//case3:测试modem为-3 MDMSTATUS_MS_NOCARRIER，该状态20140119修改后无法测试
	//拨号
	if((ret=NDK_MdmDial("123456"))!=NDK_OK)//case2.1: 判断拨号是否成功
	{
		send_result("line %d:%s拨号失败(ret%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//判断状态
#if 1//NDK_MdmCheck函数在异步中使用时由 阻塞改为非阻塞，相应修改应用使用方式 linwl 20140618 
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
			if((MdmStatus==MDMSTATUS_MS_BUSY/*MDMSTATUS_MS_NOCARRIER*/))//20140119 景水解释由于修改了初始化里的国家设置引起该用例的状态变更
			{// 判断modem状态是否为忙音
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
#else
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_MS_BUSY/*MDMSTATUS_MS_NOCARRIER*/))//20140119 景水解释由于修改了初始化里的国家设置引起该用例的状态变更
	{
		send_result("line %d:检测到异常状态(ret=%d,status=%d)", __LINE__, ret, MdmStatus);
		NDK_MdmHangup();  
		RETURN;
	}
	NDK_MdmHangup();
#endif	
	//case4:测试modem为2 MDMSTATUS_CONNECT_AFTERPREDIAL
	//拨号
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号失败(DialNum=%s,ret=%d)", __LINE__, TESTAPI, MDMDialStr ,ret);
		GOTOERR;
	}
	//case5:第一次调用应该马上返回未拨上的状态，且返回速度比较快(0.3秒之内)就能说明函数是非阻塞式的 linwl added 20140618
	lib_StartStopwatch();
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_NORETURN_AFTERPREDIAL)||(lib_ReadStopwatch()>0.3))//0.3待定
	{
		send_result("line %d:检测到异常状态(ret=%d,status=%d)", __LINE__, ret, MdmStatus);
		GOTOERR;
	}
	//判断modem状态
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s未能接通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	
	//case5:正常读写后，判断状态是否正常
	//初始化缓冲区
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
	//判断状态仍然为连接状态
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_CONNECT_AFTERPREDIAL ))
	{
		send_result("line %d:%s猫状态异常(ret=%d,status=%d)", __LINE__, TESTAPI, ret, MdmStatus);
		GOTOERR;
	}
		
	//case6:挂机后取状态为MDMSTATUS_NOPREDIAL (-11)
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s挂起失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_NOPREDIAL))
	{
		send_result("line %d:%s猫异常状态(ret=%d,status=%d)", __LINE__, TESTAPI, ret, MdmStatus);
		GOTOERR;
	}
	
	//case7: 错误参数NULL
	if((ret=NDK_MdmCheck(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s返回值错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//MDMSTATUS_NORETURN_AFTERPREDIAL
	//MDMSTATUS_OK_AFTERPREDIAL 
	//MDMSTATUS_MS_NODIALTONE  
	//MDMSTATUS_MS_ERROR  
	//MDMSTATUS_MS_BUSY
	//MDMSTATUS_LINE_VOL_TOO_LOW
	
	//测试结束
	NDK_MdmHangup();
	send_result("%s测试通过", TESTAPI);
	return;
	goto ERR;
ERR:
	NDK_MdmHangup();
	return;
}

