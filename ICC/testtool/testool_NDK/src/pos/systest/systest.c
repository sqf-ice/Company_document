/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 综合测试
* file name		: 
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20060801
* directory 		: .\SRC\general\
* description		: 综合测试总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: TCR690 API测试程序 详细设计说明书.doc
*				  TCR690&8200程序员参考手册（v1.05T)_2.doc
*				  TCR8280_软件测试案例(银税)_V2.2.doc
*
************************************************************************
* log			: 
* Revision 1.1  20060921 11:20 zhangnw
* 改写综合测试框架
* Revision 1.0  20060801 14:10 zhangnw
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	ITEMPERPAGE	(4)
#define	MAXTIMEOUT	(60)
#define	MAXWAITTIME	(180)	//30->180 for 针打

#if PRN_ENABLE
#define	PRNIMGWIDTH	MAX(MAX(MAXPRLINEWIDE_TP, MAXPRLINEWIDE_HIP), MAXPRLINEWIDE_FIP)	//(384)
#define	PRNIMGHEIGHT	(8)	//(8*8)
#define	PRNIMGBUFLEN	(PRNIMGWIDTH*PRNIMGHEIGHT/8)
#endif

#define	DEFAULT_KEEPTIME_STR	"0" //"2"
#define	DEFAULT_KEEPTIME_VLE	atoi(DEFAULT_KEEPTIME_STR)

//#define	DEBUG_CMD_ALL		(-1)		//for NL829
//调试哪条CMD就定义DEBUG_CMD为哪条(会输出发送与返回数据);正常使用时,关闭此宏
//#define	DEBUG_CMD			NL829_CPU_ACTIVE	//NL829_APDURW
//#define	DEBUG_CMD			DEBUG_CMD_ALL	//全部命令都调试//该模式在NL829测试中暂不支持

/*------------global variables definition-----------------------*/
int g_keeptime = 0;// 2;//DEFAULT_KEEPTIME_VLE;//该变量主要供压力测试错误提示等待时间,非压力测试基本都是要确认的(不超时)
uchar g_RF_bak[/*MAXLEN_DATA*/LEN_BLKDATA+8] = DATA16;
#if ISO15693_ENABLE
iso15693ProximityCard_t cards[20];
#endif
/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
int g_CycleTime = 0;//通讯压力和交叉次数
int g_PackLen = 0;//数据包长度
/* 用于获取设置的初值: 如测试次数*/
int GetCycleValue(void)
{
	return g_CycleTime;
}

int GetCommPackLen(void)
{
	return g_PackLen;
}

static int IsAPartOfB(uchar *A, int Acnt, uchar *B, int Bcnt)
{
	/*private & local definition*/
 	int i = 0, j = 0;

 	/*process body*/
 	for(i=0; i<Acnt; i++)
 	{
		for(j=0; j<Bcnt&&A[i]!=B[j]; j++);
		if(j==Bcnt)
			return FALSE;
 	}
	
	return TRUE;
}

static int GetIDInCharArray(uchar value, uchar *array, int size)
{
	/*private & local definition*/
 	int i = 0;

 	/*process body*/
 	for(; i<size&&value!=array[i]; i++);

	if(i==size)
		return -1;
	else
	{
		sprintf(NowSystestName,"systest%d.c",array[i]);
	}
 	return i;
}

#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C不支文件系统为节省代码空间进行条件编译 20170213 linwl
//#if !K21_ENABLE
static void resultlog_conf(void)
{
	int nKeyin = 0,ret = 0,fp = 0,rdlen = 0;
	char result[1024]={0};
	
	while(1)
	{
		nKeyin = cls_show_msg("测试结果配置\n"
							"1.清除log文件\n"
							"2.上送log\n");
		switch(nKeyin)
		{
			case '1':
				NDK_FsDel(LOGFILE);
				return;
				break;
			case '2':
			//	if (g_com != INVALID_COM)
			//		NDK_PortClose(g_com);

			//	if(NDK_PortOpen(g_com, "115200") != NDK_OK )//PORT_NUM_COM1
			//	{
			//		cls_show_msg1(3,"line %d:打开串口1失败", __LINE__);
			//		return;
			//	}
				if(cls_show_msg("请确保程序运行入口处有选择上送串口,若无按ESC退出选择,若有按其他键继续")==ESC)
					return;
				if(g_com==PORT_NUM_USB)
					cls_show_msg("打开PC工具AccessPort的串口后任意键继续");
				if ((fp=NDK_FsOpen(LOGFILE, "r"))< NDK_OK)
				{
					cls_show_msg1(3,"line %d:打开%s失败(%d)", __LINE__, LOGFILE,fp);
					return;
				}
				while((rdlen=NDK_FsRead(fp, result,sizeof(result)))==sizeof(result))
				{
					if((ret=NDK_PortWrite(g_com, rdlen, result))!=NDK_OK)//PORT_NUM_COM1
					{
						cls_show_msg1(3,"line %d: 发送失败ret=%d", __LINE__, ret);
						NDK_FsClose(fp);
						return;
					}
				}
				if(rdlen>0)
				{
					if((ret=NDK_PortWrite(g_com,rdlen, result))!=NDK_OK)//PORT_NUM_COM1
						cls_show_msg1(3,"line %d: 发送失败ret=%d", __LINE__, ret);	
				}
				NDK_FsClose(fp);
			//	NDK_PortClose(g_com);//关闭串口PORT_NUM_COM1

			//	if (g_com != INVALID_COM)
			//	{
			//		if ((ret=NDK_PortOpen(g_com, "115200,8,N,1")) != NDK_OK)
			//		{
			//			//初始化失败了,也不必send_result("line %d: 初始化串口错", __LINE__);
			//			cls_show_msg1(3,"line %d: 初始化串口错(%d)", __LINE__,ret);
			//			return;
			//		}
			//	}
				cls_show_msg1(2,"log上送成功");//增加个提示信息
				return;
				break;
			case ESC:
				return;
				break;
			default:
				break;
		}
	}
	return;
}
//#else
//static void resultlog_conf(void){cls_show_msg("本产品不支持写测试记录功能");return;}
//#endif

/*<
  * 针对一些夜间进行的压力测试,同一个模块的连续压力测试,例如无线的
  * 		1.通讯压力，2.拨号压力，3.通讯拨号压力，4.复位拨号压力  放在一块做
*/
static void PressAndTime_conf(void)
{
	int nKeyin = 0,nRet  = 0;
	char CntStr[16]={"100"},PackageLen[16]={"10000"}, wait[4] = {0};
	uint unLen = 0, len = 0;

	//设置g_keeptime 保持时间
	if(ENTER!=cls_show_msg1(MAXWAITTIME, "出错信息是否一直保持?\n"                                                   
												"[确认]是[其它]否\n"))
	{
		cls_printf("出错信息保持时间(s):");
		sprintf(wait, "%d", g_keeptime);
		if ((nRet=lib_kbgetinput(wait, 0, sizeof(wait)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||nRet==NDK_ERR_TIMEOUT)
			g_keeptime = DEFAULT_KEEPTIME_VLE;		
		else
			g_keeptime = atoi(wait);
	}
	else
		g_keeptime = 0;

	//以下设置连续压力相关参数
	while(1)
	{
		nKeyin = cls_show_msg("是否连续压力\n"
							"1.是\n"
							"2.否\n");
		switch(nKeyin)
		{
			case '1':
				g_SequencePressFlag = 1;
								
				cls_printf("全局连续压力测试次数:");
				if ((nRet=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen, INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||nRet==NDK_ERR_TIMEOUT)
					g_CycleTime = 100;
				else
					g_CycleTime = atoi(CntStr);

				cls_printf("全局连续压力通讯包长:");
				if ((nRet=lib_kbgetinput(PackageLen, 2, sizeof(PackageLen)-1, &unLen, INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||nRet==NDK_ERR_TIMEOUT)
					g_PackLen = 10000;
				else
					g_PackLen = atoi(PackageLen);
				return;
				break;
				
			case '2':
				g_SequencePressFlag = 0;
				return;
				break;
			default:
				g_SequencePressFlag = 0;
				return;
				break;
		}
	}
	return;
}

#if NEW_WIFI_ENABLE
static void conf_wifi_stop(void)
{
	/*private & local definition*/
 	int nKeyin = 0;

 	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg("是否保持wifi连接\n"
							"1.否\n"
							"2.是\n");
		switch(nKeyin)
		{
			case '1':
				SetWifiStopflag(1);
				return;
				break;
			case '2':
				SetWifiStopflag(0);
				return;
				break;
			default:
				SetWifiStopflag(1);
				return;
				break;
		}
	}
	return;
}
#else
static void conf_wifi_stop(void){cls_show_msg("不支持新WIFI函数");return;}
#endif

static void g_conf(void)
{
	/*private & local definition*/
 	int nKeyin = 0;

 	/*process body*/
 	do
	{
		nKeyin = cls_show_msg("1.打印  5.wifi\n"
							"2.连续压力及时间\n"
							"3.结果log配置\n"
							"4.选择NFC数据格式\n"
							);
#define MAXMENUITEM ('5')

		switch (nKeyin)
		{
		case '1':
#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME66)	
			conf_prn();
#endif
			break;
		case '2':
			PressAndTime_conf();
			break;
		case '3':
			resultlog_conf();
			break;
		case '4':
			conf_nfc_datatype();
			break;
		case MAXMENUITEM:
			conf_wifi_stop();
			break;
		case ESC:
			return;
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM
 } 

#if (ETH_ENABLE||WLAN_ENABLE||NEW_WIFI_ENABLE)
int conf_conn_LAN(void)
{
 	/*private & local definition*/
	int nKeyin = 0, lantype = -1;
 	char pInfo[64] = {0};

	/*process body*/
	do
	{
		if(g_SequencePressFlag)
		{
			if(GetParaInfo("LANType",pInfo)!=SUCC)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%s)", __LINE__, pInfo);
				return NDK_ERR_QUIT;//配置文件设置错误时默认用户取消
			}
			if(trim_string2int(pInfo,&lantype)!=SUCC)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%d)",__LINE__, lantype);
				return NDK_ERR_QUIT;
			}
			if(lantype==1)
				nKeyin='1';
			else if(lantype==2)
				nKeyin='2';
			else
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%d)",__LINE__, lantype);
				return NDK_ERR_QUIT;
			}
		}
 		else
		nKeyin = cls_show_msg("选择连接方式:\n"
								"1.Eth 2.WLAN\n");
#define MAXMENUITEM ('2')

		switch (nKeyin)
		{
		//case 0://0为超时,默认启用GPRS
		case '1':
#if ETH_ENABLE
			return conf_conn_ETH(TRUE);
			break;
#else
			cls_show_msg1(2,"本产品不支持ETH,请选择其他连接方式");
			continue;
#endif
		case MAXMENUITEM://'2':
#if WLAN_ENABLE
			return conf_conn_WLAN();
			break;
#else
			cls_show_msg1(2,"本产品不支持WIFI,请选择其他连接方式");
			continue;
#endif
		case ESC:
			return NDK_ERR_QUIT;//QUIT;
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));

	return SUCC;//never reach
#undef MAXMENUITEM	
}

void create_recv_only_packet(Packet *packet, char *buf)
{
	/*private & local definition*/

	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	packet->len = packet->orig_len=PACKMAXLEN;// MIN(PACKMAXLEN,4096);//PACKMAXLEN;//后台限制最多只能5000 -->后台无限制 20180912
	packet->lifecycle = 1000;//PACKETLIFE;//1000;
	packet->forever = FALSE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}

void send_recv_press(uint h_tcp)
{
	/*private & local definition*/
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	int i = 0, comm_succ_count = 0, timeout = SO_TIMEO;
	int slen = 0, rlen = 0;
	Packet sendpacket;

	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	while (1)
	{
		//不允许通讯过程中断
		cls_printf("开始第%d次通讯测试(已成功%d次)", i+1, comm_succ_count);
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
	
		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len)==ESC)
				break;
			else
				continue;
		}
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, sendpacket.len)==ESC)
				break;
			else
				continue;
		}
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验失败", __LINE__)==ESC)
				break;
			else
				continue;
		}
		
		comm_succ_count++;//通讯成功次数增加
	}		

	//测试后置
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯总次数%d:成功次数%d", i, comm_succ_count);
	return;
}

void recv_only_press(uint h_tcp)
{
	/*private & local definition*/
	int timeout = SO_TIMEO, rec_succ = 0, i = 0, slen = 0, rlen = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;

	/*process body*/
	create_recv_only_packet(&sendpacket, buf);
	update_snd_packet(&sendpacket);
	sendpacket.IsDataRnd = FALSE;//缓冲区已初始过了,偷偷改变为不随机
	
	//发送数据
	if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
	{
		cls_show_msg1(g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len);
		return;
	}
	
	while (1)
	{
		//不允许通讯过程中断
		cls_printf("开始第%d次通讯测试(已成功%d次)", i+1, rec_succ);
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
			
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, sendpacket.len)==ESC)
				break;
			else
				continue;
		}
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验失败", __LINE__)==ESC)
				break;
			else
				continue;
		}		
		
		rec_succ++;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯总次数%d,成功次数%d", i, rec_succ);
	return;
}

void send_only_press(uint h_tcp)
{
	/*private & local definition*/
	int timeout = SO_TIMEO, i = 0;
	int send_succ = 0, slen = 0;
	char buf[PACKMAXLEN] = {0};
	Packet sendpacket;

	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	while (i<200)//while(1) 用例不允许退出影响效率 故修改只测试200次 linwl20131115
	{
		//不允许通讯过程中断
		cls_printf("开始第%d次通讯测试(已成功%d次)", i+1, send_succ);
		i++;

		//发送数据//但如何确保数据真正被发送出去了?后台校验?后台如何知会POS?
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len)==ESC)
				break;
			else
				continue;
		}
		NDK_SysDelay(10);//给后台一些接收时间
		send_succ++;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯总次数%d,成功次数%d", i, send_succ);
	return;
}
#else
int conf_conn_LAN(void){return FAIL;}
void create_recv_only_packet(Packet *packet, char *buf){}
void send_recv_press(uint h_tcp){}
void recv_only_press(uint h_tcp){}
void send_only_press(uint h_tcp){}
#endif
#endif
/****************************************************************
* function name 	 	: general
* functional description 	: 综合测试入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20060801 	 	created
*					zhangnw		   20060921		modify
*****************************************************************/
void systest(PF* list, unsigned char id[])
{
	/*private & local definition*/
	int nKeyin = 0, nPageSel = 0;//从0开始计算(第一页码为0)
	int cnt = 0, menuid = 0, maxpage = 0, sn =0, sn1 = 0;
	unsigned char select_id[MAXID] = {0};
	
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C不支文件系统为节省代码空间进行条件编译 20170213 linwl
	int nRet  = 0;
	char CntStr[16]={"100"},PackageLen[16]={"10000"};
	uint unLen = 0;
#endif	
	//zhangnw20060921 add 以后综合测试的用例子菜单放在此数组中
	//本数组可以考虑自动生成,不过没有这个必要,故暂不实现
	//设计说明:
	//"性能、压力"关注单模块的运算速度,成功率及在一定强度下的表现
	//"交叉" 关注模块间深度交叉及在一定强度下的交叉是否正常
	//是否进行交叉测试需要参照"交叉关联矩阵"
	char* menustr[] =
	{
		"打印性能,压力",	// 1
		"MDM性能,压力",		// 2
		"ETH性能,压力",		// 3
		"无线性能,压力",	// 4
		"IC卡性能,压力",	// 5
		"磁卡性能,压力",	// 6
		"安全性能,压力",	// 7
		"fsys性能,压力",	// 8
		"串口性能,压力",	// 9
		"RTC综合测试",		// 10
		"WLAN性能,压力",	// 11
		"CPU性能测试",		// 12
		"射频卡测试",		// 13		
		"磁卡/无线交叉",	// 14
		"磁卡/打印交叉",	// 15
		"磁卡/MDM交叉",		// 16
		"磁卡/LAN交叉",		// 17
		"打印/无线交叉",	// 18
		"打印/MDM交叉",		// 19
		"打印/LAN交叉",		// 20
		"无线/MDM交叉",		// 21
		"ETH/WLAN交叉",		// 22
		"SMART/无线交叉",	// 23
		"磁卡/SMART交叉",	// 24
		"ICSAM/RF交叉",		// 25
		"SMART/MDM交叉",	// 26
		"SMART/LAN交叉",	// 27
		"SMART/打印交叉",	// 28
		"MDM/LAN交叉",		// 29
		"无线/LAN交叉",		// 30
		"硬件信息测试",  	 //31
		"USB串口综合",		// 32
		"扫描头/摄像头综合测试", // 33 
		"MEMORY卡测试",		// 34
		"NL-PP60-1测试",	// 35
		"NL829测试",		// 36
		"(旧)NL829测试",	// 37
		"打印/键盘交叉",	// 38
		"硬件统计服务",		// 39	
		"SD/U盘综合测试",	// 40
		"(SD/U盘)/SMART", 	// 41
		"(SD/U盘)/打印",	// 42
		"打印/电量交叉",	//43
		"WLAN/ETH深交叉",	//44
		"WLAN/WLM深交叉",	//45
		"WLAN/MDM深交叉",	//46
		"蜂鸣器压力测试",	//47
		"磁卡/键盘交叉",	//48
		"三界面卡测试",		//49  将磁卡刷卡、IC卡读写、射频卡寻卡读写、手输入卡号4种方式进行交叉
		"射频/键盘交叉",   	//50
		"LCD屏幕测试",		//51
		"触屏/射频交叉",	//52
		"触屏/打印交叉",	//53
		"触屏/磁卡交叉",	//54
		"触屏/电池电量",	//55
		"触屏/休眠交叉",	//56
		"蓝牙综合",         //57
		"蓝牙/取电池电量",  //58
		"BT/MDM深交叉",		//59
		"BT/无线深交叉",	//60
		"BT/ETH深交叉",		//61
		"BT/WLAN深交叉",	//62
		"磁卡/BT交叉",		//63
		"SMART/BT交叉",		//64
		"打印/BT交叉",		//65
		"多线程类测试",		//66
		"状态栏综合测试",	//67
		"休眠压力测试",		//68
		"USB从/3g深交叉",	//69
		"USB主/3g深交叉",	//70
		"U盘/3g深交叉",		//71
		"MODEM/NFC深交叉",	//72
		"LAN/NFC深交叉",	//73
		"无线/NFC深交叉",	//74
		"BT/NFC交叉",		//75
		"触屏/NFC交叉",		//76
		"NFC综合",			//77
		"NFC/RF交叉",		//78
		"打印/NFC交叉",		//79
		"磁卡/NFC交叉",		//80
		"取电量/NFC交叉",	//81
		"ICSAM/NFC交叉",	//82
		"WIFI AP综合",		//83
		"WIFI/AP交叉",		//84
		"蓝牙主模式综合",	//85
		"摄像头/WLM深交叉",  //86
		"摄像头/USB从交叉",  //87
		"摄像头/USB主交叉",  //88
		"摄像头/U盘交叉",  //89
		"蓝牙打印机综合",	//90
		"音频/无线交叉",	//91
		"SP10_T签名板",		//92
		"休眠/其他模块并发",	//93
		"SP100密码键盘",		//94
		"DBUS多线程",		//95
		"K21_fs性能,压力",		//96
		"HID串口综合",		//97
	};
	//注意与用例号对应!

	char* tmpstr = NULL;
#if SCR_DISABLE
	char tmp[64] = {0};
#endif

	/*process body*/
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C不支文件系统为节省代码空间进行条件编译 20170213 linwl
#if  STABAR_ENABLE//20140626 linwl added
	int ret = -1;
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_WLSIGNAL|STATUSBAR_DISP_BATTERY|STATUSBAR_DISP_DATE|STATUSBAR_DISP_TIME|STATUSBAR_DISP_ETH|STATUSBAR_DISP_MODEM|STATUSBAR_DISP_WIFI|STATUSBAR_DISP_BLUETOOTH))!=NDK_OK)
	//if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_ALL))!=NDK_OK)//all 不是指包含所有的状态图标,所以不能用all
	{
		cls_show_msg("状态栏显示失败!(%d)", ret);
		return;
	}
#endif

	if(auto_flag==1)//autotest_flag
	{
		cls_show_msg1(1, "自动化综合测试中...");
		//自动测试
		g_keeptime = 3;//出错等待时间设置成1秒
		g_SequencePressFlag = 1;
								
		cls_printf("全局连续压力测试次数:");
		if ((nRet=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen, INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||nRet==NDK_ERR_TIMEOUT)
			g_CycleTime = 100;
		else
			g_CycleTime = atoi(CntStr);

		cls_printf("全局连续压力通讯包长:");
		if ((nRet=lib_kbgetinput(PackageLen, 2, sizeof(PackageLen)-1, &unLen, INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||nRet==NDK_ERR_TIMEOUT)
			g_PackLen = 10000;
		else
			g_PackLen = atoi(PackageLen);
		process(list, id);
		return;
	}
	else
#endif
	{
	//手动测试
#if K21_ENABLE
	cls_show_msg1(2, "综合测试...\n" "7 键:往回翻页\n" "8 键:往后翻页\n" "ESC-退出0-配置");
#else
	cls_show_msg1(2, "综合测试...\n" "↑键:往回翻页\n" "↓键:往后翻页\n" "ESC-退出0-配置");
#endif
	cls_show_msg1(5, "注意:压力次数不建议改成比默认值还小!否则可能测不出问题!");

	strcpy((char *)select_id, (char *)id);
	while (1)
	{
		maxpage = (strlen((char *)select_id)-1)/ITEMPERPAGE;//从0开始计算(第一页码为0)
			
		//综合测试选择界面
		NDK_ScrClrs();//clrscr();
		for (cnt=0; cnt<ITEMPERPAGE; cnt++)
		{
			menuid = select_id[cnt+ITEMPERPAGE*nPageSel]-1;
			if (menuid==-1)
				break;
			else if (menuid>SZ_ARRAY(menustr)-1)
				tmpstr = "未定义的测试";
			else
				tmpstr = menustr[menuid];
			
#if SCR_DISABLE	//重定向到串口
			memset(tmp,0,sizeof(tmp));
			sprintf(tmp, "%d.%s\n", cnt+1, tmpstr);
#if  0//ANDIROD_ENABLE
			NDK_AZPortWrite(g_com, strlen(tmp), tmp);
#else
			NDK_PortWrite(g_com, strlen(tmp), tmp);
#endif
#endif
			NDK_ScrPrintf("%d.%s\n", cnt+1, tmpstr);//printf("%d.%s\n", cnt+1, tmpstr);
		}
		NDK_ScrRefresh();

		//处理按键
		switch ((nKeyin=lib_getch()))
		{
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
		case '0'://全局配置
			g_conf();
			break;
#endif
		case '1':
		case '2':
		case '3':
		case '4':
			//确定nKeyin的选择对应的select_id的序号sn
			if((sn=nKeyin-'0'+ITEMPERPAGE*nPageSel)>strlen((char *)select_id))
				break;
			else
			{
				//确定select_id[sn]在id[]中的位置sn1
				sn1 = GetIDInCharArray(select_id[sn-1], id, strlen((char *)id));
				//调用sn1号用例
				(*(list+sn1))();//(*(list+nKeyin-'1'+ITEMPERPAGE*nPageSel))();
			}
			break;
		case '9'://zhangnw20111215增加快速选择测试
			if (BTN_OK==ShowMessageBox("——提示——\n进行快选测试吗?", BTN_OK|BTN_CANCEL, -1))
			{
				nPageSel = 0;
				sn = select_test(select_id, MAXID);//只能选出一个原用例集合的子集
				if(!IsAPartOfB(select_id, sn, id, strlen((char *)id)))//校验select_id是否OK
				{
					cls_show_msg("快选用例号存在错误!任意键退出综合测试...");
					return;
				}
			}
			break;
#if K21_ENABLE
		case '7':
#else
		case DOWNKEY:
#endif
			/*下翻↓*/
			if (nPageSel>=maxpage)
				nPageSel = 0;
			else
				nPageSel++;
			break;
#if K21_ENABLE
		case '8':
#else
		case UPKEY:
#endif
			/*上翻↑*/
			if (nPageSel==0)
				nPageSel = maxpage;
			else
				nPageSel--;
			break;
		case ESC:
#if STABAR_ENABLE	//20140626 linwl added
			NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
			return;
			break;
		default:
			continue;
			break;
		}
	}
	}
}

//===============以下函数为综合模块共用的,相当于综合的lib.c=========
#if !defined ME66 //ME66空间小 不支持 不编译
#if PRN_ENABLE
//获取打印机状态(用于阻塞忙状态(阻塞时间为timeout)
//OK或OVER时,转义为SUCC(0);其它的不转义
//此行功能不实现！"忙时过长,返回TIMEOUT(-3);其它(错误)状态一律返回FAIL(-1)(具体出错信息本函数已会提示了)"
int GetPrinterStatus(unsigned int timeout)
{
	/*private & local definition*/
	EM_PRN_STATUS prnstatus = 0;
	int busy = FALSE, ret = 0;
	time_t oldtime = 0, diff = 0;
	//time_t now = 0;

	 /*process body*/
	 //异常状态才提示,正常态无须提示
	while(1)
	{
		if((ret=NDK_PrnGetStatus(&prnstatus))!=NDK_OK)//prnstatus = getprinterstatus();
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:获取打印机状态失败%d", __FILE__, __LINE__, ret);
			return ret;
		}

		switch(prnstatus)
		{ 
		case PRN_STATUS_NOPAPER:
			if(cls_show_msg_record(FILENAME ,FUNCNAME ,"打印机缺纸!装纸后,任意键继续,ESC放弃")==ESC)
			{
				NDK_ScrClrs();//clrscr();
				return prnstatus;//FAIL;
			}
			NDK_PrnStart();//print("\f");//缺纸后,数据被丢弃.为了装纸后,回到正常的状态(OK或OVER)
			NDK_ScrClrs();//clrscr();
			break;
		case PRN_STATUS_OVERHEAT:
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机过热\n", __FILE__, __LINE__);
			NDK_ScrClrs();//clrscr();
			return prnstatus;//FAIL;
			break;
		case PRN_STATUS_BUSY:
			cls_printf("正在打印,请稍候...");
			if(busy==FALSE)
			{
				oldtime = time(NULL);//从第一次收到忙状态开始计时
				busy = TRUE;
			}
			if((diff=time(NULL)-oldtime)>timeout)//if((diff=(now=time(NULL))-oldtime)>timeout)
			{
				cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机处于忙状态时间过长,请检查\n", __FILE__, __LINE__);
				NDK_ScrClrs();//clrscr();
				return prnstatus;//TIMEOUT;
			}
			show_stopwatch(ENABLE, diff);
			break;
		case PRN_STATUS_OK:
			show_stopwatch(DISABLE, 0);
			return SUCC;
			break;
		case PRN_STATUS_VOLERR:
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机电源故障\n", __FILE__, __LINE__);
			NDK_ScrClrs();//clrscr();
			return prnstatus;//FAIL;
			break;
		case PRN_STATUS_DESTROYED:
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机损坏\n", __FILE__, __LINE__);
			NDK_ScrClrs();
			return prnstatus;
			break;
		default:
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:其他错误(%d)\n", __FILE__, __LINE__, prnstatus);
			NDK_ScrClrs();//clrscr();
			return prnstatus;//FAIL;
			break;
		}
	}

	show_stopwatch(DISABLE, 0);
	NDK_ScrClrs();//clrscr();
	return SUCC;
}

//打印左右大斜三角
//注意:本例测试中仅使用压缩模式来打印
void print_triangle(int nPageWidth)
{
	/*private & local definition*/
#if !(defined ME31||ANDIROD_ENABLE)//||defined IM81||defined N900
	unsigned char buf[426*424/8] = {0}; //缓冲大小384*384/8->426*424/8,这样子允许输入越界的宽度,而缓冲不会跑飞
#else
	unsigned char buf[48*48] = {0}; //K21空间限制缓冲区只能2K左右
	nPageWidth = 48;//将K21三角块的高度限制成48，否则缓冲太大会死机 20140520 linwl
#endif
	unsigned char mask = 0;
	unsigned int i = 0, j = 0, k = 0, bytesPerLine = 0;
	int ret = 0;
	
	/*process body*/
	nPageWidth = MIN(nPageWidth, 424);//防止输入参数nPageWidth过大时,引起缓冲区溢出
	NDK_PrnSetMode(PRN_MODE_NORMAL, 0);//setprintmode(7, 0);
	NDK_PrnStr("---以下打印左右大斜三角---\n");//print("---以下打印左右大斜三角---\n");
	
	//大斜三角(左)---------
	memset(buf,0,sizeof(buf));	
	bytesPerLine =(nPageWidth+7)/8;	// 每行字节数
	for(i=0;i<nPageWidth;i++)
	{
		for(j=0;j<i/8;j++)
			buf[i*bytesPerLine+j]=0xFF;
		
		mask = 0x80;
		
		for(k=0;k<=(i&7);k++)
		{
			buf[i*bytesPerLine+j]|=mask;
			mask >>= 1;
		}
	}	
	if((ret=NDK_PrnImage(nPageWidth,nPageWidth,0,(char *)buf))!=NDK_OK)//if(printimage(nPageWidth,nPageWidth,0,(char *)buf)<0)
	{
		cls_show_msg("%s, line %d:打印左斜三角失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	
	//大斜三角(右)---------
	memset(buf,0xff,sizeof(buf));
	for(i=0;i<nPageWidth;i++)
	{
		for(j=0;j<(nPageWidth-i)/8;j++)
			buf[i*bytesPerLine+j]=0x00;

		mask = 0x7F;
		
		for(k=0;k<((nPageWidth-i)&7);k++)
		{
			buf[i*bytesPerLine+j]&=mask;
			mask >>= 1;
		}
	}
	if((ret=NDK_PrnImage(nPageWidth,nPageWidth,0,(char *)buf))!=NDK_OK)//if(printimage(nPageWidth,nPageWidth,0,(char *)buf)<0)
	{	
		cls_show_msg("%s, line %d:打印右斜三角失败(%d)", __FILE__, __LINE__, ret);
		return;
	}	
	NDK_PrnStr("\n");//print("\n");
	NDK_PrnStart();//print("\f");
	return;
}

void print_triangle_384(void)
{
	/*private & local definition*/
	int type = 0;//EM_PRN_TYPE type = 0;
	int MAXPRLINEWIDE = 0;

	/*process body*/
	type = lib_getprintertype();//NDK_PrnGetType(&type);
	MAXPRLINEWIDE = (PRN_TYPE_TP==type)?MAXPRLINEWIDE_TP:((PRN_TYPE_HIP==type)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);
	print_triangle(MAXPRLINEWIDE);
}

//打印测试页
void print_testpage(void)
{
	/*private & local definition*/
	const char *p = NULL;
	int ret = 0;

	/*process body*/
	if(time(NULL)&1)
		p = CCB_BMP;
	else 
		p = NL_BMP;//原先为NL_Icon,是有问题的!
	
	NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE, 1);//setprintmode(0,0);   //正常双向
	NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32);//setprintfont(DOT16x32+(HZ32x32<<8));	
	NDK_PrnStr("---以下打印测试页---\n");//print("---以下打印测试页---\n");
	
	if((ret=NDK_PrnImage(128,64,0,p))!=NDK_OK)//if(printimage(128,64,0,p)!=SUCC)
	{
		cls_show_msg("%s, line %d:打印图形失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	
	if((ret=NDK_PrnStr("欢迎使用福建新大陆电脑公司NL系列POS热敏打印机"))!=NDK_OK)//if(print("欢迎使用福建新大陆电脑公司NL系列POS热敏打印机")!=SUCC)
	{
		cls_show_msg("%s, line %d:送汉字失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	
	NDK_PrnSetFont(0, PRN_ZM_FONT_16x32);//setprintfont(DOT16x32);
	if((ret=NDK_PrnStr("\n[16x32] !\"#$%&()*./0123456789;<=>?@ABCDEFG\\^_`abcdejfgz{|}~"))!=NDK_OK)//if(print("\n[16x32] !\"#$%&()*./0123456789;<=>?@ABCDEFG\\^_`abcdejfgz{|}~")!=SUCC)
	{
		cls_show_msg("%s, line %d:送16x32字符失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
		
	NDK_PrnSetFont(0, PRN_ZM_FONT_16x16);//setprintfont(DOT16x16);
	if((ret=NDK_PrnStr("\n[16x16] !\"#$%&()*+,-./0123456789;<=>?@ABCDJKLPQRSTUVW\\^_`abcdejfgmnoxyz{|}~"))!=NDK_OK)//if(print("\n[16x16] !\"#$%&()*+,-./0123456789;<=>?@ABCDJKLPQRSTUVW\\^_`abcdejfgmnoxyz{|}~")!=SUCC)
	{
		cls_show_msg("%s, line %d:送16x16字符失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	
	NDK_PrnSetFont(0, PRN_ZM_FONT_6x8);//setprintfont(DOT6x8);
	if((ret=NDK_PrnStr("\n[6x8] !\"#$%&()*+-/,.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdejfghijklmnopqrstuvwyz{|}~"))!=NDK_OK)//if(print("\n[6x8] !\"#$%&()*+-/,.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdejfghijklmnopqrstuvwyz{|}~")!=SUCC)
	{
		cls_show_msg("%s, line %d:送6x8字符失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
		
	NDK_PrnSetFont(PRN_HZ_FONT_16x16, 0);//setprintfont(HZ16x16<<8); 
	if((ret=NDK_PrnStr("\n[16x16] 惜缘 惜福 惭愧 感恩!"))!=NDK_OK)//if(print("\n[16x16] 惜缘 惜福 惭愧 感恩!")!=SUCC)
	{
		cls_show_msg("%s, line %d:送16x16汉字失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	
	NDK_PrnSetFont(PRN_HZ_FONT_12x16, PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ12x16<<8));  
	if((ret=NDK_PrnStr("\n[12x16] 惜缘 惜福 惭愧 感恩!\n"))!=NDK_OK)//if(print("\n[12x16] 惜缘 惜福 惭愧 感恩!\n")!=SUCC)
	{
		cls_show_msg("%s, line %d:送12x16汉字失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	
	NDK_PrnSetFont(PRN_HZ_FONT_16x8, PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ16x8<<8));  
	if((ret=NDK_PrnStr("[16x8] 惜缘 惜福 惭愧 感恩!\n"))!=NDK_OK)//if(print("[16x8] 惜缘 惜福 惭愧 感恩!\n")!=SUCC)
	{
		cls_show_msg("%s, line %d:送16x8汉字失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	
	NDK_PrnSetFont(0, PRN_ZM_FONT_10x8);//setprintfont(DOT10x8);   //横向压缩打印
	if((ret=NDK_PrnStr("[10x8]!\"#$%&()*+-/,.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdejfghijklmnopqrstuvwyz{|}~\n"))!=NDK_OK)//if(print("[10x8]!\"#$%&()*+-/,.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdejfghijklmnopqrstuvwyz{|}~\n")!=SUCC)
	{
		cls_show_msg("%s, line %d:送10x8字符失败(%d)", __FILE__, __LINE__, ret);
		return;
	}   

	//setprintmode(0,0);//setprintmode(6,1);    
	NDK_PrnSetFont(0, PRN_ZM_FONT_10x16);//setprintfont(DOT10x16);   //纵横向压缩打印
	if((ret=NDK_PrnStr("[10x16]!\"#$%&()*+-/,.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwyz{|}~\n"))!=NDK_OK)//if(print("[10x16]!\"#$%&()*+-/,.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwyz{|}~\n")!=SUCC)
	{
		cls_show_msg("%s, line %d:送10x16字符失败(%d)", __FILE__, __LINE__, ret);
		return;
	} 
	
	//setprintmode(0,0);//setprintmode(3,1);   
	NDK_PrnSetFont(0, PRN_ZM_FONT_5x16);//setprintfont(DOT5x16);  //纵向压缩双向
	if((ret=NDK_PrnStr("[5x16]!\"#$%&()*+-/,.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwyz{|}~\n"))!=NDK_OK)//if(print("[5x16]!\"#$%&()*+-/,.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwyz{|}~\n")!=SUCC)
	{
		cls_show_msg("%s, line %d:送5x16字符失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	
	NDK_PrnSetFont(0, PRN_ZM_FONT_5x7);//setprintfont(DOT5x7);
	if((ret=NDK_PrnStr("[5x7] !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz{|}~\n"))!=NDK_OK)//if(print("[5x7] !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz{|}~\n")!=SUCC)
	{
		cls_show_msg("%s, line %d:送5x7字符失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
		
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

//打印随机数
void print_rand(void)
{
	/*private & local definition*/
	char buf[1024] = {0};
	int i = 0, ret = 0;

	/*process body*/
	memset(buf,0,sizeof(buf));
	for (i=0;i<sizeof(buf);i++)
	{
		if(!(buf[i]=rand()%256))
			break;
	}
	
	print_Ex("---以下打印%d个随机数---\n", i);
	if((ret=NDK_PrnStr(buf))!=NDK_OK)//if(print(buf)!=SUCC)
	{
		cls_show_msg("%s, line %d:打印随机数失败!(%d)", __FILE__, __LINE__, ret);
		return;
	}

	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

//打印20行"国"字
#define	NUM_GUO	(240)
void print_guo(void)
{
	/*private & local definition*/
	int i = 0, ret = 0;

	/*process body*/
	NDK_PrnSetFont(PRN_HZ_FONT_16x16, PRN_ZM_FONT_8x8);//setprintfont(DOT8x8+(HZ16x16<<8));//setprintfont(DOT8x8+(HZ24x24<<8));
	print_Ex("---以下打印%d个\"国\"---\n", NUM_GUO);
	
	for(i=0;i<NUM_GUO;i++)
	{
		if((ret=NDK_PrnStr("国"))!=NDK_OK)//if(print("国")!=SUCC)
		{
			cls_show_msg("%s, line %d:打印\"国\"字页失败!(%d)", __FILE__, __LINE__, ret);
			return;
		}
	}
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

//打印填充单
void print_fill(void)
{
	/*private & local definition*/
	char printbuf[(128*64)/8] = {0};
	int ret = -1;
	
	/*process body*/
	NDK_PrnStr("---以下打印填充单---\n");//print("---以下打印填充单---\n");
	
	memset(printbuf,0xff,sizeof(printbuf));
	if((ret=NDK_PrnImage(128,64,0,printbuf))!=NDK_OK)//if(printimage(128,64,0,printbuf)!=SUCC)
	{
		cls_show_msg("%s, line %d:打印填充单失败(%d)", __FILE__, __LINE__, ret);
		return;
	}

	NDK_PrnStr("\n");//print("\n");
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

//打印票据
void print_bill(void)
{
	/*private & local definition*/
//	struct tm stTestTime; //struct postime stTestTime;
	char psBuf[125]={0};
	uint unLen = 0;
	//char szTmp[256] = {0};
	
	/*process body*/

#if 1//20131128,根据银联《销售点POS终端硬件规范》征求意见稿修改bill单据
	if(NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO, &unLen, psBuf)!=NDK_OK||psBuf[10]==0xff)
	{
		cls_show_msg("%s, line %d:测试失败", __FILE__, __LINE__);
		return;
	}
	if(psBuf[10]>0 && psBuf[10]<0x80) //热敏
	{
		NDK_PrnSetFont(PRN_HZ_FONT_24x24A,PRN_ZM_FONT_12x24A);
		NDK_PrnSetForm(0,0,6);
		NDK_PrnSetMode(PRN_MODE_NORMAL,1);
		NDK_PrnFeedByPixel(50); //先走纸50像素点(根据高端产品出现的客诉问题新增) 20180428 add
		NDK_PrnStr("1:银联POS签购单\n");
		NDK_PrnStr("2: --------------------------\n");
		NDK_PrnStr("3:持卡人存根  CUSTOMER COPY\n");
		NDK_PrnStr("4: ---------------------------\n");
		NDK_PrnStr("5:商户名称(MERCHANT Name):\n 6:测试商户名称\n");
		NDK_PrnStr("7:商户号(MERCHANT NO.):\n  8:1234567890123\n");
		NDK_PrnStr("9:终端编号(TERMINAL NO.):123456\n");
		NDK_PrnStr("10:操作员号(Operator NO.):01\n");
		NDK_PrnStr("11: -------------------------\n");
		NDK_PrnStr("12:收单行(ACQUIRER):\n 13:收单银行 123456\n");
		NDK_PrnStr("14:发卡行(ISSUER):发卡银行12345\n");
		NDK_PrnStr("15:卡号(CARD NO.):\n");
		NDK_PrnSetFont(PRN_HZ_FONT_24x24A,PRN_ZM_FONT_16x32A);
		NDK_PrnStr("16:6200123456789012\n\n");
		NDK_PrnSetFont(PRN_HZ_FONT_24x24A,PRN_ZM_FONT_12x24A);
		NDK_PrnStr("17:交易类型(TRANS TYPE):消费\n");
		NDK_PrnStr("18:有效期(EXP Date):2015/01\n");
		NDK_PrnStr("19:批次号(BATCH NO.):12345\n");
		NDK_PrnStr("20:凭证号(VOUCHER NO.):789\n");
		NDK_PrnStr("21:授权码(AUTH NO.):345678\n");
		NDK_PrnStr("22:交易参考号(REFER NO.):\n  23:9012345678\n");
		NDK_PrnStr("24:日期/时间(DATE/TIME):\n  25:2019/07/18 12:34:56\n");
		NDK_PrnStr("26:交易金额(AMOUNT):");
		NDK_PrnSetFont(PRN_HZ_FONT_24x24A,PRN_ZM_FONT_16x32A);
		NDK_PrnStr("12345.90\n");	
		NDK_PrnSetFont(PRN_HZ_FONT_24x24A,PRN_ZM_FONT_12x24A);
		NDK_PrnStr("27: -------------------------\n");
		NDK_PrnStr("28:应用标识(AID)123456789\n");
		NDK_PrnStr("29:备注/REFERENCE\n");
		NDK_PrnStr("30:这里是发卡银行备注信息\n");
		NDK_PrnStr("31:这里是发卡银行备注信息\n");
		NDK_PrnStr("32:这里是发卡银行备注信息\n");
		NDK_PrnStr("33:郑重申明:该笔交易为测试状态,属无效交易!\n");
		NDK_PrnStr("34: -------------------------\n");
		NDK_PrnStr("35:持卡人签名:\n\n\n\n\n");
		NDK_PrnStr("36:本人确认以上交易，同意将其记入本卡账户\n");
		NDK_PrnStr("37:I ACKNOWLEDGE SATISFACTORY RECEIPT OF RELATIVE GOODS/SERVICE\n");
		NDK_PrnSetFont(PRN_HZ_FONT_24x24A,PRN_ZM_FONT_8x16);
		NDK_PrnStr("\n\n");
		NDK_PrnStr("结束: - - - - - X - - - - -- X - - - - --\n\n\n\n\n\n");
		NDK_PrnStart();
	}
	else
	{
		NDK_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_8x16);
		NDK_PrnSetForm(0,0,2);
		NDK_PrnSetMode(PRN_MODE_NORMAL,1);
		NDK_PrnFeedByPixel(50); //先走纸50像素点
		NDK_PrnStr("银联POS签购单\n");
		NDK_PrnStr("--------------------------------\n");
		NDK_PrnStr("持卡人存根CUSTOMER COPY\n");
		NDK_PrnStr("--------------------------------\n");
		NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);
		NDK_PrnStr("商户名称(MERCHANT Name):\n  测试商户名称\n");
		NDK_PrnStr("商户号(MERCHANT NO.):\n  123456789012345\n");
		NDK_PrnStr("终端编号(TERMINAL NO.):12345678\n");
		NDK_PrnStr("操作员号(Operator NO.):01\n");
		NDK_PrnStr("--------------------------------\n");
		NDK_PrnStr("收单行(ACQUIRER):\n  收单银行 12345678\n");
		NDK_PrnStr("发卡行(ISSUER):发卡银行 12345678\n");
		NDK_PrnStr("卡号(CARD NO.):\n");
		NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_10x16);
		NDK_PrnStr("  6200123456789012345\n");
		NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);
		NDK_PrnStr("交易类型(TRANS TYPE):消费/SALE\n");
		NDK_PrnStr("有效期(EXP Date):2015/01\n");
		NDK_PrnStr("批次号(BATCH  NO.):123456\n");
		NDK_PrnStr("凭证号(VOUCHER  NO.):789012\n");
		NDK_PrnStr("授权码(AUTH  NO.):345678\n");
		NDK_PrnStr("交易参考号(REFER NO.):\n  901234567890\n");
		NDK_PrnStr("日期/时间(DATE/TIME):\n  2019/07/18  12:34:56\n");
		NDK_PrnStr("交易金额(AMOUNT):");
		NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_16x32);
		NDK_PrnStr("12345678.90\n");	
		NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);
		NDK_PrnStr("--------------------------------\n");
		NDK_PrnStr("应用标识(AID)1234567890123456\n");
		NDK_PrnStr("备注/REFERENCE\n");
		NDK_PrnStr("这里是发卡银行备注信息\n");
		NDK_PrnStr("这里是发卡银行备注信息\n");
		NDK_PrnStr("这里是发卡银行备注信息\n");
		NDK_PrnStr("郑重申明:该笔交易为测试状态,属于无效交易,只供测试使用\n");
		NDK_PrnStr("--------------------------------\n");
		NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_16x16);
		NDK_PrnStr("持卡人签名:\n\n\n\n\n");
		NDK_PrnStr("本人确认以上交易，同意将其记入本卡账户\n");
		NDK_PrnStr("I ACKNOWLEDGE SATISFACTORY RECEIPT OF RELATIVE GOODS/SERVICE\n");
		NDK_PrnSetFont(PRN_HZ_FONT_24x24A,PRN_ZM_FONT_8x16);
		NDK_PrnStr("\n\n");
		NDK_PrnStr("- - - - - - - X - - - - - - - X - - - - - - - \n\n\n\n\n\n");
		NDK_PrnStart();
	}
	
#else
	NDK_SysGetPosTime(&stTestTime);//getpostime(&stTestTime);
	
	NDK_PrnSetMode(PRN_MODE_NORMAL,1);//setprintmode(6,2);
	NDK_PrnStr("---以下打印票据---\n");//print("---以下打印票据---\n");
	
#if 0
	NDK_PrnStr("\n\n金融IC卡测试单据\n 新大陆POS开发部");//print("\n\n金融IC卡测试单据\n 新大陆POS开发部");
	NDK_PrnStr("\n\nAID:   PBOC1\n    A00000000386980701"); //print("\n\nAID:   PBOC1\n    A00000000386980701");  
	NDK_PrnStr("\n\n  电子存折");//print("\n\n  电子存折");
	NDK_PrnStr("\r0002 1998 0817 0000 0001");//print("\r0002 1998 0817 0000 0001");
	NDK_PrnStr("\r ISSUER:11112222033330002");//print("\r ISSUER:11112222033330002");
	NDK_PrnStr("\n    圈提");//print("\n    圈提");
	sprintf(szTmp,"\n\n %4d年%02d月%02d日   %02d时%02d分%d秒",
						stTestTime.tm_year+1900,
						stTestTime.tm_mon+1,
						stTestTime.tm_mday,			
						stTestTime.tm_hour,
						stTestTime.tm_min,
						stTestTime.tm_sec);			
	NDK_PrnStr(szTmp);//print(szTmp);
	NDK_PrnStr("\r\r    000041       000189"
		"\r\r    00000000\r\r    RMB:       147.85\r");//print("\r\r    000041       000189"
		"\r\r    00000000\r\r    RMB:       147.85\r");
	NDK_PrnStr("\r\r\r      同意\r\r\r 签名：________");//print("\r\r\r      同意\r\r\r 签名：________");
	NDK_PrnStart();//print("\f"); 
#else
	NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32);//setprintfont(DOT16x32+(HZ32x32<<8));
	NDK_PrnStr("      POS签购单\r");//print("      POS签购单\r");
	NDK_PrnSetFont(PRN_HZ_FONT_16x16, PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ16x16<<8));
	NDK_PrnStr("                商户存根/MERCHANT COPY\r");//print("                商户存根/MERCHANT COPY\r");
	//商户名称
	NDK_PrnStr("商户名称(MERCHANT NAME):中国银联\r\r");//print("商户名称(MERCHANT NAME):中国银联\r\r");
	// 商户号
	NDK_PrnStr("商户号(MERCHANT NO.:):\r");//print("商户号(MERCHANT NO.:):\r");
	
	NDK_PrnSetFont(PRN_HZ_FONT_16x32,PRN_ZM_FONT_16x32);//setprintfont(DOT16x32+(HZ16x32<<8));
	NDK_PrnStr("  123456789123456\r");//print("  123456789123456\r");//print("  123456789123456\r");

	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ16x16<<8));
	// POS编号 操作员编号
	NDK_PrnStr("终端编号(TERMINAL NAME):12345678\r操作员号(OPERSTOR NO):02\r");//print("终端编号(TERMINAL NAME):12345678\r操作员号(OPERSTOR NO):02\r");
	//收单行
	NDK_PrnStr("收单行(ACQUIRER):23456789011\r");//print("收单行(ACQUIRER):23456789011\r");
	//发卡行
	NDK_PrnStr("发卡行(ISSUER):01025678901\r");//print("发卡行(ISSUER):01025678901\r");
	//pos中心号
	NDK_PrnStr("中心号(CENTER):87654321\r");//print("中心号(CENTER):87654321\r");
	// 卡号
	NDK_PrnStr("卡号(CARD NO.):\r"); //print("卡号(CARD NO.):\r"); 
	NDK_PrnSetFont(PRN_HZ_FONT_16x32,PRN_ZM_FONT_16x32);//setprintfont(DOT16x32+(HZ16x32<<8));
	NDK_PrnStr("  6013821600029014090\r");//print("  6013821600029014090\r");
	
#if 0	//for 11cm
	//商户名称
	NDK_PrnStr("商户名称(MERCHANT NAME):中国银联\r\r");
	// 商户号
	NDK_PrnStr("商户号(MERCHANT NO.:):\r");
	NDK_PrnSetFont(PRN_HZ_FONT_16x32,PRN_ZM_FONT_16x32);
	NDK_PrnStr("  123456789123456\r");

	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);
	// POS编号 操作员编号
	NDK_PrnStr("终端编号(TERMINAL NAME):12345678\r操作员号(OPERSTOR NO):02\r");
	//收单行
	NDK_PrnStr("收单行(ACQUIRER):23456789011\r");
	//发卡行
	NDK_PrnStr("发卡行(ISSUER):01025678901\r");
	//pos中心号
	NDK_PrnStr("中心号(CENTER):87654321\r");
	// 卡号
	NDK_PrnStr("卡号(CARD NO.):\r"); 
	NDK_PrnSetFont(PRN_HZ_FONT_16x32,PRN_ZM_FONT_16x32);
	NDK_PrnStr("  6013821600029014090\r");	
#endif	

	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ16x16<<8));
	// 交易类型
	NDK_PrnStr("交易类型(TRANS TYPE):消费\r");//print("交易类型(TRANS TYPE):消费\r");
	// 有效期
	NDK_PrnStr("有效期(EXP DATE):2038/12\r");//print("有效期(EXP DATE):2038/12\r");
	
	//批次号、凭证号
	NDK_PrnStr("批次号(BATCH NO.):800011\r凭证号(VOUCHER NO.):000107\r");//print("批次号(BATCH NO.):800011\r凭证号(VOUCHER NO.):000107\r");
	// 授权码、系统参考号
	NDK_PrnStr("授权码(AUTH NO.):author\r参考号(REFER NO.):0123456788\r");//print("授权码(AUTH NO.):author\r参考号(REFER NO.):0123456788\r");
	// 交易日期、时间		
	//print("日期时间(DATE/TIME):\r   2009/04/14   11:22:22\r");
	print_Ex("\n\n %4d年%02d月%02d日   %02d时%02d分%d秒",
						stTestTime.tm_year+1900,
						stTestTime.tm_mon+1,
						stTestTime.tm_mday,			
						stTestTime.tm_hour,
						stTestTime.tm_min,
						stTestTime.tm_sec);
	// 外卡组织代码
	NDK_PrnStr("国际卡组织:DJJ\r");//print("国际卡组织:DJJ\r");
	// 打印交易金额
	NDK_PrnSetFont(PRN_HZ_FONT_16x8,PRN_ZM_FONT_16x32);//setprintfont(DOT16x32+(HZ32x16<<8));
	NDK_PrnStr("金额:RMB222.22\r");//print("金额:RMB222.22\r");		
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ16x16<<8));
	NDK_PrnStr("备注(REFERENCE):\r");//print("备注(REFERENCE):\r");
	NDK_PrnStr("郑重申明:该笔交易为测试状态,属无效交易!\r");//print("郑重申明:该笔交易为测试状态,属无效交易!\r");
	NDK_PrnStr("持卡人签名(CARD HOLDER SIGHATURE):\r\r\r\r\r\r");//print("持卡人签名(CARD HOLDER SIGHATURE):\r\r\r\r\r\r");
	NDK_PrnStr("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\r");//print("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\r");
	NDK_PrnStr("本人确认以上交易，同意将其记入本卡账户\r\r");//print("本人确认以上交易，同意将其记入本卡账户\r\r");
	NDK_PrnStr("\r\r\r\r------- X-------X-------\r\r\r\r\r");//print("\r\r\r\r------- X-------X-------\r\r\r\r\r");
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
#endif

	NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE, 1);//setprintmode(0, 0);
#endif
	return;
}

//打印竖条纹
//注意:本例测试中只使用压缩模式来打印
void print_verticalbill(void)
{
	/*private & local definition*/
	/*unsigned */char buf_img[PRNIMGBUFLEN] = {0};
	int type = 0;//EM_PRN_TYPE type = 0;
	char c = 0;
	int ret = -1, i = 0,xsize = 0;// type = getprintertype(), xsize = (PRINTTYPE_TP==type)?MAXPRLINEWIDE_TP:((PRINTTYPE_HIP==type)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);
	
	/*process body*/
	type = lib_getprintertype();//NDK_PrnGetType(&type);
	xsize = (PRN_TYPE_TP==type)?MAXPRLINEWIDE_TP:((PRN_TYPE_HIP==type)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);
	
	NDK_PrnSetMode(PRN_MODE_NORMAL, 0);//setprintmode(7, 0);
	NDK_PrnStr("---以下打印竖条纹---\n");//print("---以下打印竖条纹---\n");

	c = (rand()&1)?0xaa:0x55;
	for(i=0;i<PRNIMGBUFLEN;i++)
	{
		buf_img[i]=c;
		c^=0xff;
	}
	if((ret=NDK_PrnImage(xsize,PRNIMGHEIGHT,0,buf_img))!=NDK_OK)//if(printimage(xsize,PRNIMGHEIGHT,0,(char *)buf_img)!=SUCC)
	{
		cls_show_msg("%s, line %d:打印图形失败(%d)", __FILE__, __LINE__, ret);
		return;
	}
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

//打印空白单
void print_blank(void)
{
	/*private & local definition*/
	int type = 0;//EM_PRN_TYPE type = 0;
	/*unsigned */char buf_img[PRNIMGBUFLEN] = {0};
	//int type = getprintertype() ,xsize = (PRINTTYPE_TP==type)?MAXPRLINEWIDE_TP:((PRINTTYPE_HIP==type)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);
	int i = 0, xsize = 0;
	
	/*process body*/
	type = lib_getprintertype();//NDK_PrnGetType(&type);
	xsize = (PRN_TYPE_TP==type)?MAXPRLINEWIDE_TP:((PRN_TYPE_HIP==type)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);
	
	NDK_PrnSetMode(PRN_MODE_NORMAL, 0);//setprintmode(7, 0);
	NDK_PrnStr("---以下打印空白单---\n");//print("---以下打印空白单---\n");

	memset(buf_img,0x00,PRNIMGBUFLEN);
	for(i=0;i<10;i++)
	{
		if(NDK_PrnImage(xsize,PRNIMGHEIGHT,0,buf_img)!=NDK_OK)//if(printimage(xsize,PRNIMGHEIGHT,0,(char *)buf_img)!=SUCC)
		{
			cls_show_msg("%s, line %d:打印空白单失败\n", __FILE__, __LINE__);
			return;
		}
		NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	}
	return;
}

//打印各种字体
void print_font(void)
{
	/*private & local definition*/
#include	"dotfont.h"
#include	"hzfont.h"

	int imode = 0, idot = 0, /*irow_space=2, */ihz = 0, i = 0 ,ret = -1;
	
	struct tm stTestTime;//struct postime stTestTime;
	//char szTmp[256] = {0};
	//char infoprint[1024] = {0};
	
	/*process body*/
	NDK_PrnStr("---以下打印各种字体---\n");//print("---以下打印各种字体---\n");
	
	for(imode=PRN_MODE_ALL_DOUBLE; imode<PRN_MODE_NORMAL; imode++)
	//for(irow_space=0;irow_space<8;irow_space++)    panxl 2007.7.18屏蔽，irow_space赋值为2
	for(; i<2; i++)
	{		
		for(ihz=0; ihz<SZ_ARRAY(hzfont); ihz++)   //for(ihz=0;ihz<strlen(hzfont);ihz++)   panxl 2007.7.18 modify
		{
			for(idot=0; idot<SZ_ARRAY(dotfont); idot++)    //for(idot=0;idot<strlen(dotfont);idot++)  panxl 2007.7.18 modify
			{	
				if((ret=NDK_PrnSetMode(imode,i))!=NDK_OK)//if(setprintmode(imode,irow_space)!=SUCC)
				{
					cls_show_msg("%s, line %d:设置打印模式失败(%d,%d,%d)", __FILE__, __LINE__, imode, i, ret);
					return;
				}
				
				if((ret=NDK_PrnSetFont(hzfont[ihz],dotfont[idot]))!=NDK_OK)//if(setprintfont(dotfont[idot]+(hzfont[ihz]<<8))!=SUCC)
				{
					cls_show_msg("%s, line %d:设置打印字体失败(%s,%s,%d)", __FILE__, __LINE__, dotfontstr[idot], hzfontstr[ihz], ret);
					return;
				}
				
				if(print_Ex("打印模式[%d,%d]\n打印字体[%s,%s]:\n(以下先打图形再打字)\n", imode, i, hzfontstr[ihz], dotfontstr[idot])!=SUCC)
				{
					cls_show_msg("%s, line %d:送数据失败\n", __FILE__, __LINE__);
					return;                        
				}

				if(time(NULL)%2) 
				{
					if((ret=NDK_PrnImage(128,64,0,CCB_BMP))!=NDK_OK)//if(printimage(128,64,0,CCB_BMP)!=SUCC)
					{
						cls_show_msg("%s, line %d:送图形数据失败(%d)", __FILE__, __LINE__, ret);
						return;
					}
				}
				else 
				{
					if((ret=NDK_PrnImage(144,64,0,CCB_BMP_144))!=NDK_OK)//if(printimage(144,64,0,CCB_BMP_144)!=SUCC)
					{
						cls_show_msg("%s, line %d:送图形数据失败(%d)", __FILE__, __LINE__, ret);
						return;
					}
				}
				if((ret=NDK_PrnStr("01234新大陆欢迎您\n0123456NewLandComputer Welcome you!\n~!@#$%^&*()_+|{[}]:;'<,>.?/\\=-dsfh\n"))!=NDK_OK)
				{
					cls_show_msg("%s, line %d:送数据失败(%d)", __FILE__, __LINE__, ret);
					return;                       
				}

				NDK_SysGetPosTime(&stTestTime);//getpostime(&stTestTime);
				if(print_Ex("\n %4d年%02d月%02d日   %02d时%02d分%d秒\n",
					stTestTime.tm_year+1900,
					stTestTime.tm_mon+1,
					stTestTime.tm_mday,			
					stTestTime.tm_hour,
					stTestTime.tm_min,
					stTestTime.tm_sec)!=SUCC)
				{
					cls_show_msg("%s, line %d:送数据失败\n", __FILE__, __LINE__);
					return;
				}

				NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
				
#if 0
				///////////////////////////////////
				//panxl 2007.7.18 add循环控制显示
				cls_show_msg1(1, "imode=%d,\nihz=%d,\nidot=%d",imode,ihz,idot);
				///////////////////////////////////
#endif
				if(GetPrinterStatus(MAXWAITTIME)!=SUCC)
				{
					cls_show_msg("%s, line %d:打印机状态异常!(%d,%d,%d,%d)", __FILE__, __LINE__, imode, i, ihz, idot);
					return;                         
				}					

				//时间太短,捕捉不到按键if(kbhit()==ESC)
				if(wait_anykey(1)==ESC)
					return;
			}
		}
	}

	return;
}

//打印图形
//注意:本例测试中仅使用压缩模式来打印
void print_image(int MageWide, int MageHigh, int MagePon, const char *p)
{
	/*private & local definition*/
	int ret = -1;
	
	/*process body*/
	NDK_PrnSetMode(PRN_MODE_NORMAL, 0);//setprintmode(7, 0);
	NDK_PrnStr("---以下打印图形---\n");//print("---以下打印图形---\n");
	
	//打印出图形和参数	
	print_Ex("起点:%d,宽度:%d,高度:%d\n",MagePon,MageWide,MageHigh);
	if((ret=NDK_PrnImage(MageWide,MageHigh,MagePon,p))!=NDK_OK)//if(printimage(MageWide,MageHigh,MagePon,p)!=SUCC)
	{
		cls_show_msg("%s, line %d:打印图像失败(%d)", __FILE__, __LINE__, ret);
		return;
	}

	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

void print_image_CCB128(void)
{
	/*private & local definition*/

	/*process body*/
	NDK_PrnImage(128,64,0,CCB_BMP);//print_image(128, 64, 0, CCB_BMP);
	
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

//打印回车
void print_enter(void)
{
	/*private & local definition*/
	int i = 0;

	/*process body*/
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,0);//setprintfont(HZ16x16<<8);
	NDK_PrnStr("---以下连续打印5个回车--\n");//print("---以下连续打印5个回车--\n");
	
	for(i=0; i<5; i++)
		NDK_PrnStr("\r");//print("\r");
	NDK_PrnStr("--以下连续打印20个回车--\n");//print("--以下连续打印20个回车--\n");
	for(i=0; i<20; i++)
		NDK_PrnStr("\r");//print("\r");

	NDK_PrnStr("----连续打印回车结束----\n");//print("----连续打印回车结束----\n");
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

//压缩打印 奇偶空白行 ,奇偶文本行,奇偶回车行			
//奇偶空白行
void print_compress(void)
{
	/*private & local definition*/
	int i = 0;
	
	/*process body*/
	NDK_PrnStr("---以下打印压缩效果---\n");//print("---以下打印压缩效果---\n");
	
	NDK_PrnStr("          \r");//print("          \r");
	
	NDK_PrnSetMode(PRN_MODE_WIDTH_DOUBLE,1);//setprintmode(2,1);
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_16x16);//setprintfont(HZ16x16<<8);   //panxl
	cls_printf("\n正在打印,请稍等\n奇偶行\n");
	for (i=1;i<=24;i++)
		NDK_PrnStr("国");	//print("国");	

	NDK_PrnSetMode(PRN_MODE_WIDTH_DOUBLE,0);//setprintmode(3,1);
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,0);//setprintfont(HZ16x16<<8);   //panxl
	NDK_PrnStr("          \r");//print("          \r");
	NDK_PrnStr("          \r");//print("          \r");
	for (i=1;i<=24;i++)
		NDK_PrnStr("国");	//print("国");	
	
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,0);//setprintfont(HZ16x16<<8);   //panxl
	NDK_PrnStr("          \r");//print("          \r");
	NDK_PrnStr("          \r");//print("          \r");
	NDK_PrnStr("          \r");//print("          \r");
	for (i=1;i<=24;i++)
		NDK_PrnStr("国");	//print("国");	

	NDK_PrnStr("\n打印前奇偶空白行\n");//print("\n打印前奇偶空白行\n");
	
	//奇偶文本行
	NDK_PrnSetMode(PRN_MODE_WIDTH_DOUBLE,1);//setprintmode(2,1);
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ16x16<<8));   //panxl
	//NDK_PrnStr("\n\n模式1,行间距1\n");//print("\n\n模式1,行间距1\n");	
	NDK_PrnStr("行间奇偶空格\n");//print("行间奇偶空格\n");
	NDK_PrnStr("国  国   国国国国  国国 \r");//print("国  国   国国国国  国国 \r");
	NDK_PrnStr(" 国 国  国国国  国国 \r");//print(" 国 国  国国国  国国 \r");
	
	//奇偶回车行
	NDK_PrnStr("\r");//print("\r");
	NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE, 1);//setprintmode(0,1);
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,0);//setprintfont(HZ16x16<<8);   //panxl
	for (i=1;i<=24;i++)
		NDK_PrnStr("国");	//print("国");
	
	//NDK_PrnStr("\n模式0,行间距1\n");//print("\n模式0,行间距1\n");	
	NDK_PrnStr("打印前一个回车行\n");//print("打印前一个回车行\n");
	
	NDK_PrnStr("\r\r\r");//print("\r\r\r");
	NDK_PrnSetMode(PRN_MODE_WIDTH_DOUBLE,1);//setprintmode(2,1);
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,0);//setprintfont(HZ16x16<<8);   //panxl
	//    print("          \r");
	
	for (i=1;i<=24;i++)
		NDK_PrnStr("国");	//print("国");

	//NDK_PrnStr("\n模式2,行间距1\n");//print("\n模式2,行间距1\n");	
	NDK_PrnStr("打印前三个回车行\n");//print("打印前三个回车行\n");
	
	NDK_PrnStr("\r\r\r\r");//print("\r\r\r\r");
	NDK_PrnSetMode(PRN_MODE_HEIGHT_DOUBLE,0);//setprintmode(5,1);
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,0);//setprintfont(HZ16x16<<8);   //panxl
	for (i=1;i<=24;i++)
		NDK_PrnStr("国");	//print("国");

	//NDK_PrnStr("\n模式5,行间距1\n");	//print("\n模式5,行间距1\n");
	NDK_PrnStr("打印前四个回车行\n");//print("打印前四个回车行\n");
	
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//while(getprinterstatus()==TPNOTREADY);
	{
		cls_show_msg("%s, line %d:打印机状态异常!", __FILE__, __LINE__);
		return;                         
	}
				
	NDK_PrnSetMode(PRN_MODE_WIDTH_DOUBLE,1);//setprintmode(2,1);
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ12x16<<8));   //panxl	
	cls_printf("测试print函数:\n"
				"\\r后面是其\n他字符"	
				"\n正在打印,请稍等\n");
	//NDK_PrnStr("\n模式2,行间距1\n");//print("\n模式2,行间距1\n");		
	NDK_PrnStr("DOT8x16 HZ12x16\n");//print("DOT8x16 HZ12x16\n");
	for (i=1;i<=48;i++)		
		NDK_PrnStr("国");	//print("国");			

	NDK_PrnStr("\r\r	");	//print("\r\r	"); \r\r后面是TAB键,会造成POS重新启动.
	NDK_PrnStr("\r\r "); //print("\r\r "); \r\r后面是空格,正常打印.
	
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志		
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//while(getprinterstatus()==TPNOTREADY);
	{
		cls_show_msg("%s, line %d:打印机状态异常!", __FILE__, __LINE__);
		return;                         
	}						  
				
#if 0 //在NDK中不适用
	//setprintpagelen(0),在针打POS中是进行打印复位的操作.
	//此函数热敏兼容,但是没有什么应用.
	setprintpagelen(0);//setprintpagelen(300); 
	NDK_PrnSetMode(PRN_MODE_WIDTH_DOUBLE ,0);//setprintmode(3,6);
	NDK_PrnSetFont(PRN_HZ_FONT_12x16,PRN_ZM_FONT_8x16);//setprintfont(DOT8x16+(HZ12x16<<8));   //panxl
	cls_printf("将pagelen设置成0\n"
				"\n正在打印,请稍等\n");
	NDK_PrnStr("\n模式3,行间距6\n");//print("\n模式3,行间距6\n");
	NDK_PrnStr("setprintpagelen(0)\n");//print("setprintpagelen(0)\n");		
	NDK_PrnStr("DOT8x16 HZ12x16\n");	//print("DOT8x16 HZ12x16\n");	
	for (i=1;i<=48;i++)		
		NDK_PrnStr("国");	//print("国");		

	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//while(getprinterstatus()==TPNOTREADY);
	{
		cls_show_msg("%s, line %d:打印机状态异常!", __FILE__, __LINE__);
		return;                         
	}
#endif
	
	return;
}

//打印联迪单据
void print_landi(void)
{
	/*private & local definition*/
	
	/*process body*/
	NDK_PrnSetFont(PRN_HZ_FONT_16x8,PRN_ZM_FONT_8x8);//setprintfont(DOT8x8+(HZ16x16<<8));//setprintfont(DOT8x8+(HZ24x24<<8));
	NDK_PrnSetMode(PRN_MODE_NORMAL,1);//setprintmode(6,2);
	NDK_PrnStr("备份电池：3.02V\n");//print("备份电池：3.02V\n");
	NDK_PrnStr("WELCOME TO POS EQUIPMENT\n");//print("WELCOME TO POS EQUIPMENT\n");	
	NDK_PrnStr("国国国国国国国国国国国国国国国国国国国国国国\n");//print("国国国国国国国国国国国国国国国国国国国国国国\n");
	NDK_PrnStr("国国国国国国国国国国国国国国国国国国国国国国\n");//print("国国国国国国国国国国国国国国国国国国国国国国\n");
	NDK_PrnSetMode(PRN_MODE_HEIGHT_DOUBLE,1);//setprintmode(4,2);
	NDK_PrnStr("国国国国国国国国国国国国国国国国国国国国国国\n");//print("国国国国国国国国国国国国国国国国国国国国国国\n");
	NDK_PrnStr("国国国国国国国国国国国国国国国国国国国国国国\n");//	print("国国国国国国国国国国国国国国国国国国国国国国\n");
	NDK_PrnStr("档位打印效果：\n");//print("档位打印效果：\n");
	NDK_PrnStr("档位4(#2)\n");//print("档位4(#2)\n");
	NDK_PrnStr("||  ||  ||  ||  ||\n");//print("||  ||  ||  ||  ||\n");
	NDK_PrnStr("||  ||  ||  ||  ||\n");//print("||  ||  ||  ||  ||\n");
	NDK_PrnStr("||  ||  ||  ||  ||\n");//print("||  ||  ||  ||  ||\n");
	NDK_PrnStr("各针出针效果\n");//print("各针出针效果\n");
	NDK_PrnStr(" 1 2 3 4 5 6 7 8 9 \n");//print(" 1 2 3 4 5 6 7 8 9 \n");
	NDK_PrnStr("--------------------------------------------------------------------\n");
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}

//打印出现空白走纸的用例
//高端安卓产品使用打印脚本函数验证 20170609新增
#if ANDIROD_ENABLE
void print_whitebill(void)
{
	/*private & local definition*/
	int ret = -1;
	//由于打印脚本函数传送一条数据后会调用NDK_PrnStart,故不能分成几条,而是一整条数据传递,否则会出现感觉卡顿的现象
	char buf_str[] = "!hz l\n !asc l\n !yspace 15\n !gray 10\n*text c 通莞金服\n!hz s\n !asc s\n !yspace 5\n !gray 5\n*line \n*text l 商户存根                             请妥善保管\n*line \n!hz n\n !asc n\n !yspace 10\n !gray 5\n*text l 商户名称:银联商务有限公司福州分公司\n*text l 商户编号:899330160120021\n*text l 终端编号:05315830\n*text l 交易渠道:现金消费\n*text l 交易类型:消费\n*text l 交易时间:2017/02/09 15:51:55\n*text l 支付金额:\n!hz l\n !asc l\n !yspace 15\n !gray 10\n*text l     RMB108\n!hz n\n !asc n\n !yspace 10\n !gray 5\n*text l 交易状态:付款成功\n*text l 订单号:9847355443331596288\n *barcode c 29847355443331596288\n!hz n\n !asc s\n !yspace 5\n !gray 5\n *text l 微支付服务商:通莞金服\n";
	
	/*process body*/
	if((ret = NDK_Script_Print(buf_str,strlen(buf_str)))!= NDK_OK)
	{
		cls_show_msg("%s, line %d:脚本打印失败(%d)", __FILE__, __LINE__,ret);
		return; 						
	}
	return;
}
#else
void print_whitebill(void)
{
	/*private & local definition*/
	
	/*process body*/
	NDK_PrnSetMode(PRN_MODE_NORMAL,1);
	NDK_PrnSetFont(PRN_HZ_FONT_32x32,PRN_ZM_FONT_16x32);
	NDK_PrnSetForm(0,0,15);
	NDK_PrnSetGreyScale(5);
	NDK_PrnStr("        通莞金服        \n");

	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);
	NDK_PrnSetForm(0,0,5);
	NDK_PrnSetGreyScale(3);
	NDK_PrnStr("------------------------------------------------\n");
	NDK_PrnStr("商户存根                             请妥善保管\n");
	NDK_PrnStr("------------------------------------------------\n");

	NDK_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_12x24A);
	NDK_PrnSetForm(0,0,10);
	NDK_PrnSetGreyScale(3);
	NDK_PrnStr("商户名称:银联商务有限公司福州分公司\n");
	NDK_PrnStr("商户编号:899330160120021\n");
	NDK_PrnStr("终端编号:05315830\n");
	NDK_PrnStr("交易渠道:现金消费\n");
	NDK_PrnStr("交易类型:消费\n");
	NDK_PrnStr("交易时间:2017/02/09 15:51:55\n");
	NDK_PrnStr("支付金额:\n");

	NDK_PrnSetFont(PRN_HZ_FONT_32x32,PRN_ZM_FONT_16x32A);
	NDK_PrnSetForm(0,0,15);
	NDK_PrnSetGreyScale(5);
	NDK_PrnStr("    RMB 108\n");

	NDK_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_12x24A);
	NDK_PrnSetForm(0,0,10);
	NDK_PrnSetGreyScale(3);
	NDK_PrnStr("交易状态:付款成功\n");
	NDK_PrnStr("订单号:9847355443331596288\n");
	
	//scriptBuilder.append("*barcode c ").append("29847355443331596288").append("\n");
	NDK_PrnImage(128,64,0,CCB_BMP);
	
	NDK_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_12x12);
	NDK_PrnSetForm(0,0,5);
	NDK_PrnSetGreyScale(3);
	NDK_PrnStr("\n微支付服务商:通莞金服\n");
	
	NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	return;
}
#endif
//打印新国都单据
void print_xinguodu(void)
{
	/*private & local definition*/
	char szTmp[256] = {0};	
	struct tm stTestTime;//struct postime stTestTime;
	
	/*process body*/
	NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE, 1);//setprintmode(0,0);
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_6x8);//setprintfont(HZ16x16<<8|DOT6x8);
	NDK_PrnSetFont(0,PRN_ZM_FONT_6x8);//setprintfont(DOT6x8);
	NDK_PrnStr("\r\r");//print("\r\r");
	NDK_PrnStr("福州市仓山区个性服饰店\r\r");//print("福州市仓山区个性服饰店\r\r");
	NDK_PrnStr("            309350156911410\r");//print("            309350156911410\r");
	NDK_PrnStr("       11100495         01\r\r");//print("       11100495         01\r\r");
	NDK_PrnSetFont(0,PRN_ZM_FONT_8x16);//setprintfont(DOT8x16);
	NDK_PrnStr("   622200*********7950\r\r");//print("   622200*********7950\r\r");
	NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_6x8);//setprintfont(HZ16x16<<8|DOT6x8);
	NDK_PrnSetFont(0,PRN_ZM_FONT_6x8);//setprintfont(DOT6x8);
	NDK_PrnStr("     01020000    兴业银行\r\r");//print("     01020000    兴业银行\r\r");
	NDK_PrnStr("   消费/SALE      2000/00\r\r");//print("   消费/SALE      2000/00\r\r");
	NDK_PrnStr("   000003          000934\r\r");//print("   000003          000934\r\r");
	NDK_PrnStr("   012345\r");//print("   012345\r");
	NDK_SysGetPosTime(&stTestTime);
	sprintf(szTmp,"\n\n %4d年%02d月%02d日   %02d时%02d分%d秒",
						stTestTime.tm_year+1900,
						stTestTime.tm_mon+1,
						stTestTime.tm_mday,			
						stTestTime.tm_hour,
						stTestTime.tm_min,
						stTestTime.tm_sec);	
	NDK_PrnStr(szTmp);//print(szTmp);
	NDK_PrnStr("   151019019346\r");
	NDK_PrnSetFont(0,PRN_ZM_FONT_8x16);//setprintfont(DOT8x16);
	NDK_PrnStr("   RMB 0.01\r\r");
	NDK_PrnStr("   RMB 0.01");
	NDK_PrnStart();//print("\f");
	return;
}

void print_signlfont(void)
{
	/*private & local definition*/
#include	"dotfont.h"
#include	"hzfont.h"

	int ret = -1, i = 0, j = 0, k = 0, l = 0;
	char tmp[100]={0};
	
	for(k= 32;k<=126;k++,l++)
		tmp[l]=k;//赋值ASCII码值
		
	/*process body*/
	for(i=0;i<SZ_ARRAY(hzfont);i++)
	{
		if((ret=NDK_PrnSetFont(hzfont[i],PRN_ZM_FONT_8x16))!=NDK_OK)//if(setprintfont(hzfont[i]<<8)!=SUCC)
		{
			cls_show_msg("%s, line %d:设置打印字体[%s]失败(%d)", __FILE__, __LINE__, hzfontstr[i], ret);
			return;
		}
		if(print_Ex("HZ:[%s]新大陆\n", hzfontstr[i])!=SUCC)
		{
			cls_show_msg("%s, line %d:送数据失败\n", __FILE__, __LINE__);
			return;                        
		}
		NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	}
	//NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)
	{
		cls_show_msg("%s, line %d:打印机状态异常!", __FILE__, __LINE__);
		return;                         
	}
	
	for(j=0;j<SZ_ARRAY(dotfont);j++)
	{
		if((ret=NDK_PrnSetFont(PRN_HZ_FONT_16x16,dotfont[j]))!=NDK_OK)//if(setprintfont(dotfont[j])!=SUCC)
		{
			cls_show_msg("%s, line %d:设置打印字体[%s]失败(%d)", __FILE__, __LINE__, dotfontstr[j], ret);
			return;
		}
		if(print_Ex("字符:[%s]%s\n", dotfontstr[j], tmp)!=SUCC)
		{
			cls_show_msg("%s, line %d:送数据失败\n", __FILE__, __LINE__);
			return;                        
		}
		NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	}
	//NDK_PrnStart();//print("\f");//\f是打印数据完整性标志
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)
	{
		cls_show_msg("%s, line %d:打印机状态异常!", __FILE__, __LINE__);
		return;                         
	}
	return;
}

//本函数用于打印压力测试及以打印为主体的交叉测试
//主要关注函数执行成功否及实际效果
//title:测试标题说明;
//func:额外的测试动作(主要用于交叉测试,若是函数列表,由用户自行封装在func中即可),无需额外测试时,赋NULL
void prn_press(const char *title, PF func)
{
	/*private & local definition*/
#define	DEFAULT_CNT_STR	"1"//"200"	//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(200)
	int cnt = DEFAULT_CNT_VLE, bak = 0, ret = 0;
	uint len = 0;
	char CntStr[8] = DEFAULT_CNT_STR;

	PF pfunc[] = {print_triangle_384, print_testpage, print_rand, print_guo, print_fill, print_bill, 
				print_verticalbill, print_blank, print_font, print_image_CCB128, print_enter, print_compress, print_landi, print_xinguodu};
	char *func_info[] = {"三角块", "测试页", "随机数", "国字页", "填充单", "票据", "竖条纹", 
					"空白单", "模式字体组合", "图形打印", "回车", "压缩杂项", "联迪单据", "新国都单据"};
	int id = 0;
	
	/*process body*/
	cls_printf("默认测试次数:\n");
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
	if ((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	//注意:复位动作仅在进入函数压力前做一次,在之后的不同打印压力过程中不做(即用例间不做复位),以加大压力
	//为了避免其它打印用例的影响，先清一下打印BUF并同时复位一下
	//if(NDK_PrnInit(g_PrnSwitch)!=NDK_OK)//if (FAIL==clrprintbuf())
	if(lib_initprn(g_PrnSwitch)!=NDK_OK)//初始化使开始的配置有效 modify 20131122 jiangym
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
		if (ESC==cls_show_msg1(2, "%s测试中...\n还剩%d次,请注意打印效果,ESC中断测试", title, cnt))
			break;
		cnt--;

		for(id=0; id<SZ_ARRAY(pfunc); id++)
		{
			if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//避免其它用例的干扰
			{
				cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机状态异常!(当前用例:%s)", __FILE__, __LINE__, func_info[id]);
				return;                         
			}
			pfunc[id]();
			if(func!=NULL)//用于与打印进行交叉测试
				func();
			if(wait_anykey(1)==ESC)
				goto OUT;
		}
	}

OUT:
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试完成,已执行次数为%d", title, bak-cnt);
	return;
}
#endif

#if MAG_ENABLE
//TK2_3必测,TK1可选
//本函数对KEYIN做转义为STRIPE(暂不支持)
//刷卡哪道出错,就返回哪道的宏(大于0),故不能以>0来作成功否的判断条件
int MagcardReadTest(int select_TK, int IsDisplayed, int waittime)
{
	/*private & local definition*/
	int ret = 0, errcode = 0, flg = 0x00;
	char buf2[MAXTRACKLEN] = {0}, buf3[MAXTRACKLEN] = {0}, buf1[MAXTRACKLEN] = {0}/*, c = 0*/;

	/*process body*/
	memset(buf2, 0, sizeof(buf2));
	memset(buf3, 0, sizeof(buf3));
	memset(buf1, 0, sizeof(buf1));

	cls_printf("请刷卡(%02ds)--->>\n", waittime);
	ret = readcard3(select_TK, waittime, buf2, buf3, buf1, &errcode);
    //注意:出错信息不受IsDisplayed控制.只要出错,出错信息都显示!
	switch(ret)
	{
	case FAIL:
		cls_show_msg1(2, "%s, line %d:刷卡失败", __FILE__, __LINE__);
		break;
	case NDK_ERR_QUIT://QUIT:
		cls_show_msg1(2, "%s, line %d:用户取消刷卡", __FILE__, __LINE__);
		break;
	case NDK_ERR_TIMEOUT://TIMEOUT:
		cls_show_msg1(2, "%s, line %d:刷卡超时", __FILE__, __LINE__);		
		break;
	case STRIPE:
		//TK1
		if((select_TK&TK1)==TK1)
		{
			if(buf1[0]==0x7f)//读卡错
			{
				cls_show_msg1(g_keeptime, "%s, line %d:1道读卡错![0x7F]\n实际:%2x\nerrcode:0x%x\n", __FILE__, __LINE__, buf1[0], errcode);
				flg |= TK1;
			}
			else if(buf1[0]==0x7e)//无数据
			{
				cls_show_msg1(g_keeptime, "%s, line %d:1道无数据![0x7E]\n实际:%2x\nerrcode:0x%x\n", __FILE__, __LINE__, buf1[0], errcode);
			}
			else 
			{
				NDK_ScrClrs();//clrscr();
				if((errcode&MAGCARD_TK1_LRC_ERR)==MAGCARD_TK1_LRC_ERR)//校验错误
				{    
					NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE, NULL);//settextattr(REVERSE_TEXT);//反显错误值
					flg |= TK1;
				}
				if(IsDisplayed || (flg&TK1)==TK1)
				{
					show_msg1(2, "1磁道数据(%d个):%s\n", strlen(buf1), buf1);//printf("1磁道数据(%d个):\n", strlen(buf1));
					//NDK_ScrDispString(0, 0, buf1, 1);
					//NDK_ScrRefresh();
					//wait_anykey(1);//getch();
				}
			}
		}
		//TK2
		if(buf2[0]==0x7f)//读卡错
		{			
			cls_show_msg1(g_keeptime, "%s, line %d:2道读卡错![0x7F]\n实际:%2x\nerrcode:0x%x\n", __FILE__, __LINE__, buf2[0], errcode);
			flg |= TK2;
		}
		else if(buf2[0]==0x7e)//无数据
		{
			cls_show_msg1(g_keeptime, "%s, line %d:2道无数据![0x7E]\n实际:%2x\nerrcode:0x%x\n", __FILE__, __LINE__, buf2[0], errcode);
		}
		else
		{
			NDK_ScrClrs();//clrscr();
			if((errcode&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR)//校验错误
			{    
				NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE, NULL);//settextattr(REVERSE_TEXT);//反显错误值
				flg |= TK2;
			}
			if(IsDisplayed || (flg&TK2)==TK2)
			{
				show_msg1(2, "2磁道数据(%d个):%s\n", strlen(buf2), buf2);//printf("2磁道数据(%d个):\n", strlen(buf2));
				//NDK_ScrDispString(0, 0, buf2, 1);
				//NDK_ScrRefresh();
				//wait_anykey(1);//getch();
			}
		}
		//TK3
		if(buf3[0]==0x7f)//读卡错
		{			
			cls_show_msg1(g_keeptime, "%s, line %d:3道读卡错![0x7F]\n实际:%2x\nerrcode:0x%x\n", __FILE__, __LINE__, buf3[0], errcode);
			flg |= TK3;
		}
		else if(buf3[0]==0x7e)//无数据
		{
			cls_show_msg1(g_keeptime, "%s, line %d:3道无数据![0x7E]\n实际:%2x\nerrcode:0x%x\n", __FILE__, __LINE__, buf3[0], errcode);
		}
		else
		{
			NDK_ScrClrs();//clrscr();
			if((errcode&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR)//校验错误
			{
				NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE, NULL);//settextattr(REVERSE_TEXT);//反显错误值
				flg |= TK3;
			}
			if(IsDisplayed || (flg&TK3)==TK3)
			{
				show_msg1(2, "3磁道数据(%d个):%s\n", strlen(buf3), buf3);//printf("3磁道数据(%d个):\n", strlen(buf3));
				//NDK_ScrDispString(0, 0, buf3, 1);
				//NDK_ScrRefresh();
				//wait_anykey(1);//getch();
			}
		}

		NDK_ScrClrs();//clrscr();
		ret |= flg<<8;
		break;
	default:
		cls_show_msg1(g_keeptime, "%s, line %d:刷卡返回未知码:0x%x", __FILE__, __LINE__, ret);
		break;  
	}
	
	return ret;
}

int systest_MagCard(void){return MagcardReadTest(TK2_3, TRUE, 0);}
#endif

#if !(defined ME15CHX||defined ME15B||defined ME66)
void set_snd_packet(Packet *packet)
{
	/*private & local definition*/
	int nKeyin = 0, j = 0, ret = -1;
	uint len = 0;
	char tbuf[16] = {0}, *ptr = packet->header;
	LinkType type = getlinktype();

	/*process body*/
	cls_printf("设置包的发送次数:\n");//一般地,认为发送次数==包更新生命周期+1
	//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
	if(g_SequencePressFlag)
	{
		packet->lifecycle= g_CycleTime;//GetCycleValue();
	}
	else
	{
		strcpy(tbuf, "200"/*"300"*/);//zhangnw20110113 50->200,因为有些问题会在100次后出现
		if ((ret=lib_kbgetinput(tbuf, 0, sizeof(tbuf)-1, &len, INPUTDISP_OTHER, 10, INPUT_CONTRL_LIMIT))==NDK_ERR ||ret == NDK_ERR_TIMEOUT)
			strcpy(tbuf, "200"/*"300"*/);
		
		//适应"发送前先update"if((packet->lifecycle=atoi(tbuf)-1)<0)
		if((packet->lifecycle=atoi(tbuf))<0)
			return;//此包已死,直接返回(卫语句)
	}
	packet->forever = FALSE;
	
	cls_printf("设置初始包长:\n");
	if(g_SequencePressFlag)
	{
		packet->len = packet->orig_len = g_PackLen;//GetCommPackLen();
	}	
	else
	{
		strcpy(tbuf, "13000");//zhangnw20110225提升1000->10000,默认按最大的包来发// 20180905 提升10000->13000 
		if ((ret=lib_kbgetinput(tbuf, 0, sizeof(tbuf)-1, &len,INPUTDISP_OTHER, 10, INPUT_CONTRL_LIMIT))==NDK_ERR ||ret == NDK_ERR_TIMEOUT)
			strcpy(tbuf, "13000");		
		packet->len = packet->orig_len = atoi(tbuf);
	}
	
	if(packet->len>PACKMAXLEN)
		packet->len = packet->orig_len = PACKMAXLEN;
	if((type==ASYN || type==SYNC) && packet->len<5)
		packet->len = packet->orig_len = 5;
	if(type==SYNC && packet->len>SDLCPCKTMAXLEN)
		packet->len = packet->orig_len = SDLCPCKTMAXLEN;
	if(type==ASYN && packet->len>ASYNPCKTMAXLEN)
		packet->len = packet->orig_len = ASYNPCKTMAXLEN;
	if(type==ETH && packet->len>ETHPACKMAXLEN)
		packet->len = MIN(packet->orig_len, ETHPACKMAXLEN);//新增ETH达到最大长度时,返回最初设置的长度(中低端有区别) 20180806 modify
	if((type==GPRS||type==CDMA||type==TD)&& packet->len>WLMPACKMAXLEN)
		packet->len = MIN(packet->orig_len, WLMPACKMAXLEN);//新增WLM达到最大长度时,返回最初设置的长度(中低端有区别) 20180806 modify
#if 0
	if( (type==SERIAL) && packet->len>BUFSIZE_SERIAL)
		packet->len = packet->orig_len = BUFSIZE_SERIAL;
#endif
	if( (type==BT) && packet->len>BUFSIZE_BT-MIN_BTCMD_LEN)//实际可发送的数据长度为BUFSIZE_BT-最小指令长度MIN_BTCMD_LEN
		packet->len = packet->orig_len = BUFSIZE_BT-MIN_BTCMD_LEN;

	if(g_SequencePressFlag)
	{
		if(ENTER==cls_show_msg1(5,"包长是否递增?\n"
								"[确认]是[其它]否\n"))
			packet->IsLenRec = TRUE;
		else
			packet->IsLenRec = FALSE;
	}
	else
	{
		if(ENTER==cls_show_msg("包长是否递增?\n"
							"[确认]是[其它]否\n"))
			packet->IsLenRec = TRUE;
		else
			packet->IsLenRec = FALSE;
	}
	
	do
	{
		if(g_SequencePressFlag)
		{
			nKeyin = cls_show_msg1(5,"_设置发送数据_\n"
									"1.固定数据\n"
									"2.随机数据\n");
		}
		else
		{
			nKeyin = cls_show_msg("_设置发送数据_\n"
								"1.固定数据\n"
								"2.随机数据\n");
		}
#define MAXMENUITEM ('2')		
		
		switch (nKeyin)
		{
		case '1':
			packet->IsDataRnd = FALSE;
			cls_printf("设置固定数据:\n");
			strcpy(tbuf, "0");
			if((ret=lib_kbgetinput(tbuf, 0, sizeof(tbuf)-1, &len,INPUTDISP_OTHER, 20, INPUT_CONTRL_LIMIT))==NDK_ERR ||ret == NDK_ERR_TIMEOUT)
				strcpy(tbuf, "0");
			memset(packet->header, atoi(tbuf), packet->len);
			break;
		case 0:  /*超时默认随机数*/
		case MAXMENUITEM://'2':
			packet->IsDataRnd = TRUE;
			for (j=0; j<packet->len; j++)
				*ptr++ = rand()%256;
			break;
		case ESC:
			return;
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM

	//进一步修正数据
#if SDLC_ENABLE
	if (type==SYNC)//\x60\x80\x00\x80\x00
		update_SDLC_packet(packet->header, packet->len);
#endif
#if ASYN_ENABLE
	if (type==ASYN)
		update_ASYN_packet(packet->header, packet->len);
#endif
#if BT_ENABLE
	if(type==BT)
		update_BT_packet(packet->header, packet->len);
#endif	
	return;
}

//每update一次,生命周期自动减1
int  update_snd_packet(Packet *packet)
{
	/*private & local definition*/
	int j = 0;
	char *ptr = packet->header;
	LinkType type = getlinktype();

	/*process body*/
	if(packet->forever==FALSE && packet->lifecycle<=0)
		return FAIL;

	if(packet->IsLenRec)
		packet->len++;

	if((packet->len>PACKMAXLEN))
		packet->len = MIN(packet->orig_len, PACKMAXLEN);//达到最大长度时,返回最初设置的长度
#if 0		
	if( (type==SERIAL) && packet->len>BUFSIZE_SERIAL)
		packet->len = MIN(packet->orig_len, BUFSIZE_SERIAL);//达到最大长度时,返回最初设置的长度
#endif	
	if(type==ETH && packet->len>ETHPACKMAXLEN)
		packet->len = MIN(packet->orig_len, ETHPACKMAXLEN);//新增ETH达到最大长度时,返回最初设置的长度(中低端有区别) 20180806 modify
	if((type==GPRS||type==CDMA||type==TD)&& packet->len>WLMPACKMAXLEN)
		packet->len = MIN(packet->orig_len, WLMPACKMAXLEN);//新增WLM达到最大长度时,返回最初设置的长度(中低端有区别) 20180806 modify
	if(type==BT && packet->len>BUFSIZE_BT-MIN_BTCMD_LEN)
		packet->len = MIN(packet->orig_len, BUFSIZE_BT-MIN_BTCMD_LEN);//达到最大长度时,返回最初设置的长度
	if(type==SYNC && packet->len>SDLCPCKTMAXLEN)
		packet->len = MIN(packet->orig_len, SDLCPCKTMAXLEN);//达到最大长度时,返回最初设置的长度
	if(type==ASYN && packet->len>ASYNPCKTMAXLEN)
		packet->len = MIN(packet->orig_len, ASYNPCKTMAXLEN);//达到最大长度时,返回最初设置的长度
	if(packet->IsDataRnd)
	{
		for (j=0; j<packet->len; j++)
			*ptr++ = rand()%256;
	}

	//进一步修正数据
#if SDLC_ENABLE
	if (type==SYNC)//\x60\x80\x00\x80\x00
		update_SDLC_packet(packet->header, packet->len);
#endif
#if ASYN_ENABLE
	if (type==ASYN)
		update_ASYN_packet(packet->header, packet->len);
#endif
#if BT_ENABLE
	if(type==BT)
		update_BT_packet(packet->header, packet->len);
#endif
	if(packet->forever==FALSE)
		packet->lifecycle--;

	return SUCC;
}

//建议统一开避大小为PACKMAXLEN的缓冲区
//本函数只生成一个死包,要通过set_snd_packet函数后,才可正常使用
void init_snd_packet(Packet *packet, char *buf)
{
	/*private & local definition*/

	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	packet->len = packet->orig_len = 0;
	packet->lifecycle = 0;
	packet->forever = FALSE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = FALSE;
	return;
}
#else
void set_snd_packet(Packet *packet){return;}
int update_snd_packet(Packet *packet){return SUCC;}
void init_snd_packet(Packet *packet, char *buf){return;}
#endif
#endif

#if !(defined ME15CHX||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
#if (SDLC_ENABLE || ASYN_ENABLE)
int mdm_send(char *buf, int len)
{
	/*private & local definition*/
	int ret = 0;
	LinkType type = getlinktype();
	
	/*process body*/
	cls_printf("数据发送中...\n"
				"POS->%s", MDMDialStr);
	
	if(type==ASYN||type==SYNC)
		return ((ret=NDK_MdmWrite(buf, len))==NDK_OK)?len:ret;
	else 
		return FAIL;
}

int mdm_rev(char *buf, int len, int timeout)
{
	/*private & local definition*/
	int ret = 0;
	uint rlen = len;
	LinkType type = getlinktype();
	
	/*process body*/
	cls_printf("数据接收中...\n"
				"POS<-%s", MDMDialStr);
	
	if(type==ASYN)
	{
		if((ret=getmodemreadlenN())<=0)
			return ret;

		return ((ret=NDK_MdmRead(buf, &rlen, timeout))==NDK_OK)?rlen:ret;
	}
	else if(type==SYNC)
	{
		rlen = 0;//防止SDLC_read没有对rlen做清0
		return ((ret=NDK_MdmRead(buf, &rlen, timeout))==NDK_OK)?rlen:ret;
	}
	else
		return FAIL;
}

void mdm_clrportbuf_all(void)
{
	/*private & local definition*/
	LinkType type = getlinktype();
	
	/*process body*/
	if (type==ASYN||type==SYNC)
		NDK_MdmClrbuf();
	else
		return;
}

void mdm_reset(void)
{
	/*private & local definition*/
	LinkType type = getlinktype();

	/*process body*/
	if(type==ASYN||type==SYNC)
		NDK_MdmReset();
	else
		return;
}

int mdm_init(void)
{
	/*private & local definition*/
	LinkType type = getlinktype();
	//int ret = 0;

	/*process body*/
	if(type==ASYN)
		return NDK_MdmAsynInit(MDMPatchType);
	else if(type==SYNC)
		{
#if defined SP50||defined SP80
			if(sdlc_init_flag)
				return NDK_MdmSdlcSingleForceInit(MDMPatchType, g_sdlc_bps);
			else
#endif
				return NDK_MdmSdlcInit(MDMPatchType);
		}
	else
		return FAIL;
}

int mdm_dial_comm(char *dialstr, Packet *sendpacket, int rst, float *call_time_once, float *hangdowntime_once)
{
	/*private & local definition*/
	int send_len = 0, rec_len = 0, ret = 0;
	char rbuf[PACKMAXLEN] = {0};
	*call_time_once = *hangdowntime_once = 0.0;
	
   	/*process body*/
	if(rst==TRUE)
	{
		//复位MODEM
		cls_printf("复位MODEM中...");
		mdm_reset();
	}
	//初始化MODEM
	cls_printf("初始化MODEM中...");
	if((ret=mdm_init())!=SUCC)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:初始化MODEM失败(%d)", __FILE__, __LINE__, ret);
		return ret;//FAIL;//使用ret可以把if中的函数返回值进一步返给上层
	}
	//拨号
	cls_printf("MODEM拨%s中...", dialstr);
	lib_StartStopwatch();
	if((ret=mdm_dial(dialstr))!=SUCC)
	{
		mdm_hangup();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:MODEM拨%s失败", __FILE__, __LINE__, dialstr);
		return ret;//FAIL;
	}
	*call_time_once = lib_ReadStopwatch();
	//数据通讯
	cls_printf("MODEM数据通讯中...");
	//发送数据
	if((send_len=mdm_send(sendpacket->header, sendpacket->len))!=sendpacket->len)
	{
		mdm_hangup();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:发送数据失败(实际%d, 预期%d)", __FILE__, __LINE__, send_len, sendpacket->len);
		return FAIL;//这里不返回send_len,而以FAIL表明if判断中的逻辑值为假(逻辑判断失败)!
	}
	//接收数据
	if((rec_len=mdm_rev(rbuf, sendpacket->len, 20))!=sendpacket->len)
	{
		mdm_hangup();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:接收数据失败(实际%d, 预期%d)", __FILE__, __LINE__, rec_len, sendpacket->len);
		return FAIL;
	}
	//比较数据
	if(MemCmp(sendpacket->header, rbuf, sendpacket->len))
	{
		mdm_hangup();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:数据校验失败", __FILE__, __LINE__);
		return FAIL;
	}
		
	//挂断
	cls_printf("MODEM挂断中...");
	lib_StartStopwatch();
	if((ret=mdm_hangup())!=SUCC)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:MODEM挂断失败", __FILE__, __LINE__);
		return ret;//FAIL;
	}
	*hangdowntime_once = lib_ReadStopwatch()-(getlinktype()==SYNC?SLEEP_SDLC_HANGUP:0);

	return SUCC;
}

int MDM_NetDown(int nMdmType)
{
	EM_MDMSTATUS MdmStatus = 0;
	
	Local2Global(nMdmType);
	setlinktype(nMdmType);

	if(nMdmType==ASYN)
	{
		if(__LinkDown_ASYN()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:断开链路失败", __LINE__);
			return FAIL;
		}
	}
	else
	{
		//挂断状态测试
		if(NDK_MdmCheck(&MdmStatus)!=NDK_OK|| MdmStatus != MDMSTATUS_CONNECT_AFTERPREDIAL)
		{
			__LinkDown_SDLC();
			cls_show_msg1(g_keeptime, "line %d:NDK_MdmCheck发生错误(MdmStatus=%d)", __LINE__, MdmStatus);
			return FAIL;
		}
		if(__LinkDown_SDLC()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:断开链路失败", __LINE__);
			return FAIL;;
		}	
		if(NDK_MdmCheck(&MdmStatus)!=NDK_OK|| MdmStatus != MDMSTATUS_NOPREDIAL)
		{
			cls_show_msg1(g_keeptime, "line %d:NDK_MdmCheck发生错误", __LINE__);
			return FAIL;
		}
	}
	return SUCC;
}
#endif

//提供封装的阻塞收发函数
#if (NDKSOCK_ENABLE ||NDKSSL_ENABLE)
int sock_recv(uint s, char  *buf, int len, int timeout)
{
	/*private & local definition*/

	/*process body*/
	cls_printf("数据接收中...\n"
				"%s:%d<-%s:%d\n", 
				netsetting.LocalIP.Str, netsetting.LocalPort, netsetting.SvrIP.Str, (netsetting.socktype==SOCK_NDKUDP)?netsetting.SvrUdpPort:netsetting.SvrTcpPort);
	if(netsetting.socktype==SOCK_NDKTCP||netsetting.socktype==SOCK_NDKUDP)
		return NDKTransReceiveN(s, buf, len, timeout);
#if NDKSSL_ENABLE
	else if(netsetting.socktype==SOCK_NDKSSL)
		return NDKSSLReceiveN((NDK_HANDLE)s, buf, len);
#endif
	else
		return FAIL;
}

int sock_send(uint s, char  *buf, int len, int timeout)
{
	/*private & local definition*/

	/*process body*/
	cls_printf("数据发送中...\n"
				"%s:%d->%s:%d\n", 
				netsetting.LocalIP.Str, netsetting.LocalPort, netsetting.SvrIP.Str, (netsetting.socktype==SOCK_NDKUDP)?netsetting.SvrUdpPort:netsetting.SvrTcpPort);
	if(netsetting.socktype==SOCK_NDKTCP||netsetting.socktype==SOCK_NDKUDP)
		return NDKTransSendN(s, buf, len, timeout);
#if NDKSSL_ENABLE
	else if(netsetting.socktype==SOCK_NDKSSL)
		return NDKSSLSendN((NDK_HANDLE)s, buf, len);
#endif
	else
		return FAIL;
}
#endif

#if (WLM_ENABLE && (NDKSOCK_ENABLE || NDKSSL_ENABLE))
int wireless_dial_comm(Packet *sendpacket, int flag)
{
	/*private & local definition*/
	uint h_tcp = -1;
	char rbuf[PACKMAXLEN] = {0};
	int slen = 0, rlen = 0, ret = 0, timeout = SO_TIMEO;
	
	/*process body*/
	//测试前置
	if((ret=NetUp())!=SUCC)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:NetUp失败", __FILE__, __LINE__);
		return ret;//FAIL;
	}
	if(flag==RESET_PPPOPEN)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,2, "PPP已打开(TCP未打开)...即将软重启...");
		NDK_SysReboot();//nlexit(0);
	}
	if((ret=TransUp(&h_tcp))!=SUCC)
	{
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:TransUp失败", __FILE__, __LINE__);
		return ret;//FAIL;
	}
	if(flag==RESET_TCPOPEN)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,2, "TCP已打开(PPP已打开)...即将软重启...");
		NDK_SysReboot();//nlexit(0);
	}
	//发送数据
	if((slen=sock_send(h_tcp, sendpacket->header, sendpacket->len, timeout))!=sendpacket->len)
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:发送数据失败(实际%d, 预期%d)", __FILE__, __LINE__, slen, sendpacket->len);
		return FAIL;
	}
#if 0//K21_ENABLE
	cls_show_msg1(5,"如果需要测试后台挂断后仍能收到缓冲区中数据的功能,请在5秒内关闭TCP后台");
#endif
	//接收数据
	memset(rbuf, 0, sizeof(rbuf));
	if ((rlen=sock_recv(h_tcp, rbuf, sendpacket->len, timeout))!=sendpacket->len)
	{
		TransDown(h_tcp);//TransDown(netsetting.socktype, h_tcp);
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:接收数据失败(实际%d, 预期%d)", __FILE__, __LINE__, rlen, sendpacket->len);
		return FAIL;
	}
	if(MemCmp(sendpacket->header, rbuf, rlen))
	{
		TransDown(h_tcp);
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:校验失败", __FILE__, __LINE__);
		return FAIL;
	}	
	if ((ret=TransDown(h_tcp))!=SUCC)
	{
		NetDown();
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "%s, line %d:TransDown失败", __FILE__, __LINE__);
		return ret;//FAIL;
	}
	if(flag==RESET_TCPCLOSE)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,2, "TCP已关闭(PPP未关闭)...即将软重启...");
		NDK_SysReboot();//nlexit(0);
	}
	NetDown();
	if(flag==RESET_PPPCLOSE)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,2, "PPP已关闭(TCP已关闭)...即将软重启...");
		NDK_SysReboot();//nlexit(0);
	}
	return SUCC;
}
#endif

#if RFID_ENABLE
#if K21_ENABLE //低端平台产品暂时不支持felica
int NDK_RfidFelicaApdu(int nSendlen, uchar *psSendbuf, int *pnRecvlen, uchar *psRecebuf){return NDK_ERR;}
int NDK_RfidFelicaPoll(uchar *psRecebuf,int *pnRecvlen){return NDK_ERR;}
#endif
//注:本函数一般只在测试配置中使用一次即可,在测试中一般不调用
int RFID_init(SMART_t type)
{
	/*private & local definition*/
	//uchar sta = 0;

	/*process body*/
	//NDK_RfidInit(NULL);//NDK_RfidInit(&sta);
	//NDK_RfidOpenRf();
	switch(type)
	{
	case CPU_A:
	case MIFARE_1:
	case CPU_B:
	case FELICA:
	case MIFARE_0:
		NDK_RfidInit(NULL);//NDK_RfidInit(&sta);
		NDK_RfidOpenRf();
		break;
#if ISO15693_ENABLE
	case ISO15693:
		NDK_ISO15693_init();
		break;
#endif
	default:
		return FAIL;
		break;
	}
	
	switch(type)
	{
	case CPU_A:
	case MIFARE_1:
		NDK_RfidPiccDeactivate(0);//icpowerdown(RFCARD);//保护
		return NDK_RfidPiccType(RFCARD_TYPE_A);//SetICType(0xff, RFCARD_TYPE_A);
		break;
	case CPU_B:
		NDK_RfidPiccDeactivate(0);//icpowerdown(RFCARD);//保护
		return NDK_RfidPiccType(RFCARD_TYPE_B);//SetICType(0xff, RFCARD_TYPE_B);
		break;
	case FELICA:
		//FELICA卡不需要设置寻卡策略
		NDK_RfidPiccDeactivate(0);//icpowerdown(RFCARD);//保护
		return SUCC;
		break;
	case MIFARE_0:
		//不需要设置寻卡策略NDK_MifareActive  里边设置了
		NDK_RfidPiccDeactivate(0);//icpowerdown(RFCARD);//保护
		return SUCC;
		break;
#if ISO15693_ENABLE
	case ISO15693:
		return SUCC;
		break;
#endif
	default:
		return FAIL;
		break;
	}
}

//函数设计上对于if条件是复合条件的情况,应有提示信息,而不能直接return给上层,以利于问题定位
int RFID_detect(SMART_t type, uchar *UID, int *pUIDlen)
{
	/*private & local definition*/
	uchar /*out[MAXLEN_DATA] = {0}, */SAK[LEN_SAK] = {0};
	int ret = 0;
	//uchar picctype = 0;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
#if ISO15693_ENABLE
	uint cardcount = 0, maxcards = 20;
#endif

	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
		if((ret=NDK_RfidPiccDetect(NULL))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s寻卡失败(%d)",__FILE__ , __LINE__, card[type], ret);
			return (ret!=NDK_OK)?ret:FAIL;
		}
		return SUCC;
		break;
	case MIFARE_1:
		if((ret=NDK_M1Request(REQA, pUIDlen, UID))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s寻卡失败(%d)",__FILE__ ,  __LINE__, card[type], ret);
			return ret;
		}
		if((ret=NDK_M1Anti(pUIDlen, UID))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s防冲突失败(%d)",__FILE__ ,  __LINE__, card[type], ret);
			return ret;
		}
		if((ret=NDK_M1Select(*pUIDlen, UID, SAK))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s选卡失败(%d)",__FILE__ ,  __LINE__, card[type], ret);
			return ret;
		}
		return SUCC;
		break;
	case FELICA:
		//FELICA卡寻卡操作
		//int NDK_RfidFelicaPoll(uchar *psRecebuf,int *pnRecvlen)
		if((ret=NDK_RfidFelicaPoll(UID, pUIDlen))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s寻卡失败(%d)",__FILE__ ,  __LINE__, card[type], ret);
			return ret;
		}
		return SUCC;
		break;
#if M0_ENABLE 
	case MIFARE_0:
		//M0卡寻卡操作
		if((ret=NDK_MifareActive(REQA, UID, pUIDlen,SAK))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s寻卡失败(%d)",__FILE__ ,  __LINE__, card[type], ret);
			return ret;
		}
		return SUCC;
		break;	
#endif		
#if ISO15693_ENABLE
	case ISO15693:
		if((ret=NDK_ISO15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s防冲突失败(%d)",__FILE__ ,  __LINE__, card[type], ret);
			return ret;
		}
		if((ret=NDK_iso15693SelectPicc(cards[0])) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s选卡失败(%d)",__FILE__ ,  __LINE__, card[type], ret);
			return ret;
		}	
		return SUCC;
		break;
#endif
	default:
		return FAIL;
		break;
	}	
}

//此函数跟上面那个函数的差别在于如果寻卡失败不会马上返回，而是在超时范围内轮询寻卡
int RFID_detect_poll(SMART_t type, uchar *UID, int *pUIDlen,int nTimeOut)
{
	/*private & local definition*/
	uchar /*out[MAXLEN_DATA] = {0}, */SAK[LEN_SAK] = {0};
	int ret = 0;
	//uchar picctype = 0;
	time_t oldtime = 0, diff = 0 ;
#if ISO15693_ENABLE
	uint cardcount = 0, maxcards = 20;
#endif

	/*process body*/

	oldtime=time(NULL);
	while((diff=time(NULL)-oldtime)<nTimeOut)
	{
		NDK_SysDelay(1);
		switch(type)
		{
		case CPU_A:
		case CPU_B:
			if((ret=NDK_RfidPiccDetect(NULL))!=NDK_OK)
				continue;
				
			return SUCC;
			break;
		case MIFARE_1:
			if((ret=NDK_M1Request(REQA, pUIDlen, UID))!=NDK_OK)
				continue;
			
			if((ret=NDK_M1Anti(pUIDlen, UID))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"%s,line %d:测试失败%d",__FILE__ ,  __LINE__,  ret);
				return ret;
			}
			if((ret=NDK_M1Select(*pUIDlen, UID, SAK))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"%s,line %d:测试失败%d",__FILE__ ,  __LINE__,  ret);
				return ret;
			}
			return SUCC;
			break;
		case FELICA:
			//FELICA卡寻卡操作
			if((ret=NDK_RfidFelicaPoll(UID, pUIDlen))!=NDK_OK)
				continue;
			return SUCC;
			break;
#if M0_ENABLE 
		case MIFARE_0:
			//M0卡寻卡操作
			if((ret=NDK_MifareActive(REQA, UID, pUIDlen,SAK))!=NDK_OK)
				continue;
			return SUCC;
			break;	
#endif
#if ISO15693_ENABLE
		case ISO15693:
			if((ret=NDK_ISO15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_OK)
				continue;
			if((ret=NDK_iso15693SelectPicc(cards[0])) != NDK_OK)
			{
				cls_show_msg1(g_keeptime, "%s,line %d:选卡失败(%d)",__FILE__ ,  __LINE__, ret);
				return ret;
			}	
			return SUCC;
			break;
#endif
		default:
			cls_show_msg("未知类型的卡类");
			return FAIL;
			break;
		}	
	}
	return FAIL;
}

//UIDlen:可用于输入参数也可用于输出参数,场合如下
//对CPU卡而言,用于输出参数;
//对M-1卡而言,用于输入参数,故而应先赋值(由RFID_detect获得)再将变量地址传入
//FELICA卡不需要激活操作
int RFID_active(SMART_t type, uchar *UID, int *pUIDlen)
{
	/*private & local definition*/
	int ret = 0;
	uchar picctype = 0;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
	
	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
		if((ret=NDK_RfidPiccActivate(&picctype, pUIDlen, UID))!=NDK_OK||((type!=CPU_A||picctype!=RFID_TYPE_A)&&(type!=CPU_B||picctype!=RFID_TYPE_B)))
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s激活失败(%d,%d)",__FILE__ , __LINE__, card[type], ret, picctype);
			return (ret!=NDK_OK)?ret:FAIL;
		}
		return SUCC;
		break;
	case MIFARE_1:
		//由于不知道4k卡的B密钥是多少,使用A密钥(经测试A密钥为全0xff)
#if 0	//不必提示,直接返回给上层即可
		if((ret=CMD_Auth_Ex(AUTHKEY_TYPE_A, 0x01, AUTHKEY, UID))!=MI_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:外部认证失败(ret=%d)", __LINE__, ret);
			return ret;
		}
		return SUCC;
#else
		return NDK_M1ExternalAuthen(*pUIDlen, UID, AUTHKEY_TYPE_A, AUTHKEY, 0x01);//return ((ret=CMD_Auth_Ex(AUTHKEY_TYPE_A, 0x01, AUTHKEY, UID))!=MI_OK)?ret:SUCC;
#endif
		break;
	case FELICA:
		//FELICA卡不需要激活操作
		//int NDK_RfidFelicaApdu(int nSendlen, uchar *psSendbuf, int *pnRecvlen, uchar *psRecebuf)
		return SUCC;
		break;
	case MIFARE_0:
		//为了兼容不带C的M0卡(不需要认证)就不做认证
		return SUCC;
		break;
#if ISO15693_ENABLE
	case ISO15693:
		//15693卡不需要激活操作
		return SUCC;
		break;
#endif
	default:
		return FAIL;
		break;
	}	
}

int RFID_APDUrw(SMART_t type, uchar *UID, int *cpurcvlen)
{
	/*private & local definition*/
	int ret = 0, rcvlen = 0;
	uchar atrbuf[300] = {0};
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	uchar Select_1pay[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
#else
	uchar Select_1pay[] = 
	{0x00, 0xA4, 0x04, 0x00, 0x0E,
	  '1',  'P',  'A',  'Y',  '.',  'S',  'Y',  'S',  '.',  'D',  'D',  'F',  '0',  '1',
	  0x00
	};//查询CPU卡的文件目录结构,mifare-1卡不支持
#endif
	uchar out[MAXLEN_DATA] = {0};
	ushort sw = 0;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
#if ISO15693_ENABLE
	iso15693PiccMemoryBlock_t w_memBlock;
	iso15693PiccMemoryBlock_t r_memBlock;
	uchar ucBlocknum = 0;
	//15693卡片写的数据
	ucBlocknum = 0x01;
	w_memBlock.data[0] = 0x02;
	w_memBlock.data[1] = 0x08;
	w_memBlock.data[2] = 0x04;
	w_memBlock.data[3] = 0x09;	
#endif

	//FELICA卡不认证读命令
	uchar  sendbuf[300] = {0};
	sendbuf[0] = 16;
	sendbuf[1] = 0x06;
	memcpy(&sendbuf[2], &UID[2], 8);
	sendbuf[10] = 0x01;
	sendbuf[11] = 0x09;
	sendbuf[12] = 0x00;
	sendbuf[13] = 0x01;
	sendbuf[14] = 0x80;
	sendbuf[15] = 0x00;
	
	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
		if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, cpurcvlen, atrbuf))!=NDK_OK||((sw=SMART_GETSW(atrbuf+*cpurcvlen-2))!=0x9000&&(sw&0x6000)!=0x6000))
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s读写失败(%d, 0x%04x)",__FILE__ , __LINE__, card[type], ret, sw);
			return (ret!=NDK_OK)?ret:FAIL;
		}
		return SUCC;
		break;
	case MIFARE_1:
#if 0	//考虑到测试卡的寿命只做读操作,换一种测试策略
		if((ret=CMD_BLKWrite(0x02, BLK02DATA_FF))!=MI_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:块写失败(ret=%d)", __LINE__, ret);
			return ret;
		}
#endif
		if((ret=NDK_M1Read(0x02, &rcvlen, out))!=NDK_OK)//if((ret=CMD_BLKRead(0x02, out))!=MI_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s块读失败(%d)",__FILE__ , __LINE__, card[type], ret);
			return ret;
		}
		else
		{
#if 0	//考虑到测试卡的寿命只做读操作,换一种测试策略
			if(memcmp(BLK02DATA_FF, out, LEN_BLKDATA))
#else
			//预期rcvlen应等于LEN_BLKDATA
			if(!memcmp(g_RF_bak, DATA16, rcvlen))//首次
				memcpy(g_RF_bak, out, rcvlen);
			else if(memcmp(g_RF_bak, out, rcvlen))//其它次
#endif
			{
				cls_show_msg1(g_keeptime, "%s,line %d:%s数据校验失败(%d)",__FILE__ , __LINE__, card[type], rcvlen);
				return FAIL;
			}
		}
		return SUCC;
		break;
	case FELICA:
		//FELICA卡读写操作
		if((ret=NDK_RfidFelicaApdu(sendbuf[0], sendbuf, &rcvlen, out))!=NDK_OK||rcvlen!=out[0])
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s不认证读失败(%d)(%d,%d)",__FILE__ , __LINE__, card[type], ret, rcvlen, out[0]);
			return ret;
		}
		else
		{
			if(out[1]!=0x07||memcmp(sendbuf+2, out+2, 8))
			{
				cls_show_msg1(g_keeptime, "%s,line %d:%s数据校验失败(%02x)",__FILE__ , __LINE__, card[type], out[1]);
				return FAIL;
			}
		}
		return SUCC;
		break;
#if M0_ENABLE 
	case MIFARE_0:
		//考虑到测试卡的寿命只做读操作 
		if((ret=NDK_M0Read(12,&rcvlen,out))!=NDK_OK) 
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s块读失败(%d)",__FILE__ , __LINE__, card[type], ret);
			return ret;
		}
		else
		{
			//预期rcvlen应等于LEN_BLKDATA
			if(!memcmp(g_RF_bak, DATA16, rcvlen))//首次
				memcpy(g_RF_bak, out, rcvlen);
			else if(memcmp(g_RF_bak, out, rcvlen))//其它次
			{
				cls_show_msg1(g_keeptime, "%s,line %d:%s数据校验失败(%d)",__FILE__ , __LINE__, card[type], rcvlen);
				return FAIL;
			}
		}
		return SUCC;
		break;	
#endif		
#if ISO15693_ENABLE
	case ISO15693:
		// write   read
		if((ret=NDK_iso15693Write_SingleBlock(cards[0],w_memBlock,ucBlocknum)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s写失败(%d)",__FILE__ , __LINE__, card[type], ret);
			return FAIL;
		}
		memset(&r_memBlock, 0, sizeof(iso15693PiccMemoryBlock_t));
		if((ret=NDK_iso15693Read_SingleBlock(cards[0],ucBlocknum,&r_memBlock)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s,line %d:%s读失败(%d)",__FILE__ , __LINE__, card[type], ret);
			return FAIL;
		}	
		//compare
		if((r_memBlock.data[0]!=w_memBlock.data[0])||(r_memBlock.data[1]!=w_memBlock.data[1])||(r_memBlock.data[2]!=w_memBlock.data[2])||(r_memBlock.data[3]!=w_memBlock.data[3]))
		{
			cls_show_msg1(g_keeptime,"%s,line %d:%s数据校验失败(%02x:%02x:%02x:%02x)", __FILE__ , __LINE__, card[type], r_memBlock.data[0], r_memBlock.data[1], r_memBlock.data[2], r_memBlock.data[3]);
			return FAIL;
		}
		return SUCC;
		break;
#endif
	default:
		return FAIL;
		break;
	}	
}

int RFID_deactive(SMART_t type, ushort msec)
{
	/*private & local definition*/
	//int ret = 0;

	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
	case MIFARE_1:
	case FELICA:
	case MIFARE_0:
		return NDK_RfidPiccDeactivate(msec);
		break;
#if ISO15693_ENABLE
	case ISO15693:
		return NDK_ISO15693_Deinit();
		break;
#endif
	default:
		return FAIL;
		break;
	}
}
int RFID_finish(SMART_t type)
{
	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
	case MIFARE_1:
	case FELICA:
	case MIFARE_0:
		return NDK_RfidCloseRf();
		break;
#if ISO15693_ENABLE
	case ISO15693:
		return NDK_ISO15693_Deinit();
		break;
#endif
	default:
		return FAIL;
		break;
	}
}

#else
int RFID_init(SMART_t type){return FAIL;}
int RFID_detect(SMART_t type, uchar *UID, int *pUIDlen){return FAIL;}
int RFID_detect_poll(SMART_t type, uchar *UID, int *pUIDlen,int nTimeOut){return FAIL;}
int RFID_active(SMART_t type, uchar *UID, int *pUIDlen){return FAIL;}
int RFID_APDUrw(SMART_t type, uchar *UID, int *cpurcvlen){return FAIL;}
int RFID_deactive(SMART_t type, ushort msec){return FAIL;}
int RFID_finish(SMART_t type){return FAIL;}
#endif
#endif

#if !(defined ME15CHX||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
#if (ICSAMNUM>0)
int select_ICSAM(void)
{
	/*private & local definition*/
//#include "icsam.h"
	char *CardNoStr[] = {"IC1", "SAM1", "SAM2", "SAM3", "SAM4"};
	int i = 0;
	char pInfo[64] = {0};
#if SCR_DISABLE
	char tmp[32] = {0};
#endif
	/*process body*/
	if(g_SequencePressFlag)
	{
		if(GetParaInfo("ICSAMType",pInfo)!=SUCC)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%s)", __LINE__, pInfo);
			return 0;//配置文件设置错误时默认测试IC卡
		}
		if(trim_string2int(pInfo,&i)!=SUCC)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%d)", __LINE__, i);
			return 0;
		}
		return i;
	}
	
	do
	{
		cls_printf("_选择测试卡_\n");
		for(i=0; i<ICSAMNUM/*SZ_ARRAY(CardNo)-1*/; i++)
#if SCR_DISABLE//重定向到串口
		{
			memset(tmp,0,sizeof(tmp));
			sprintf(tmp, (i%2)?"%d.%s\n":"%d.%s ", i+1, CardNoStr[i]);
#if 0//ANDIROD_ENABLE
			NDK_AZPortWrite(g_com, strlen(tmp), tmp);
#else
			NDK_PortWrite(g_com, strlen(tmp), tmp);
#endif
		}
#endif
		NDK_ScrPrintf((i%2)?"%d.%s\n":"%d.%s ", i+1, CardNoStr[i]);
		NDK_ScrRefresh();
		if((i=lib_getch()-'1')==(ESC-'1'))
			return NDK_ERR_QUIT;//QUIT;
	}while(i<0 || i>ICSAMNUM-1/*SZ_ARRAY(CardNo)-2*/);

	return i;
}

int ICSAM_detect(EM_ICTYPE cardno)
{
	/*private & local definition*/
	int ret = 0, sta = 0;
#if !K21_ENABLE
	int lshift[] = {1, 4, 5, 6, 3};
#endif
	/*process body*/
	if((ret=NDK_IccDetect(&sta))!=NDK_OK)
		return ret;

#if K21_ENABLE   //K21只支持插卡状态的查询，不支持上电状态的查询
	return SUCC;
#else
	return (sta&0x000000ff&(0x01<<lshift[cardno]))?SUCC:FAIL;//return (sta&0x000000ff&(0x01<<(cardno+1)))?SUCC:FAIL
#endif

}

int ICSAM_APDUrw(EM_ICTYPE cardno)
{
	/*private & local definition*/
	int ret = 0, rlen = 0;
	uchar recv[128] = {0}, send[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
	//根据所发的卡的不同,有的可能支持下面的命令,与机具无关,与使用的测试卡有关
	//char sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 0};
	ushort sw = 0;
	char *card[] = {"IC1", "SAM1", "SAM2", "SAM3", "SAM4"};

	/*process body*/
	//使用iccrw/iccrw_new发取随机数命令
	//if ((ret=iccrw(cardno, sizeof(send), send, recv))!=SUCC||recv[(int)recv[0]-1]!=0x90||recv[(int)recv[0]]!=0x00)
	if((ret=NDK_Iccrw(cardno, sizeof(send), send, &rlen, recv))!=NDK_OK||((sw=SMART_GETSW(recv+rlen-2))!=0x9000&&(sw=SMART_GETSW(recv+rlen-2))!=0x6d00))
	{
		cls_show_msg1(g_keeptime, "%s,line %d:%s读写失败(%d, 0x%04x)",__FILE__ ,__LINE__, card[cardno], ret, sw);
		return (ret!=NDK_OK)?ret:FAIL;
	}

	return SUCC;
}
#else
int select_ICSAM(void){return FAIL;}
int ICSAM_detect(EM_ICTYPE cardno){return FAIL;}
int ICSAM_APDUrw(EM_ICTYPE cardno){return FAIL;}
#endif

SMART_t select_smarttype(void)
{
	/*private & local definition*/
	int nKeyin = 0, rfctype=-1;
	SMART_t type[] = {CPU_A, CPU_B, MIFARE_1, FELICA, MIFARE_0, ISO15693, SAM, IC};
	char pInfo[64] = {0};

	/*process body*/
	while (1)
	{
		if(g_SequencePressFlag)
		{
			if(GetParaInfo("SMARTType",pInfo)!=SUCC)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%s)", __LINE__, pInfo);
				return CPU_A;//配置文件设置失败时默认测试A卡
			}
			if(trim_string2int(pInfo,&rfctype)!=SUCC)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line =%d:读配置文件失败(%d)", __LINE__, rfctype);
				return CPU_A;
			}
			switch(rfctype)
			{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				return type[rfctype-1];
				break;
			case ESC:
			default:
				cls_show_msg1(2, "将设置为CPU_A(默认)...");
				return CPU_A;
				break;
			}
		}

		nKeyin = cls_show_msg("选择卡类型...\n"
							  "1.CPU_A 2.CPU_B\n"
							  "3.M-1 4.FELICA\n"
							  "5.M-0 6.15693\n"
							  "7.SAM 8.IC\n");

		switch(nKeyin)
		{
		case '3':
			//memcpy(gen31_bak, DATA16, LEN_BLKDATA);
		case '1':
		case '2':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			return type[nKeyin-'1'];
			break;
		case ESC:
			cls_show_msg1(2, "将设置为CPU_A(默认)...");
			return CPU_A;
			break;
		default:
			continue;
			break;
		}
	}
}

int smart_detect(SMART_t type, uchar *UID , int *pUIDlen)
{
	/*private & local definition*/
	uchar atr[300] = {0};
	int len = 0;

	/*process body*/
	switch (type)
	{
	case CPU_A:
	case CPU_B:
	case MIFARE_1:
	case FELICA:
	case ISO15693:
	case MIFARE_0:	
		return RFID_detect(type, UID, pUIDlen);
		break;
	case SAM:
	case IC:
		return ICSAM_powerup(UID[0], atr, &len);
		break;
	default:
		return FAIL;
		break;
	}
}

int smart_active(SMART_t type, uchar *UID, int *pUIDlen)
{
	/*private & local definition*/

	/*process body*/
	switch (type)
	{
	case CPU_A:
	case CPU_B:
	case MIFARE_1:
	case FELICA:
	case ISO15693:
	case MIFARE_0:	
		return RFID_active(type, UID, pUIDlen);
		break;
	case SAM:
	case IC:
		return  ICSAM_detect(UID[0]);
		break;
	default:
		return FAIL;
		break;
	}
}

int smart_APDUrw(SMART_t type, EM_ICTYPE cardno, uchar *UID)
{
	/*private & local definition*/
	int rcvlen = 0;
	
	/*process body*/
	switch (type)
	{
	case CPU_A:
	case CPU_B:
	case MIFARE_1:
	case FELICA:
	case ISO15693:
	case MIFARE_0:	
		return RFID_APDUrw(type, UID, &rcvlen);
		break;
	case SAM:
	case IC:
		return ICSAM_APDUrw(cardno);
		break;
	default:
		return FAIL;
		break;
	}
}

int smart_deactive(SMART_t type, EM_ICTYPE cardno, ushort msec)
{
	/*private & local definition*/
	
	/*process body*/
	switch (type)
	{
	case CPU_A:
	case CPU_B:
	case MIFARE_1:
	case FELICA:
	case ISO15693:
	case MIFARE_0:	
		return RFID_deactive(type, msec);
		break;
	case SAM:
	case IC:
		return ICSAM_powerdown(cardno);
		break;
	default:
		return FAIL;
		break;
	}
}
#else
int select_ICSAM(void)
{
	/*private & local definition*/
	
	/*process body*/
	return 0;//ME15C只支持IC卡，直接返回0表示选择IC卡
}

int ICSAM_detect(EM_ICTYPE cardno)
{
	/*private & local definition*/
	
	/*process body*/
	return SUCC;
}

int ICSAM_APDUrw(EM_ICTYPE cardno)
{
	/*private & local definition*/
	int ret = 0, rlen = 0;
	uchar recv[128] = {0}, send[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
	//根据所发的卡的不同,有的可能支持下面的命令,与机具无关,与使用的测试卡有关
	//char sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 0};
	ushort sw = 0;
	char *card[] = {"IC1", "SAM1", "SAM2", "SAM3", "SAM4"};

	/*process body*/
	//使用iccrw/iccrw_new发取随机数命令
	if((ret=NDK_Iccrw(cardno, sizeof(send), send, &rlen, recv))!=NDK_OK||((sw=SMART_GETSW(recv+rlen-2))!=0x9000&&(sw=SMART_GETSW(recv+rlen-2))!=0x6d00))
	{
		cls_show_msg1(g_keeptime, "%s,line %d:%s读写失败(%d, 0x%04x)",__FILE__ ,__LINE__, card[cardno], ret, sw);
		return (ret!=NDK_OK)?ret:FAIL;
	}

	return SUCC;
}

SMART_t select_smarttype(void)
{
	/*private & local definition*/
	
	/*process body*/
	cls_show_msg1(2, "将设置为IC卡(默认)...");
	return IC;
}

int smart_detect(SMART_t type, uchar *UID , int *pUIDlen)
{
	/*private & local definition*/
	uchar atr[150] = {0};
	int len = 0;

	/*process body*/
	return ICSAM_powerup(UID[0], atr, &len);
}

int smart_active(SMART_t type, uchar *UID, int *pUIDlen)
{
	/*private & local definition*/

	/*process body*/
	return  ICSAM_detect(UID[0]);
}

int smart_APDUrw(SMART_t type, EM_ICTYPE cardno, uchar *UID)
{
	/*private & local definition*/
	
	/*process body*/
	return ICSAM_APDUrw(cardno);
}

int smart_deactive(SMART_t type, EM_ICTYPE cardno, ushort msec)
{
	/*private & local definition*/
	
	/*process body*/
	return ICSAM_powerdown(cardno);
}
#endif

#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
#if CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE||ANDIROD_ENABLE
//将小数序转为NL829中采用的大数序
void little2big_ushort(ushort src, ushort *des)
{
	/*private & local definition*/
	uchar tmp[8] = {0}, *addr = (uchar *)&src;
	int loop = sizeof(ushort)-1;
	
	/*process body*/
	for(; loop>=0; loop--)
		tmp[loop] = *addr++;
	memcpy((void *)des, tmp, sizeof(ushort));
}

//本函数只负责命令串的构造(data->frame),命令对错不管,由上层发送后,对方应答可知
int NL829_pack_data(uchar *data, int datalen, uchar *frame)
{
	/*private & local definition*/
	int LRCOffset = NL829_LEN_FRAMEHEAD+datalen;
	int framelen = LRCOffset+NL829_LEN_FRAMETAIL;
	
	/*process body*/
	if(data==NULL||datalen<NL829_LEN_CMD||datalen>NL829_MAXLEN_DATA)
		return FAIL;
	frame[NL829_OFFSET_STX] = STX;
	frame[NL829_OFFSET_DATALEN] = datalen/256;
	frame[NL829_OFFSET_DATALEN+1] = datalen%256;
	memcpy(frame+NL829_OFFSET_DATA, data, datalen);
	frame[LRCOffset] = MakeLRC(data, datalen);
	frame[LRCOffset+NL829_LEN_LRC] = ETX;
	return framelen;
}

//本函数负责对命令响应的解构(data--->status+append),并返回除状态码以外的附加数据的长度给调用者
//上层应为append开辟足够空间(建议MAXLEN_DATA)
int NL829_unpack_data(uchar *data, int datalen, uchar *status, uchar *append)
{
	/*private & local definition*/
	
	/*process body*/
        //射频卡寻卡和按键输入轮询指令允许返回一个字节的状态，当状态值为AA时，键盘输入成功，其后的数据为输入的字符，当状态值为55时，键盘输入超时或者取消键返回
        if(datalen<NL829_LEN_STATUS)
        {
            if(1==datalen)
            {
                if (0x55==data[0])
                    return NL829_PP60_FAIL;
                else if (0xAA!=data[0])
                    return FAIL;
            }
            else
                return FAIL;//-1表示发生错误
        }
        if (PP60_SUCC==data[0])
        {
			memcpy(status, data, 1);
        	memcpy(append, &data[1], datalen-1);
        	return (datalen-1);//>=0表示正常返回
        }
    	memcpy(status, data, NL829_LEN_STATUS);
    	memcpy(append, &data[NL829_LEN_STATUS], datalen-NL829_LEN_STATUS);
    	return (datalen-NL829_LEN_STATUS);//>=0表示正常返回
}

ushort NL829_send_frame(uchar *cmdstr, int cmdlen)
{
	/*private & local definition*/
	
	/*process body*/
	serial_clrportbuf_all(g_devicecom);
	//外设的命令帧长度一般不会超过串口最大缓冲,故portwrite一般都会成功,可不判或简单判断返回
	return (NDK_PortWrite(g_devicecom, cmdlen, (char *)cmdstr)==NDK_OK)?NL829_SUCC:NL829_FAIL;//return (portwrite(g_devicecom, cmdlen, (char *)cmdstr)==SUCC)?NL829_SUCC:NL829_FAIL;
}

//由于按协议字段接收,故提前判断除data以外的字段是否正确,而将data域放到解包中去分析与校验
ushort NL829_recv_frame(uchar *data, int *datalen, ushort timeout)
{
	/*private & local definition*/
	int len = 0, LRCOffset = 0, ret = 0, rlen = 0;
	uchar frame[NL829_MAXLEN_FRAME] = {0};
	
	/*process body*/
	//注意:实际上由于829也是一个应答帧一齐回过来,故其实在能读到STX时,后续数据也早已到达接收缓冲的,
	//故只要接收预期长度合适,接收超时可以设短(以下设为0),以提高接收效率
	//if((ret=portread(g_devicecom, NL829_LEN_STX, (char *)frame, NL829_TIMEOUT_BASE+timeout))!=NL829_LEN_STX||frame[NL829_OFFSET_STX]!=STX)//接收STX的超时时间要考虑到829的操作时间及串口传输的时间
	if((ret=NDK_PortRead(g_devicecom, NL829_LEN_STX, (char *)frame, (NL829_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=NL829_LEN_STX||frame[NL829_OFFSET_STX]!=STX)
	{
		//cls_show_msg("%s, line %d:%d, %02x", __FILE__, __LINE__, ret, frame[NL829_OFFSET_STX]);
		return NL829_ERR_FRAME_STX;
	}
	//if((ret=portread(g_devicecom, NL829_LEN_DATALEN, (char *)frame+NL829_OFFSET_DATALEN, /*0*/NL829_TIMEOUT_BASE+timeout))!=NL829_LEN_DATALEN)//考虑到上面超时时间未到就返回了,若此处超时为0且数据未到达时(低BPS),会出错.故超时时间改为20+timeout
	if((ret=NDK_PortRead(g_devicecom, NL829_LEN_DATALEN, (char *)frame+NL829_OFFSET_DATALEN, /*0*/(NL829_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=NL829_LEN_DATALEN)	
		return NL829_ERR_FRAME_DATALEN1;
	len=NL829_2BYTESTOUSHORT(frame+NL829_OFFSET_DATALEN);
	if(len<1/*NL829_LEN_STATUS*/||len>NL829_MAXLEN_DATA)//新增的0x32 0x29指令status状态可以只有1个字节
		return NL829_ERR_FRAME_DATALEN2;
	//if((ret=portread(g_devicecom, len, (char *)frame+NL829_OFFSET_DATA, /*0*/NL829_TIMEOUT_BASE+timeout))!=len)
	if((ret=NDK_PortRead(g_devicecom, len, (char *)frame+NL829_OFFSET_DATA, /*0*/(NL829_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=len)			
		return NL829_ERR_FRAME_DATA;
	LRCOffset = NL829_LEN_FRAMEHEAD+len;
	//if((ret=portread(g_devicecom, NL829_LEN_LRC, (char *)frame+LRCOffset, /*0*/NL829_TIMEOUT_BASE+timeout))!=NL829_LEN_LRC||frame[LRCOffset]!=MakeLRC(frame+NL829_OFFSET_DATA, len))
	if((ret=NDK_PortRead(g_devicecom, NL829_LEN_LRC, (char *)frame+LRCOffset, /*0*/(NL829_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=NL829_LEN_LRC||frame[LRCOffset]!=MakeLRC(frame+NL829_OFFSET_DATA, len))					
		return NL829_ERR_FRAME_LRC;
	//if((ret=portread(g_devicecom, NL829_LEN_ETX, (char *)frame+LRCOffset+NL829_LEN_LRC, /*0*/NL829_TIMEOUT_BASE+timeout))!=NL829_LEN_ETX||frame[LRCOffset+NL829_LEN_LRC]!=ETX)
	if((ret=NDK_PortRead(g_devicecom, NL829_LEN_ETX, (char *)frame+LRCOffset+NL829_LEN_LRC, /*0*/(NL829_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=NL829_LEN_ETX||frame[LRCOffset+NL829_LEN_LRC]!=ETX)			
		return NL829_ERR_FRAME_ETX;

	//以上都按预期接收到了才更新输出参数并返回SUCC
	memcpy(data, frame+NL829_OFFSET_DATA, len);
	*datalen = len;
	return NL829_SUCC;
}

//data为应用发送的命令及参数,status为应用接收的状态码,append为除状态码以外的附加数据
//datalen为上层指定发送的命令及参数的总长度
//appendlen为底层返回给上层的除状态码以外的附加数据的长度
//timeout:对于一些有超时要求的指令(执行完毕或超时才返回)可以使用,不理会的传入0即可
//本函数返回NL829_SUCC或NL829_ERR_FRAME_BASE为基的一族错误码,另外,本函数不判断回应帧中status是否是0x0000
ushort NL829_frame_factory(uchar *data, int datalen, uchar *status, uchar *append, int *appendlen, ushort timeout)
{
	/*private & local definition*/
	int ret = 0, len = 0;
	ushort ret1 = NL829_FAIL;
	uchar frame_cmd[NL829_MAXLEN_FRAME] = {0};
	uchar resp[NL829_MAXLEN_DATA] = {0};//frame_resp中的data域
	
	/*process body*/
	//组包
	len = NL829_pack_data(data, datalen, frame_cmd);
#if defined DEBUG_CMD//调试命令范例
	if(!memcmp(data, DEBUG_CMD, NL829_LEN_CMD))
	{
		int i = 0;
		cls_printf("line %d:", __LINE__);//__FILE__太长了,影响后续数据的显示,故此处不显示它
		for(; i<len; i++)
			NDK_ScrPrintf(" %02X", *(frame_cmd+i));
		NDK_ScrRefresh();
		lib_getch();
	}
#endif
	//发包
	if((ret1=NL829_send_frame(frame_cmd, len))!=NL829_SUCC)
		return ret1;//return NL829_ERR_FRAME_SND;
	//收包
	if((ret1=NL829_recv_frame(resp, &len, timeout))!=NL829_SUCC)
		return ret1;//返回NL829_ERR_FRAME_XXX(具体的接收错误)
#if defined DEBUG_CMD//调试命令范例
	if(!memcmp(data, DEBUG_CMD, NL829_LEN_CMD))
	{
		int i = 0;
		cls_printf("line %d:", __LINE__);//__FILE__太长了,影响后续数据的显示,故此处不显示它
		for(; i<len; i++)
			NDK_ScrPrintf(" %02X", *(resp+i));
		NDK_ScrRefresh();
		lib_getch();
	}
#endif		
	//解包
	ret=NL829_unpack_data(resp, len, status, append);
	if(ret==FAIL)
		return NL829_ERR_FRAME_UNPACK;
    else if(ret==NL829_PP60_FAIL)
		return NL829_PP60_FAIL;
	*appendlen = ret;
	return NL829_SUCC;
}

//一般而言,以下CMD接口设计时注意参数的合法性不用判断,透传到链路层由829应答即可,这样子可以测试到错误参数时829的应答
//返回成功状态码后立即生效，不保存，开机总是57600
ushort NL829_SetBPS(uchar bps)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_SETBPS, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = bps;
	if((ret=NL829_frame_factory(data, NL829_SETBPS_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_SETBPS_APPLEN_RCV)
		return NL829_ERR_CMD_SETBPS;
	return NL829_GETSTAVAL(status);
}

ushort NL829_pack_PP60cmd(uchar *PP60cmd, int PP60cmdlen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0, datalen = NL829_LEN_CMD+1+PP60cmdlen;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/	
	memcpy(data, NL829_PP60_PACKCMD, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = DEVICE_PP60RF;
	memcpy(data+NL829_OFFSET_ARGS+1, PP60cmd, PP60cmdlen);
	if((ret=NL829_frame_factory(data, datalen, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(NL829_GETSTAVAL(status)!=NL829_GETSTAVAL(NL829_PP60_PACKCMD))
		return NL829_ERR_PP60_PACKCMD;
	if(appendlen!=NL829_PP60_PACKCMD_APPLEN_RCV)
		return NL829_ERR_PP60_PACKCMD1;
	return NL829_GETSTAVAL(append);
}

//sec为等待读取PP60命令返回的时间（秒）,==0:读取现有缓冲区即返回;>300,外设按300算
//inlen为希望收到的PP60返回数据的长度,大于512，外设按512算
//注意:根据设计,实际收到的数据量必定小等于希望收到的量
ushort NL829_get_PP60rsp(ushort sec, ushort inlen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, sec1 = 0, inlen1 = 0, outlen1 = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;//注:staval不再是829的status域的值了而是其后的PP60返回的状态域的值
	
	/*process body*/	
	memcpy(data, NL829_PP60_GETRSP, NL829_LEN_CMD);
	little2big_ushort(sec, &sec1);
	memcpy(data+NL829_OFFSET_ARGS, &sec1, sizeof(sec1));
	little2big_ushort(inlen, &inlen1);
	memcpy(data+NL829_OFFSET_ARGS+sizeof(sec1), &inlen1, sizeof(inlen1));
	if((ret=NL829_frame_factory(data, NL829_PP60_GETRSP_DATALEN_SND, status, append, &appendlen, sec))!=NL829_SUCC)
		return ret;
	if(NL829_GETSTAVAL(status)!=NL829_GETSTAVAL(NL829_PP60_GETRSP))
		return NL829_ERR_PP60_GETRSP;
	if((staval=NL829_GETSTAVAL(append))==NL829_PP60_SUCC)
	{
		outlen1 = NL829_2BYTESTOUSHORT(append+sizeof(staval));
		if(appendlen!=sizeof(staval)+sizeof(outlen1)+outlen1)
			return NL829_ERR_PP60_GETRSP1;
		if(outlen1==0)//超时未收到数据
			return NL829_ERR_PP60_GETRSP2;
		*outlen = outlen1;
		memcpy(out, append+sizeof(staval)+sizeof(outlen1), outlen1);
	}
	else
	{
		if(appendlen!=sizeof(staval))
			return NL829_ERR_PP60_GETRSP3;
	}
	return staval;
}

//本函数可用于判断nini829与PP60能否通讯上
ushort NL829_cnnt_PP60(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;//注:staval不再是829的status域的值了而是其后的PP60返回的状态域的值
	
	/*process body*/
	memcpy(data, NL829_PP60_CNNT, NL829_LEN_CMD);
	strcpy((char *)data+NL829_OFFSET_ARGS, NL829_PP60_CNNT_DATA);
	if((ret=NL829_frame_factory(data, NL829_PP60_CNNT_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(NL829_GETSTAVAL(status)!=NL829_GETSTAVAL(NL829_PP60_CNNT))
		return NL829_ERR_PP60_CNNT;
	if((staval=NL829_GETSTAVAL(append))==NL829_PP60_SUCC)
	{
		if(appendlen!=sizeof(staval)+1)
			return NL829_ERR_PP60_CNNT1;
	}
	else
	{
		if(appendlen!=sizeof(staval))
			return NL829_ERR_PP60_CNNT2;
	}
	return staval;
}

//新增sp10_T命令帧处理包括组包、发包、收包、解包
//此函数是命令串的构造(data是命令包括其后的数据,不包括尾部ETX和LRC)
int SP10_T_pack_data(uchar *data, int datalen, uchar *frame)
{
	int ETXOffset = SP10_T_LEN_FRAMEHEAD+datalen;
	int framelen = ETXOffset+SP10_T_LEN_FRAMETAIL;
	
	/*process body*/
	if(data==NULL||datalen<SP10_T_LEN_CMD||datalen>SP10_T_MAXLEN_DATA)
		return FAIL;
	frame[SP10_T_OFFSET_STX] = STX;
	frame[SP10_T_OFFSET_DATALEN] = (datalen+SP10_T_LEN_ETX)/256; //实际len是包含ETX
	frame[SP10_T_OFFSET_DATALEN+1] = (datalen+SP10_T_LEN_ETX)%256;
	memcpy(frame+SP10_T_OFFSET_DATA, data, datalen);
	frame[ETXOffset] =ETX ;
	frame[ETXOffset+SP10_T_LEN_LRC] = MakeLRC(frame+SP10_T_OFFSET_DATALEN, SP10_T_LEN_DATALEN+datalen+SP10_T_LEN_ETX);
	return framelen;
}

//此函数是对命令响应的解析(对data域的解析cmd+数据:answer+append)
int SP10_T_unpack_data(uchar *data, int datalen, uchar *answer, uchar *append)
{
	//如果是Fx\Ex自定义类的命令有状态值返回,如果是Bx系列的命令无状态值就直接对数据判断?
	int appendlen = 0;
	memcpy(answer, data, SP10_T_LEN_CMD);//响应命令
	memcpy(append, data+SP10_T_LEN_CMD, datalen-SP10_T_LEN_CMD);
	appendlen=(datalen-1);
	return appendlen;
}

//发送
ushort SP10_T_send_frame(uchar *cmdstr, int cmdlen)
{
	/*private & local definition*/
	int ret = 0;
	/*process body*/
	ret=serial_clrportbuf_all(g_devicecom);
	//外设的命令帧长度一般不会超过串口最大缓冲,故portwrite一般都会成功,可不判或简单判断返回
	return (NDK_PortWrite(g_devicecom, cmdlen, (char *)cmdstr)==NDK_OK)?SP10_T_SUCC:SP10_T_FAIL;
}

//由于按协议字段接收,故提前判断除data以外的字段是否正确,而将data域放到解包中去分析与校验
ushort SP10_T_recv_frame(uchar *data, int *datalen, ushort timeout)
{
	/*private & local definition*/
	int len = 0, ETXOffset = 0, ret = 0, rlen = 0;
	uchar frame[SP10_T_MAXLEN_FRAME] = {0};
	
	/*process body*/
	//注意:实际上由于应答帧一齐回过来,故其实在能读到STX时,后续数据也早已到达接收缓冲的,
	//故只要接收预期长度合适,接收超时可以设短(以下设为0),以提高接收效率
	if((ret=NDK_PortRead(g_devicecom, SP10_T_LEN_STX, (char *)frame, (SP10_T_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP10_T_LEN_STX||frame[SP10_T_OFFSET_STX]!=STX)
	{
		return SP10_T_ERR_FRAME_STX;
	}
	if((ret=NDK_PortRead(g_devicecom, SP10_T_LEN_DATALEN, (char *)frame+SP10_T_OFFSET_DATALEN, /*0*/(SP10_T_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP10_T_LEN_DATALEN)	
		return SP10_T_ERR_FRAME_DATALEN1;
	len=SP10_T_2BYTESTOUSHORT(frame+SP10_T_OFFSET_DATALEN)-1;	//计算出来的len是包含ETX 所以需要减1
	if(len<1||len>SP10_T_MAXLEN_DATA)
		return SP10_T_ERR_FRAME_DATALEN2;
	//len:CMD+后面的数据不包含ETX
	if((ret=NDK_PortRead(g_devicecom, len, (char *)frame+SP10_T_OFFSET_DATA, /*0*/(SP10_T_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=len)			
		return SP10_T_ERR_FRAME_DATA;
	ETXOffset = SP10_T_LEN_FRAMEHEAD+len;
	if((ret=NDK_PortRead(g_devicecom, SP10_T_LEN_ETX, (char *)frame+ETXOffset, /*0*/(NL829_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP10_T_LEN_ETX||frame[ETXOffset]!=ETX)	
		return SP10_T_ERR_FRAME_ETX;
	if((ret=NDK_PortRead(g_devicecom, SP10_T_LEN_LRC, (char *)frame+ETXOffset+SP10_T_LEN_ETX, /*0*/(SP10_T_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP10_T_LEN_LRC||frame[ETXOffset+SP10_T_LEN_ETX]!=MakeLRC(frame+SP10_T_OFFSET_DATALEN, SP10_T_LEN_DATALEN+len+SP10_T_LEN_ETX))					
		return SP10_T_ERR_FRAME_LRC;

	//以上都按预期接收到了才更新输出参数并返回SUCC
	memcpy(data, frame+SP10_T_OFFSET_DATA, len);
	*datalen = len;
	return SP10_T_SUCC;
}

//data为应用发送的命令及参数,answer为应用接收的响应命令,append为除命令以外的附加数据
//datalen为上层指定发送的命令及参数的总长度
//appendlen为底层返回给上层的除命令以外的附加数据的长度
//timeout:对于一些有超时要求的指令(执行完毕或超时才返回)可以使用,不理会的传入0即可
//本函数返回SP10_T_SUCC或SP10_T_ERR_FRAME_BASE为基的一族错误码
ushort SP10_T_frame_factory(uchar *data, int datalen, uchar *answer, uchar *append, int *appendlen, ushort timeout)
{
	/*private & local definition*/
	int ret = 0, len = 0;
	ushort ret1 = SP10_T_FAIL;
	uchar frame_cmd[SP10_T_MAXLEN_FRAME] = {0};
	uchar resp[SP10_T_MAXLEN_DATA] = {0};//frame_resp中的data域
	
	/*process body*/
	//组包
	len = SP10_T_pack_data(data, datalen, frame_cmd);
#if defined DEBUG_CMD//调试命令范例
	if(!memcmp(data, DEBUG_CMD, SP10_T_LEN_CMD))
	{
		int i = 0;
		cls_printf("line %d:", __LINE__);//__FILE__太长了,影响后续数据的显示,故此处不显示它
		for(; i<len; i++)
			NDK_ScrPrintf(" %02X", *(frame_cmd+i));
		NDK_ScrRefresh();
		lib_getch();
	}
#endif
	//发包
	if((ret1=SP10_T_send_frame(frame_cmd, len))!=SP10_T_SUCC)
		return ret1;	
	//收包
	if((ret1=SP10_T_recv_frame(resp, &len, timeout))!=SP10_T_SUCC)
		return ret1;//返回具体的接收错误
#if defined DEBUG_CMD//调试命令范例
	if(!memcmp(data, DEBUG_CMD, SP10_T_LEN_CMD))
	{	
		int i = 0;
		cls_printf("line %d: %d", __LINE__, len);//__FILE__太长了,影响后续数据的显示,故此处不显示它
		for(; i<len; i++)
			NDK_ScrPrintf(" %02X", *(resp+i));
		NDK_ScrRefresh();
		lib_getch();
	}
#endif	

	//解包
	ret=SP10_T_unpack_data(resp, len, answer, append);
	if((ret!=len-1) && (ret!=0))//返回的append长度可能为0或者是len-1,不是这两者报错
		return SP10_T_ERR_FRAME_UNPACK;
	*appendlen = ret;
	return SP10_T_SUCC;
}

//新增SP100密码键盘海外版指令集帧处理包括组包、发包、收包、解包
//此函数是命令串的构造(data是命令包括其后的数据,不包括尾部ETX和LRC)
int SP100_pack_data(uchar *data, int datalen, uchar *frame)
{
	int ETXOffset = SP100_LEN_FRAMEHEAD+datalen;
	int framelen = ETXOffset+SP100_LEN_FRAMETAIL;
	int len =2;
	uchar bcdbuf[2] = {0};
	
	/*process body*/
	if(data==NULL||datalen<SP100_LEN_CMD||datalen>SP100_MAXLEN_DATA)
		return FAIL;
	frame[SP100_OFFSET_STX] = STX;
	NDK_IntToBcd(bcdbuf,&len,datalen);//len使用的是2字节的BCD码,使用int转bcd
	memcpy(frame+SP100_OFFSET_DATALEN, bcdbuf, 2);
	memcpy(frame+SP100_OFFSET_DATA, data, datalen);
	frame[ETXOffset] =ETX ;
	frame[ETXOffset+SP100_LEN_ETX] = MakeLRC(frame+SP100_OFFSET_DATALEN, SP100_LEN_DATALEN+datalen+SP100_LEN_ETX);
	return framelen;
}
//此函数是对命令响应的解析(对data域的解析cmd+数据:answer+append)
int SP100_unpack_data(uchar *data, int datalen, uchar *answer, uchar *append)
{
	int appendlen = 0;
	memcpy(answer, data, SP100_LEN_CMD);//响应命令
	memcpy(append, data+SP100_LEN_CMD, datalen-SP100_LEN_CMD);
	appendlen=(datalen-2);
	return appendlen;
}
//发送
ushort SP100_send_frame(uchar *cmdstr, int cmdlen)
{
	/*private & local definition*/
	int ret = 0;
	/*process body*/
	ret=serial_clrportbuf_all(g_devicecom);
	//外设的命令帧长度一般不会超过串口最大缓冲,故portwrite一般都会成功,可不判或简单判断返回
	return (NDK_PortWrite(g_devicecom, cmdlen, (char *)cmdstr)==NDK_OK)?SP100_SUCC:SP100_FAIL;
}
//由于按协议字段接收,故提前判断除data以外的字段是否正确,而将data域放到解包中去分析与校验
ushort SP100_recv_frame(uchar *data, int *datalen, ushort timeout)
{
	/*private & local definition*/
	int len = 0, ETXOffset = 0, ret = 0, rlen = 0;
	uchar frame[SP100_MAXLEN_FRAME] = {0};
	
	/*process body*/
	//注意:实际上由于应答帧一齐回过来,故其实在能读到STX时,后续数据也早已到达接收缓冲的,
	//故只要接收预期长度合适,接收超时可以设短(以下设为0),以提高接收效率
	if((ret=NDK_PortRead(g_devicecom, SP100_LEN_STX, (char *)frame, (SP100_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP100_LEN_STX||frame[SP100_OFFSET_STX]!=STX)
	{
		return SP100_ERR_FRAME_STX;
	}
	
	if((ret=NDK_PortRead(g_devicecom, SP100_LEN_DATALEN, (char *)frame+SP100_OFFSET_DATALEN, /*0*/(SP100_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP100_LEN_DATALEN)	
		return SP100_ERR_FRAME_DATALEN1;
	NDK_BcdToInt(frame+SP100_OFFSET_DATALEN, &len);//len的格式是2字节bcd码,要转成int	
	if(len<1||len>SP100_MAXLEN_DATA)
		return SP100_ERR_FRAME_DATALEN2;
	//len:CMD+后面的数据不包含ETX
	if((ret=NDK_PortRead(g_devicecom, len, (char *)frame+SP100_OFFSET_DATA, /*0*/(SP100_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=len)			
		return SP100_ERR_FRAME_DATA;
	ETXOffset = SP100_LEN_FRAMEHEAD+len;
	if((ret=NDK_PortRead(g_devicecom, SP100_LEN_ETX, (char *)frame+ETXOffset, /*0*/(SP100_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP100_LEN_ETX||frame[ETXOffset]!=ETX)	
		return SP10_T_ERR_FRAME_ETX;
	if((ret=NDK_PortRead(g_devicecom, SP100_LEN_LRC, (char *)frame+ETXOffset+SP100_LEN_ETX, /*0*/(SP100_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP100_LEN_LRC||frame[ETXOffset+SP100_LEN_ETX]!=MakeLRC(frame+SP100_LEN_STX, SP100_LEN_DATALEN+len+SP100_LEN_ETX))					
		return SP100_ERR_FRAME_LRC;
	//以上都按预期接收到了才更新输出参数并返回SUCC
	memcpy(data, frame+SP100_OFFSET_DATA, len);
	*datalen = len;
	return SP100_SUCC;
}
//判断接收到的是ACK还是NAK
ushort SP100_recv_ACKorNAK(uchar *data, int *datalen, ushort timeout)
{
	/*private & local definition*/
	int   ret = 0,rlen = 0;
	uchar frame[2] = {0};
	
	//先收到ACK或者NAK答复
	if((ret=NDK_PortRead(g_devicecom, SP100_LEN_ACK_NAK, (char *)frame, (SP100_TIMEOUT_BASE+timeout)*1000, &rlen))!=NDK_OK||rlen!=SP100_LEN_ACK_NAK)
	{
		return SP100_ERR_ACK_NAK;
	}
	memcpy(data, frame, rlen);
	*datalen = rlen;
	return SP100_SUCC;
}
//data为应用发送的命令及参数,answer为应用接收的响应命令,append为除命令以外的附加数据
//datalen为上层指定发送的命令及参数的总长度
//appendlen为底层返回给上层的除命令以外的附加数据的长度
//timeout:对于一些有超时要求的指令(执行完毕或超时才返回)可以使用,不理会的传入0即可
//本函数返回SP100_SUCC或SP100_ERR_FRAME_BASE为基的一族错误码
ushort SP100_frame_factory(uchar *data, int datalen, uchar *answer, uchar *append, int *appendlen, ushort timeout)
{
	/*private & local definition*/
	int ret = 0, len = 0, len1;
	ushort ret1 = SP100_FAIL;
	uchar frame_cmd[SP100_MAXLEN_FRAME] = {0};
	uchar resp[SP100_MAXLEN_DATA] = {0};//frame_resp中的data域
	uchar recv_ackornak[2] = {0};
	
	/*process body*/
	//组包
	len = SP100_pack_data(data, datalen, frame_cmd);
#if defined DEBUG_CMD//调试命令范例
	if(!memcmp(data, DEBUG_CMD, SP100_LEN_CMD))
	{
		int i = 0;
		cls_printf("line %d:", __LINE__);//__FILE__太长了,影响后续数据的显示,故此处不显示它
		for(; i<len; i++)
			NDK_ScrPrintf(" %02X", *(frame_cmd+i));
		NDK_ScrRefresh();
		lib_getch();
	}
#endif
	//发包
	if((ret1=SP100_send_frame(frame_cmd, len))!=SP100_SUCC)
		return ret1;	
	//收应答包ack or nak
	if((ret1=SP100_recv_ACKorNAK(recv_ackornak, &len1, timeout))!=SP100_SUCC)
		return ret1;
	//如果是ack 说明成功接收消息然后再进行收包解包
	if(recv_ackornak[0] == ACK)
	{
		//收包
		if((ret1=SP100_recv_frame(resp, &len, timeout))!=SP100_SUCC)
			return ret1;//返回具体的接收错误
#if defined DEBUG_CMD//调试命令范例
		if(!memcmp(data, DEBUG_CMD, SP100_LEN_CMD))
		{	
			int i = 0;
			cls_printf("line %d: %d", __LINE__, len);//__FILE__太长了,影响后续数据的显示,故此处不显示它
			for(; i<len; i++)
				NDK_ScrPrintf(" %02X", *(resp+i));
			NDK_ScrRefresh();
			lib_getch();
		}
#endif	

		//解包
		ret=SP100_unpack_data(resp, len, answer, append);
		//返回的append 待定
		//return SP100_ERR_FRAME_UNPACK;
		*appendlen = ret;
		//发送ACK给pinpad
		if((ret1=SP100_send_frame("\x06", 1))!=SP100_SUCC)
			return ret1;
		return SP100_SUCC;
	}
}
//只有ACK 返回的指令帧层进行封装发包 收包
ushort SP100_frame_factory_ACK(uchar *data, int datalen, uchar *answer, int *answerlen, ushort timeout)
{
	/*private & local definition*/
	int len = 0;
	ushort ret = SP100_FAIL;
	uchar frame_cmd[SP100_MAXLEN_FRAME] = {0};
	
	/*process body*/
	//组包
	len = SP100_pack_data(data, datalen, frame_cmd);
#if defined DEBUG_CMD//调试命令范例
	if(!memcmp(data, DEBUG_CMD, SP100_LEN_CMD))
	{
		int i = 0;
		cls_printf("line %d:", __LINE__);//__FILE__太长了,影响后续数据的显示,故此处不显示它
		for(; i<len; i++)
			NDK_ScrPrintf(" %02X", *(frame_cmd+i));
		NDK_ScrRefresh();
		lib_getch();
	}
#endif
	//发包
	if((ret=SP100_send_frame(frame_cmd, len))!=SP100_SUCC)
		return ret;	
	//收应答包ack or nak
	if((ret=SP100_recv_ACKorNAK(answer, answerlen, timeout))!=SP100_SUCC)
		return ret;
	
	return SP100_SUCC;
}

#endif

#if USB_ENABLE||SD_ENABLE
#define	BUFFERSIZE	(1024*20)
#define	TESTFILE	"test.file"
int systest_SDCard( int disktype)
{
	/*private & local definition*/
	char rootdir[16] = {0}, fname[64] = {0}, writebuf[BUFFERSIZE] = {0}, readbuf[BUFFERSIZE] = {0};
	int ret = 0, fp = 0, loop = 0,state = 0, cnt = rand()%1023+2 /*测试文件大小大约介于[40K, 20M]*/, wrlen = BUFFERSIZE-rand()%8;
	time_t oldtime = 0, oldtime1 = 0, diff2 = 0;	
	
	/*process body*/
	if((ret=NDK_DiskOpen(disktype,MAXTIMEOUT))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:%s打开失败(%d)", __FILE__,  __LINE__,(disktype==UDISK)?"U盘":"SD卡", ret);
		return FAIL;
	}
	oldtime1=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&state))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s, line %d:获取状态失败(%d)", __FILE__, __LINE__, ret);
			return FAIL;
		} 
		if(state==DISKMOUNTSUCC)
			break;
		if(time(NULL)-oldtime1>30)
		{
			cls_show_msg1(g_keeptime, "%s, line %d:获取状态超时(%d)", __FILE__, __LINE__, ret);	
			return FAIL;
		}
		NDK_SysMsDelay(1);
	}
	if((ret=NDK_DiskGetRootDirName(disktype,(char **)&rootdir))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:获取路径失败(%d)", __FILE__, __LINE__, ret);
		goto ERR;
	}
	//cls_show_msg1(5, "以下测试使用%dB作为每次读写长度!", wrlen);
	sprintf(fname, "%s/%s", rootdir, TESTFILE);
	NDK_FsDel(fname);
	if ((fp=NDK_FsOpen(fname, "w"))<0)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:创建文件失败(%d)", __FILE__, __LINE__, ret);
		goto ERR;
	}
	
	for (loop=0; loop<wrlen; loop++)
		writebuf[loop] = rand()%256;
	for (loop=0; loop<cnt; loop++)
	{
		cls_printf("生成%s文件中(约%dkB)已生成(约%dkB),请稍候...",(disktype==UDISK)?"U盘":"SD卡", cnt*wrlen/1024, loop*wrlen/1024);
		if ((ret=NDK_FsWrite(fp,writebuf, wrlen ))!=wrlen)
		{
			cls_show_msg1(g_keeptime, "%s, line %d:写文件失败(cnt=%d, loop=%d, ret=%d, wrlen=%d)", __FILE__, __LINE__, cnt, loop, ret, wrlen);
			goto ERR;
		}
	}
	
	if(NDK_FsClose(fp)!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:关闭文件失败(%d)", __FILE__, __LINE__, ret);
		goto ERR;
	}
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:%s关闭失败(%d)", __FILE__,  __LINE__,(disktype==UDISK)?"U盘":"SD卡", ret);
		goto ERR;
	}
	if((ret=NDK_DiskOpen(disktype,MAXTIMEOUT))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:%s打开失败(%d)", __FILE__,  __LINE__,(disktype==UDISK)?"U盘":"SD卡", ret);
		goto ERR;
	}	
	oldtime1=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&state))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "%s, line %d:获取状态失败(%d)", __FILE__, __LINE__, ret);
			goto ERR;
		} 
		if(state==DISKMOUNTSUCC)
			break;
		if(time(NULL)-oldtime1>30)
		{
			cls_show_msg1(g_keeptime, "%s, line %d:获取状态超时(%d)", __FILE__, __LINE__, ret);	
			goto ERR;
		}
		NDK_SysMsDelay(1);	
	}
	if ((fp=NDK_FsOpen(fname, "r"))<0)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:打开文件失败(%d)", __FILE__, __LINE__, ret);
		goto ERR;
	}
	//文件大小是否发生改变
	//if ((ret=lib_filelength(fp))!=cnt*wrlen)
		
	if(NDK_FsFileSize(fname,(uint *)&ret)!=NDK_OK||ret!=cnt*wrlen)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:文件大小校验失败(实测:%dB, 预期:%dB)", __FILE__, __LINE__, ret, cnt*wrlen);
		goto ERR;
	}
	
	cls_printf("校验SD盘文件中(约%dkB),请稍候...", cnt*wrlen/1024);
	oldtime = time(NULL);
	for (loop=0; loop<cnt; loop++)
	{
		if ((ret=NDK_FsRead(fp,readbuf, wrlen ))!=wrlen||memcmp(writebuf, readbuf, wrlen))
		{
			cls_show_msg1(g_keeptime, "%s, line %d:数据读写不一致(cnt=%d, loop=%d, ret=%d, wrlen=%d)", __FILE__, __LINE__, cnt, loop, ret, wrlen);
			debug_buf("发送缓冲", writebuf, wrlen);
			debug_buf("接收缓冲", readbuf, ret);
			cls_show_msg("请保存数据并关闭串口,任意键继续...");//防止过早进入UsbDiskClose后,也将USB调试信息记录下来产生干扰
			goto ERR;
		}
		show_stopwatch(ENABLE, (diff2=time(NULL)-oldtime));
	}
	show_stopwatch(DISABLE, 0);	
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:关闭文件失败(%d)", __FILE__, __LINE__, ret);
		goto ERR;
	}
	if((ret=NDK_FsDel(fname))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:删除文件失败(%d)", __FILE__, __LINE__, ret);
		goto ERR;
	}
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "%s, line %d:%s关闭失败(%d)", __FILE__,  __LINE__,(disktype==UDISK)?"U盘":"SD卡", ret);
		goto ERR;
	}
	return SUCC;
ERR:
	NDK_FsClose(fp);
	NDK_FsDel(fname);
	NDK_DiskClose(disktype);
	return FAIL;
}
#else
int systest_SDCard( int disktype){return FAIL;}
#endif

/*生成按钮,并校验射频交易过程是否会影响到触屏的准确性*/
#if TOUCH_ENABLE
int systest_touch(void)
{
	int start_x1_point = 0, start_y1_point = 0, ret = 0;
#if CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG
	int wide = 50, height = 50;
#else
	int wide = 20, height = 20;
#endif
	ST_PADDATA pad_data;
	uint unWidth = 0, unHeight = 0;

	NDK_ScrGetViewPort(NULL, NULL, &unWidth, &unHeight);
	//unWidth = GUI_GetLCDWidth();
	//unHeight = GUI_GetLCDHeight();
	
	start_x1_point = rand()%(unWidth-wide-2);
	start_y1_point = rand()%(unHeight-height-2);

	NDK_ScrClrs();
	cls_printf("请点击屏幕上显示的黑框中心");
	if((ret = NDK_ScrRectangle(start_x1_point,start_y1_point,wide,height,RECT_PATTERNS_SOLID_FILL,0))!=NDK_OK)
	{
		cls_show_msg1(2, "%s,line %d测试失败(%d)", __FILE__,__LINE__,ret);
		return FAIL;
	}
	NDK_ScrRectangle(start_x1_point-2,start_y1_point-2,wide+4,height+4,RECT_PATTERNS_NO_FILL,0);//该语句于测试无关,不判断返回,linwl 20140126
	//if((ret = NDK_ScrRectangle(start_x1_point-2,start_y1_point-2,wide+4,height+4,RECT_PATTERNS_NO_FILL,0))!=NDK_OK)
	//{
		//cls_show_msg1(2, "%s,line %d测试失败(%d)", __FILE__,__LINE__,ret);
		//return FAIL;
	//}
	NDK_ScrRefresh();
	while(1)
	{
		memset(&pad_data,0,sizeof(pad_data));
		if((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
		{
			cls_show_msg1(2, "%s,line %d测试失败(%d)", __FILE__,__LINE__,ret);
			return FAIL;
		}
		if (pad_data.emPadState == PADSTATE_KEY  && pad_data.unKeycode == ESC)
		{
			cls_show_msg1(3,"用户取消");
			return SUCC;
		}
		if(pad_data.emPadState == PADSTATE_UP)
		{
			if(((pad_data.unPadX >= start_x1_point) && (pad_data.unPadX <= start_x1_point+wide))&&((pad_data.unPadY >= start_y1_point) && (pad_data.unPadY <= start_y1_point+height)))
			{	
				return SUCC;
			}	
			else
			{
				cls_show_msg("屏幕点漂移,预期(%d~%d,%d~%d),实际(%d,%d)",start_x1_point,start_x1_point+wide,start_y1_point,start_y1_point+height,pad_data.unPadX,pad_data.unPadY);
				return FAIL;
			}
		}
	}
}
#else
int systest_touch(void){return FAIL;}
#endif

#if 0	//该接口不再支持
//这个接口不是设置PP60的BPS,而是设置与PP60通讯的829串口BPS
ushort NL829_SetBPS_PP60(uchar type, uchar bps, uchar *ret_bps)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;//注:staval不再是829的status域的值了而是其后的PP60返回的状态域的值
	
	/*process body*/
	memcpy(data, NL829_PP60_SETBPS, NL829_LEN_CMD);
	if(type==DEVICE_PP60_1&&bps==PP60_BPS_SELFADAPT)
		type = 0x03;
	else if(type==DEVICE_PP60_2&&bps==PP60_BPS_SELFADAPT)
		type = 0x04;
	data[NL829_OFFSET_ARGS] = type;
	data[NL829_OFFSET_ARGS+1] = bps;
	if((ret=NL829_frame_factory(data, NL829_PP60_SETBPS_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(NL829_GETSTAVAL(status)!=NL829_GETSTAVAL(NL829_PP60_SETBPS))
		return NL829_ERR_PP60_SETBPS;
	if((staval=NL829_GETSTAVAL(append))=NL829_PP60_SUCC)
	{
		if(appendlen!=sizeof(staval)+1)
			return NL829_ERR_PP60_SETBPS1;
		*ret_bps = append[sizeof(staval)];
	}
	else
	{
		if(appendlen!=sizeof(staval))
			return NL829_ERR_PP60_SETBPS2;
	}
	return staval;
}
#endif

//深度交叉用例保存连接参数的函数
LinkSetting links[10];
NetworkSetting nets[10];

#if 1//根据交叉类型参数保存全局变量和恢复全局变量
void Local2Global(LinkType lnktype)
{
	memset(&linksetting,0,sizeof(LinkSetting));	
	memset(&netsetting,0,sizeof(NetworkSetting));	
	memcpy(&linksetting, &links[lnktype], sizeof(LinkSetting));	
	memcpy(&netsetting, &nets[lnktype], sizeof(NetworkSetting));
}

void Global2Local(LinkType lnktype)
{	
	memset(&links[lnktype],0,sizeof(LinkSetting));	
	memset(&nets[lnktype],0,sizeof(NetworkSetting));		
	memcpy(&links[lnktype], &linksetting, sizeof(LinkSetting));	
	memcpy(&nets[lnktype], &netsetting, sizeof(NetworkSetting));
}
#endif

#if (NDKSOCK_ENABLE || NDKSSL_ENABLE)
void tcp_abnormal(void)
{
	/*private & local definition*/
	uint h_tcp = 0;
	char buf[ETHPACKMAXLEN] = {0};
	int  j = 0, timeout = 1, nKey = 0;
	int slen = 0;
	//Packet sendpacket;

	/*process body*/
	//case1:分别进行强制挂断和协议挂断的测试
	for (j=0; j<sizeof(buf); j++)
		buf[j]=rand()%256;

	nKey = cls_show_msg("请选择挂断方式\n"
						"1.强制挂断\n"
						"2.协议挂断");
	if(nKey=='1')//强制挂断则修改挂断标志为0
		TCPRESETFLAG=TRUE;
	
	if(NetUp()!=SUCC)
	{
		cls_show_msg("line %d:NetUp失败", __LINE__);
		return;
	}

	if(TransUp(&h_tcp)!=SUCC)
	{
		NetDown();
		cls_show_msg("line %d:TransUp失败", __LINE__);
		return;
	}
		
	cls_show_msg("请拔掉POS或后台的网线,任意键继续");
	if((slen=sock_send(h_tcp, buf, ETHPACKMAXLEN, timeout))!=ETHPACKMAXLEN)
	{
		NetDown();
		cls_show_msg1(g_keeptime,"line %d:发送失败(实际%d)", __LINE__, slen);
		return;
	}
		
	TransDown(h_tcp);

	if(TCPRESETFLAG==FALSE)//协议挂断时，POS会重发数据包，重新插上网线后，后台会收到重发的数据包
	{
		if(cls_show_msg("重新插上POS或后台的网线后,等待20秒查看后台是否有收到数据,是:[确认],否:[其它]")!=ENTER)
		{
			cls_show_msg1(g_keeptime,"line %d:协议挂断测试失败", __LINE__);
		}
		else
		{
			cls_show_msg1(g_keeptime,"协议挂断测试通过", __LINE__);
		}
	}
	else//强制挂断时，POS不会重发数据包，重新插上网线后，后台不会收到重发的数据包
	{
		if(cls_show_msg("重新插上POS或后台的网线后,等待20秒查看后台是否有收到数据,是:[确认],否:[其它]")==ENTER)
		{
			cls_show_msg1(g_keeptime,"line %d:强制挂断测试失败", __LINE__);
		}
		else
		{
			cls_show_msg1(g_keeptime,"强制挂断测试通过", __LINE__);
		}
	}
	NetDown();
	TCPRESETFLAG = FALSE;//恢复默认方式:协议挂断
	
	return;
}
#endif
#endif
