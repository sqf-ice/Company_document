/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest27.c
* Author 			: chensl
* version			: 
* DATE			: 20130124
* directory 	: 
* description	: SMART/LAN交叉测试
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"SMART/LAN交叉"

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
* history 		 		: author			date			remarks
*			  		chensl		   20130124	 		created
*****************************************************************/
static void smart_LAN(SMART_t type, int i)
{
	/*private & local definition*/
#include "icsam.h"

	int ret = 0, ret1 = 0, timeout = SO_TIMEO, slen = 0, rlen = 0, icount = 0, isucc = 0, uidlen = 0;
	uint h_tcp = 0; 
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0}, *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"}, msg[32] = {0};
	uchar UID[LEN_UID] = {0};
	
	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	cls_show_msg1(5, "请确保已安装%s卡,任意键继续...", (type==SAM||type==IC)?CardNoStr[i]:card[type]);
	if(type==SAM||type==IC)
	{	
		sprintf(msg, "%s/LAN交叉测试", CardNoStr[i]);
		UID[0] = CardNo[i];
	}
	else
		sprintf(msg, "%s/LAN交叉测试", card[type]);
	
	//NetUp
	if (NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:NetUp失败", __LINE__);
		return;
	}	
	while(1)
	{
		TransDown(h_tcp);//保护
		smart_deactive(type, CardNo[i], 10);
		
		if(ESC==cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", icount+1, msg, isucc))
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		icount++;

		//TransUp和smart寻卡
		if ((ret=TransUp(&h_tcp))!=SUCC||(ret1=smart_detect(type, UID, &uidlen))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d,%d)", __LINE__, icount, msg, ret, ret1);
			continue;
		}
		
		//LAN发送数据和smart激活
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len||(ret1=smart_active(type, UID, &uidlen))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(实际%d, 预期%d)(%d)", __LINE__, icount, msg, slen, sendpacket.len, ret1);
			continue;
		}
		
		//LAN接收数据和smart读写
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len||(ret1=smart_APDUrw(type, CardNo[i], UID))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(实际%d, 预期%d)(%d)", __LINE__, icount, msg, rlen, sendpacket.len, ret1);
			continue;
		}
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据校验失败", __LINE__, icount);
			continue;
		}
		
		//TransDown和smart下电
		if ((ret=TransDown(h_tcp))!=SUCC||(ret1=smart_deactive(type, CardNo[i], 10))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(%d,%d)", __LINE__, icount, msg, ret, ret1);
			continue;
		}		
		
		isucc++;
	}
	
	NetDown();
	smart_deactive(type, CardNo[i], 0);	
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s完成,已执行次数为%d,成功%d次", msg, icount, isucc);
	return;
}


void systest27(void)
{
	/*private & local definition*/
//#include "icsam.h"
	int ret = 0, i = 0, tmpflag = 0;
	SMART_t type = CPU_A;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
	char *CardNoStr[] = {"IC1", "SAM1", "SAM2", "SAM3", "SAM4"};
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=0)
		SetWifiStopflag(0);//确保本用例在维持wifi连接的情况下测试
	
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_LAN()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试LAN配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		
		g_CycleTime = 10;
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
				SetWifiStopflag(tmpflag);
				return;
			}
			//根据配置文件读入的射频卡类型进行射频卡和LAN的交叉
			smart_LAN(type, i);
			break;
		case SAM:
		case IC:
			//type = CPU_A;
			break;
		default:
			break;
		}
		
		//按顺序轮流进行IC/SAM1/SAM2/SAM3和LAN的交叉
#if IC1_ENABLE
		type = IC;
		i = 0;
		smart_LAN(type, i);
#endif
#if SAM1_ENABLE
		type = SAM;
		i = 1;
		smart_LAN(type, i);
#endif
#if SAM2_ENABLE
		type = SAM;
		i = 2;
		smart_LAN(type, i);
#endif
#if SAM3_ENABLE
		type = SAM;
		i = 3;
		smart_LAN(type, i);
#endif
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		SetWifiStopflag(tmpflag);
		return;
	}
	
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.LAN配置\n"
							"9.smart配置");		
		memcpy(g_RF_bak, DATA16, LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题
		switch(ret)
		{
		case '1':
			smart_LAN(type, i);
			break;
		case '0':
			switch(conf_conn_LAN())
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
				break;
			case NDK_ERR_QUIT://用户放弃
			default:
				break;
			}
			break;
		case '9':
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
		case ESC:
			cls_show_msg("请选择其它卡片重新测试!");
			SetWifiStopflag(tmpflag);
			return;
			break;
		default:
			continue;
			break;
		}
	}

	SetWifiStopflag(tmpflag);
	return;
}

