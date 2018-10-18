/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: NFC/打印交叉
* file name		: systest79.c
* Author 			: sunwx
* version			: V1.0
* DATE			: 20141008
* directory 		: 
* description		: 打印/NFC交叉
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20130221  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"打印/NFC交叉"

#define	DEFAULT_CNT_STR	"10"//"100"
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)
#define	MAXWAITTIME	(180)	//30->180 for 针打

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
*						sunwx		2014/10/08     created
*****************************************************************/
static void nfc_print(void)
{
	/*private & local definition*/

	int ret = 0, ret1 = 0, succ = 0, cnt = 0, bak = 0, pnRecvlen = 0, nSeekCnt = 3;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen=0;
	uchar psRecebuf[64] = {0};
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr, 1,sizeof(CntStr)-1,&unLen, INPUTDISP_OTHER, 0,INPUT_CONTRL_LIMIT))==ESC||ret==NDK_ERR_TIMEOUT)//按取消,则使用default
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	cls_show_msg("请确保已打开手机NFC测试工具并贴近pos,任意键继续...");
	
	//if(NDK_PrnInit(g_PrnSwitch)!=NDK_OK)//if (FAIL==clrprintbuf())
	if(lib_initprn(g_PrnSwitch)!=NDK_OK)//初始化且使配置有效
	{
		cls_show_msg("%s, line %d:打印机复位失败", __FILE__, __LINE__);
		return;
	}
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//避免其它用例的干扰
	{
		cls_show_msg("line %d:打印机状态异常!", __LINE__);
		return;                         
	}
	
	while(cnt)
	{
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出",TESTITEM, cnt, succ))
			break;
		
		print_bill();
		//取打印状态和nfc测试
		if ((ret=GetPrinterStatus(MAXWAITTIME))!=SUCC||(ret1=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(0x%x,%d)",__LINE__,bak-cnt,TESTITEM,ret,ret1);
			continue;
		}
			
			succ++;
			cnt--;
	}
		
	cls_show_msg("%s完成,已执行次数为%d,成功%d次",TESTITEM, bak-cnt, succ);
	return;
}

void systest79(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	
	int ret = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	while(1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"9.打印配置");
		
		switch(ret)
		{
		case '1':
			nfc_print();
			break;
		case '9':
			conf_prn();
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

