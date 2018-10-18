/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: event模块
* file name		: event4.c
* Author 		:  
* version		: 
* DATE			: 20180521
* directory 		: 
* description		: 测试复位卡片(NDK_RfidPiccResetCard(uchar usDelayms)):传入0为关闭场强，传入6-10为合法值
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
#define TESTAPI	"NDK_RfidPiccResetCard"
#define MAXWAITTIME	30
#define PINTIMEOUT_MAX	(200*1000)

/*------------global variables definition-----------------------*/
static EM_SYS_EVENT flag = 0;

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static int notifyEvent(EM_SYS_EVENT eventNum, int msgLen, char * msg)
{
	flag = eventNum;
	NDK_SYS_UnRegisterEvent(eventNum);

	return SUCC;
}
static int notifyEvent_noexit(EM_SYS_EVENT eventNum, int msgLen, char * msg)
{
	flag = eventNum;
	//cls_printf("0x%08x",eventNum);
	return SUCC;
}

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*			  	  	 	jiangym		20180521 	created
*****************************************************************/
void event4(void)
{
	if(auto_flag==1)
	{
		send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1, i = 0, pnDatalen=0, rcvlen = 0;
	int errpara[] = {0,-1,11,8 };
	uchar psPicctype;
	uchar psRecebuf[300]={0}, psDatabuf[256]={0}, UID[LEN_UID]={0}, psSakbuf[256]={0}, psBlockdata[MAXLEN_DATA]={0};	
	uchar Select_1pay[]= {'\x00', '\x84', '\x00', '\x00', 8};

	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	//测试前置:
	for(i=SYS_EVENT_MAGCARD;i<=SYS_EVENT_PRNTER;i++)
		NDK_SYS_UnRegisterEvent(i);
	NDK_RfidPiccDeactivate(0);//射频卡下电

	//case1:参数异常测试(类似NDK_RfidPiccDeactivate 函数)
	// 传入非法参数以及6-10范围内的参数，以及0，据严彬反馈会修改为正确的值，传入的值<6时，修改值为6，传入的值>10时，修改为10
	if(NDK_RfidInit(NULL)!=NDK_OK)
	{
		cls_show_msg("射频接口器件初始化失败,任意键退出...");
		RETURN;
	}
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for(i=0;i<4;i++)
	{	
		if((ret=NDK_RfidPiccResetCard(errpara[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	// 下电后进行APDU操作应失败，返回-7，此为驱动底下的返回值
	if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=-7)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_RfidPiccDeactivate(0);//保护作用

	//case2:非接卡未上电前进行[复位卡片]应返回成功(参数传入0与非0)
	if((ret=NDK_RfidPiccResetCard(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccResetCard(100))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:非接卡上电后再进行[复位卡片]，此时进行APDU操作应失败
	for(i=0;i<2;i++) 
	{
		cls_show_msg("请在感应区放置1张标准%s卡,任意键继续",i==0?"A":"B");
		if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}	
		if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=(i==0?RFCARD_TYPE_A:RFCARD_TYPE_B))
		{
			send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccResetCard(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))==NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		NDK_RfidPiccDeactivate(0);//保护作用
	}
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	cls_show_msg("请在感应区放置一张Mifare-1卡,任意键继续...");
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_M1Anti(&pnDatalen, UID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_M1Select(pnDatalen, UID, psSakbuf))!=NDK_OK)||((psSakbuf[0]&0x20)!=0x00))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_M1ExternalAuthen(pnDatalen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x01))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_RfidPiccResetCard(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Read(0x02, &pnDatalen, psBlockdata))==NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_RfidPiccDeactivate(0);//保护作用
	
	// case4.1:事件机制注册前进行[复位卡片]操作，不影响事件机制流程
	if((ret=NDK_RfidPiccResetCard(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_RFID, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒内在感应区放置A卡后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	cls_show_msg("移走A卡后任意键继续");

	// case4.2:事件机制注册后进行[复位卡片]操作，不影响事件机制流程(JNI_Rfid_PiccResetCard下电后要重启事件才可以监听到非接卡事件)
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_RFID, MAXWAITTIME*1000, notifyEvent_noexit))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccResetCard(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//重启事件
	if((ret=NDK_SysResumeEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒内在感应区放置A卡后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	cls_show_msg("移走A卡后任意键继续");
	
	// case5:监听到事件后进行[复位卡片]操作应正常
	if((ret=NDK_RfidPiccResetCard(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	// case6.1:重启事件前进行[复位卡片]操作应正常，应可正常监听到事件
	if((ret=NDK_SysResumeEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒内在感应区放置A卡后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	cls_show_msg("移走A卡后任意键继续");

	// case6.2:重启事件后进行[复位卡片]操作应正常，应可正常监听到事件(非接卡下电之后应无法监听到射频事件，不影响其他事件的监听)
	if((ret=NDK_SysResumeEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccResetCard(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒内在感应区放置A卡后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	cls_show_msg("移走A卡后任意键继续");

	// case7:注册其他事件后进行[复位卡片]操作，不影响其他事件监听(例如IC卡的监听)
#if IC1_ENABLE
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccResetCard(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒插入IC卡后按任意键继续");
	if(flag != SYS_EVENT_ICCARD)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		goto ERR;
	}	
#endif
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);//射频卡下电
	for(i=SYS_EVENT_MAGCARD;i<=SYS_EVENT_PRNTER;i++)
		NDK_SYS_UnRegisterEvent(i);	
	return;	
}

