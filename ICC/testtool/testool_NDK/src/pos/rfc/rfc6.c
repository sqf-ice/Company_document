/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfid模块
* file name		: rfc6.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试NDK_SetIgnoreProtocol,NDK_GetIgnoreProtocol接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SetIgnoreProtocol,NDK_GetIgnoreProtocol"

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
void rfc6(void)
{
	//以下为局部变量
	uchar psPicctype, psDatabuf[256]={0}, psRecebuf[256]={0};
	int ret= -1, ret1= -1, loop=0, pnDatalen=0, rcvlen=0;
	ushort sw=0;
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	uchar Select_1pay[]= {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
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

	//case1:异常测试    
	if((ret=NDK_GetIgnoreProtocol(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:设置为屏蔽判断,对标/非标卡进行测试,两种卡均应操作成功
	//先连续两次设置非屏蔽(0),读取应该获取到非屏蔽(0)  20140603 added by jiangym
	ret1= -1;
	if((ret=NDK_SetIgnoreProtocol(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SetIgnoreProtocol(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_GetIgnoreProtocol(&ret1))!=NDK_OK||ret1!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret);
		GOTOERR;
	}
	//再设置屏蔽(非0),读取应该是屏蔽(非0),不需要设置两次 20140603 added by jiangym
	if((ret=NDK_SetIgnoreProtocol(rand()%MAX_SIGNINT+1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_GetIgnoreProtocol(&ret1))!=NDK_OK||ret1==0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret);
		GOTOERR;
	}
	//测试设置
	for(loop=0;loop<4;loop++)
	{
		if(loop<2)//标准卡的测试
			cls_show_msg("请在感应区放置1张标准%c卡,任意键继续...", (loop%2==0)?'A':'B');
		else//非标卡的测试//由于没有非标测试卡,非标A或B卡可不测试(若此子用例使用M-1模拟非标A卡,则在iccrw_new时可能会报错)
		{
			if(ESC==cls_show_msg("请在感应区放置1张非标准%c卡,[ESC]略过[其它]继续", (loop%2==0)?'A':'B'))
				continue;
		}
		if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=((loop%2==0)?RFCARD_TYPE_A:RFCARD_TYPE_B))
		{
			send_result("line %d:%s测试失败(%d, 0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=NDK_OK||((sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x9000&&(sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x6d00))
		{
			send_result("line %d:%s测试失败(%d, 0x%04x)", __LINE__, TESTAPI, ret, sw);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case2:设置为保持判断,对标/非标卡进行测试,标卡可以正常操作,非标卡会出错
	//设置
	ret1= -1;
	if((ret=NDK_SetIgnoreProtocol(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_GetIgnoreProtocol(&ret1))!=NDK_OK||ret1!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}
	//测试设置
	//标准卡的测试
	for(loop=0;loop<2;loop++)
	{
		cls_show_msg("请在感应区放置1张标准%c卡,任意键继续...", loop==0?'A':'B');
		if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(0x%02x)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=(loop==0?RFCARD_TYPE_A:RFCARD_TYPE_B))
		{
			send_result("line %d:%s测试失败(%d, 0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=NDK_OK||((sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x9000&&(sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x6d00))
		{
			send_result("line %d:%s测试失败(%d, 0x%04x)", __LINE__, TESTAPI, ret, sw);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	//非标卡的测试
	//由于没有非标测试卡,测试方案可调整为使用M-1模拟非标A卡,非标B卡不测试
	for(loop=0;loop<2;loop++)
	{
		if(ESC==cls_show_msg("请在感应区放置1张非标准%c卡,[ESC]略过[其它]继续", loop==0?'A':'B'))
			continue;
		//非标(A或B)卡在寻卡或在上电激活时返回失败都可以
		//以下判定条件有所放宽,事实上对于B卡应在寻卡时返回失败,但考虑到非标卡的多样性,还是放宽判定条件
		if((ret=NDK_RfidPiccDetect(&psPicctype))==NDK_OK)//若寻卡返回0x00,则在上电激活时一定要检测到多张A卡?
		{
			if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_PROTOCOL)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
		}
		else if(ret!=NDK_ERR_RFID_PROTOCOL)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}	

	//测试结束
	send_result("%s测试通过", TESTAPI);
	goto ERR;
ERR:
	NDK_SetIgnoreProtocol(0);//用例测试毕,恢复默认//保持协议判断
	NDK_RfidPiccDeactivate(0);
	return;
}

