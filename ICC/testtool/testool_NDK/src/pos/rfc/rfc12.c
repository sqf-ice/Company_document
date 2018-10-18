/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc12.c
* Author 			:  jiangym
* version			: 
* DATE				:  20140811
* directory 			: 
* description			: 测试NDK_RfidTypeARats接口能否在M1卡寻卡防冲突和选卡的基础上激活A卡进行读写操作
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidTypeARats"

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
*			  		       jiangym	        20140811	 	created
*****************************************************************/
void rfc12(void)
{
	//以下为局部变量
	int ret=0, pnDatalen=0, rcvlen=0;
	uchar psRecebuf[256]={0}, psDatabuf[256]={0}, UID[LEN_UID]={0}, psSakbuf[10]={0};
	ushort sw=0;
	uchar Select_1pay[]= {'\x00', '\x84', '\x00', '\x00', 8};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);

	//case1:正常寻卡防冲突选卡前的调用,应该返回失败
	if((ret=NDK_RfidTypeARats(0,&pnDatalen, psDatabuf))!=NDK_ERR_MI_WriteFifo)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//正常M1卡流程
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在感应区放置一张标准A卡,任意键继续...");
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
	if((ret=NDK_M1Select(pnDatalen, UID, psSakbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//根据pssakbuf 值判断是A卡还是M卡若是A卡执行获取A卡的ATS
	if((psSakbuf[0]&0x20)!=0x20)//bit5为1时可判断为A卡,然后执行A卡激活 apdu
	{
		send_result("line %d:%s测试失败(0x%02x)", __LINE__, TESTAPI, psSakbuf[0]);
		GOTOERR;
	}
	
	//case2:参数为NULL
	if((ret=NDK_RfidTypeARats(0,NULL, psDatabuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidTypeARats(0,&pnDatalen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case3:正常测试
	if((ret=NDK_RfidTypeARats(0,&pnDatalen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=NDK_OK||((sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x9000&&(sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x6d00))
	{
		send_result("line %d:%s测试失败(%d,0x%04x)", __LINE__, TESTAPI, ret, sw);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}
