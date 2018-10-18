/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: SMART/打印交叉
* file name		: systest28.c
* Author 			: chensj
* version			: V1.0
* DATE			: 20130221
* directory 		: 
* description		: SMART/打印交叉
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
#define	TESTITEM	"SMART/打印交叉"

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
*					chensj			2013/02/21     created
*****************************************************************/
static void smart_print(SMART_t type, int i)
{
	/*private & local definition*/
#include "icsam.h"
	int ret = 0, ret1 = 0, succ = 0, cnt = 0, bak = 0;
	char CntStr[8] = DEFAULT_CNT_STR, *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"}, msg[32] = {0};
	uchar UID[LEN_UID] = {0};
	uint unLen=0;
	int nUIDlen=0;
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr, 1,sizeof(CntStr)-1,&unLen, INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==ESC||ret==NDK_ERR_TIMEOUT)//按取消,则使用default
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	cls_show_msg1(g_keeptime, "请确保已安装%s卡,任意键继续...", (type==SAM||type==IC)?CardNoStr[i]:card[type]);
	if (type==SAM||type==IC)
	{
		sprintf(msg, "%s/打印交叉", CardNoStr[i]);
		UID[0] = CardNo[i];
	}
	else
		sprintf(msg, "%s/打印交叉", card[type]);	
	
	//if(NDK_PrnInit(g_PrnSwitch)!=NDK_OK)//if (FAIL==clrprintbuf())
	if(lib_initprn(g_PrnSwitch)!=NDK_OK)//初始化且使配置有效modify 20131122 jiangym
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机复位失败", __FILE__, __LINE__);
		return;
	}
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//避免其它用例的干扰
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:打印机状态异常!", __LINE__);
		return;                         
	}
	
	while(cnt)
	{
		//保护
		smart_deactive(type, CardNo[i], 10);
		
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
			break;
		
		//取打印状态和smart寻卡
		print_bill();
		if ((ret=GetPrinterStatus(MAXWAITTIME))!=SUCC||(ret1=smart_detect(type, UID,&nUIDlen))!=SUCC)
		{
			cnt--;
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:第%d次%s失败(0x%x,%d)", __LINE__, bak-cnt, msg, ret, ret1);
			continue;
		}
		
		//取打印状态和smart激活
		print_bill();
		if ((ret=GetPrinterStatus(MAXWAITTIME))!=SUCC||(ret1=smart_active(type,UID,&nUIDlen))!=SUCC)
		{
			cnt--;
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:第%d次%s失败(0x%x,%d)", __LINE__, bak-cnt, msg, ret, ret1);
			continue;
		}
		
		while(cnt)
		{
			if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
				break;
			cnt--;
			
			//取打印状态和smart读写
			print_bill();
			if((ret=GetPrinterStatus(MAXWAITTIME))!=SUCC||(ret1=smart_APDUrw(type, CardNo[i], UID))!=SUCC)
			{
				cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:第%d次%s失败(0x%x,%d)", __LINE__, bak-cnt, msg, ret, ret1);
				break;
			}
			
			succ++;
		}
	}
	
	//smart下电
	print_bill();
	if ((ret=GetPrinterStatus(MAXWAITTIME))!=SUCC||(ret1=smart_deactive(type, CardNo[i], 0))!=SUCC)//测试完毕,可以永久关闭
	{
		cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:%s失败(0x%x,%d)", __LINE__, msg, ret, ret1);
		//此时smart下电失败,仍允许报告压力测试成功率return;
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s完成,已执行次数为%d,成功%d次", msg, bak-cnt, succ);
	return;
}

void systest28(void)
{
	/*private & local definition*/
//#include "icsam.h"
	int ret = 0, i = 0;
	SMART_t type = CPU_A;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
	char *CardNoStr[] = {"IC1", "SAM1", "SAM2", "SAM3", "SAM4"};
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		conf_prn();

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		
		switch(type=select_smarttype())
		{
		case CPU_A:
		case CPU_B:
		case MIFARE_1:
		case FELICA:
		case MIFARE_0:	
		case ISO15693:
			if ((ret=RFID_init(type))!=SUCC)
			{
				cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:射频卡初始化失败(%d)!请检查配置是否正确.", __LINE__, ret);
				return;
			}
			//根据配置文件读入的射频卡类型进行射频卡和PRINT的交叉
			smart_print(type, i);
			break;
		case SAM:
		case IC:
			//type = CPU_A;
			break;
		default:
			break;
		}
		
		//按顺序轮流进行IC/SAM1/SAM2/SAM3和print的交叉
#if IC1_ENABLE
		type = IC;
		i = 0;
		smart_print(type, i);
#endif
#if SAM1_ENABLE
		type = SAM;
		i = 1;
		smart_print(type, i);
#endif
#if SAM2_ENABLE
		type = SAM;
		i = 2;
		smart_print(type, i);
#endif
#if SAM3_ENABLE
		type = SAM;
		i = 3;
		smart_print(type, i);
#endif
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.smart配置\n"
							"9.打印配置");
		
		memcpy(g_RF_bak/*gen31_bak*/, DATA16, LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题
		switch(ret)
		{
		case '1':
			smart_print(type, i);
			break;
		case '0':
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
			case SAM:
			case IC:
			//	conf_icsam_pps();
				if ((i=select_ICSAM())==NDK_ERR_QUIT)
					;//QUIT不提示
				else
					cls_show_msg1(2, "已选择%s", CardNoStr[i]);
				break;
			default:
				break;
			}
			break;
		case '9':
			conf_prn();
			break;			
		case ESC:
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

