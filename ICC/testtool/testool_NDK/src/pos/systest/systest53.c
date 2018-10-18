/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	触屏/打印交叉
* file name			: systest53.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: 触屏/打印交叉测试
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
#define	TESTITEM	"触屏/打印交叉"
#define	MAXWAITTIME	(120)	//30->120 for 针打
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
						jiangym			20131230	created
*****************************************************************/	
void print_touch(void)
{	
	int cnt = DEFAULT_CNT_VLE, bak = 0, ret = 0;
	uint len = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	
	/*process body*/
	cls_printf("默认测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	//注意:复位动作仅在进入函数压力前做一次,在之后的不同打印压力过程中不做(即用例间不做复位),以加大压力
	//为了避免其它打印用例的影响，先清一下打印BUF并同时复位一下
	if(lib_initprn(g_PrnSwitch)!=NDK_OK)
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
		if (ESC==cls_show_msg1(2, "%s测试中...\n还剩%d次,请注意打印效果,ESC中断测试", TESTITEM, cnt))
			break;
		cnt--;
		
		print_bill();
		if(systest_touch()!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败,总共%d次,成功%d次", __LINE__, TESTITEM, bak, bak-cnt-1);
			return;
		}		
	}
	cls_show_msg("%s测试完成,总次数为%d次,成功次数为%d", TESTITEM, bak, bak-cnt);
	return;
}
void systest53(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);

	while(1)
    {
		keyin = cls_show_msg("1.触屏/打印\n"
							 "0.打印配置");
		switch(keyin) 
		{
			case '1':
				print_touch();
				break;
			case '0':
				conf_prn();
				break;	
			case ESC:
				return;
			default:
				continue;
				break;
		}
	}	
	return;
}
