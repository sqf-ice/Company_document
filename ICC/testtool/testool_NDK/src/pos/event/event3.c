/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: event模块
* file name		: event3.c
* Author 		:  
* version		: 
* DATE			
* directory 		: 
* description		: 测试NDK_SYS_ResumeEvent重启事件监听
* related document	: 高端平台统一API动态链接库编程手册V2.00.doc
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysResumeEvent"
#define MAXWAITTIME	30
#define PINTIMEOUT_MAX	(200*1000)

/*------------global variables definition-----------------------*/
static EM_SYS_EVENT flag = 0;

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static int notifyEvent2(EM_SYS_EVENT eventNum, int msgLen, char * msg)
{
	flag = eventNum;
	//cls_printf("0x%08x",eventNum);
	return SUCC;
}

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	:
* return value		 	:
* history 		 		:  author		date		remarks
*			  	  	 	sull				20180522	created
*****************************************************************/
void event3(void)
{
	/*private & local definition*/
	int ret = -1, i = 0;
	uchar szPinOut[8+1]={0};
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	if(auto_flag==1)
	{
		send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}

	//测试前置
	for(i=SYS_EVENT_MAGCARD; i<= SYS_EVENT_PRNTER; i++)
		NDK_SYS_UnRegisterEvent(i);
	NDK_MagClose();
	NDK_RfidPiccDeactivate(0);

	//case1.1:参数异常测试，应返回NDK_ERR_PARA
	//case1.2:传入错误的事件号(不存在的事件号或大于取值范围的事件号),应该返回NDK_ERR_POSNDK_EVENT_NUM
	if((ret = NDK_SysResumeEvent(SYS_EVENT_NONE-1)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_SysResumeEvent(SYS_EVENT_MAX+1)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_SysResumeEvent(0x0001001)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:流程相关的异常
	cls_printf("流程相关异常测试中...");
	//case2.1:未注册过事件，无法重启事件(未注册过事件调用重启事件都是返回-3303)
	if((ret = NDK_SysResumeEvent(SYS_EVENT_ICCARD)) != NDK_ERR_EVENT_UNREALIZED/*NDK_ERR_POSNDK_EVENT_NUM*/)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

#if MAG_ENABLE
	//case2.2:注册了一个事件，在事件还未返回时调用重启事件，会返回NDK_ERR_EVENT_UNREALIZED 
	//修改为注册事件后立即重启事件，会返回未注册事件
	flag = SYS_EVENT_NONE;
	NDK_MagOpen();
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_MAGCARD, MAXWAITTIME*1000, notifyEvent2)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SysResumeEvent(SYS_EVENT_MAGCARD)) !=NDK_ERR_EVENT_UNREALIZED/* NDK_OK*/)
	{
		send_result("line %d: %s流程异常测试失败(%d)", __LINE__, TESTAPI, ret);  
		GOTOERR;
	}
	cls_show_msg("请在30s内刷磁卡，刷完卡后按任意键继续");
	if(flag != SYS_EVENT_MAGCARD)
	{
		send_result("line %d: %s监听测卡事件失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	NDK_MagClose();
	NDK_SYS_UnRegisterEvent(SYS_EVENT_MAGCARD);
#endif

	//case2.3:多次重启事件，应返回NDK_ERR_POSNDK_BUSY，据严彬反馈不会有问题，多次重启事件均返回成功
	if(NDK_RfidInit(NULL)!=NDK_OK)
	{
		cls_show_msg("射频接口器件初始化失败,任意键退出...");
		RETURN;
	}
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	flag = SYS_EVENT_NONE;
	cls_show_msg("请在感应区放置射频A卡后按任意键继续");
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID, MAXWAITTIME*1000, notifyEvent2)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//等待事件回调完毕
	while(flag != SYS_EVENT_RFID);
	cls_show_msg("请移开射频A卡后按任意键继续");
	if((ret = NDK_SysResumeEvent(SYS_EVENT_RFID)) != NDK_OK)
	{
		send_result("line %d: %s流程异常测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SysResumeEvent(SYS_EVENT_RFID)) != NDK_OK)
	{
		send_result("line %d: %s流程异常测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID);
	//下电保护
	NDK_RfidPiccDeactivate(0);

	//case3:正常测试
	if((ret = NDK_RfidPiccType(RFCARD_TYPE_AB)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//原先在register注册事件后，回调操作完毕后调用Deactivate会再次注册事件，新固件该功能已去除
	flag = SYS_EVENT_NONE;
	cls_show_msg("请在感应区放置射频A卡后按任意键继续");
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID, 60*1000, notifyEvent2)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	while(flag != SYS_EVENT_RFID);
	flag = SYS_EVENT_NONE;
	NDK_RfidPiccDeactivate(0);
	if(flag == SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret = NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID)) != NDK_OK)
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

#if MAG_ENABLE
	//case4:测试磁卡的重启事件，重启事件仍应被监听到，注销监听后重启事件应失败，并且无法监听到事件
	//case4.1:注册mag监听事件
	NDK_MagOpen();
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_MAGCARD, 60*1000, notifyEvent2)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在30s内刷磁卡，刷卡后按任意键继续");
	if( flag != SYS_EVENT_MAGCARD)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

	//case4.2:重启mag事件仍应被监听到(根据王震懿确认，只读走磁卡数据是不会重启事件，需要关闭磁卡再开启磁卡才可以 modify 20180126)
	NDK_MagClose();
	NDK_MagOpen();
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_MAGCARD)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);//-3303 预期要成功
		GOTOERR;
	}
	cls_show_msg("请在30s内刷磁卡，刷完卡后按任意键继续");
	if( flag != SYS_EVENT_MAGCARD)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	
	//case4.3:mag事件注销后重启事件无效并且监听不到任何事件(严彬反馈说返回值要修改为-4006，注销事件之后都返回-4006) 这边返回-3303
	NDK_SYS_UnRegisterEvent(SYS_EVENT_MAGCARD);
	NDK_MagClose();
	NDK_MagOpen();
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_MAGCARD)) != NDK_ERR_EVENT_UNREALIZED/*NDK_ERR_POSNDK_EVENT_NUM*/)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在30s内刷磁卡，刷卡后按任意键继续");
	if(flag == SYS_EVENT_MAGCARD)
	{
		send_result("line %d: %s测试失败(注销事件后仍可以监听到事件)(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	NDK_MagClose();
#endif

#if IC1_ENABLE
	//case5:测试IC卡的重启事件，重启事件后应可监听到事件发生，注销监听后重启事件应失败，并且无法监听到事件	
	//case5.1:注册IC监听事件
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD, MAXWAITTIME*1000, notifyEvent2)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在30秒内插入IC卡后按任意键继续");
	if(flag == SYS_EVENT_ICCARD)
		cls_show_msg("监听到IC卡插入，请拔出IC卡，完成后按任意键继续");

	//case5.2:重启IC卡事件后仍应被监听到
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_ICCARD)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret); //-3303 预期要成功
		GOTOERR;
	}
	cls_show_msg("请在30秒内插入IC卡后按任意键继续");
	if(flag != SYS_EVENT_ICCARD)
	{
		send_result("line %d: %s测试失败(重启事件未监听到IC卡事件)(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

	//case5.3:IC注销后重启事件应无效并且监听不到任何事件
	NDK_SYS_UnRegisterEvent(SYS_EVENT_ICCARD);
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_ICCARD)) != NDK_ERR_EVENT_UNREALIZED/*NDK_ERR_POSNDK_EVENT_NUM*/)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret); //-3303  返回值不同
		GOTOERR;
	}
	cls_show_msg("请在30秒内插入IC卡后按任意键继续");
	if(flag == SYS_EVENT_ICCARD)
	{
		send_result("line %d: %s测试失败(注销事件后仍可以监听到事件)(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
#endif

	//case6:测试非接卡的重启事件，重启事件后应可监听到事件发生，注销监听后重启事件应失败，并且无法监听到事件
	//case6.1:注册非接卡事件
	if((ret = NDK_RfidPiccType(RFCARD_TYPE_AB)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID, 60*1000, notifyEvent2)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在30秒内放置射频A卡后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

	//case6.2:重启事件，应可监听到非接卡放置
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_RFID)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在30秒内放置射频A卡后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(重启事件未监听到非接卡放置)(0x%08x)", __LINE__, TESTAPI,flag);
		GOTOERR;
	}

	//case6.3:注销非接卡事件，注销后重启事件无效并且监听不到事件发生
	NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID);
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_RFID)) != NDK_ERR_EVENT_UNREALIZED/*NDK_ERR_POSNDK_EVENT_NUM*/)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);//-3303 返回值不同
		GOTOERR;
	}
	cls_show_msg("请在30秒内放置射频A卡后按任意键继续");
	if(flag == SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(注销事件后仍可以监听到非接事件)(0x%08x))", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

	//case7:测试PIN输入事件，重启事件后应可监听到事件发生，注销监听后重启事件应失败，并且无法监听到事件
	//前置://安装TPK1(16bytes), ID=1 明文安装
	NDK_SecKeyErase();
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x19, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//初始化
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case7.1:注册PIN事件
	flag = SYS_EVENT_NONE;
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_PIN, 120*100/*MAXWAITTIME*1000*/, notifyEvent2))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("请尽快按确认...");//阻塞式可以用,非阻塞式也可以用 
	if((ret=NDK_SecGetPin(1, (uchar *)"0", NULL, szPinOut, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);  
		GOTOERR;
	}
	NDK_SysDelay(5);//延时0.5s,可能底层处理需要点时间
	if(flag != SYS_EVENT_PIN)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	

	//case7.2:重启PIN事件，仍可以监听到PIN事件
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	} 
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_PIN)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); //-3303 预期要成功
		GOTOERR;
	}
	cls_printf("请尽快再次按确认...");//这里使用非阻塞式改成使用阻塞式(关注点是事件机制而不是getpin使用阻塞还是非阻塞)
	if((ret=NDK_SecGetPin(1, (uchar *)"0", NULL, szPinOut, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	NDK_SysDelay(5);
	if(flag != SYS_EVENT_PIN)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

	//case7.3:PIN事件注销，重启事件无效并且监听不到事件触发 
	NDK_SYS_UnRegisterEvent(SYS_EVENT_PIN);
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_PIN)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); //-3303 预期要成功
		GOTOERR;
	}
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_printf("请尽快按确认...");//阻塞式可以用,非阻塞式也可以用 
	if((ret=NDK_SecGetPin(1, (uchar *)"0", NULL, szPinOut, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(5);
	if(flag == SYS_EVENT_PIN)
	{
		send_result("line %d: %s测试失败(注销事件后仍可以监听到PIN事件)(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

#if PRN_ENABLE  //N700不支持打印模块
	//case8:测试打印事件，重启事件后应可监听到事件发生，注销监听后重启事件应失败，并且无法监听到事件
	//cls_show_msg("请确保有纸后按任意键继续...");
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		NDK_PrnInit(g_PrnSwitch);
		GOTOERR;
	}
	cls_show_msg("请确保装不足10cm的打印纸后按任意键继续...");
	//case8.1:注册打印事件
	flag = SYS_EVENT_NONE;
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_PRNTER, MAXWAITTIME*1000, notifyEvent2))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}		
	NDK_PrnStr("打印新大陆NEWLAND打印测试页1:将打印1---80行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
		11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
		31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n\
		51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n\
		71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n");
	if(flag == SYS_EVENT_PRNTER)
		cls_show_msg("监听到打印机状态变化，请确保装不足10cm的打印纸后按任意键继续");
	else
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	
	//case8.2:重启打印事件
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_PRNTER)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret); //-3303 预期要成功
		GOTOERR;
	}
	NDK_PrnStr("打印新大陆NEWLAND打印测试页1:将打印1---80行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
		11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
		31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n\
		51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n\
		71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n");
	if(flag != SYS_EVENT_PRNTER)
	{
		send_result("line %d: %s测试失败(重启事件未监听到打印状态变化)(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

	//case8.3:注销打印事件后，重启打印事件应无效并且监听不到事件触发
	NDK_SYS_UnRegisterEvent(SYS_EVENT_PRNTER);
	flag = SYS_EVENT_NONE;
	cls_show_msg("请确保装不足10cm的打印纸后按任意键继续...");
	if((ret = NDK_SysResumeEvent(SYS_EVENT_PRNTER)) != NDK_ERR_EVENT_UNREALIZED/*NDK_ERR_POSNDK_EVENT_NUM*/)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret); //-3303 返回值不同
		GOTOERR;
	}
	NDK_PrnStr("打印新大陆NEWLAND打印测试页1:将打印1---80行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
		11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
		31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n\
		51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n\
		71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n");
	if(flag != SYS_EVENT_NONE)
	{
		send_result("line %d: %s测试失败(注销事件后仍可以监听到打印事件)(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
#endif
/**
  * 注册事件的时间以及重启事件的时间测试(将单元测试与时间测试整合到同一函数中)
  */
	//测试前置:注销所有事件
	for(i=SYS_EVENT_MAGCARD; i<= SYS_EVENT_PRNTER; i++)
		NDK_SYS_UnRegisterEvent(i);

	//case9:射频卡的重启事件时间的测试
	//case9.1:射频卡的重启事件后超时未触发事件，应超时
	cls_printf("测试非接卡重启事件后超时未触发事件...请等待25s...");
	NDK_RfidInit(NULL);
	NDK_RfidPiccType(RFCARD_TYPE_AB);
	flag = SYS_EVENT_NONE;
	NDK_SYS_RegisterEvent(SYS_EVENT_RFID, 60*1000, notifyEvent2);
	NDK_SysMsDelay(25*1000); //延时25s
	cls_show_msg("请在5s内放置A卡,放置完毕后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d:测试失败(未监听到非接卡放置事件)(0x%08x)", __LINE__, flag);
		GOTOERR;
	}
	else
		cls_show_msg("已检测到非接卡放置,请移开非接卡后按任意键继续");
	//该case暂时有问题，开发还未修复
	flag = SYS_EVENT_NONE;
	if((ret = NDK_SysResumeEvent(SYS_EVENT_RFID)) != NDK_OK)
	{
		send_result("line %d:%s重启非接卡事件失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("将延时35s,35s后关注屏幕信息...");
	NDK_SysMsDelay(35*1000); //延时35s应超时
	cls_show_msg("请在2s内放置射频A卡,放置完毕后按任意键继续");
	if(flag == SYS_EVENT_RFID)
	{
		send_result("line %d:%s测试失败(重启事件超时后监听到非接卡放置事件)(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	else
		cls_show_msg("请移开非接卡后按任意键继续");

	//case9.2:射频卡的重启事件后在最后几s触发事件，应能正常接收事件
	cls_printf("测试非接卡重启事件在最后5s触发...请耐心等待25s,25s后关注屏幕信息...");
	flag = SYS_EVENT_NONE;
	NDK_SYS_RegisterEvent(SYS_EVENT_RFID, MAXWAITTIME*1000, notifyEvent2);
	if((ret = NDK_SysResumeEvent(SYS_EVENT_RFID)) != NDK_OK)
	{
		send_result("line %d:%s重启非接卡事件失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysMsDelay(25*1000); //延时25s应超时
	cls_show_msg("请在5s内放置A卡,放置完毕后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d:测试失败(重启事件超时未监听到非接卡放置事件)(0x%08x)", __LINE__, flag);
		goto ERR;
	}
	NDK_RfidPiccDeactivate(0);

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_MagClose();
	NDK_RfidPiccDeactivate(0);
	for(i= SYS_EVENT_MAGCARD; i<=SYS_EVENT_PRNTER; i++)
		NDK_SYS_UnRegisterEvent(i);
	return;
}
  

