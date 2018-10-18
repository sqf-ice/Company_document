/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: ICSAM/NFC交叉
* file name		: systest82.c
* Author 			: 
* version			: V1.0
* DATE			: 20130120
* directory 		: 
* description		: ICSAM/NFC交叉
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
#define	TESTITEM	"ICSAM/NFC交叉"

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
*						sunwx		20141121		created
*****************************************************************/
static void icsam_nfc(int i)
{
	/*private & local definition*/
#include "icsam.h"
	
	int ret = 0, succ = 0, cnt = 0, bak = 0, pnAtrlen  = 0,pnRecvlen,nSeekCnt = 3;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR, msg[32] = {0};
	uchar psRecebuf[64],buf[256] = {0};

	/*process body*/
	cls_printf("默认交叉测试次数:\n");	
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	cls_show_msg("请确保已安装%s卡,打开手机NFC测试工具并贴近pos,任意键继续...",  CardNoStr[i]);
	sprintf(msg, "%s/nfc交叉", CardNoStr[i]);
	
	while(cnt)
	{
		//保护
		ICSAM_powerdown(CardNo[i]);
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
			break;
		
		//NFC  
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
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
		while(cnt)
		{	
			if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
				break;
			cnt--;		
			//NFC
			sleep(2);
			if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
				break;
			}
			//ICSAM读写
			if((ret=ICSAM_APDUrw(CardNo[i]))!=SUCC)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
				break;
			}	
			succ++;
		}
	}
	if ((ret=ICSAM_powerdown(CardNo[i]))!=NDK_OK)//测试完毕,可以永久关闭
	{	
		cls_show_msg1(g_keeptime, "line %d:%s失败(0x%x)", __LINE__, msg, ret);
		//此时ICSAM下电失败,仍允许报告压力测试成功率return;
	}
	cls_show_msg("%s完成,已执行次数为%d,成功%d次", msg, bak-cnt, succ);
	return;
}

void systest82(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
#include "icsam.h"

	int ret = 0, i = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.ICSAM配置\n");
		
		switch(ret)
		{
		case '1':
			icsam_nfc(i);
			break;
		case '0':
			if ((i=select_ICSAM())==NDK_ERR_QUIT)
				;//QUIT不提示
			else
				cls_show_msg1(2, "已选择%s",  CardNoStr[i]);
			break;
		case ESC:
			ICSAM_powerdown(CardNo[i]);
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

