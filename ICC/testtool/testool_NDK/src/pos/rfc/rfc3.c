/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc3.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试NDK_RfidPiccActivate,NDK_RfidPiccDeactivate,NDK_RfidPiccState接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidPiccActivate,NDK_RfidPiccDeactivate"

//,NDK_RfidPiccState已不使用,永远返回OK,故不测试

/*------------global variables definition-----------------------*/
#define WUCHASEC 0.03
/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		    chenfm	        20130108	 	created
*****************************************************************/
void rfc3(void)
{
	//以下为局部变量
	uchar psPicctype=0x00, psDatabuf[256]={0}; 
	int ret= -1, ret1= -1, ret2= -1, pnDatalen = 0;
	float tmp = 0.0,unTime = 0.0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case9:异常测试，错误参数	     
	if((ret=NDK_RfidPiccActivate(NULL, &pnDatalen, psDatabuf))!=NDK_ERR_PARA
	||(ret1=NDK_RfidPiccActivate(&psPicctype, NULL, psDatabuf))!=NDK_ERR_PARA
	||(ret2=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2);
		GOTOERR;
	}

	//子用例间只有在操作动作不同时(如,需要放不一样的卡),才需要提示一下
	//case1:"激活(上电)前未寻卡"
	//寻卡成功,激活失败(泛指所有不成功的情况),应再寻卡(底层会有FSM来控制!)
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请将感应区的标准A卡移开,任意键继续...");
	ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf);
	if(ret!=NDK_ERR_RFID_NOCARD)//&& state!=0x0a)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_NOTDETE)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
	
	//cls_show_msg1(2, "子用例测试通过!");//用以区别子case间相同的操作提示信息

	//case2:"不支持ISO14443-4协议的卡"
	cls_show_msg("请在感应区放置MIfare-1卡,任意键继续...");//MIfare-1卡只有A卡一种类型
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_PROTOCOL)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:"A卡冲突(多张卡存在)"
	//在寻卡或在上电激活时检测到0x03都可以
	//事实上对于A卡冲突而言,多数情况下,应是寻卡OK,在上电激活时检测到冲突
	//但在测试中发现有些厂商的卡会在寻卡时就提前返回检测到冲突了
	//对于B卡冲突而言,则基本上应在寻卡阶段就返回检测到冲突了
	cls_show_msg("请在感应区放置多张标准A卡,任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))==NDK_OK)//若寻卡返回0x00,则在上电激活时一定要检测到多张A卡
	{
		if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_AANTI)
		{
			send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
			GOTOERR;
		}
	}
	else if(ret!=NDK_ERR_RFID_AANTI)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_RfidPiccDeactivate(0);//增加保护动作防止成功寻到一张卡情况下后续用例返回已激活
	cls_show_msg1(2, "子用例测试通过!");

	//case4:激活A卡
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
#if 0
	if((ret=NDK_RfidPiccState())!=NDK_ERR_RFID_NOTACTIV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}	
#endif
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_A)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
#if 0
	if((ret=NDK_RfidPiccState())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
#endif
	//因为后面的用例需要换卡,所以要先下电.即处于激活态(该状态表明可以进行读写)后,
	//只能通过下电回到初始态.否则寻卡,激活都不会成功.
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0
	if((ret=NDK_RfidPiccState())!=NDK_ERR_RFID_NOTACTIV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
#endif
	//cls_show_msg1(2, "子用例测试通过!");//子case间相同的操作提示信息不同,不必提示

	//case5:激活B卡
	cls_show_msg("请在感应区放置1张标准B卡,任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
#if 0
	if((ret=NDK_RfidPiccState())!=NDK_ERR_RFID_NOTACTIV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}	
#endif
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_B)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
#if 0
	if((ret=NDK_RfidPiccState())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}	
#endif
	//cls_show_msg1(2, "子用例测试通过!");

	//case6:"已经激活(上电)"//激活后再寻卡，或者激活后再激活
	//cls_show_msg("请在感应区放置1张B卡,任意键继续...");
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_UPED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_ERR_RFID_UPED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:以RFCARD+1方式关闭天线输出场强,不过RFCARD+1参数的优势何在呢
	//RFCARD+1:关闭6ms后会再度打开,但可能可以加快再次寻卡时的速度
#if !defined ME15B
	lib_StartStopwatch();
	unTime = 0;
#else
	unTime = lib_ReadStopwatch();
#endif
	if((ret=NDK_RfidPiccDeactivate(6))!=NDK_OK||((tmp=(lib_ReadStopwatch()-unTime-0.006))>WUCHASEC))
	{
		send_result("line %d:%s测试失败(%d,%f)", __LINE__, TESTAPI, ret, tmp);
		GOTOERR;
	}
#if 0
	if((ret=NDK_RfidPiccState())!=NDK_ERR_RFID_NOTACTIV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
#endif
	cls_show_msg1(2, "子用例测试通过!");

	//case8:以RFCARD方式关闭天线输出场强(完全关闭)
	//cls_show_msg("请在感应区放置1张B卡,任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_B)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	ERR:NDK_RfidPiccDeactivate(0);
	return;
}

