/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc9.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试能否正确操作Mifare-1卡(APDU读写)
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_M1xxx函数"

/*------------global variables definition-----------------------*/
static int (*rf5_subcase)(uchar *, int) = NULL;

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
static int subcase_0(uchar *UID, int UidLen);
static int subcase_1(uchar *UID, int UidLen);
static int subcase_2(uchar *UID, int UidLen);
static int subcase_3(uchar *UID, int UidLen);
static int subcase_4(uchar *UID, int UidLen);
static int subcase_5(uchar *UID, int UidLen);

/*---------------functions definition---------------------------*/
//以下子用例的前置条件均一样为"寻卡成功",之后就可调用以下子用例了
//目前以下几个子用例建议顺序测试(出于减少代码行的考虑,用例间可能有依赖关系)
//以下设计采用类似状态模式的C语言实现,有较好的可扩展性与可维护性
//子用例使用注意:存储,装载key若失败(因不与卡发生通信),是不需要重新发寻卡命令的,若一定要这么做,应关闭天线场后再打开,再寻卡...

static int subcase_0(uchar *UID, int UidLen)
{
#if !ANDIROD_ENABLE  //根据秋云反馈说高端产品NDK_M1KeyStore已废除 故不测 20180718 modify
	/*private & local definition*/
	int ret=0, ret1=0, ret2=0;

	/*process body*/
	//cls_show_msg1(2, "存储密钥A中...");
	//NDK_M1KeyStorey异常参数测试
	if((ret=NDK_M1KeyStore(0xff, 0x00, AUTHKEY))!=NDK_ERR_PARA
	||(ret=NDK_M1KeyStore(AUTHKEY_TYPE_B, 0x10, AUTHKEY))!=NDK_ERR_PARA
	||(ret=NDK_M1KeyStore(AUTHKEY_TYPE_B, 0x00, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2);
		return FAIL;
	}	
	//NDK_M1KeyStorey正常测试，应成功返回
	if((ret=NDK_M1KeyStore(AUTHKEY_TYPE_B, 0x00, AUTHKEY))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if((ret=NDK_M1KeyStore(AUTHKEY_TYPE_A, 0x00, AUTHKEY))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if((ret=NDK_M1InternalAuthen(-1, UID, AUTHKEY_TYPE_A, 0x01))!=NDK_ERR_PARA
	||(ret1=NDK_M1InternalAuthen(UidLen, NULL, AUTHKEY_TYPE_A, 0x01))!=NDK_ERR_PARA
	||(ret2=NDK_M1InternalAuthen(UidLen, UID, 0xff, 0x01))!=NDK_ERR_PARA)
	{
		send_result("line %d:测试失败(%d,%d,%d)", __LINE__, ret, ret1, ret2);
		return FAIL;
	}
#if !K21_ENABLE  //此接口已废除,为了避免二次进入测试会报错,低端产品不测此点 20180714 modify
	if((ret=NDK_M1InternalAuthen(UidLen, UID, AUTHKEY_TYPE_A, 0x01))==NDK_OK)//!=MI_AUTHERR)//实际返回-1??
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return FAIL;
	}	
#endif
#endif
	rf5_subcase=subcase_1;
	return SUCC;
}

static int subcase_1(uchar *UID, int UidLen)
{
	/*private & local definition*/
	int ret=0, pnDatalen=0;
	uchar out[MAXLEN_DATA]={0};
	/*process body*/

	if((ret=NDK_M1Read(0x01, &pnDatalen, out))!=NDK_ERR_MI_NOTAUTHERR)//==MI_OK)//!=MI_NOTAUTHERR)//实际返回-12??
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	rf5_subcase=subcase_2;
	return SUCC;
}

static int subcase_2(uchar *UID, int UidLen)
{
#if !ANDIROD_ENABLE  //根据秋云反馈说高端产品NDK_M1KeyLoad已废除 故不测 20180718 modify
	/*private & local definition*/
	int ret=0, ret1=0;
	
	/*process body*/
	//NDK_M1KeyLoad异常测试
	if((ret=NDK_M1KeyLoad(0xff, 0x00))!=NDK_ERR_PARA||(ret1=NDK_M1KeyLoad(AUTHKEY_TYPE_A, 0x10))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		return FAIL;
	}
	//NDK_M1KeyLoad正常测试
	if((ret=NDK_M1KeyLoad(AUTHKEY_TYPE_A, 0x00))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if((ret=NDK_M1InternalAuthen(UidLen, UID, AUTHKEY_TYPE_A, 0x01))!=NDK_OK)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return FAIL;
	}

	//caseX:连续认证需求20130225add by zhangnw.原先不支持这样使用,多次认证之间必须要上下电,现在支持此使用方式了
	if((ret=NDK_M1InternalAuthen(UidLen, UID, AUTHKEY_TYPE_A, 0x01))!=NDK_OK)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return FAIL;
	}
#endif
	//注意:"这种错误对应关系"指内部认证方式中,store/load的ucKeynum与NDK_M1InternalAuthen中ucBlocknum所对应的sector号不要求要一样,
	//如可以以ucKeynum=9去认证ucBlocknum=01或10,只需要00扇区(01所在)及02扇区(10所在)的key与使用API存在POS上的key一样即可
	//但对外部认证而言,操作函数中无ucKeynum,故无此一说.但无论如何,要读写块前,内或外认证都是必须的,同时读写的
	//块号与认证的块号所在扇区必须一致.而不是说认证任一扇区,所有扇区都可以操作了.
#if 0	//目前MIFARE-1允许这种错误对应关系,责任交由应用者自行正确对应
	if((ret=CMD_Auth(AUTHKEY_TYPE_A, UID, 0x00, 0x09))==MI_OK)//用00扇区的密钥认证其它扇区的块
	{
		send_result("line %d:认证应失败(ret=%d)", __LINE__, ret);
		return FAIL;
	}
#endif

	rf5_subcase=subcase_3;
	return SUCC;
}

static int subcase_3(uchar *UID, int UidLen)
{
	/*private & local definition*/
	int ret=0, ret1=0, pnDatalen=0;
	uchar out[MAXLEN_DATA]={0};
	//uchar inc[]={0x01, 0x00, 0x00, 0x00};//"\x01\x00\x00\x00";//uint inc = 0x01;

	/*process body*/
#if !ANDIROD_ENABLE //根据开发反馈NDK_M1InternalAuthen已废除20180718 modify
	//subcase_4已load过密钥了,本处只需要重新认证一下即可
	if((ret=NDK_M1InternalAuthen(UidLen, UID, AUTHKEY_TYPE_A, 0x01))!=NDK_OK)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return FAIL;
	}
#endif

	//NDK_M1Write异常测试1：参数异常
	if((ret=NDK_M1Write(0x02, NULL, BLK02DATA_FF))!=NDK_ERR_PARA||(ret1=NDK_M1Write(0x02, &pnDatalen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:测试失败(%d,%d)", __LINE__, ret, ret1);
		return FAIL;
	}
	
	//NDK_M1Write异常测试2：未认证，进行读写
	pnDatalen=LEN_BLKDATA;//strlen(BLK02DATA_FF);
	if((ret=NDK_M1Write(0x09, &pnDatalen, BLK02DATA_FF))==NDK_OK/*NDK_ERR_MI_NOTAUTHERR*/||(ret1=NDK_M1Read(0x09, &pnDatalen, out))==NDK_OK/*NDK_ERR_MI_NOTAUTHERR*/)
	{
		send_result("line %d:测试失败(%d,%d)", __LINE__, ret, ret1);
		return FAIL;
	}
	
	rf5_subcase=subcase_4;
	return SUCC;
}

static int subcase_4(uchar *UID, int UidLen)
{
	/*private & local definition*/
	int ret=0, ret1=0, pnDatalen=0;
	uchar out[MAXLEN_DATA]={0};
	uchar inc[]={0x01, 0x00, 0x00, 0x00};//"\x01\x00\x00\x00";//uint inc = 0x01;

	/*process body*/
#if !ANDIROD_ENABLE //根据开发反馈NDK_M1InternalAuthen已废除20180718 modify
	//subcase_4已load过密钥了,本处只需要重新认证一下即可
	if((ret=NDK_M1InternalAuthen(UidLen, UID, AUTHKEY_TYPE_A, 0x01))!=NDK_OK)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return FAIL;
	}
#endif

	//NDK_M1Write正常测试
	pnDatalen=LEN_BLKDATA;//strlen(BLK02DATA_FF);
	if((ret=NDK_M1Write(0x02, &pnDatalen, BLK02DATA_FF))!=NDK_OK)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return FAIL;
	}
	//NDK_M1Read异常测试
	if((ret=NDK_M1Read(0x01, NULL, out))!=NDK_ERR_PARA||(ret1=NDK_M1Read(0x01, &pnDatalen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		return FAIL;
	}
	//NDK_M1Read正常测试
	if((ret=NDK_M1Read(0x02, &pnDatalen, out))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	else
	{
		if(memcmp(BLK02DATA_FF, out, LEN_BLKDATA))
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
	}
	//增减量操作之前,块上的数据必须先符合钱包格式,否则操作会失败
	if((ret=NDK_M1Increment(0x02, sizeof(inc), NULL))!=NDK_ERR_PARA||(ret1=NDK_M1Increment(0x02, -1, inc))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		return FAIL;
	}	
	if((ret=NDK_M1Increment(0x02, sizeof(inc), inc))!=NDK_ERR_MI_INCRERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if((ret=NDK_M1Decrement(0x02, sizeof(inc), NULL))!=NDK_ERR_PARA||(ret1=NDK_M1Decrement(0x02, -1, inc))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		return FAIL;
	}
	if((ret=NDK_M1Decrement(0x02, sizeof(inc), inc))!=NDK_ERR_MI_DECRERR)//实际返回-1
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}

	rf5_subcase=subcase_5;
	return SUCC;
}

static int subcase_5(uchar *UID, int UidLen)
{
	/*private & local definition*/
	int ret=0, pnDatalen=0;
	uchar out[MAXLEN_DATA]={0};
	uchar inc[]={0x01, 0x00, 0x00, 0x00};//"\x01\x00\x00\x00";//uint inc = 0x01;

	/*process body*/
#if !ANDIROD_ENABLE //根据开发反馈NDK_M1InternalAuthen已废除20180718 modify
	//subcase_4已load过密钥了,本处只需要重新认证一下即可
	if((ret=NDK_M1InternalAuthen(UidLen, UID, AUTHKEY_TYPE_A, 0x01))!=NDK_OK)//!=MI_AUTHERR)//实际返回-1??
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return FAIL;
	}
#endif
	
	pnDatalen=LEN_BLKDATA;//strlen(BLK02DATA_ORI);
	if((ret=NDK_M1Write(0x02, &pnDatalen, BLK02DATA_ORI))!=NDK_OK)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		return FAIL;
	}		
#if 0 //目前的底层设计是增减后都直接将结果存回E2ROM(不需要传输,恢复指令,这两条指令实际上无效了,直接返回成功)
		if((ret=CMD_BLKInc(0x02, inc))!=MI_OK)
		{
			send_result("line %d:增减量操作失败(ret=%d)", __LINE__, ret);
			return FAIL;
		}
		if((ret=CMD_BLKRestore(0x02))!=MI_OK)
		{
			send_result("line %d:块恢复失败(ret=%d)", __LINE__, ret);
			return FAIL;		
		}
#endif
	if((ret=NDK_M1Increment(0x02, sizeof(inc), inc))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if((ret=NDK_M1Increment(0x02, sizeof(inc), inc))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	
#if 0 //Transfer有效后进行的调整,调整为 +1 3次
	if((ret=NDK_M1Decrement(0x02, sizeof(inc), inc))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
#else
	if((ret=NDK_M1Increment(0x02, sizeof(inc), inc))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if((ret=NDK_M1Transfer(0x02))!=NDK_OK)//注意:NDK中Transfer有效,且只对Transfer前最后一次inc/dec有效
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}	
#endif

	//上面三个调用的最终实际效果为+1
#if 0
		if((ret=NDK_M1Read(0x02,&pnDatalen,out))!=MI_OK)
		{
			send_result("line %d:测试失败(%d)", __LINE__, ret);
			return FAIL;
		}
		else
		{
			if(memcmp(BLK02DATA_ORI, out, LEN_BLKDATA))
			{
				send_result("line %d:测试失败", __LINE__);
				return FAIL;
			}			
		}
		if((ret=NDK_M1Transfer(0x02))!=MI_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,ret);
			return FAIL;			
		}
#endif
	if((ret=NDK_M1Read(0x02, &pnDatalen, out))!=NDK_OK||pnDatalen!=LEN_BLKDATA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	else
	{
		if(memcmp(BLK02DATA_INC1, out, LEN_BLKDATA))
		{
			send_result("line %d:块读写失败", __LINE__);
			return FAIL;
		}
	}
	rf5_subcase=NULL;
	return SUCC;
}

static int handle_subcase(uchar *UID, int UidLen)
{
	/*private & local definition*/

	/*process body*/
	if(rf5_subcase!=NULL)
		return rf5_subcase(UID, UidLen);
	else
		return (-2);
}

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		    chenfm	        20130108	 	created
*****************************************************************/
void rfc9(void)
{
	/*private & local definition*/
	uchar UID[LEN_UID]={0}, UID1[LEN_UID]={0}, psDatabuf[256]={0}, psSakbuf[256]={0};
	int ret=0, ret1= -1, ret2= -1, subcase=0, pnDatalen=0;
	rf5_subcase=subcase_0;
	
	/*process body*/
#if !K21_ENABLE//低端平台由于空间问题不支持内部认证方式断电后继续保存密钥的功能，所以不做相关测试 20150324 linwl
	if(ENTER!=cls_show_msg("是否为机具上电后首次运行本用例?\n[确认]是[其它]否"))
	{
		send_result("line %d:%s测试中止(请重新上下电)", __LINE__, TESTAPI);
		return;
	}
	if(ENTER==cls_show_msg("是否跳过保存密钥的操作?\n[确认]是[其它]否"))
	{
		rf5_subcase=subcase_1;
	}
#endif

	cls_printf("测试%s...", TESTAPI);
	NDK_RfidPiccDeactivate(0);//保护

	cls_show_msg("请将感应区放置Mifare-1卡,任意键继续...");	
	//case1:设置卡类型为B卡，寻M1卡，应失败返回。
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_B))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_ERR_MI_CardTypeErr)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//caseX:设置卡类型为AB卡，寻M1卡，应失败返回。
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_ERR_MI_CardTypeErr)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//caseX:设置卡类型为A卡，寻M1卡。
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:MI_NOTAGERR
	cls_show_msg("请将感应区的Mifare-1卡移开,任意键继续...");
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_ERR_MI_NOTAGERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:防拔处理测试//zhangnw20110617add
	//对接触与非接而言,拔插后卡会回到最初态,POS也应回到最初态.然后重新上电/激活...
	//同张卡
	cls_show_msg("请在感应区放置一张Mifare-1卡,任意键继续...");//目前不支持真正的防冲突, (subcase==0)?"多":"同一");
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请将感应区的Mifare-1卡移开,任意键继续...");
	if((ret=NDK_M1Anti(&pnDatalen, UID))==NDK_OK)
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
	if((ret=NDK_M1Anti(&pnDatalen, UID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	cls_show_msg("请将感应区的Mifare-1卡移开,任意键继续...");
	if((ret=NDK_M1Select(pnDatalen, UID, psSakbuf))==NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//以下测试基于正常流程扩展异常测试（同时，一般先做异常测试再做正常测试）。
	cls_show_msg("请在感应区放置同一张Mifare-1卡,任意键继续...");//目前不支持真正的防冲突, (subcase==0)?"多":"同一");
	if((ret=NDK_M1Request(REQA, NULL, psDatabuf))!=NDK_ERR_PARA||(ret=NDK_M1Request(REQA, &pnDatalen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}	
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M1Anti(NULL, UID1))!=NDK_ERR_PARA||(ret1=NDK_M1Anti(&pnDatalen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}	
	if((ret=NDK_M1Anti(&pnDatalen, UID1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(memcmp(UID, UID1, sizeof(UID)))
	{
		send_result("line %d:同张卡UID不一致", __LINE__);
		GOTOERR;
	}
	if((ret=NDK_M1Select(-1, UID1, psSakbuf))!=NDK_ERR_PARA
	||(ret1=NDK_M1Select(pnDatalen, NULL, psSakbuf))!=NDK_ERR_PARA
	||(ret2=NDK_M1Select(pnDatalen, UID1, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2);
		GOTOERR;
	}
	if(((ret=NDK_M1Select(pnDatalen, UID1, psSakbuf))!=NDK_OK)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//不同卡,验证返回的UID应不同
	cls_show_msg("请在感应区换另一张Mifare-1卡,任意键继续...");//目前不支持真正的防冲突, (subcase==0)?"多":"同一");
	//GUI_DispStringEx(12*6, 0, "另", TEXT_ATTRIBUTE_REVERSE);
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_M1Anti(&pnDatalen, UID1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(!memcmp(UID, UID1, sizeof(UID)))
	{
		send_result("line %d:不同卡UID应不一致", __LINE__);
		GOTOERR;
	}
	NDK_RfidPiccDeactivate(10);//关闭一下再打开避免对下一个用例的干扰

#if !(defined ME30MH||defined ME32||defined ME30GX||defined ME32GX||defined ME15B) //开发说ME30MH不支持密钥存储\装载\内部认证这个几个接口故相关用例不测 20170527
	//case4:其它子用例测试
TAG:cls_show_msg("请在感应区放置一张Mifare-1卡,任意键继续...");//目前不支持真正的防冲突, (subcase==0)?"多":"同一");
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
	cls_show_msg1(2, "标准三步寻卡已成功!");

	//更细的子用例测试
	cls_show_msg1(2, "%d#子用例测试...", subcase++);
	ret=handle_subcase(UID, pnDatalen);
	if(ret==FAIL)
		GOTOERR
	else if(ret==SUCC)
	{
		show_msg1(2, "成功!");
		//说明:子用例中若仅是发生存储或装载key失败,实际上可以不寻卡(因与卡通信没有错误,再去寻卡会对错交替出现),但为了调用的统一,本处处理均为先关闭再打开场,以保证寻卡成功
		NDK_RfidPiccDeactivate(10);//关闭一下再打开避免对下一个用例的干扰
		goto TAG;
	}
	else	//-2
		;
#endif

	//CMD_Powerdown(阻塞的)
	cls_printf("天线已关闭,255ms后打开.请保持感应区内一直有Mifare-1卡,并耐心等待!");//cls_printf("天线已关闭,10s后打开.请保持感应区内一直有Mifare-1卡,并耐心等待!");
	if((ret=NDK_RfidPiccDeactivate(255))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0 //底层设计为阻塞的
	if((ret=NDK_M1Request(REQA))!=MI_RESETERR)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI,ret);
		GOTOERR;
	}
	//cls_printf("天线已关闭,10s后打开.请保持感应区内一直有Mifare-1卡,并耐心等待!");
	msdelay(10000);
	if((ret=NDK_RfidPiccDeactivate(10))!=NDK_OK)//关闭10s后再打开
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI,ret);
		GOTOERR;
	}
	msdelay(10);
#endif
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);//永久性关闭
	return;
}

