/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc10.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试 NDK_M1ExternalAuthen接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_M1ExternalAuthen"

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
void rfc10(void)
{
	/*private & local definition*/
	uchar psDatabuf[MAXLEN_DATA]={0}, UID[LEN_UID]={0}, psSakbuf[256]={0}, psBlockdata[MAXLEN_DATA]={0};
	int ret=0, ret1= -1, ret2= -1, ret3= -1, pnDatalen=0, UIDlen = 0;
	uchar inc[]={0x01, 0x00, 0x00, 0x00};//"\x01\x00\x00\x00";//uint inc = 0x01;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	NDK_RfidPiccDeactivate(0);//保护

	//case1:iccrw,错误情况已在rf5中测试了,本处侧重于流程的正常功能
	//并使用CMD_Auth_Ex替代上面的存,装,认证三步调用
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在感应区放置一张Mifare-1卡,任意键继续...");
	if((ret=NDK_M1Request(REQA, &UIDlen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Anti(&UIDlen, UID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_M1Select(UIDlen, UID, psSakbuf))!=NDK_OK)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
	{
		send_result("line %d:%s测试失败(%d,0x%2x)", __LINE__, TESTAPI, ret, psSakbuf[0]);
		GOTOERR;
	}

#if 1	//本用例采用外部认证,内部认证的方式已在rf9中测试了
	if((ret=NDK_M1ExternalAuthen(-1, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x01))!=NDK_ERR_PARA
	||(ret1=NDK_M1ExternalAuthen(UIDlen, NULL, AUTHKEY_TYPE_B, AUTHKEY, 0x01))!=NDK_ERR_PARA
	||(ret2=NDK_M1ExternalAuthen(UIDlen, UID, 0xff, AUTHKEY, 0x01))!=NDK_ERR_PARA
	||(ret3=NDK_M1ExternalAuthen(UIDlen, UID, AUTHKEY_TYPE_B, NULL, 0x01))!=NDK_ERR_PARA)
	{
		send_result("line %d:测试失败(%d,%d,%d,%d)", __LINE__, ret, ret1, ret2, ret3);
		GOTOERR;
	}
	if((ret=NDK_M1ExternalAuthen(UIDlen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x01))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	if((ret=NDK_M1KeyStore(AUTHKEY_TYPE_B, 0x00, AUTHKEY))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1KeyLoad(AUTHKEY_TYPE_B, 0x00))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1InternalAuthen(pnDatalen, UID, AUTHKEY_TYPE_B, 0x01))!=NDK_OK)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		GOTOERR;
	}
#endif
	pnDatalen=LEN_BLKDATA;//strlen(BLK02DATA_ORI);
	if((ret=NDK_M1Write(0x02, &pnDatalen, BLK02DATA_ORI))!=NDK_OK)//01块认证过了,所在扇区的其它块也认证过
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Increment(0x02, sizeof(inc), inc))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Transfer(0x02))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Increment(0x02, sizeof(inc), inc))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Transfer(0x02))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_M1Decrement(0x02, sizeof(inc), inc))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Transfer(0x02))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//上面三个调用的实际效果为+1

#if 0	//目前NDK设计:inc/dec后,需要BLKTransfer一下才生效,BLKRestore仍无用,不测
	//原GP平台设计:CMD_BLKTransfer与CMD_BLKRestore实际无用了,但直接返回00
	if((ret=NDK_M1Transfer(0x02))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Restore(0x02))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	if((ret=NDK_M1Read(0x02, &pnDatalen, psBlockdata))!=NDK_OK||pnDatalen!=LEN_BLKDATA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	else
	{
		if(memcmp(BLK02DATA_INC1, psBlockdata, LEN_BLKDATA))
		{
			send_result("line %d:块读写失败", __LINE__);
			GOTOERR;
		}
	}
	
#if 0
	if((ret=CMD_Halt())!=MI_OK)
	{
		send_result("line %d:CMD_Halt失败(ret=%d)", __LINE__, ret);
		GOTOERR;
	}
	if((ret=CMD_Request(WUPA, out))!=MI_OK)
	{
		send_result("line %d:寻卡失败(ret=%d)", __LINE__, ret);
		GOTOERR;
	}
#endif

	//case3:连续认证需求20130225add by zhangnw: 认证一块,读写一块(读写某块前先认证这个规则还是没变的)
	if((ret=NDK_M1ExternalAuthen(UIDlen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x04))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Read(0x04, &pnDatalen, psBlockdata))!=NDK_OK||pnDatalen!=LEN_BLKDATA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_M1ExternalAuthen(UIDlen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x08))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Read(0x08, &pnDatalen, psBlockdata))!=NDK_OK||pnDatalen!=LEN_BLKDATA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:使用错误的key进行认证20130528 add by zhengdc
	if((ret=NDK_M1ExternalAuthen(UIDlen, UID, AUTHKEY_TYPE_B, (uchar *)"\x11\x11\x11\x11\x11\x11", 0x08))==NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret=NDK_RfidPiccDeactivate(10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//msdelay(10);

	//case2为原gen31兼容性测试移到本处//zhangnw20110829
	//case2:使用4k M1来测试边界
	cls_show_msg("请在感应区放置1张4k M1卡,任意键继续...");
	//寻卡
	if((ret=NDK_M1Request(REQA, &UIDlen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Anti(&UIDlen, UID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_M1Select(UIDlen, UID, psSakbuf))!=NDK_OK)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}		
	//认证/激活,此处使用外部认证(内部认证类似,但会对卡的存储寿命有所影响)
	//由于不知道4k卡的B密钥是多少,使用A密钥(经测试A密钥为全0xff)
	if((ret=NDK_M1ExternalAuthen(UIDlen, UID, AUTHKEY_TYPE_A, AUTHKEY, 0xfe))!=NDK_OK)//认证的块在第4K内的扇
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//读写,考虑到测试卡寿命,仅做读操作
	if((ret=NDK_M1Read(0xfd, &pnDatalen, psBlockdata))!=NDK_OK)//0xfe所在扇区的块应都能读
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//关闭场
	if((ret=NDK_RfidPiccDeactivate(10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);//永久性关闭
	return;
}

