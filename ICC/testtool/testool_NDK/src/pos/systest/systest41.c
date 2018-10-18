/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: SMART/SD交叉
* file name		: 
* Author 			: chensj
* version			: V1.0
* DATE			: 2013/06/18
* directory 		: 
* description		: (SD/U盘)/SMART交叉
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20130618
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"(SD/U盘)/SMART交叉"

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
*					chensj			2013/06/18		creat
*****************************************************************/
static void sdcard_rfcard(SMART_t type, int disktype,int i)
{
	/*private & local definition*/
#include "icsam.h"
	int ret = 0, succ = 0, cnt = 0, bak = 0,UIDlen=0;
	char CntStr[8] = DEFAULT_CNT_STR, *card[] = {"CPU_A", "CPU_B"/*, "HS_B"*/, "M-1"/*, "RFID-SIM"*/, "FELICA", "M-0", "15693"}, msg[32]={0};
	uchar UID[LEN_UID] = {0};	
	uint unLen = 0;
	
	/*process body*/
	cls_printf("默认交叉测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 5,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		bak  = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	cls_show_msg1(g_keeptime, "请确保已安装%s卡,任意键继续...", (type==SAM||type==IC)?CardNoStr[i]:card[type]);
	if (type==SAM||type==IC)
	{
		sprintf(msg, "%s/%s交叉", CardNoStr[i], (disktype==UDISK)?"U盘":"SD");
		UID[0] = CardNo[i];
	}
	else
		sprintf(msg, "%s/%s交叉", card[type], (disktype==UDISK)?"U盘":"SD");
	
	while(cnt)
	{
		NDK_DiskClose(disktype);
		smart_deactive(type, CardNo[i], 0);
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次),按ESC退出", msg, cnt, succ))
			break;
		cnt--;
		
		//RF寻卡
		
		if ((ret=smart_detect(type, UID,&UIDlen))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		//RF激活
		if ((ret=smart_active(type, UID, &UIDlen))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		//RF读写
		if((ret=smart_APDUrw(type, CardNo[i], UID))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		//RF下电
		if ((ret=smart_deactive(type, CardNo[i], 0))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		//SD卡
		if((ret=systest_SDCard(disktype))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d)", __LINE__, bak-cnt, msg, ret);
			continue;
		}
		succ++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"%s完成,已执行次数为%d,成功%d次", msg, bak-cnt, succ);
	return;
}

void systest41(void)
{
	/*private & local definition*/
//#include "icsam.h"
	int ret = 0, disktype = SDDISK,i=0;
	SMART_t type = CPU_A;
	char *card[] = {"CPU_A", "CPU_B"/*, "HS_B"*/, "M-1"/*, "RFID-SIM"*/, "FELICA", "M-0", "15693"};
	char *CardNoStr[] = {"IC1", "SAM1", "SAM2", "SAM3", "SAM4"};
	
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

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//smart配置
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
			//根据配置文件读入的射频卡类型进行射频卡和SD/U盘的交叉
			sdcard_rfcard(type, disktype,i);
			break;
		case SAM:
		case IC:
			//type = CPU_A;
			break;
		default:
			break;
		}

		//按顺序轮流进行IC/SAM1/SAM2/SAM3和SD/U盘的交叉
#if IC1_ENABLE
		type = IC;
		i = 0;
		sdcard_rfcard(type, disktype,i);
#endif
#if SAM1_ENABLE
		type = SAM;
		i = 1;
		sdcard_rfcard(type, disktype,i);
#endif
#if SAM2_ENABLE
		type = SAM;
		i = 2;
		sdcard_rfcard(type, disktype,i);
#endif
#if SAM3_ENABLE
		type = SAM;
		i = 3;
		sdcard_rfcard(type, disktype,i);
#endif
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}

	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.SD/U盘配置\n"
							"9.smart配置\n");
		
		switch(ret)
		{
		case '1':
			sdcard_rfcard(type, disktype,i);
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
			switch(type=select_smarttype())
			{
			case CPU_A:
			case CPU_B:
		//case HS_B:
			case MIFARE_1:
			case FELICA:
			case MIFARE_0:	
		//case RFID_SIM:
			case ISO15693:
				if ((ret=RFID_init(type))!=SUCC)
					cls_show_msg("line %d:初始化失败(%d)!请检查配置是否正确.", __LINE__, ret);
				else
					cls_show_msg1(2, "%s初始化成功!", card[type]);
				break;
			case SAM:
			case IC:
				if ((i=select_ICSAM())==NDK_ERR_QUIT)
					;//QUIT不提示
				else
					cls_show_msg1(2, "已选择%s", CardNoStr[i]);
				break;
			default:
				cls_show_msg("line %d:配置错误!请配置smart(%d).", __LINE__, type);
				break;
			}
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


