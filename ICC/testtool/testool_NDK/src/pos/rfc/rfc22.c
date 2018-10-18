/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc22.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description		: 测试NDK_M1Anti_SEL和NDK_M1Select_SEL防冲突和选卡接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_M1Anti_SEL,NDK_M1Select_SEL"

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
*			  		        linying		    20180420		created 
*****************************************************************/
//获取卡片uid
static int get_mifare_UID(int *UIDlen,uchar* UID)
{
	uchar rece_buf[30];
	int ret=0,i=0,pnDatalen=0,len=0;
	unsigned char sak;
	unsigned char sel_cmd[3] = {0x93, 0x95, 0x97};
	//寻卡
	if((ret=NDK_M1Request(REQA, &pnDatalen, rece_buf))!=NDK_OK)
	{
		cls_show_msg("line %d:M1卡寻卡失败(%d)", __LINE__, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	len = (rece_buf[0] >> 6) + 1;//0x00 4  0x01 7 0x02 10
	i = 0;
    	do
	{
		//M1卡防冲突
		if((ret=NDK_M1Anti_SEL(sel_cmd[i],&pnDatalen,rece_buf))!=NDK_OK)
		{
		    cls_show_msg("line %d:M1卡防冲突失败(%d)", __LINE__,  ret);
		    NDK_RfidPiccDeactivate(0);
		    return FAIL;
		}
		//M1选卡
		if(((ret=NDK_M1Select_SEL(sel_cmd[i],pnDatalen,rece_buf,&sak))!=NDK_OK)||((sak&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
		{
		    cls_show_msg("line %d:测试失败(%d,0x%2x)", __LINE__,  ret, sak);
		    NDK_RfidPiccDeactivate(0);
		    return FAIL;
		}
		memcpy((UID + (i << 2)), rece_buf, 4);
		if((sak & 0x04) == 0x00)
			break;	//UID transfer complete
	}while(i++ < 3);
	
	if((len == 1) && (*UID == 0x88)){
		cls_show_msg(" 预期为4字节的UID信息有误");
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	if((len == 2) && (*(UID+4) == 0x88)){
		cls_show_msg(" 预期为7字节的UID信息有误");
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	switch(len){
		case 1:
			*UIDlen = 4;
		break;
		case 2:
			*UIDlen = 7;
			memcpy(UID,&UID[1],3);/*first*/
			memcpy(&(UID[3]),&UID[4],4);
		break;
		case 3:
			*UIDlen = 10;
			memcpy(UID,&UID[1],3);/*first*/
			memcpy(&(UID[3]),&UID[5],3);
			memcpy(&(UID[6]),&UID[8],4);

		default:
			*UIDlen = 0;
			UID = 0x00;
			return NDK_ERR_MI_PROTOCOLERR;
	}	
    return SUCC;
}

void rfc22(void)
{
	//以下为局部变量
	int ret=0, pnDatalen,UIDlen=0;
	uchar UID[LEN_UID]={0},UID1[LEN_UID]={0},psDatabuf[256]={0},psSakbuf[256]={0},out[MAXLEN_DATA]={0};
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);
	
	NDK_RfidPiccType(RFCARD_TYPE_A);	 
	//case1:参数异常测试
	if((ret=NDK_M1Anti_SEL(0xff,&pnDatalen,UID1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_M1Select_SEL(0xff,pnDatalen,UID1,psSakbuf))!=NDK_ERR_PARA)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
	{
		send_result("line %d:%s测试失败(%d,0x%2x)", __LINE__, TESTAPI, ret, psSakbuf[0]);
		GOTOERR;
	}
	
	//case2:未进行寻卡就进行防冲突和选卡测试应该返回失败
	cls_show_msg("请在感应区放置1张Mifare-1卡,任意键继续...");
	if((ret=NDK_M1Anti_SEL(0x93,&pnDatalen,UID1))!=NDK_ERR_MI_WriteFifo)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_M1Select_SEL(0x93,4,UID1,psSakbuf))!=NDK_ERR_MI_WriteFifo)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡,给uid长度直接赋为4
	{
		send_result("line %d:%s测试失败(%d,%02x)", __LINE__, TESTAPI, ret, psSakbuf[0]);
		GOTOERR;
	}
	
	//case3:寻卡成功后进行防冲突时把卡移开,或者是防冲突成功后把卡移开
	//cls_show_msg("请在感应区放置一张Mifare-1卡,任意键继续...");//目前不支持真正的防冲突, (subcase==0)?"多":"同一");
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请将感应区的Mifare-1卡移开,任意键继续...");
	if((ret=NDK_M1Anti_SEL(0x93,&pnDatalen,UID))==NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	cls_show_msg("请在感应区放置同一张Mifare-1卡,任意键继续...");//目前不支持真正的防冲突, (subcase==0)?"多":"同一");
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Anti_SEL(0x93,&pnDatalen,UID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	cls_show_msg("请将感应区的Mifare-1卡移开,任意键继续...");
	if((ret=NDK_M1Select_SEL(0x93,4,UID,psSakbuf))==NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:同张卡获取的uid应该一样,不同卡获取的uid不同
	cls_show_msg("请在感应区放置一张Mifare-1卡,任意键继续...");
	if((ret=get_mifare_UID(&UIDlen,UID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在感应区放置同一张Mifare-1卡,任意键继续...");
	if((ret=get_mifare_UID(&UIDlen,UID1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(UID, UID1, sizeof(UID)))
	{
		send_result("line %d:同张卡UID不一致", __LINE__);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在感应区放置一张4K Mifare-1卡,任意键继续...");
	if((ret=get_mifare_UID(&UIDlen,UID1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(!memcmp(UID, UID1, sizeof(UID)))
	{
		send_result("line %d:不同张卡UID不一致", __LINE__);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:使用4字节的M1卡进行正常流程测试
	cls_show_msg("请在感应区放置一张Mifare-1卡,任意键继续...");
	if((ret=get_mifare_UID(&UIDlen,UID1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1ExternalAuthen(UIDlen, UID1, AUTHKEY_TYPE_A, AUTHKEY, 0x01))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	pnDatalen=LEN_BLKDATA; 
	if((ret=NDK_M1Write(0x02, &pnDatalen, BLK02DATA_FF))!=NDK_OK)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Read(0x02, &pnDatalen, out))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	else
	{
		if(memcmp(BLK02DATA_FF, out, LEN_BLKDATA))
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:获取7字节的UID并判断长度,可以使用M0卡进行验证,只做判断uid长度
	cls_show_msg("请在感应区放置1张UItralight C卡,任意键继续...");
	if((ret=get_mifare_UID(&UIDlen,UID1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(UIDlen!=7)
	{
		send_result("获取UID长度有误，%d",UIDlen);
		goto ERR;
	}
	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}

