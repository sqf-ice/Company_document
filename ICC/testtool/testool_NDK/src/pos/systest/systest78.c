/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: NFC/RF交叉
* file name		: systest78.c
* Author 			: sunwx
* version			: V1.0
* DATE			: 20130120
* directory 		: 
* description		: NFC/RF交叉
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
#define	TESTITEM	"NFC/RF交叉"

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
*					sunwx			20141121		created
*****************************************************************/
static void rfid_nfc(SMART_t type)
{
	/*private & local definition*/
	
	int ret = 0, succ = 0, cnt = 0, bak = 0,pUIDlen = 0,pnRecvlen = 0, nSeekCnt = 3, rcvlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR, *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"}, msg[32] = {0};
	uchar UID[LEN_UID] = {0}, psRecebuf[64] = {0};
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");	
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default    
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	sprintf(msg, "%s/nfc交叉", card[type]);
	
	while(cnt)
	{
		//保护
		RFID_deactive(type, 10);
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
			break;

		//NFC  		
		cls_show_msg("请移开射频卡，打开手机NFC测试工具并贴近pos,任意键继续...");
		cnt--;
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		cls_show_msg("请移开手机，确保安装%s卡,任意键继续...", card[type]);

		//RF寻卡
		if ((ret=RFID_detect(type, UID,&pUIDlen))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}

		//RF激活
		if ((ret=RFID_active(type, UID,&pUIDlen))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		//RF读写
		if((ret=RFID_APDUrw(type, UID, &rcvlen))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}	
		succ++;
	
	}
	
	if ((ret=RFID_deactive(type, 0))!=SUCC)//测试完毕,可以永久关闭
	{	
		cls_show_msg1(g_keeptime, "line %d:%s失败(%d)", __LINE__, msg, ret);
		//此时rf下电失败,仍允许报告压力测试成功率return;
	}
	
	cls_show_msg("%s完成,已执行次数为%d,成功%d次", msg, bak-cnt, succ);
	return;
}

void systest78(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/

	int ret = 0;
	SMART_t type = CPU_A;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"9.RFID配置\n");
		
		memcpy(g_RF_bak/*gen31_bak*/, DATA16, LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题
		switch(ret)
		{
		case '1':
			rfid_nfc(type);
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

