/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	NFC/取电池电量
* file name			: systest81.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: NFC/取电池电量测试
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
#define	TESTITEM	"NFC/取电池电量"
#define	DEFAULT_CNT_STR	"10"
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
*					 huangjianb		20141013	created
*****************************************************************/	
void systest81(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	int ret = 0, bak = 0,cnt = 0, succ = 0, nSeekCnt = 3 ,pnRecvlen = 0;
	uint unLen = 0;
	char  CntStr[8] = DEFAULT_CNT_STR;
	unsigned int unVol = 300;
	uchar psRecebuf[64] = {0};

	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	cls_show_msg("手持设备请确保只通过电池供电,不连接外接电源,没有连接串口线USB线等,并打开手机NFC测试工具并贴近pos任意键继续...");
	if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
	{
		cls_show_msg1(g_keeptime, "line %d:%s失败(ret=%d,vol=%d)", __LINE__, TESTITEM, ret, unVol);
		return;
	}
	
	while(cnt)
	{
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次)(电池电量:%d),按ESC退出", TESTITEM, cnt, succ, unVol))
			break;	
		cnt--;

		//NFC
		//两次NFC调用之间需要有个2S的休眠时间，避免手机端来不及相应
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试失败(%d)", __LINE__, ret);
			continue;
		}

		//取电池电量
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(ret=%d,vol=%d)", __LINE__, bak-cnt, TESTITEM, ret, unVol);
			continue;
		}
		succ++;
	}
	cls_show_msg("%s完成,已执行次数为%d,成功%d次", TESTITEM, bak-cnt, succ);
	return;
}

