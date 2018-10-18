/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc12.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试NDK_RfidSuspend,NDK_RfidResume接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidSuspend,NDK_RfidResume"

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
void rfc11(void)
{
	//以下为局部变量
	int ret=0, pnDatalen=0, rcvlen=0, i;
	uchar psPicctype, psRecebuf[256]={0}, psDatabuf[256]={0}, UID[LEN_UID]={0}, psSakbuf[256]={0}, psBlockdata[MAXLEN_DATA]={0};
	ushort sw=0;
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	uchar Select_1pay[]= {'\x00', '\x84', '\x00', '\x00', 8};
#else
	uchar Select_1pay[]={0x00, 0xA4, 0x04, 0x00, 0x0E, '1', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1', 0x00};
#endif

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);

	//case1:寻卡，上电，强制休眠，APDU交互应失败返回，唤醒之后APDU交互应成功返回。
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	for(i=0;i<2;i++)
	{
		cls_show_msg("请在感应区放置一张标准%s卡,任意键继续...", i==0?"A":"B");
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
		if((ret=NDK_RfidSuspend())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}	
		if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=NDK_ERR)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_RfidResume())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=-7)//"-7 未上电(寻卡激活)"
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
		if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=NDK_OK||((sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x9000&&(sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x6d00))
		{
			send_result("line %d:%s测试失败(%d,0x%04x)", __LINE__, TESTAPI, ret, sw);
			GOTOERR;
		}
		if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case2:对M1卡休眠唤醒测试。
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
	if(((ret=NDK_M1Select(pnDatalen, UID, psSakbuf))!=NDK_OK)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_M1ExternalAuthen(pnDatalen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x01))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_RfidSuspend())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Read(0x02, &pnDatalen, psBlockdata))!=NDK_ERR_MI_WriteFifo)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidResume())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Read(0x02, &pnDatalen, psBlockdata))!=NDK_ERR_MI_WriteFifo)//"-7 未上电(寻卡激活)"
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
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
	if(((ret=NDK_M1Select(pnDatalen, UID, psSakbuf))!=NDK_OK)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1ExternalAuthen(pnDatalen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x01))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Read(0x02, &pnDatalen, psBlockdata))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

#if 0 //还未有产品导入,为了不影响其他产品编译先关闭,后续导入时开启
	//case3:对M0卡休眠唤醒测试20180122 add
	cls_show_msg("请在感应区放置一张Mifare-0卡,任意键继续...");
	if((ret=NDK_MifareActive(REQA, UID, &pnDatalen,psSakbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
/*	if((ret=NDK_M0Authen(AUTHKEY_M0))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}*/ //兼容不带C的M0卡不做认证
	if((ret=NDK_RfidSuspend())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Read(8,&pnDatalen,psBlockdata))!=NDK_ERR_MI_WriteFifo)//读8-11数据
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidResume())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Read(8,&pnDatalen,psBlockdata))!=NDK_ERR_MI_WriteFifo)//"-7 未上电(寻卡激活)"
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MifareActive(REQA, UID, &pnDatalen,psSakbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Read(8,&pnDatalen,psBlockdata))!=NDK_OK)//读8-11数据
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}

