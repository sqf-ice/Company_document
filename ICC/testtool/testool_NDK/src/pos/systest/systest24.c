/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 磁卡/SMART交叉
* file name		: systest24.c
* Author 			: chenfm
* version			: V1.0
* DATE			: 20130120
* directory 		:
* description		: 磁卡/SMART交叉
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
#define	TESTITEM	"磁卡/SMART交叉"

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
*					chenfm			20130120			created
*****************************************************************/
static void mag_smart(SMART_t type, int i)
{
	/*private & local definition*/
#include "icsam.h"
	
	int ret = 0, ret1 = 0, succ = 0, cnt = 0, bak = 0,pUIDlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR, *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693" }, msg[32] = {0};
	uchar UID[LEN_UID] = {0};
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)		
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	cls_show_msg("请确保已安装%s卡,任意键继续...", (type==SAM||type==IC)?CardNoStr[i]:card[type]);
	if(type==SAM||type==IC)
	{	
		strcpy(msg, CardNoStr[i]);//sprintf(msg, "%s", CardNoStr[i]);
		UID[0] = CardNo[i];
	}
	else
		strcpy(msg, card[type]);
	
	while(cnt)
	{
		//smart_powerdown(type, CardNo[i], 10);//保护
		smart_deactive(type, CardNo[i], 10);
		if(ESC==cls_show_msg1(2, "磁卡/%s交叉中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
			break;
		
		//刷磁卡和smart激活
		if ((ret=systest_MagCard())!=STRIPE||(ret1=smart_detect(type,UID,&pUIDlen))!=SUCC)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次磁卡/%s交叉失败(0x%x,%d)", __LINE__, bak-cnt, msg, ret, ret1);
			continue;
		}
		
		//刷磁卡和smart获卡状态
		if ((ret=systest_MagCard())!=STRIPE||(ret1=smart_active(type, UID,&pUIDlen))!=SUCC)
		{
			cnt--;
			cls_show_msg1(g_keeptime, "line %d:第%d次磁卡/%s交叉失败(0x%x,%d)", __LINE__, bak-cnt, msg, ret, ret1);
			continue;
		}
		
		while(cnt)
		{
			if(ESC==cls_show_msg1(2, "磁卡/%s交叉中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
				break;
			cnt--;
			
			//刷磁卡和smart读写
			if((ret=systest_MagCard())!=STRIPE||(ret1=smart_APDUrw(type, CardNo[i], UID))!=SUCC)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次磁卡/%s交叉失败(%d,%d)", __LINE__, bak-cnt, msg, ret, ret1);
				break;
			}
			succ++;
		}
	}
	
	//smart下电
	if ((ret=smart_deactive(type, CardNo[i], 0))!=SUCC)//测试完毕,可以永久关闭
	{	
		cls_show_msg1(g_keeptime, "line %d:%s下电失败(0x%x)", __LINE__, msg, ret);
		//此时smart下电失败,仍允许报告压力测试成功率return;
	}
	
	cls_show_msg("磁卡/%s交叉完成,已执行次数为%d,成功%d次", msg, bak-cnt, succ);
	return;
}

void systest24(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}
	/*private & local definition*/
#include "icsam.h"

	int ret = 0, i = 0;
	SMART_t type = CPU_A;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693" };
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.smart配置");
		
		memcpy(g_RF_bak/*gen31_bak*/, DATA16, LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题
		switch(ret)
		{
		case '1':
			mag_smart(type, i);
			break;
		case '0':
			switch(type=select_smarttype())
			{
#if !defined ME15CHX//ME15C为节省代码空间进行条件编译 20170213 linwl
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
#endif
			case SAM:
			case IC:
				//conf_icsam_pps();
				if ((i=select_ICSAM())==NDK_ERR_QUIT)
					;//QUIT不提示
				else
					cls_show_msg1(2, "已选择%s",  CardNoStr[i]);
				break;
			default:
				break;
			}
			break;
		case ESC:
			smart_deactive(type, CardNo[i], 0);
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

