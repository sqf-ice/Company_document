/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 磁卡/NFC交叉
* file name		: systest80.c
* Author 			: sunwx
* version			: V1.0
* DATE			: 20141008
* directory 		:
* description		: 磁卡/NFC交叉
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
#define	TESTITEM	"磁卡/NFC交叉"

#define	DEFAULT_CNT_STR	"10"//"100"
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
*						sunwx		20141008		created
*****************************************************************/

void systest80(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	
	int ret = 0, succ = 0, cnt = 0, bak = 0,  pnRecvlen = 0, nSeekCnt = 3;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uchar psRecebuf[64] = {0};

	cls_show_msg1(1, "%s测试中...", TESTITEM);
	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	cls_show_msg("请打开手机NFC测试工具并贴近pos,任意键继续...");
	
	while(cnt)
	{
		if(ESC==cls_show_msg1(2, "%s测试中...\n还剩%d次(已成功%d次),按ESC退出",TESTITEM,cnt,succ))
			break;
		
		//刷磁卡
		if ((ret=systest_MagCard())!=STRIPE)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(0x%x)", __LINE__, bak-cnt, TESTITEM, ret);
			continue;
		}
		//NFC
		//两次NFC调用之间需要有个2S的休眠时间，避免手机端来不及相应
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, TESTITEM,ret);
			continue;
		}
		
		cnt--;
		succ++;
		
	}
	
	cls_show_msg("%s完成,已执行次数为%d,成功%d次",TESTITEM, bak-cnt, succ);
	return;
}



