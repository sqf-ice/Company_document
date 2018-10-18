/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: 
* file name			: systest39.c
* Author 			: chensj
* version			: 
* DATE				: 20130722
* directory 		: 
* description		: 硬件统计服务
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM "硬件统计服务"
#define	MAXTIMEOUT 30
#define 	TEMPFILE_NUM	"temp_OnDownNum"
#define 	TEMPFILE_TIME	"temp_OnTime"
#define STRCNT "30"
#define	MAXCNT atoi (STRCNT)
#define interval_time 5  //误差时间每次间隔5s 

/*------------global variables definition-----------------------*/
typedef enum
{
	KK_F1= 		0x01,
	KK_F2=		0x02,
	KK_F3=		0x03,
	KK_F4=		0x04,
	KK_BASP=	0x0a ,     /**<退格键*/
	KK_ENTER=	0x0D,	/**<确认键*/
	KK_ESC=		0x1B,	/**<取消键*/
	KK_ZMK=		0x1C,	/**<字母键*/
	KK_DOT=		0x2E,	/**<小数点*/
	KK_ZERO=	0x30 ,     /**<数字0键*/
	KK_ONE=		0x31,      /**<数字1键*/
	KK_TWO=		0x32,	/**<数字2键*/
	KK_THREE=	0x33,	/**<数字3键*/
	KK_FOUR=	0x34,	/**<数字4键*/
	KK_FIVE=		0x35,	/**<数字5键*/
	KK_SIX=		0x36,	/**<数字6键*/
	KK_SEVEN=	0x37,	/**<数字7键*/
	KK_EIGHT=	0x38,	/**<数字8键*/
	KK_NINE=	0x39,	/**<数字9键*/
}K_VALUE;

EM_SS_DEV_ID devid[] ={SS_KEYBOARD_ENTER, SS_KEYBOARD_BASP,SS_KEYBOARD_ESC,SS_KEYBOARD_ZMK,SS_KEYBOARD_ZERO,SS_KEYBOARD_DOT,\
		SS_KEYBOARD_NINE,SS_KEYBOARD_EIGHT,SS_KEYBOARD_SEVEN,SS_KEYBOARD_SIX,SS_KEYBOARD_FIVE,SS_KEYBOARD_FOUR,SS_KEYBOARD_THREE,\
		SS_KEYBOARD_TWO,SS_KEYBOARD_ONE,SS_KEYBOARD_F1,SS_KEYBOARD_F2,SS_KEYBOARD_F3,SS_KEYBOARD_F4,SS_KEYBOARD_0_ID,SS_KEYBOARD_1_ID,\
		SS_KEYBOARD_2_ID,SS_KEYBOARD_3_ID,/*SS_KEYBOARD_4_ID,SS_KEYBOARD_5_ID,SS_KEYBOARD_6_ID,SS_KEYBOARD_7_ID,SS_KEYBOARD_11_ID,\
		SS_KEYBOARD_15_ID,SS_KEYBOARD_19_ID,SS_KEYBOARD_27_ID,*/SS_KEYBOARD_TOTAL};

char devstr[][20]={"确认键","退格键","取消键","字母键","数字键0","小数点键",\
		"数字键9","数字键8","数字键7","数字键6","数字键5","数字键4","数字键3",\
		"数字键2","数字键1","方向键F1","方向键F2","方向键F3","方向键F4","未定义键0","未定义键1",\
		"未定义键2","未定义键3",/*"未定义键4","未定义键5","未定义键6","未定义键7","未定义键11",\
		"未定义键15","未定义键19","未定义键27",*/"所有按键"};

K_VALUE devkey[]={KK_ENTER,KK_BASP,KK_ESC,KK_ZMK,KK_ZERO,KK_DOT,\
			KK_NINE,KK_EIGHT,KK_SEVEN,KK_SIX,KK_FIVE,KK_FOUR,KK_THREE,\
			KK_TWO,KK_ONE,KK_F1,KK_F2,KK_F3,KK_F4};

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

static void KeyValue_stat(void)
{
	ulong  stOutPut,stOutPut2;
	int nRet = 0, i = 0;
	
	//case1:正常测试功能
	for(i=0;i<(sizeof(devid)/sizeof(devid[0]));i++)
	{
		stOutPut = 0;
		if((nRet = NDK_SysGetStatisticsData(devid[i],&stOutPut))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
			return;
		}
		switch(devid[i])
		{
			case SS_KEYBOARD_ENTER:
			case SS_KEYBOARD_BASP:
			case SS_KEYBOARD_ESC:
			case SS_KEYBOARD_ZMK:
			case SS_KEYBOARD_ZERO:
			case SS_KEYBOARD_DOT:
			case SS_KEYBOARD_NINE:
			case SS_KEYBOARD_EIGHT:
			case SS_KEYBOARD_SEVEN:
			case SS_KEYBOARD_SIX:
			case SS_KEYBOARD_FIVE:
			case SS_KEYBOARD_FOUR:
			case SS_KEYBOARD_THREE:
			case SS_KEYBOARD_TWO:
			case SS_KEYBOARD_ONE:
			case SS_KEYBOARD_F1:
			case SS_KEYBOARD_F2:
			case SS_KEYBOARD_F3:
			case SS_KEYBOARD_F4:
				//验证已定义的按键
				{
					while(1)
					{
						nRet = cls_show_msg("请输入一个[%s],如无该键请%s跳过测试",devstr[i],(devkey[i]==K_ESC?"ENTER":"ESC"));
						if(nRet == devkey[i])
							break;
						if(devkey[i]==K_ESC&&nRet==ENTER)
							goto END;
						if(nRet == ESC)
							goto END;
					}
					stOutPut2 = 0;
					if((nRet = NDK_SysGetStatisticsData(devid[i],&stOutPut2))!=NDK_OK)
					{
						cls_show_msg1(g_keeptime,"line %d:%s测试统计%s失败(%d)", __LINE__, TESTITEM, devstr[i], nRet);
						return;
					}
					if(stOutPut2-stOutPut != 1)
					{
						cls_show_msg1(g_keeptime,"line %d:%s测试统计%s失败(%ld,%ld)", __LINE__, TESTITEM, devstr[i], stOutPut2, stOutPut);
						return;
					}
					
					END: ;
				}
				break;
			case SS_KEYBOARD_0_ID:
			case SS_KEYBOARD_1_ID:
			case SS_KEYBOARD_2_ID:
			case SS_KEYBOARD_3_ID:
				//验证未定义的按键
				{
					stOutPut2 = 0;
					if((nRet = NDK_SysGetStatisticsData(devid[i],&stOutPut2))!=NDK_OK)
					{
						cls_show_msg1(g_keeptime,"line %d:%s测试统计%s失败(%d)", __LINE__, TESTITEM, devstr[i], nRet);
						return;
					}
					if(stOutPut2!=stOutPut)
					{
						cls_show_msg1(g_keeptime,"line %d:%s测试统计%s失败(%ld,%ld)", __LINE__, TESTITEM, devstr[i], stOutPut2, stOutPut);
						return;
					}
				}
				break;
			case SS_KEYBOARD_TOTAL:
				//验证总按键次数
				{
					cls_show_msg("按任意键继续");
					stOutPut2 = 0;
					if((nRet = NDK_SysGetStatisticsData(devid[i],&stOutPut2))!=NDK_OK)
					{
						cls_show_msg1(g_keeptime,"line %d:%s测试统计%s失败(%d)", __LINE__, TESTITEM, devstr[i], nRet);
						return;
					}
					if(stOutPut2-stOutPut!=1)
					{
						cls_show_msg1(g_keeptime,"line %d:%s测试统计%s失败(%ld,%ld)", __LINE__, TESTITEM, devstr[i], stOutPut2, stOutPut);
						return;
					}
				}
				break;
			default:
				break;
		}
	}
	return;
}
#if PRN_ENABLE//ME51不支持打印20170315 linying modified
static void Prnt_stat(void)
{
	int nRet = 0;
	ulong printlong = 0, heattime = 0, printlong2 = 0, heattime2 = 0, stitch = 0, stitch2 = 0;
	
	//统计打印的米数 单位毫米 mm
	if((nRet = NDK_SysGetStatisticsData(SS_PRT_PAPER_ID,&printlong))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	//统计加热时长
	if((nRet = NDK_SysGetStatisticsData(SS_PRT_HEAT_ID,&heattime))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	//统计打印出针次数
	if((nRet = NDK_SysGetStatisticsData(SS_PRT_STITCH_ID,&stitch))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	cls_show_msg("确保有打印纸,按任意键继续");

	if(NDK_PrnInit(0)!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败", __LINE__, TESTITEM);
		return;
	}
/*
	if(NDK_PrnStr("0国国国国\n1国国国国\n2国国国国\n3国国国国\n4国国国国\n5国国国国\n6国国国国\n")!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败", __LINE__, TESTITEM);
		return;
	}
	if(NDK_PrnStart()!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败", __LINE__, TESTITEM);
		return;
	}
*/
	NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE,1);//保持和性能测试部分效果一致
	print_fill();//使用填充单来代替打印国字，方便计算热敏加热时长和打印出针次数 20140708 linwl
	
	//统计打印的米数 单位毫米 mm
	if((nRet = NDK_SysGetStatisticsData(SS_PRT_PAPER_ID,&printlong2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	if(cls_show_msg("打印的长度是否为%ld毫米,是:[确认],否:[其它键]", printlong2-printlong)!=ENTER)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, printlong, printlong2);
		return;
	}
	
	//case2:统计热敏加热头加热时长		//针打不统计 允许底层返回的是0，且不增加
	if((nRet = NDK_SysGetStatisticsData(SS_PRT_HEAT_ID,&heattime2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	if(cls_show_msg("加热时长是否为%ld豪秒,是:[确认],否:[其它键]", heattime2-heattime)!=ENTER)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, heattime, heattime2);
		return;
	}

	//case2:统计针打出针次数		//热敏不统计 允许底层返回的是0，且不增加
	if((nRet = NDK_SysGetStatisticsData(SS_PRT_STITCH_ID,&stitch2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	if(cls_show_msg("针打出针次数是否为%ld次,是:[确认],否:[其它键]", stitch2-stitch)!=ENTER)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, stitch, stitch2);
		return;
	}
	
	cls_show_msg("统计类打印设备测试通过,任意键继续");
	return;
}
#else
static void Prnt_stat(void) {cls_show_msg("不支持打印!"); return;}
#endif
static void Rfid_stat(void)
{
	int nRet = 0, pUIDlen = 0, i = 0, cnt = 0, succ = 0, rcvlen = 0;
	ulong seektimes = 0, seektimes2 = 0;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA"};
	uchar UID[LEN_UID] = {0};
	SMART_t cardtype = CPU_A;
	
	cardtype = select_smarttype();
	switch(cardtype)
	{
		case CPU_A:
		case CPU_B:
		case MIFARE_1:
		case FELICA:
		case MIFARE_0:	
			break;
		case SAM:
		case IC:
			cls_show_msg1(3,"默认射频A卡,任意键继续");
			cardtype = CPU_A;
			break;
	}
	
	if(RFID_init(cardtype)!=SUCC)
		cls_show_msg("line %d:初始化失败!请检查配置是否正确.", __LINE__);
	else
		cls_show_msg1(2, "%s初始化成功!", card[cardtype]);
	
	//case1:统计寻卡次数	 
	if((nRet = NDK_SysGetStatisticsData(SS_RFID_TIMES_ID,&seektimes))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}

	cnt = rand()%20+1;
	for(i=0;i<cnt;i++)
	{	
		memset(UID,0,sizeof(UID));
		pUIDlen = 0;
		RFID_deactive(cardtype, 0);
		cls_printf("请放一张%s卡,按任意键继续,现在%d次,总共%d次",card[cardtype],i+1,cnt);
		//寻卡
		if((nRet=RFID_detect(cardtype, UID, &pUIDlen))!=SUCC)
		{
			cls_show_msg1(2,"寻卡失败");
			continue;
		}	
		//激活
		if ((nRet=RFID_active(cardtype, UID,&pUIDlen))!=SUCC)
		{	
			continue;
		}
		//读写
		if ((nRet=RFID_APDUrw(cardtype, UID, &rcvlen))!=SUCC)
		{	
			continue;
		}
		//关闭场
		if ((nRet=RFID_deactive(cardtype, 0))!=SUCC)
		{	
			continue;
		}
		succ++;
	}
	if((nRet = NDK_SysGetStatisticsData(SS_RFID_TIMES_ID,&seektimes2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM,nRet);
		return;
	}
	//底层调用一次寻卡函数会寻卡多次，无法精确计算次数,就模糊比较大小,jiangym
	if(seektimes2<=seektimes)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, seektimes2, seektimes);
		return;
	}
	
	cls_show_msg("统计%s卡测试通过,重新跑此用例测其他卡,任意键继续",card[cardtype]);
	return;
}

//IC卡插卡次数驱动未支持
static void Ic_stat(void)
{
	int nRet = 0, nAtrlen=0, i=0, cnt=0, succ=0;
	ulong ictimes = 0, ictimes2 = 0;
	uchar psAtrbuf[256]={0};
	
	//case1:统计插卡次数
	 
	if((nRet = NDK_SysGetStatisticsData(SS_ICCARD_BASE_ID,&ictimes))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM,nRet);
		return;
	}
	cnt = rand()%20+1;
	
	for(i=0;i<cnt;i++)
	{
		memset(&psAtrbuf, 0, sizeof(psAtrbuf));
		nAtrlen = 0;
		cls_show_msg("请插入IC卡后按,任意键继续,现在%d次,总共%d次", i+1, cnt);
		if((nRet=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &nAtrlen))!=NDK_OK)
		{
			continue;
		}
		if((nRet=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
		{
			continue;
		}		
		//cls_show_msg("请拔掉IC卡,任意键继续");4.1版本固件:以上电次数作为插卡次数计数，所以测试过程中不拔卡 20140623 linwl
		succ++;
	}
	if((nRet = NDK_SysGetStatisticsData(SS_ICCARD_BASE_ID,&ictimes2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	if((ictimes2-ictimes) !=succ)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, ictimes2, ictimes);
		return;
	}

	cls_show_msg("统计类IC卡测试通过,任意键继续");
	return;
}

static void Mag_stat(void)
{
	int nRet = 0, pnErrorCode = 0, i = 0, cnt = 0, succ = 0;
	ulong magtimes = 0, magtimes2 = 0;
	uchar pcSwiped = 0;
	char pszTk1[128] = {0}, pszTk2[128] = {0}, pszTk3[128] = {0};

	//case1:统计刷卡次数	 
	if((nRet = NDK_SysGetStatisticsData(SS_MAG_TIMES_ID,&magtimes))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM,nRet);
		return;
	}

	if((nRet=NDK_MagOpen())!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	
	cnt = rand()%20+1;
	
	for(i=0;i<cnt;i++)
	{
		pcSwiped = 0;
		pnErrorCode = 0;
		memset(pszTk1,0,sizeof(pszTk1));
		memset(pszTk2,0,sizeof(pszTk2));
		memset(pszTk3,0,sizeof(pszTk3));
		NDK_MagReset();//根据叶明统提供的流程在读卡之后要复位磁头且清除磁卡缓冲区数据 20140304 linwl
		cls_show_msg("请刷卡后按任意键继续,第%d次,总共%d次",i+1,cnt);
		
		if((nRet=NDK_MagSwiped(&pcSwiped))!=NDK_OK||pcSwiped!=0x01)
		{
			continue;
		}		
		
		if((nRet=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_OK)
		{
			continue;
		}
		succ++;
	}
	NDK_MagClose();
	
	if((nRet = NDK_SysGetStatisticsData(SS_MAG_TIMES_ID,&magtimes2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		NDK_MagClose();
		return;
	}
	if((magtimes2-magtimes) != succ)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, magtimes2, magtimes);
		return;
	}
	cls_show_msg("统计类磁条卡测试通过,任意键继续");
	return;
}

static void Card_stat(void)
{
	int nKeyin=0;
	
	while(1)
	{
		nKeyin = cls_show_msg("统计卡类设备\n"
								"1.磁条卡 2.IC卡\n"
								"3.射频卡\n");
		switch(nKeyin)
		{
			case '1':
				Mag_stat();
				break;
			case '2':
				Ic_stat();
				break; 
			case '3':
				Rfid_stat();
				break;
			case ESC:
				return;
			default:
				break;
		}
	}
}

static void Wireless_stat(void)
{
	int nRet = 0,cnt = 0,i = 0,succ=0,nLen = 0;
	ulong ppptime=0, ppptime2=0, wlstimes=0, wlstimes2=0, failtimes=0, failtimes2=0;
	char CntStr[3]=STRCNT;
	
	if(getlinktype()!=GPRS && getlinktype()!=CDMA && getlinktype()!=TD)
	{
		cls_show_msg("配置错误");
		return;
	}

	cls_printf("统计类无线测试,默认压力测试次数:");
	if ((nRet=lib_kbgetinput(CntStr,0, sizeof(CntStr)-1,(uint *)&nLen, 2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||nRet==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = MAXCNT;
	else
		cnt = atoi(CntStr);
	
	//case1:统计无线连接时长 
	if((nRet = NDK_SysGetStatisticsData(SS_WLS_PPPTIME_ID,&ppptime))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	
	//case2:统计无线连接次数
	 
	if((nRet = NDK_SysGetStatisticsData(SS_WLS_TIMES_ID,&wlstimes))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}

	//case3:统计无线连接失败次数	
	if((nRet = NDK_SysGetStatisticsData(SS_WLS_FAILTIMES_ID,&failtimes))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}

	for(i=0;i<cnt;i++)
	{
		cls_printf("统计无线连接中,第%d次,总共%d次",i+1,cnt);
		if(NetUp()!=SUCC)
		{
			continue;
		}
		sleep(10);
		NetDown();
		succ++;
	} 

	//连接时长检测
	if((nRet = NDK_SysGetStatisticsData(SS_WLS_PPPTIME_ID,&ppptime2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	if(ppptime >= ppptime2)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, ppptime2, ppptime);
		return;
	}

	//连接次数检测
	if((nRet = NDK_SysGetStatisticsData(SS_WLS_TIMES_ID,&wlstimes2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM,nRet);
		return;
	}
	if(wlstimes2-wlstimes != cnt)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, wlstimes2, wlstimes);
		return;
	}
 	
	//连接失败次数检测
	if(succ != cnt)
	{
		if((nRet = NDK_SysGetStatisticsData(SS_WLS_FAILTIMES_ID,&failtimes2))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
			return;
		}
		if((failtimes2-failtimes) != (cnt - succ))
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, failtimes2, failtimes);
			return;
		}
	}
	if((cls_show_msg("是否进行无线连接失败次数统计\n[确认]是 [其他]否"))==ENTER)
	{
		cls_show_msg("进入配置设置错误的服务器地址后再运行此案例,任意键继续");
		return;
	}
	cls_show_msg("统计类无线测试成功,任意键继续");
	return;
}

#if (SDLC_ENABLE||ASYN_ENABLE)
static void Modem_stat(void)
{
	int nRet = 0,cnt = 0,i = 0,succ=0,nLen = 0, ret = -1;
	ulong modemtimes=0, modemtimes2=0, connectime=0, connectime2=0, failtimes=0, failtimes2=0;
	char CntStr[3]=STRCNT;

	if(getlinktype()!=ASYN&&getlinktype()!=SYNC)
	{
		cls_show_msg("配置错误");
		return;
	}

	cls_printf("统计类modem测试,默认压力测试次数:");
	if ((nRet=lib_kbgetinput(CntStr,0, sizeof(CntStr)-1,(uint *)&nLen, 2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||nRet==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = MAXCNT;
	else
		cnt = atoi(CntStr);
	
	//case1:统计modem连接次数
	if((nRet = NDK_SysGetStatisticsData(SS_MODEM_TIMES_ID,&modemtimes))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	
	if(getlinktype()==SYNC)
	{
		//case2:统计modem同步时长 
		if((nRet = NDK_SysGetStatisticsData(SS_MODEM_SDLCTIME_ID,&connectime))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
			return;
		}
	}
	else
	{
		//case3:统计modem异步时长
		if((nRet = NDK_SysGetStatisticsData(SS_MODEM_ASYNTIME_ID,&connectime))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
			return;
		}
	}

	//case4:统计modem连接失败次数
	if((nRet = NDK_SysGetStatisticsData(SS_MODEM_FAILTIMES_ID,&failtimes))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	
	for(i=0;i<cnt;i++)
	{	
		cls_printf("modem连接断开中,第%d次,总共%d次",i+1,cnt);
		if((ret=mdm_init())!=SUCC)
		{
			cls_show_msg1(3, "line %d:初始化MODEM(第%d次)失败(%d)", __LINE__, i+1, ret);
			continue;
		}
		if(mdm_dial(MDMDialStr)!=SUCC)
		{
			cls_show_msg1(3, "line %d:MODEM拨号(第%d次)失败", __LINE__, i+1);
			continue;
		}
		if(mdm_hangup()!=SUCC)
		{
			cls_show_msg1(3, "line %d:MODEM挂断(第%d次)失败", __LINE__, i+1);
			//continue;
		}
		sleep(3);
		succ++;
	}

	//modem连接次数检测
	if((nRet = NDK_SysGetStatisticsData(SS_MODEM_TIMES_ID,&modemtimes2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	if(modemtimes2-modemtimes != cnt)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, modemtimes2, modemtimes);
		return;
	}

	//modem连接时长检测
	if(getlinktype()==SYNC)
	{
		if((nRet = NDK_SysGetStatisticsData(SS_MODEM_SDLCTIME_ID,&connectime2))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
			return;
		}
		if(connectime >= connectime2)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, connectime2, connectime);
			return;
		}
	}
	else
	{	 
		if((nRet = NDK_SysGetStatisticsData(SS_MODEM_ASYNTIME_ID,&connectime2))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
			return;
		}
		if(connectime >= connectime2)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, connectime2, connectime);
			return;
		}
	}

	//modem连接失败次数
	if(cnt != succ)
	{
		if((nRet = NDK_SysGetStatisticsData(SS_MODEM_FAILTIMES_ID,&failtimes2))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM,nRet);
			return;
		}
		if((failtimes2-failtimes) != (cnt-succ))
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, failtimes2, failtimes);
			return;
		}
	}
	else
	{	
		mdm_init();
		mdm_dial("5840");	
		mdm_hangup();
		
		modemtimes2 = 0;
		if((nRet = NDK_SysGetStatisticsData(SS_MODEM_TIMES_ID,&modemtimes2))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM,nRet);
			return;
		}
		if(modemtimes2-modemtimes != (cnt+1))
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, modemtimes2, modemtimes);
			return;
		}
		
		if((nRet = NDK_SysGetStatisticsData(SS_MODEM_FAILTIMES_ID,&failtimes2))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM,nRet);
			return;
		}
		if((failtimes2-failtimes) != 1)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, failtimes2, failtimes);
			return;
		}
	}
	cls_show_msg("统计类modem测试成功,任意键继续");
	return;
}
#else
static void Modem_stat(void){cls_show_msg("本产品不支持有线mdm,任意键退出");return;}
#endif

static void Wifi_stat(void)
{
	int nRet = 0,nLen = 0,cnt = 0,i = 0;
	ulong wifitimes=0, wifitimes2=0, connectime=0, connectime2=0;
	ST_WIFI_PARAM param;
	time_t diff = 0, oldtime = 0;
	EM_WPA_CONSTATE state;
	char CntStr[3]=STRCNT;
	float con_time = 0.0, total_time = 0.0;

	if(getlinktype()!=WLAN)
	{
		cls_show_msg("配置错误");
		return;
	}
	
	cls_printf("统计类wifi测试,默认压力测试次数:");
	if ((nRet=lib_kbgetinput(CntStr,0, sizeof(CntStr)-1,(uint *)&nLen, 2, 0,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||nRet==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = MAXCNT;
	else
		cnt = atoi(CntStr);
	
	//case1:统计wifi连接次数
	if((nRet = NDK_SysGetStatisticsData(SS_WIFI_TIMES_ID,&wifitimes))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	
	//case2:统计wifi连接时长
	if((nRet = NDK_SysGetStatisticsData(SS_WIFI_TIME_ID,&connectime))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}

	if(cls_show_msg("请确保打开所有AP...ESC退出本模块测试")==ESC)
		return;

	memset(&param,0,sizeof(param));
	param.pszKey =  linksetting.WLANPwd;
	param.emKeyType = linksetting.WLANKeyType;
	param.emSecMode = linksetting.WLANEncMode;
	param.psEthIp = netsetting.LocalIP.Str;
	param.psEthGateway = netsetting.LocalGateway.Str;
	param.psEthNetmask =  netsetting.LocalMask.Str;
	param.ucIfDHCP = netsetting.DHCPenable;
	param.psEthDnsPrimary = netsetting.DNSHost[0];
	param.psEthDnsSecondary = netsetting.DNSHost[1];

	for(i = 0;i < cnt;i++)
	{
		cls_printf("Wifi统计测试中,第%d次,总共%d次",i+1,cnt);
		NDK_WiFiShutdown();
		if(NDK_WiFiInit()!=NDK_OK)
		{
			continue;
		}	

		if ((nRet=NDK_WiFiConnect(WlanEssid, &param)) != NDK_OK)
		{
			continue;
		}

		oldtime = time(NULL);
		state = -1;
		while(1)
		{
			if ((nRet=NDK_WiFiConnectState(&state)) != NDK_OK||(diff=time(NULL)-oldtime)>MAXTIMEOUT)
			{
	    		cls_show_msg1(10, "line %d:连接%s失败!(%d)", __LINE__, WlanEssid, nRet);
	    		NDK_WiFiShutdown();
		    	break ;
	    	}
			show_stopwatch(ENABLE, diff);
			
			if (state == WIFI_WPA_CONSTATE_CONTEXT)
			{
				lib_StartStopwatch();
				break;
			}
			else
				NDK_SysMsDelay(10);//稳定一段时间再看看
		}
		show_stopwatch(DISABLE, diff);
		sleep(3);
		NDK_WiFiShutdown();
		con_time=lib_ReadStopwatch();
		total_time=total_time+con_time;
	}

	//连接次数检测
	if((nRet = NDK_SysGetStatisticsData(SS_WIFI_TIMES_ID,&wifitimes2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	if(wifitimes2-wifitimes != cnt)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, wifitimes2, wifitimes);
		return;
	}
	
	//连接时长检测
	if((nRet = NDK_SysGetStatisticsData(SS_WIFI_TIME_ID,&connectime2))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	if(((connectime2 - connectime)/1000- total_time)>interval_time)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%ld,%ld)", __LINE__, TESTITEM, connectime2, connectime);
		return;
	}
	cls_show_msg("统计类wifi测试成功,任意键继续");
	return;
}

void Commconf(void)
{
	int nKeyin=0;
	
	while(1)
	{
		nKeyin = cls_show_msg("通讯配置\n"
								"1.无线配置\n"
								"2.modem配置\n"
								"3.Wifi配置\n");
		switch(nKeyin)
		{
			case '1':
				conf_conn_WLM(TRUE);
				break;
			case '2':
				conf_conn_MDM();
				break;
			case '3':
				conf_conn_WLAN();
				break;
			case ESC:
				return;
			default:
				break;
		}
	}
}

static void Comm_stat(void)
{
	int nKeyin=0;
	
	while(1)
	{
		nKeyin = cls_show_msg("统计通讯类\n"
								"1.无线  2.modem\n"
								"3.Wifi  4.配置\n");
		switch(nKeyin)
		{
			case '1':
				Wireless_stat();
				break;
			case '2':
				Modem_stat();
				break;
			case '3':
				Wifi_stat();
				break;
			case '4':
				Commconf();
				break;
			case ESC:
				return;
			default:
				break;
		}
	}
}

static void Record_Read(int *Record)
{
	int fp;
	char szBuf[20] = {0};
	int nRet = 0;
	int nNum=0;
	
	if((fp = NDK_FsOpen(TEMPFILE_NUM,"r"))<0)
	{
		NDK_FsClose(fp);	
		return;
	}
	nRet = NDK_FsSeek(fp, 0, SEEK_SET);
	if(nRet != NDK_OK)
	{
		NDK_FsClose(fp);	
		return ;
	}
	nRet = NDK_FsRead(fp, szBuf,sizeof(szBuf));
	if(nRet != sizeof(szBuf))
	{
		NDK_FsClose(fp);	
		return ;
	}
	NDK_C4ToInt((unsigned int * )&nNum, (unsigned char * )szBuf);
	*Record=nNum;
	NDK_FsClose(fp);	
	return;
}

static void Record_Write(int Record)
{
	int fp;
	char szBuf[20] = {0};
	int nRet = 0;
	
	if((fp = NDK_FsOpen(TEMPFILE_NUM,"w"))<0)
	{
		NDK_FsClose(fp);	
		return ;
	}
	nRet = NDK_FsSeek(fp, 0, SEEK_SET);
	if(nRet != NDK_OK)
	{
		NDK_FsClose(fp);	
		return ;
	}
	NDK_IntToC4((unsigned char * )szBuf, Record);
	
	nRet = NDK_FsWrite(fp, szBuf,sizeof(szBuf));
	if(nRet != sizeof(szBuf))
	{
		NDK_FsClose(fp);	
		return ;
	}
	NDK_FsClose(fp);	
	return ;
}

static void OnDownNum(void)
{
	int nRet = 0,nNum=-1;
	ulong pstOutPut ;
	
	//case1:统计开关机次数:重启后再次进入此案例不算首次进入
	if((cls_show_msg("首次进入此案例\n[确认]是 [其他]否"))==ENTER)
	{
		cls_show_msg("重启机器,再进入此案例,同时要选择否,任意键继续");
		NDK_FsDel(TEMPFILE_NUM);
	}
	nRet = NDK_FsExist(TEMPFILE_NUM);
	if(nRet == NDK_OK)
	{
		Record_Read(&nNum);
	}
	
	if((nRet = NDK_SysGetStatisticsData(SS_POWER_TIMES_ID,&pstOutPut))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
		return;
	}
	
	Record_Write(pstOutPut);
	if(nNum!=-1)
	{
		if(pstOutPut-nNum!=1)
		{
			cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d,%ld)", __LINE__, TESTITEM, nNum, pstOutPut);
			return;
		}
		cls_show_msg("开关次数测试通过,任意键继续");
	}
	
}

static void OnTime(void)
{
	int nRet = 0;
	ulong pstOutPut ;

	//case1:统计开关机时间
	 
	if((nRet = NDK_SysGetStatisticsData(SS_POWERUP_TIME_ID,&pstOutPut))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM,nRet);
		return;
	}
	if(cls_show_msg("是否首次进入此案例,[确认]是,并记录POS设备信息统计值%ld秒,再次进入此案例,[其他]否",pstOutPut)==ENTER)
		return;
	if(cls_show_msg("POS设备信息统计值%ld秒,是否大于首次记录的,[确认]是,[其他]否",pstOutPut) != ENTER)
	{
		cls_show_msg1(g_keeptime,"line %d:%s测试失败", __LINE__, TESTITEM);
		return;
	}
}

static void Pos_stat(void)
{
	int nKeyin=0;

	while(1)
	{
		nKeyin = cls_show_msg("1.开关机次数\n"
							  "2.重启机子\n"
							  "3.开机时间");
		switch(nKeyin)
		{
			case '1':
				OnDownNum();
				break;
			case '2':
				NDK_SysReboot();
				break;
			case '3':
				OnTime();
				break;
			case ESC:
				return;
			default:
				break;
		}
	}
}

//断电异常，断电重启后执行其他测试应该正常
static void powerdown_stat(void)
{
	int nRet = 0;
	
	cls_printf("请拔电源,重启后进行其他正常测试应通过");
	while(1) //死循环
	{
		if((nRet = NDK_SysInitStatisticsData())!=NDK_OK)
		{
			cls_show_msg("line %d:%s测试失败(%d)", __LINE__, TESTITEM, nRet);
			return;
		}
	}	
	return;
}

//重启后
static void reboot_stat(void)
{
	int nRet = 0 , i = 0, nKey = 0;
	ulong stOutPut = 0;

	nKey = cls_show_msg("请按键选择需要统计的键");
	for(i = 0; i<SZ_ARRAY(devkey) ;i++)
	{
		if(nKey == devkey[i])
			break;
	}

	if((nRet = NDK_SysGetStatisticsData(devid[i],&stOutPut))!=NDK_OK)
	{
		cls_show_msg("line %d:%s测试失败(%s,%d)", __LINE__, TESTITEM,devstr[i],nRet);
		return;
	}

	cls_show_msg("统计%s的按键次数为%d次,记录按键名及次数,任意键继续",devstr[i],stOutPut);

	cls_show_msg("请关机或重启机器后,再取%s的按键次数应大于现在的次数",devstr[i]);
	return;
}

static void Abnormal_stat(void)
{
	int nKeyin=0;

	while(1)
	{	
		nKeyin = cls_show_msg("1.重启异常\n"
							  "2.断电异常\n");
		switch(nKeyin)
		{
			case '1':
				reboot_stat();
				break;
			case '2':
				powerdown_stat();
				break;
			case ESC:
				return;
			default:
				break;
		}
	}
}

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		   chensj	          20130722	 	created
*****************************************************************/
void systest39(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	//以下为局部变量
	int nKeyin=0;

	//以下为函数体
	cls_printf("测试%s...", TESTITEM); 

	while(1) 
	{
		nKeyin = cls_show_msg("统计类函数测试\n"
								"1.键值    2.打印\n"
								"3.卡类    4.通讯\n"
								"5.开关机  6.异常\n");
		switch(nKeyin)
		{
			case '1':
				KeyValue_stat();
				break;
			case '2':
				Prnt_stat();
				break;
			case '3':
				Card_stat();
				break;
			case '4':
				Comm_stat();
				break;
			case '5':
				Pos_stat();
				break;
			case '6':
				Abnormal_stat();
				break;
			case ESC:
				return;
			default:
				break;
		}
	}
	return;
}

