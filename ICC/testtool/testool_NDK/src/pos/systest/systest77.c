/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	NFC综合
* file name		: 	systest77.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: 	NFC综合测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20131202  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/

#define	TESTITEM	"NFC综合测试"
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
					 huangjianb		20140926	created
*****************************************************************/	
static void nfc_test(void)
{
	int ret=0, succ = 0 ,cnt=0 ,i=0, bak = 0, nSeekCnt = 3 ,pnRecvlen = 0;
	uint unLen = 0;
	uchar psRecebuf[64] = {0};
	char CntStr[8] = DEFAULT_CNT_STR;
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
		
	cls_show_msg("请确保已打开手机NFC测试工具并贴近pos,任意键继续...");
	
	while(cnt)
	{
		if(cls_show_msg1(2,"正在进行第%d次%s测试(已成功%d次),按ESC退出", i+1, TESTITEM, succ)==ESC)
			break;
		i++;
		cnt--;
		//两次NFC调用之间需要有个2S的休眠时间，避免手机端来不及相应
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)	
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试失败(%d)", __LINE__, ret);
			continue;
		}

		succ++;
	}
	
	cls_show_msg("压力完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

void systest77(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int key = 0;
	
	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	
	while (1)
	{
		key = cls_show_msg("NFC综合测试_\n"
							"1.NFC压力测试\n");
		switch(key)
		{
		case '1':
			nfc_test();
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	return;
	
}
