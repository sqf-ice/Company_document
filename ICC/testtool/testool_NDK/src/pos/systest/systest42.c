/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: SMART/打印交叉
* file name		: 
* Author 			: chensj
* version			: 
* DATE			: 2013/06/18
* directory 		: 
* description		: SD/打印交叉
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20030618
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"(SD/U盘)/打印交叉"

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
*					chensj		2013/06/18			created
*****************************************************************/
static void sdcard_print(int disktype)
{
	/*private & local definition*/
	
	int ret = 0, succ = 0, cnt = 0, bak = 0;
	char CntStr[8] = DEFAULT_CNT_STR,msg[32] = {0};
	uint unLen=0;
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 5,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		bak  = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	cls_show_msg1(g_keeptime, "请确保已安装%s卡,任意键继续...", (disktype==UDISK)?"U盘":"SD");	
	sprintf(msg, "%s/打印交叉", (disktype==UDISK)?"U盘":"SD");

	if(lib_initprn(g_PrnSwitch)!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"%s, line %d:打印机初始化失败",__FILE__,__LINE__);
		return;
	}
	while(cnt)
	{
		//保护
		NDK_DiskClose(disktype);		
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
			break;	
		cnt--;
		
		//取打印状态
		print_bill();
		if ((ret=GetPrinterStatus(MAXWAITTIME))!=SUCC)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:第%d次%s失败(0x%x)", __LINE__, bak-cnt, msg, ret);
			continue;
		}		
		//SD卡
		if((ret=systest_SDCard(disktype))!=SUCC)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		succ++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"%s完成,已执行次数为%d,成功%d次", msg, bak-cnt, succ);
	return;
}

void systest42(void)
{
	/*private & local definition*/	
	int ret = 0, disktype = SDDISK;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_disktype()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,SD/U盘配置失败,请检查配置文件",TESTITEM);
			return;
		}
		disktype=get_disktype();
		//打印配置
		conf_prn();
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉压力
		sdcard_print(disktype);
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.SD/U盘配置\n"
							"9.打印配置");
	
		switch(ret)
		{
		case '1':
			sdcard_print(disktype);
			break;
		case '0':
			if((ret=conf_disktype())!=SUCC)
			{
				cls_show_msg1(2, "设备选择失败,请重新选择", ret);
				return;
			}
			disktype=get_disktype();
			break;
		case '9':
			conf_prn();
			break;			
		case ESC:
			cls_show_msg("请选择其它SD/USB重新测试!");
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

