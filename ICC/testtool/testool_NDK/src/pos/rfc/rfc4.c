/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc4.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试NDK_RfidPiccApdu接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidPiccApdu"

/*------------global variables definition-----------------------*/

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
void rfc4(void)
{
	//以下为局部变量
	uchar psPicctype;
	int ret= -1, ret1= -1, ret2= -1, ret3= -1, rcvlen=0, pnDatalen=0;
	uchar psRecebuf[300]={0}, psDatabuf[256]={0};
	ushort sw=0;
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	uchar Select_1pay[]= {'\x00', '\x84', '\x00', '\x00', 8};
#else
	uchar Select_1pay[]={0x00, 0xA4, 0x04, 0x00, 0x0E, '1', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1', 0x00};//查询CPU卡的文件目录结构,mifare-1卡不支持
#endif

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case1:异常测试，错误参数     
	if((ret=NDK_RfidPiccApdu(-1, Select_1pay, &rcvlen, psRecebuf))!=NDK_ERR_PARA
	||(ret1=NDK_RfidPiccApdu(sizeof(Select_1pay), NULL, &rcvlen, psRecebuf))!=NDK_ERR_PARA
	||(ret2=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, NULL, psRecebuf))!=NDK_ERR_PARA
	||(ret3=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:"-7 未上电(寻卡激活)"
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=-7)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例测试通过!");//用以区别子case间相同的操作提示信息

	//case3:"-1 表示读写失败"
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_A)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
	cls_show_msg("寻卡成功!请将感应区的标准A卡移开,任意键继续...");
	if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=-1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//虽然iccrw_new失败,但已激活成功(底层处于激活态),故再寻卡会提示"已经激活(上电)"
	//设计变更了!(按EMV安全要求执行),故上一行文字无法测试到	
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_ERR_RFID_NOCARD)//因为底层的设计为APDU交互出错,会下电,回到初始
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//(根据EMV要求),目前这个动作已由底层完成!
	//"必须下电（清除已激活状态标志），然后再寻卡、激活。"
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:A卡APDU交互
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_A)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=NDK_OK||((sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x9000&&(sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x6d00))
	{
		send_result("line %d:%s测试失败(%d,0x%04x)", __LINE__, TESTAPI, ret, sw);
		GOTOERR;
	}
	//下面的用例需要换B卡测试，应先下电关闭场
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:B卡APDU交互
	cls_show_msg("请在感应区放置1张标准B卡,任意键继续...");
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
	if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=NDK_OK||((sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x9000&&(sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x6d00))
	{
		send_result("line %d:%s测试失败(%d,0x%04x)", __LINE__, TESTAPI, ret, sw);
		GOTOERR;
	}
	//下面的用例需要换卡测试，应先下电关闭场
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}

