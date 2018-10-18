/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: IC卡/RF交叉
* file name		: systest25.c
* Author 			: chenfm
* version			: V1.0
* DATE			: 20130120
* directory 		: 
* description		: IC卡/RF交叉
* related document	: NL_POS程序员参考手册
*				  
************************************************************************
* log			: 
* Revision 1.0  20130120 chenfm
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"ICSAM/RF交叉"

#define	DEFAULT_CNT_STR	"10"//"1000"
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)

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
* history 				: author			date			remarks
*					zhenglw			20120314			created
*					zhangnw			20120321			modify
*****************************************************************/
static void iccard_rfcard(SMART_t type, int i)
{
	/*private & local definition*/
#include "icsam.h"
	
	int ret = 0, ret1 = 0, succ = 0, cnt = 0, bak = 0,pnAtrlen  = 0,pUIDlen = 0, rcvlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR, *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"}, msg[32] = {0};
	uchar UID[LEN_UID] = {0},buf[256] = {0};
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");	
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	cls_show_msg1(g_keeptime, "请确保已安装%s/%s卡,任意键继续...",  CardNoStr[i], card[type]);
	sprintf(msg, "%s/%s交叉", CardNoStr[i], card[type]);
	
	while(cnt)
	{
		//保护
		ICSAM_powerdown(CardNo[i]);
		RFID_deactive(type, 10);

		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
			break;

		//RF寻卡
		if ((ret=RFID_detect(type, UID,&pUIDlen))!=SUCC)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		//ICSAM上电
		if ((ret=NDK_IccPowerUp(CardNo[i], buf,&pnAtrlen ))!=NDK_OK)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}		

		//RF激活
		if ((ret=RFID_active(type, UID,&pUIDlen))!=SUCC)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		//APDU读写
		while(cnt)
		{
			if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
				break;
			cnt--;
			
			//RF和ICSAM读写
			if((ret=ICSAM_APDUrw(CardNo[i]))!=SUCC||(ret1=RFID_APDUrw(type, UID, &rcvlen))!=SUCC)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d,%d)", __LINE__, bak-cnt, msg, ret, ret1);
				break;
			}
			
			succ++;
		}
	}
	
	if ((ret=ICSAM_powerdown(CardNo[i]))!=NDK_OK||(ret1=RFID_deactive(type, 0))!=SUCC)//测试完毕,可以永久关闭
	{	
		cls_show_msg1(g_keeptime, "line %d:%s失败(%d,%d)", __LINE__, msg, ret, ret1);
		//此时smart下电失败,仍允许报告压力测试成功率return;
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s完成,已执行次数为%d,成功%d次", msg, bak-cnt, succ);
	return;
}

static void ic_rf_active(SMART_t type)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = 0, bak = 0,pnAtrlen  = 0,pUIDlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uchar UID[LEN_UID] = {0},buf[256] = {0};
	uchar SAK[LEN_SAK] = {0},picctype = 0;;
	
	/*process body*/
	if(cls_show_msg("此测试需要一张即有IC卡又有射频卡功能的多功能卡,[ESC]退出")==ESC)
		return;

	cls_printf("默认交叉测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	cls_show_msg("请将多功能卡插在IC卡卡槽,任意键继续");
	while(cnt)
	{
		//保护
		ICSAM_powerdown(ICTYPE_IC);
		RFID_deactive(type, 10);

		cls_printf("%s测试中...\n还剩%d次(已成功%d次)", TESTITEM, cnt, succ);
			
		//RF寻卡、激活
		switch(type)
		{
		case CPU_A:
		case CPU_B:
			NDK_RfidPiccDetect(NULL);
			NDK_RfidPiccActivate(&picctype, &pUIDlen, UID);
			break;
		case MIFARE_1:
			NDK_M1Request(REQA, &pUIDlen, UID);
			NDK_M1Anti(&pUIDlen, UID);
			NDK_M1Select(pUIDlen, UID, SAK);
			NDK_M1ExternalAuthen(pUIDlen, UID, AUTHKEY_TYPE_A, AUTHKEY, 0x01);
			break;
		case FELICA:
			NDK_RfidFelicaPoll(UID, &pUIDlen);
			break;
#if M0_ENABLE 
		case MIFARE_0:
			NDK_MifareActive(REQA, UID, &pUIDlen,SAK);			
			break;
#endif
		default:
			return ;
			break;
		}
		
		//IC上电
		if ((ret=NDK_IccPowerUp(ICTYPE_IC, buf,&pnAtrlen ))!=NDK_OK)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次IC卡上电失败(%d)", __LINE__, bak-cnt,ret);
			continue;
		}	
#if 0
		//IC读写
		if((ret=ICSAM_APDUrw(ICTYPE_IC))!=SUCC)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次IC卡读写失败(%d)", __LINE__, bak-cnt, ret);
			continue;
		}
#endif		
		//IC下电
		if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次IC卡下电失败(%d)", __LINE__, bak-cnt, ret);
			continue;
		}

		succ++;
		cnt--;
	}
	
	cls_show_msg("%s完成,已执行次数为%d,成功%d次", TESTITEM, bak-cnt, succ);
	return;
}

void systest25(void)
{
	/*private & local definition*/
#include "icsam.h"

	int ret = 0, i = 0;
	SMART_t type = CPU_A, type1 = IC;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	if(auto_flag==1)
	{
		//自动配置参数，失败则退出
		switch(type=select_smarttype())
		{
		case CPU_A:
		case CPU_B:
		case MIFARE_1:
		case FELICA:
		case MIFARE_0:
		case ISO15693:
			break;
		case SAM:
		case IC:
			type = CPU_A;
			break;
		default:
			break;
		}
		if ((ret=RFID_init(type))!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:射频卡初始化失败(%d)!请检查配置是否正确.", __LINE__, ret);
			return;
		}
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//根据配置文件读入的射频卡类型进行射频卡和IC/SAM的交叉
#if IC1_ENABLE
		i = 0;
		iccard_rfcard(type, i);
#endif
#if SAM1_ENABLE
		i = 1;
		iccard_rfcard(type, i);
#endif
#if SAM2_ENABLE
		i = 2;
		iccard_rfcard(type, i);
#endif
#if SAM3_ENABLE
		i = 3;
		iccard_rfcard(type, i);
#endif
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
						     "2.IC/RF深交叉\n"
							"0.ICSAM配置\n"
							"9.RFID配置\n");
		
		memcpy(g_RF_bak/*gen31_bak*/, DATA16, LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题
		switch(ret)
		{
		case '1':
			iccard_rfcard(type, i);
			break;
		case '2':
			ic_rf_active(type);
			break;
		case '0':
			switch(type1=select_smarttype())
			{
			case SAM:
			case IC:
				//conf_icsam_pps();
				if ((i=select_ICSAM())==NDK_ERR_QUIT)
					;//QUIT不提示
				else
					cls_show_msg1(2, "已选择%s",  CardNoStr[i]);
				break;
			default:
				cls_show_msg("line %d:配置错误!请配置IC/SAM(%d).", __LINE__, type1);
				break;
			}
			break;
		case '9':
			switch(type=select_smarttype())
			{
			case CPU_A:
			case CPU_B:
			case MIFARE_1:
			case FELICA:
			case MIFARE_0:	
			case ISO15693:
				if ((ret=RFID_init(type))!=SUCC)
					cls_show_msg("line %d:初始化失败(%d)!请检查配置是否正确.", __LINE__, ret);
				else
					cls_show_msg1(2, "%s初始化成功!", card[type]);
				break;
			default:
				cls_show_msg("line %d:配置错误!请配置RFID(%d).", __LINE__, type);
				break;
			}
			break;
		case ESC:
			ICSAM_powerdown(CardNo[i]);
			RFID_finish(type);
			cls_show_msg("请选择其它卡片重新测试!");
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

