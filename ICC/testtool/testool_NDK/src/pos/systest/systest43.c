/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 打印/取电池电量交叉
* file name		: systest43.c
* Author 			: linwl
* version			: V1.0
* DATE			: 20130707
* directory 		: 
* description		: 打印/取电池电量交叉
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
#define	TESTITEM	"打印/取电池电量交叉"

#define	DEFAULT_CNT_STR	"10"//"300"
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
*					linwl			2013/02/21     created
*****************************************************************/
static void print_getpowervol(void)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = 0, bak = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen = 0;
	unsigned int unVol = 300;
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr, 1,sizeof(CntStr)-1,&unLen, INPUTDISP_OTHER, 5,INPUT_CONTRL_LIMIT))==ESC||ret==NDK_ERR_TIMEOUT)//按取消,则使用default
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	cls_show_msg_record(FILENAME ,FUNCNAME ,"如果是手持设备请确保只通过电池供电,不连接外接电源,没有连接串口线USB线等,任意键继续...");
	
	//if(NDK_PrnInit(g_PrnSwitch)!=NDK_OK)
	if(lib_initprn(g_PrnSwitch)!=NDK_OK)//初始化且使之前的配置有效modify 20131122 jiangym
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:打印机复位失败", __LINE__);
		return;
	}
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//避免其它用例的干扰
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:打印机状态异常", __LINE__);
		return;                         
	}
	if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:%s失败(ret=%d,vol=%d)", __LINE__, TESTITEM, ret, unVol);
		return;
	}
	while(cnt)
	{
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次)(电池电量:%d),按ESC退出", TESTITEM, cnt, succ, unVol))
			break;	
		cnt--;
		
		//取打印状态
		print_bill();
		//取电池电量
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:第%d次%s失败(ret=%d,vol=%d)", __LINE__, bak-cnt, TESTITEM, ret, unVol);
			continue;
		}
		if ((ret=GetPrinterStatus(MAXWAITTIME))!=SUCC)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, TESTITEM, ret);
			continue;
		}		
		//取电池电量
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:第%d次%s失败(ret=%d,vol=%d)", __LINE__, bak-cnt, TESTITEM, ret, unVol);
			continue;
		}
		succ++;
	}
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s完成,已执行次数为%d,成功%d次", TESTITEM, bak-cnt, succ);
	return;
}

void systest43(void)
{
	/*private & local definition*/
	int ret = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	if(auto_flag==1)
	{
		//自动化测试内容		
		//打印配置
		conf_prn();
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉压力
		print_getpowervol();

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"9.打印配置");
		
		switch(ret)
		{
		case '1':
			print_getpowervol();
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

