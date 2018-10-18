/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 综合模块
* file name		: 28.c
* Author 			: 
* version			: V1.0
* DATE			: 20110620
* directory 		: .\SRC\general\
* description		: NL829测试
* related document	: NL829程序员使用手册.doc等相关文档
*				  
************************************************************************
* log			: 
* Revision 1.0  20110620 10:10 jennywone163
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"NL829"

#define	MAXWAITTIME 30
//#define	DATA16		"0123456789ABCDEF"

#define	DEFAULT_CNT_STR	"15000"	//"3000"//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(1000)

/*------------global variables definition-----------------------*/
static uchar gen28_bak[/*MAXLEN_DATA*/NL829_LEN_BLKDATA+8] = DATA16;
static int g_NL829type = DEVICE_NL829LCD;

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern ushort PP60_cmd_L(uchar);//编译时需要同时编译systest35，否则该函数无法找到原型编译会出错
extern ushort PP60_cmd_N(uchar);
extern ushort PP60_cmd_C(void);
extern ushort PP60_cmd_D(uchar, uchar, int);
/*---------------functions definition---------------------------*/
static int select_port(void)
{
	/*private & local definition*/
	int nKeyin = 0, port = 0;

	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg("请选择连接外设的串口:\n①串口1\n②串口2\n③USB串口\n");
		switch (nKeyin)
		{
		case '1':
			port = PORT_NUM_COM1;
			break;
		case '2':
			port = PORT_NUM_COM2;
			break;
		case '3':
			port = PORT_NUM_USB_HOST;
			break;
		default:
			continue;
			break;
		}
		return port;
	}
}

static int conf_test_bps(void)
{
	/*private & local definition*/
	int nKeyin = 0, loop = 0, ret = -1;
	char *bps[] = {"9600", "19200", "38400", "57600", "115200"};
	static int first = TRUE;//首次开机上电的标志
	
	/*process body*/
	// 1.配置与NL829连接的POS串口参数
	cls_show_msg1(2, "将选择外接设备连接的物理口");
	g_devicecom = select_port();

	if(g_devicecom!=PORT_NUM_USB_HOST)
	{
		// 2.配置NL829串口参数
		cls_show_msg1(2, "将进行外接设备通讯波特率bps的配置");
		do
		{
			nKeyin = cls_show_msg("1.9600   2.19200\n3.38400  4.57600\n5.115200");
#define MAXMENUITEM ('5')

			switch(nKeyin)
			{
			case ESC:
				cls_show_msg1(2, "将设置为BPS57600(默认)...");
				nKeyin = '4';//转义
			case '1':
			case '2':
			case '3':
			case '4':	
			case MAXMENUITEM:
				if(NDK_PortOpen(g_devicecom, (first==TRUE)?"57600":g_devicecombps)!=NDK_OK)
				{
					cls_show_msg1(2, "line %d:初始化POS串口失败", __LINE__);
					return FAIL;
				}
				cls_printf("设置外设BPS中...");
				if(NL829_SetBPS(nKeyin-'1')!=NL829_SUCC)
				{
					cls_printf("外设BPS自动适配中...请耐心等待");
					//若不成功,对所有可能的BPS进行尝试
					for(; loop<SZ_ARRAY(bps); loop++)
					{
						NDK_PortOpen(g_devicecom, bps[loop]);
						if(NL829_SetBPS(nKeyin-'1')==NL829_SUCC)
							break;
					}
					if(loop==SZ_ARRAY(bps))
					{
						cls_show_msg1(2, "line %d:设置NL829的BPS失败,请重启后再尝试", __LINE__);
						return FAIL;
					}
				}
				break;
			default:
				cls_show_msg1(1, "若要退出,请按ESC...");
				continue;
				break;			
			}
		}while(nKeyin<'1'||nKeyin>MAXMENUITEM);
#undef MAXMENUITEM	
		
		// 3.根据NL829当前BPS调整POS串口的BPS
		if((ret=NDK_PortOpen(g_devicecom, bps[nKeyin-'1']))!=NDK_OK)//if(initaux(g_devicecom, bps[nKeyin-'1'], DB8|STOP1|NP)!=SUCC)
		{
			cls_show_msg("line %d:初始化POS串口失败ret=%d", __LINE__, ret);
			return FAIL;
		}
		strcpy(g_devicecombps, bps[nKeyin-'1']);//g_devicecombps = bps[nKeyin-'1'];//g_bps根据配置进行调整备份
		first = FALSE;
	}
	else 
	{
		if((ret=NDK_PortOpen(g_devicecom, "115200,8,N,1,20"))!=NDK_OK)//if(initaux(g_devicecom, bps[nKeyin-'1'], DB8|STOP1|NP)!=SUCC)
		{
			cls_show_msg("line %d:初始化POS串口失败ret=%d", __LINE__, ret);
			return FAIL;
		}
	}
	
	return SUCC;
}

static int select_NL829(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	int type[] = {DEVICE_NL829LCD, DEVICE_NL829MR_PP60RF, DEVICE_YH100};

	/*process body*/
	while (1)
	{
		nKeyin = cls_show_msg("_选择PP60类型_\n"
							"1.旧的带LCD型\n"
							"2.829MR/PP60RF\n"//不带LCD的旧机型也选2
							"3.YH100\n");
		switch(nKeyin)
		{
		case '1':
		case '2':
		case '3':
			return type[nKeyin-'1'];
			break;
		case ESC:
			cls_show_msg1(2, "将设置为829MR或PP60RF(默认)...");
			return DEVICE_NL829MR_PP60RF;
			break;
		default:
			continue;
			break;
		}
	}
}

static uchar NL829_select_ICSAM(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	uchar ICSAM[] = {NL829_CARDNO_SAM1, NL829_CARDNO_SAM2, NL829_CARDNO_SAM3, NL829_CARDNO_SAM4, NL829_CARDNO_IC1};//改为829自定义的,不要依赖于POS对SAM的定义

	/*process body*/
	while (1)
	{	
		nKeyin = cls_show_msg("_选择卡类型_\n"
							"1.SAM1 2.SAM2\n"
							"3.SAM3 4.SAM4\n"
							"5.IC1\n");
		switch(nKeyin)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':	
			return ICSAM[nKeyin-'1'];
			break;
		case ESC:
			cls_show_msg1(2, "将使用默认的SAM1...");
			return NL829_CARDNO_SAM1;
			break;
		default:
			continue;
			break;
		}
	}	
}

static void little2big_uint(uint src, uint *des)
{
	/*private & local definition*/
	uchar tmp[8] = {0}, *addr = (uchar *)&src;
	int loop = sizeof(uint)-1;
	
	/*process body*/
	for(; loop>=0; loop--)
		tmp[loop] = *addr++;
	memcpy((void *)des, tmp, sizeof(uint));
}

//ver输出bios版本字符串
static ushort NL829_GetBiosVer(uchar *ver)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_GETBIOSVER, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_GETBIOSVER_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		if(appendlen==0)
			return NL829_ERR_CMD_GETBIOSVER;
		memcpy(ver, append, appendlen);
		ver[appendlen] = 0x00;
	}
	return staval;
}

static ushort NL829_beep(ushort msec, uchar cnt)
{
	/*private & local definition*/
	ushort ret = 0, msec1 = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_BEEP, NL829_LEN_CMD);
	little2big_ushort(msec, &msec1);
	memcpy(data+NL829_OFFSET_ARGS, &msec1, sizeof(msec1));
	data[NL829_OFFSET_ARGS+sizeof(msec1)] = cnt;
#if 1	//阻塞作法
	if((ret=NL829_frame_factory(data, NL829_BEEP_DATALEN_SND, status, append, &appendlen, msec*cnt/1000))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_BEEP_APPLEN_RCV)
		return NL829_ERR_CMD_BEEP;
	return NL829_GETSTAVAL(status);
#else	//这种方法可提高效率(不长时间阻塞)
	//注意:  该状态码并不立即返回,在蜂鸣器鸣叫结束后返回,应用也可以不理会返回值,在发送下一条命令前清接收缓冲即可。
	//故不必理会NL829_frame_factory的返回值,人工判断实际效果即可,本函数永远返回成功
	//但若有返回,则状态域及附加域必须与预期一致
	if((ret=NL829_frame_factory(data, NL829_BEEP_DATALEN_SND, status, append, &appendlen, 0))==NL829_SUCC)//说明正常收发
	{
		if(appendlen!=NL829_BEEP_APPLEN_RCV)
			return NL829_ERR_CMD_BEEP;
		if(NL829_GETSTAVAL(status)==NL829_SUCC)
			return NL829_SUCC;
	}
	else if(ret==NL829_ERR_FRAME_SND||ret==NL829_ERR_FRAME_UNPACK)
		return ret;
	else if(ret==NL829_ERR_FRAME_STX)//不理会未接收到应答的错误
		return NL829_SUCC;
	else
		return ret;//其它的接收错误
#endif
}

static ushort NL829_LEDCtl(uchar ctl)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_LEDCTL, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = ctl;
	if((ret=NL829_frame_factory(data, NL829_LEDCTL_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_LEDCTL_APPLEN_RCV)
		return NL829_ERR_CMD_LEDCTL;
	return NL829_GETSTAVAL(status);
}

//新增加LED非常阻塞闪烁控制\x31\x16   linwl 20120220
static ushort NL829_LEDCtlT(uchar ctl)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_LEDCTLT, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = ctl;
	if((ret=NL829_frame_factory(data, NL829_LEDCTLT_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_LEDCTLT_APPLEN_RCV)
		return NL829_ERR_CMD_LEDCTLT;
	return NL829_GETSTAVAL(status);
}

//purchase与balance正常应为BCD格式
static ushort NL829_LCDCtl(uchar flag, uint purchase, uint balance)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	uint purchase1 = 0, balance1 = 0;
	
	/*process body*/
	memcpy(data, NL829_LCDCTL, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = flag;
	little2big_uint(purchase, &purchase1);
	little2big_uint(balance, &balance1);
	memcpy(data+NL829_OFFSET_ARGS+1, &purchase1, sizeof(purchase1));
	memcpy(data+NL829_OFFSET_ARGS+1+sizeof(purchase1), &balance1, sizeof(balance1));
	if((ret=NL829_frame_factory(data, NL829_LCDCTL_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_LCDCTL_APPLEN_RCV)
		return NL829_ERR_CMD_LCDCTL;
	return NL829_GETSTAVAL(status);
}

//注意: 该命令并不是立即显示在LCD上，而是在其它显示命令中改变标题栏的内容。
static ushort NL829_LCDCtl_new(uchar mode, uchar *title)
{
	/*private & local definition*/
	ushort ret = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	int appendlen = 0, datalen = NL829_LEN_CMD+1+sizeof(mode)+strlen((char *)title);
	
	/*process body*/
	if(datalen>NL829_MAXLEN_DATA)
		return NL829_ERR_FRAME_DATALEN2;
	memcpy(data, NL829_LCDCTL, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = 0x07;//协议规定死的
	data[NL829_OFFSET_ARGS+1] = mode;
	memcpy(data+NL829_OFFSET_ARGS+2, title, strlen((char *)title));
	if((ret=NL829_frame_factory(data, datalen, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_LCDCTL_APPLEN_RCV)
		return NL829_ERR_CMD_LCDCTL;
	return NL829_GETSTAVAL(status);
}

//机号(12字节)或商户号(16字节)字符串
static ushort NL829_SetHWinfo(uchar flag, uchar *info)
{
	/*private & local definition*/
	ushort ret = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	int appendlen = 0, datalen = NL829_OFFSET_ARGS+strlen(NL829_SETHWINFO_CHECK)+1+strlen((char *)info);
	
	/*process body*/
	memcpy(data, NL829_SETHWINFO, NL829_LEN_CMD);
	strcpy((char *)data+NL829_OFFSET_ARGS, (char *)NL829_SETHWINFO_CHECK);
	data[NL829_OFFSET_ARGS+strlen(NL829_SETHWINFO_CHECK)] = flag;
	strcpy((char *)data+NL829_OFFSET_ARGS+strlen(NL829_SETHWINFO_CHECK)+1, (char *)info);
	if((ret=NL829_frame_factory(data, datalen, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_SETHWINFO_APPLEN_RCV)
		return NL829_ERR_CMD_SETHWINFO;
	return NL829_GETSTAVAL(status);
}

static ushort NL829_GetHWinfo(uchar flag, uchar *info)
{
	/*private & local definition*/
	ushort ret = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	int appendlen = 0;
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_GETHWINFO, NL829_LEN_CMD);
	strcpy((char *)data+NL829_OFFSET_ARGS, NL829_GETHWINFO_CHECK);
	data[NL829_OFFSET_ARGS+strlen(NL829_GETHWINFO_CHECK)] = flag;
	if((ret=NL829_frame_factory(data, NL829_GETHWINFO_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	staval = NL829_GETSTAVAL(status);
	if(staval==NL829_SUCC)
	{
		if(appendlen==0)
			return NL829_ERR_CMD_GETHWINFO;
		else
		{
			memcpy(info, append, appendlen);
			info[appendlen] = 0x00;
		}
	}
	if((staval==NL829_ARGERR||staval==NL829_HWINFO_ERR)&&appendlen!=0)
		return NL829_ERR_CMD_GETHWINFO1;
	return staval;
}

//读取的机号(12字节)或商户号(16字节)存在info中(本函数会处理成字符串),上层接口可根据文档的应答格式进行判断处理
static ushort NL829LCD_HWinfo(uchar flag, uchar *info)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829LCD_HWINFO, NL829_LEN_CMD);
	strcpy((char *)data+NL829_OFFSET_ARGS, (char *)NL829LCD_HWINFO_CHECK);
	data[NL829_OFFSET_ARGS+strlen(NL829LCD_HWINFO_CHECK)] = flag;
	if((ret=NL829_frame_factory(data, NL829LCD_HWINFO_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	staval = NL829_GETSTAVAL(status);
	if(flag==NL829LCD_FLAG_SETSN&&appendlen!=0)
		return NL829LCD_ERR_CMD_HWINFO;
	if(flag==NL829LCD_FLAG_GETMACHINE||flag==NL829LCD_FLAG_GETMERCHANT)
	{
		if(staval==NL829_SUCC)
		{
			if(appendlen==0)
				return NL829LCD_ERR_CMD_HWINFO;
			else
			{
				memcpy(info, append, appendlen);
				info[appendlen] = 0x00;
				//return NL829_SUCC;
			}		
		}
		if(staval==0x0002&&appendlen!=0)
			return NL829LCD_ERR_CMD_HWINFO1;
	}
	return staval;
}

//rtc为输出的8字节BCD时间
static ushort NL829LCD_GetRTC(uchar *rtc)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829LCD_GETRTC, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829LCD_GETRTC_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		if(appendlen!=NL829LCD_GETRTC_APPLEN_RCV)
			return NL829LCD_ERR_CMD_GETRTC;
		memcpy(rtc, append, appendlen);
		//ver[appendlen] = 0x00;
	}
	return staval;
}

//rtc为输入的BCD时间,inlen正常值为8
static ushort NL829LCD_SetRTC(uchar *rtc, uchar inlen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829LCD_SETRTC, NL829_LEN_CMD);
	memcpy(data+NL829_LEN_CMD, rtc, inlen);
	if((ret=NL829_frame_factory(data, NL829_LEN_CMD+inlen, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829LCD_SETRTC_APPLEN_RCV)
		return NL829LCD_ERR_CMD_SETRTC;
	return NL829_GETSTAVAL(status);
}

//若成功,则返回一字节状态码存于sta中
static ushort NL829_ICSAM_GetSta(uchar *sta)
{
	/*private & local definition*/
	ushort ret = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	int appendlen = 0;
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_ICSAM_GETSTA, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_ICSAM_GETSTA_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	staval = NL829_GETSTAVAL(status);
	if(staval==NL829_SUCC)
	{
		if(appendlen!=NL829_ICSAM_GETSTA_APPLEN_RCV)
			return NL829_ERR_ICSAM_GETSTA;
		else
			*sta = append[0];
	}
	if(staval==NL829_ARGERR&&appendlen!=0)
		return NL829_ERR_ICSAM_GETSTA1;
	return staval;
}

//测试sam卡/psam卡的上电指令
static ushort NL829_ICSAM_poweron(uchar cardnum, uchar *cardtype, uchar *ATR, int *ATRlen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_ICSAM_POWERON, NL829_LEN_CMD);
	memcpy(data+NL829_OFFSET_ARGS, NL829_ICSAM_POWERON_RESERVED, NL829_ICSAM_POWERON_RESERVED_LEN);
	data[NL829_OFFSET_ARGS+NL829_ICSAM_POWERON_RESERVED_LEN] = cardnum;
	if((ret=NL829_frame_factory(data, NL829_ICSAM_POWERON_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	staval = NL829_GETSTAVAL(status);
	if(staval==NL829_SUCC)
	{
		if(appendlen==0)
			return NL829_ERR_ICSAM_POWERON;
		else
		{
			*cardtype = append[0];
			*ATRlen = appendlen-1;
			memcpy(ATR, append+1, *ATRlen);
			ATR[*ATRlen] = 0x00;
		}
	}
	if(staval==NL829_ARGERR&&appendlen!=0)
		return NL829_ERR_ICSAM_POWERON1;
	if(staval==NL829_SAM_FAIL_POWERON&&appendlen!=1)
		return NL829_ERR_ICSAM_POWERON2;
	return staval;
}

//测试sam卡/psam卡的下电指令
static ushort NL829_ICSAM_powerdown(uchar cardnum)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_ICSAM_POWERDOWN, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = cardnum;
	if((ret=NL829_frame_factory(data, NL829_ICSAM_POWERDOWN_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_ICSAM_POWERDOWN_DATALEN_RCV)
		return NL829_ERR_ICSAM_POWERDOWN;
	return NL829_GETSTAVAL(status);
}

//输出相应域的数据(非字串)
static ushort NL829_CPU_active(ushort sec, uchar *type, uchar *UID, int *UIDlen, uchar *ATR, int *ATRlen)
{
	/*private & local definition*/
	ushort ret = 0, sec1 = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_CPU_ACTIVE, NL829_LEN_CMD);
	little2big_ushort(sec, &sec1);
	memcpy(data+NL829_OFFSET_ARGS, &sec1, sizeof(sec1));
	if((ret=NL829_frame_factory(data, NL829_CPU_ACTIVE_DATALEN_SND, status, append, &appendlen, sec))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		if(appendlen!=0)
		{
			//先判断,无错了才更新输出参数
			if(append[1]+append[append[1]+2]+3!=appendlen)
				return NL829_ERR_CPU_ACTIVE;
			*type = append[0];
			*UIDlen = append[1];
			memcpy(UID, &append[2], *UIDlen);
			*ATRlen = append[append[1]+2];
			memcpy(ATR, &append[append[1]+3], *ATRlen);
			return NL829_SUCC;
		}
		else
			return NL829_ERR_CPU_ACTIVE1;
	}
	else
		return staval;
}

static ushort NL829_CPU_discnnt(ushort sec)
{
	/*private & local definition*/
	ushort ret = 0, sec1 = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_CPU_DISCNNT, NL829_LEN_CMD);
	little2big_ushort(sec, &sec1);
	memcpy(data+NL829_OFFSET_ARGS, &sec1, sizeof(sec1));
	if((ret=NL829_frame_factory(data, NL829_CPU_DISCNNT_DATALEN_SND, status, append, &appendlen, sec))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_CPU_DISCNNT_APPLEN_RCV)
		return NL829_ERR_CPU_DISCNNT;
	return NL829_GETSTAVAL(status);
}

static ushort NL829_APDUrw(uchar cardno, uchar *apdu, int apdulen, uchar *r_apdu, int *r_apdulen)
{
	/*private & local definition*/
	ushort ret = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	int appendlen = 0, datalen = NL829_LEN_CMD+1+apdulen;
	ushort staval = NL829_FAIL;
	
	/*process body*/
	*r_apdulen = 0;
	memcpy(data, NL829_APDURW, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = cardno;
	memcpy(data+NL829_OFFSET_ARGS+1, apdu, apdulen);
	if((ret=NL829_frame_factory(data, datalen, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		*r_apdulen = appendlen;
		if(appendlen!=0)
			memcpy(r_apdu, append, appendlen);	
	}
	else
	{
		if(appendlen!=0)
			return NL829_ERR_APDURW;
	}
	return staval;
}

//应用程序在对非接触卡（CPU卡或者Mifare-1卡）操作结束后，或者无需寻卡的时候，应该关闭射频天线。
static ushort NL829_AntennaCtl(ushort msec)
{
	/*private & local definition*/
	ushort ret = 0, msec1 = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_ANTENNACTL, NL829_LEN_CMD);
	little2big_ushort(msec, &msec1);
	memcpy(data+NL829_OFFSET_ARGS, &msec1, sizeof(msec1));
	if((ret=NL829_frame_factory(data, NL829_ANTENNACTL_DATALEN_SND, status, append, &appendlen, (msec==0xffff)?0:(msec/1000)))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_ANTENNACTL_APPLEN_RCV)
		return NL829_ERR_ANTENNACTL;
	return NL829_GETSTAVAL(status);
}

//输出相应域的数据(非字串)
static ushort NL829_M1_active(uchar mode, ushort sec, uchar *ATQ, int *ATQlen, uchar *SNR, int *SNRlen, uchar *SAK, int *SAKlen)
{
	/*private & local definition*/
	ushort ret = 0, sec1 = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_M1_ACTIVE, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = mode;
	little2big_ushort(sec, &sec1);
	memcpy(data+NL829_OFFSET_ARGS+1, &sec1, sizeof(sec1));
	if((ret=NL829_frame_factory(data, NL829_M1_ACTIVE_DATALEN_SND, status, append, &appendlen, sec))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		if(appendlen!=0)
		{
			//先判断,无错了才更新输出参数
			if(append[0]+append[append[0]+1]+append[append[0]+append[append[0]+1]+2]+3!=appendlen)
				return NL829_ERR_M1_ACTIVE;
			*ATQlen = append[0];
			memcpy(ATQ, &append[1], *ATQlen);
			*SNRlen = append[append[0]+1];
			memcpy(SNR, &append[append[0]+2], *SNRlen);
			*SAKlen = append[append[0]+append[append[0]+1]+2];
			memcpy(SAK, &append[append[0]+append[append[0]+1]+3], *SAKlen);		
			return NL829_SUCC;
		}
		else
			return NL829_ERR_M1_ACTIVE1;
	}
	else
		return staval;
}

//不关闭天线，使卡进入HALT状态。可以用WUPA寻卡命令重新唤醒。
static ushort NL829_M1_halt(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_M1_HALT, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_M1_HALT_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_M1_HALT_APPLEN_RCV)
		return NL829_ERR_M1_HALT;
	return NL829_GETSTAVAL(status);
}

static ushort NL829_M1_storekey(uchar mode, uchar sector, uchar *key)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_M1_STOREKEY, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = mode;
	data[NL829_OFFSET_ARGS+1] = sector;
	memcpy(data+NL829_OFFSET_ARGS+2, key, NL829_LEN_KEY);
	if((ret=NL829_frame_factory(data, NL829_M1_STOREKEY_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_M1_STOREKEY_APPLEN_RCV)
		return NL829_ERR_M1_STOREKEY;
	return NL829_GETSTAVAL(status);
}

static ushort NL829_M1_loadkey(uchar mode, uchar sector)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_M1_LOADKEY, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = mode;
	data[NL829_OFFSET_ARGS+1] = sector;
	if((ret=NL829_frame_factory(data, NL829_M1_LOADKEY_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_M1_LOADKEY_APPLEN_RCV)
		return NL829_ERR_M1_LOADKEY;
	return NL829_GETSTAVAL(status);
}

//SNR其实就是UID了,它在"防冲突"阶段返回
static ushort NL829_M1_auth(uchar mode, uchar *SNR, int SNRlen, uchar block)//20161019新增SNRlen参数,SNR可能包含0x00不能用strlen函数
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_M1_AUTH, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = mode;
	memcpy(data+NL829_OFFSET_ARGS+1, SNR, SNRlen/*strlen((char *)SNR)*//*NL829_LEN_SNR*/);//改后取到M卡的uid才会正确
	data[NL829_OFFSET_ARGS+1+SNRlen/*strlen((char *)SNR)*//*NL829_LEN_SNR*/] = block;
	if((ret=NL829_frame_factory(data, NL829_M1_AUTH_DATALEN_SND-NL829_LEN_SNR+SNRlen/*strlen((char *)SNR)*/, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_M1_AUTH_APPLEN_RCV)
		return NL829_ERR_M1_AUTH;
	return NL829_GETSTAVAL(status);
}

static ushort NL829_M1_auth_ex(uchar mode, uchar *SNR, int SNRlen, uchar block, uchar *key)//20161019新增SNRlen参数,SNR可能包含0x00不能用strlen函数
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_M1_AUTH_EX, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = mode;
	memcpy(data+NL829_OFFSET_ARGS+1, SNR, SNRlen/*strlen((char *)SNR)*//*NL829_LEN_SNR*/);//改后取到M卡的uid才会正确
	data[NL829_OFFSET_ARGS+1+SNRlen/*strlen((char *)SNR)*//*NL829_LEN_SNR*/] = block;
	memcpy(data+NL829_OFFSET_ARGS+SNRlen/*strlen((char *)SNR)*//*NL829_LEN_SNR*/+2, key, NL829_LEN_KEY);
	if((ret=NL829_frame_factory(data, NL829_M1_AUTH_EX_DATALEN_SND-NL829_LEN_SNR+SNRlen/*strlen((char *)SNR)*/, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_M1_AUTH_EX_APPLEN_RCV)
		return NL829_ERR_M1_AUTH_EX;
	return NL829_GETSTAVAL(status);
}

static ushort NL829_M1_BLKRead(uchar block, uchar *out)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_M1_BLKREAD, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = block;
	if((ret=NL829_frame_factory(data, NL829_M1_BLKREAD_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		if(appendlen!=NL829_M1_BLKREAD_APPLEN_RCV)
			return NL829_ERR_M1_BLKREAD;
		memcpy(out, append, appendlen);
	}
	else
	{
		if(appendlen!=0)
			return NL829_ERR_M1_BLKREAD1;
	}
	return staval;
}

static ushort NL829_M1_BLKWrite(uchar block, uchar *wdata)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_M1_BLKWRITE, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = block;
	memcpy(data+NL829_OFFSET_ARGS+1, wdata, NL829_LEN_BLKDATA);
	if((ret=NL829_frame_factory(data, NL829_M1_BLKWRITE_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_M1_BLKWRITE_APPLEN_RCV)
		return NL829_ERR_M1_BLKWRITE;
	return NL829_GETSTAVAL(status);
}

//wdata 4字节,是采用小数序的(从POS上移植而来,故也是小数序,而与NL829大多都采用大数序不同)
//故直接使用unit*,直接使用不进行转换
static ushort __NL829_M1_BLKIncDec(uchar cmd_L, uchar block, uint *wdata)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	data[0] = NL829_M1_BLKINCDEC_H;
	data[1] = cmd_L;
	data[2] = block;
	memcpy(data+NL829_OFFSET_ARGS+1, wdata, NL829_LEN_BLKINCDEC);
	if((ret=NL829_frame_factory(data, NL829_M1_BLKINCDEC_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_M1_BLKINCDEC_APPLEN_RCV)
		return NL829_ERR_M1_BLKINCDEC;
	return NL829_GETSTAVAL(status);
}

static ushort NL829_M1_BLKInc(uchar block, uint *wdata){return __NL829_M1_BLKIncDec(NL829_M1_BLKINC_L, block, wdata);}

static ushort NL829_M1_BLKDec(uchar block, uint *wdata){return __NL829_M1_BLKIncDec(NL829_M1_BLKDEC_L, block, wdata);}

//rats指令封装，发送给键盘并接受键盘响应，格式和前面的NL829_M1_BLKRead等一模一样 add by zhengjd 20141029
int NL829_RfidTypeARats(uchar cid,int *pnDatalen, uchar *psDatabuf)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_RFIDTYPEARATS, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS] = cid;
	if((ret=NL829_frame_factory(data, NL829_RFIDTYPEARATS_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		*pnDatalen = appendlen;
		if(appendlen!=0)
			memcpy(psDatabuf, append, appendlen);	
	}
	else
	{
		if(appendlen!=0)
			return NL829_ERR_APDURW;
	}
	return NL829_GETSTAVAL(status);

}

int NL829CpuActiveKeyInput(ushort sec, uchar *type, uchar *UID, int *UIDlen, uchar *ATR, int *ATRlen,uchar *keyin)
{
	/*private & local definition*/
	ushort ret = 0, sec1 = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_CPU_ACTIVE_KEY_INPUT, NL829_LEN_CMD);
	little2big_ushort(sec, &sec1);
	memcpy(data+NL829_OFFSET_ARGS, &sec1, sizeof(sec1));
	if((ret=NL829_frame_factory(data, NL829_CPU_ACTIVE_KEY_INPUT_DATALEN, status, append, &appendlen, sec))!=NL829_SUCC)
		return ret;
	staval=NL829_GETSTAVAL(status);
	if(staval==NL829_SUCC)
	{
		if(appendlen!=0)
		{
			//先判断,无错了才更新输出参数
			if(append[1]+append[append[1]+2]+3!=appendlen)
				return NL829_ERR_CPU_ACTIVE;
			*type = append[0];
			*UIDlen = append[1];
			memcpy(UID, &append[2], *UIDlen);
			*ATRlen = append[append[1]+2];
			memcpy(ATR, &append[append[1]+3], *ATRlen);
			return NL829_SUCC;
		}
		else
			return NL829_ERR_CPU_ACTIVE1;
	}
	else if(staval==NL829_CPU_ACTIVE_INPUT_SUCC)
	{
		memcpy(keyin,append,appendlen);
		return NL829_SUCC;
	}   
	else
		return staval;
}

//测试SP10扫描头开始扫描命令
static ushort NL829_SCAN_Startscan(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_SCAN_STARTSCAN, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_SCANSTARTSCAN_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_SCANSTARTSCAN_APPLEN_RCV)
		return NL829_ERR_SCAN_START;
	return NL829_GETSTAVAL(status);
}

//测试SP10扫描头获取扫描头条码数据指令
static ushort NL829_SCAN_GetData(uchar *out, int *outlen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_SCAN_GETDATA, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_SCANGETDATA_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		*outlen = appendlen;
		memcpy(out, append, appendlen);//将扫描到的数据上送
	}
	else
	{
		if(appendlen!=0)
			return NL829_ERR_SCAN_GETDATA;
	}

	return staval;
}

//测试SP10扫描头关闭扫描命令
static ushort NL829_SCAN_Stopscan(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_SCAN_STOPSCAN, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_SCANSTOPSCAN_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_SCANSTOPSCAN_APPLEN_RCV)
		return NL829_ERR_SCAN_STOP;
	return NL829_GETSTAVAL(status);
}

//测试SP10扫描头阻塞式扫描命令
static ushort NL829_SCAN_Doscan(int timeout, uchar *out, int *outlen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_SCAN_DOSCAN, NL829_LEN_CMD);
	data[NL829_LEN_CMD]=timeout;
	if((ret=NL829_frame_factory(data, NL829_SCANDOSCAN_DATALEN_SND, status, append, &appendlen, timeout))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		if(appendlen==0)
			return NL829_ERR_SCAN_DOSCAN;
		else
			*outlen = appendlen;
			memcpy(out, append, appendlen);//将扫描到的数据上送
	}

	return staval;
}
#if 0  //根据开发提供的NL829MR_PP60RF程序员使用手册v1.7  (删除扫描头寄存器写指令) 扫描头设置关闭
//测试SP10扫描头设置寄存器指令
static ushort NL829_SCAN_SetScan(uchar *setdata, int datalen, uchar *out, int *outlen)
{
	/*private & local definition*/
	ushort ret = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	int appendlen = 0;
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_SCAN_SETSCAN, NL829_LEN_CMD);
	memcpy(data+NL829_LEN_CMD, setdata, datalen);
	if((ret=NL829_frame_factory(data, NL829_SCANSETSCAN_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		if(appendlen!=NL829_SCANSETSCAN_APPLEN_RCV)
			return NL829_ERR_SCAN_SETSCAN;
		else
			*outlen = appendlen;
			memcpy(out, append, appendlen);//将扫描到的数据上送
	}
	
	return staval;
}
#endif
//新增SP10T显示图片
static ushort NL829_Show_Image(uchar *x,uchar *y,uchar *w,uchar *h,uchar *cnum,uchar *snum,uchar *sdata, int dlen)
{
	
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_SHOW_IMAGE, NL829_LEN_CMD);
	memcpy(data+NL829_OFFSET_ARGS,x,2);
	memcpy(data+NL829_OFFSET_ARGS+2,y,2);
	memcpy(data+NL829_OFFSET_ARGS+4,w,2);
	memcpy(data+NL829_OFFSET_ARGS+6,h,2);
	memcpy(data+NL829_OFFSET_ARGS+8,cnum,2);
	memcpy(data+NL829_OFFSET_ARGS+10,snum,2);
	memcpy(data+NL829_OFFSET_ARGS+12,sdata,dlen);
	if((ret=NL829_frame_factory(data, NL829_SHOWIMAGE_DATALEN_SND+dlen, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_SHOWIMAGE_APPLEN_RCV)
		return NL829_ERR_SHOW_IMAGE;
	return NL829_GETSTAVAL(status);
}

//新增按键获取相关的三个指令20170804 by jiangym
static ushort NL829_Start_Recv_Key(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_STR_REC_KEY, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_STR_REC_KEY_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_STR_REC_KEY_APPLEN_RCV)
		return NL829_ERR_STR_REC_KEY;
	return NL829_GETSTAVAL(status);
}
static ushort NL829_Get_Key_Value(uchar *outdata, int *datalen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	ushort staval = NL829_FAIL;
	
	/*process body*/
	memcpy(data, NL829_GET_KEY_VALUE, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_GET_KEY_VALUE_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;	
	if((staval=NL829_GETSTAVAL(status))==NL829_SUCC)
	{
		*datalen = appendlen;
		memcpy(outdata, append, appendlen);//将获取的键值上送
	}
	else
	{
		if(appendlen!=0)
			return NL829_ERR_GET_KEY_VALUE;
	}
	return staval;
}
static ushort NL829_End_Recv_Key(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_END_REC_KEY, NL829_LEN_CMD);
	if((ret=NL829_frame_factory(data, NL829_END_REC_KEY_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_END_REC_KEY_APPLEN_RCV)
		return NL829_ERR_END_REC_KEY;
	return NL829_GETSTAVAL(status);
}
static ushort NL829_Scan_Animation(uchar ctrl,uchar icon_num,uchar speed,uchar *d_time,uchar *d_icons,int n)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, NL829_SCAN_ANIMATION, NL829_LEN_CMD);
	data[NL829_OFFSET_ARGS]=ctrl;
	data[NL829_OFFSET_ARGS+1]=icon_num;
	data[NL829_OFFSET_ARGS+2]=speed;
	memcpy(data+NL829_OFFSET_ARGS+3,d_time,2);
	memcpy(data+NL829_OFFSET_ARGS+5,d_icons,n);
	if((ret=NL829_frame_factory(data, NL829_SCAN_ANIMATION_DATALEN_SND+n, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_SCAN_ANIMATION_APPLEN_RCV)
		return NL829_ERR_SCAN_ANIMATION;
	return NL829_GETSTAVAL(status);
}


#if 0	//实际中无用
static ushort __NL829_M1_BLKTransRest(uchar cmd_L, uchar block)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};
	
	/*process body*/
	data[0] = NL829_M1_BLKTRANSREST_H;
	data[1] = cmd_L;
	data[2] = block;
	if((ret=NL829_frame_factory(data, NL829_M1_BLKTRANSREST_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC)
		return ret;
	if(appendlen!=NL829_M1_BLKTRANSREST_APPLEN_RCV)
		return NL829_ERR_M1_BLKTRANSREST;
	return NL829_GETSTAVAL(status);
}

//特别说明:
//目前虽然指令文档还是给出了以下两条指令,但由于增减量操作已经包括了传送操作，不要再执行传送，否则有的卡会返回不可预知的错误。
//只是出于文档给出了,于是随手实现一下而已。不要使用(虽然大多情况下这两条指令应直接返回成功)。
static ushort NL829_M1_BLKTrans(uchar block){return __NL829_M1_BLKTransRest(NL829_M1_BLKTRANS_L, block);}

static ushort NL829_M1_BLKRest(uchar block){return __NL829_M1_BLKTransRest(NL829_M1_BLKREST_L, block);}
#endif

//L4 testcmd要从正常,异常(如设置错误的BPS)两个方面写,在正常使用的流程上进行扩充即可,这样子可以节省重复代码
static void test_control_cmd(void)
{
	/*private & local definition*/
	uchar buf[NL829_MAXLEN_DATA] = {0}, old_time[16] = {0}, new_time[16] = {0};
	ushort staval = NL829_FAIL;
	int loop = 0, test_rtc = FALSE;
	char *led[] = {"绿", "红", "黄", "蓝"};
	
	/*process body*/
	cls_show_msg1(2, "非卡类控制命令集测试...");//加提示以刷掉测试菜单
	//测试前置
	NL829_LEDCtl(0x0f);
	NL829_LEDCtlT(0x0f);
	if(g_NL829type==DEVICE_NL829LCD)
		NL829_LCDCtl(0x06, 0x00000000, 0x00000000);

	if(g_NL829type!=DEVICE_YH100)//YH100 使用USB串口不用设置波特率
	{
		//测试设置BPS(仅异常情况,正常情况通过"配置"改变合理的BPS进行其它指令的全面测试即可)
		if((staval=NL829_SetBPS(NL829_BPS115200+1))!=NL829_BPS_NOTSUPPORT)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return;
		}
		if((staval=NL829_SetBPS(0xff))!=NL829_BPS_NOTSUPPORT)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return;
		}
	}

	//测试取版本
	if((staval=NL829_GetBiosVer(buf))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	else
	{
		if(cls_show_msg("取到版本:%s,[ENTER]通过[其它]不通过", buf)!=ENTER)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			return;
		}
	}

	if(g_NL829type!=DEVICE_YH100)//不支持蜂鸣器和LED灯
	{
		//测试蜂鸣器
		if((staval=NL829_beep(0, 100))!=NL829_SUCC||cls_show_msg("NL829若发出beep声,按[ENTER];否则按[其它]")==ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return;
		}
		cls_show_msg1(2, "子用例测试通过!");

		if((staval=NL829_beep(10000, 0))!=NL829_SUCC||cls_show_msg("NL829若发出beep声,按[ENTER];否则按[其它]")==ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return;
		}
		cls_show_msg1(2, "子用例测试通过!");

		cls_show_msg("NL829将发出3声beep(每声持续2s),任意键继续...");
		if((staval=NL829_beep(2000, 3))!=NL829_SUCC||cls_show_msg("NL829若发出3声beep,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return;
		}
	
		//LED动画演示
		cls_show_msg1(2, "NL829 LED将逐一亮灭...");
		strcpy((char *)buf, "\x8f\x4f\x2f\x1f");//预设测试数据(测试前置)
		for(loop=0; loop<12; loop++)
		{
			if((staval=NL829_LEDCtl(buf[loop%4]))!=NL829_SUCC)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
			cls_printf("%s灯亮...", led[loop%4]);
			sleep(2);//msdelay(500);//时间长些,已让测试者有足够时间确认
		}
		NL829_LEDCtl(0x0f);
		cls_show_msg1(2, "NL829 LED将全亮全灭...");
		for(loop=0; loop<8; loop++)
		{
			if((staval=NL829_LEDCtl((loop%2)?0x0f:0xff))!=NL829_SUCC)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
			sleep(2);//msdelay(500);
		}
		//LED 闪烁测试 by linwl 20120220
		cls_show_msg1(2, "NL829 LED将逐一闪烁...");
		for(loop=0; loop<12; loop++)
		{
			if((staval=NL829_LEDCtlT(buf[loop%4]))!=NL829_SUCC)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
			cls_printf("%s灯闪烁...", led[loop%4]);
			sleep(5);//msdelay(500);//时间长些,已让测试者有足够时间确认
		}
		NL829_LEDCtlT(0x0f);
		cls_show_msg1(2, "NL829 LED将全闪全灭...");
		for(loop=0; loop<8; loop++)
		{
			if((staval=NL829_LEDCtlT((loop%2)?0x0f:0xff))!=NL829_SUCC)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
			sleep(5);//msdelay(500);
		}
		cls_show_msg1(2, "NL829 LED将全部闪烁...");
		NL829_LEDCtlT(0xff);
		NL829_LEDCtl(0x0f);
		NL829_LEDCtlT(0xff);
		sleep(10);
		if(cls_show_msg("NL829 LED继续闪烁,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			goto ERR;
		}
		NL829_LEDCtlT(0x0f);
		if(cls_show_msg("NL829 LED效果OK,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			goto ERR;
		}
	}
	else
	{
		//LED动画演示
		cls_show_msg1(2, "NL829 LED蓝灯将亮灭...");
		strcpy((char *)buf, "\x8f\x4f\x2f\x1f");//预设测试数据(测试前置)
		if((staval=NL829_LEDCtl(0x1f))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		cls_printf("蓝灯亮...");
		sleep(2);//msdelay(500);//时间长些,已让测试者有足够时间确认
		NL829_LEDCtl(0x0f);
		cls_show_msg1(2, "NL829 LED蓝灯将亮灭...");
		for(loop=0; loop<8; loop++)
		{
			if((staval=NL829_LEDCtl((loop%2)?0x0f:0xff))!=NL829_SUCC)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
			sleep(2);//msdelay(500);
		}
		//LED 闪烁测试 by linwl 20120220
		cls_show_msg1(2, "NL829 蓝LED将闪烁...");
		if((staval=NL829_LEDCtlT(0x1f))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		cls_printf("蓝灯闪烁...");
		sleep(5);//msdelay(500);//时间长些,已让测试者有足够时间确认
		NL829_LEDCtlT(0x0f);
		cls_show_msg1(2, "NL829 LED将闪灭...");
		for(loop=0; loop<8; loop++)
		{
			if((staval=NL829_LEDCtlT((loop%2)?0x0f:0xff))!=NL829_SUCC)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
			sleep(5);//msdelay(500);
		}
		cls_show_msg1(2, "NL829 蓝LED将闪烁...");
		NL829_LEDCtlT(0xff);
		NL829_LEDCtl(0x0f);
		NL829_LEDCtlT(0xff);
		sleep(10);
		if(cls_show_msg("NL829 LED继续闪烁,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			goto ERR;
		}
		NL829_LEDCtlT(0x0f);
		if(cls_show_msg("NL829 LED效果OK,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			goto ERR;
		}
	}

	//LCD测试
	if(g_NL829type==DEVICE_NL829LCD)
	{
		//异常
		if((staval=NL829_LCDCtl(0x00, 0xaaaaaaaa, 0xffffffff))!=NL829LCD_LCD_FMTERR||cls_show_msg("NL829若显示任何金额,按[ENTER];否则按[其它]")==ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;			
		}
		if((staval=NL829_LCDCtl(0x08, 0x00000000, 0x00000000))==NL829_SUCC||cls_show_msg("NL829若显示任何金额,按[ENTER];否则按[其它]")==ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;			
		}
		//NL829_LCDCtl_new的异常测试揉到以下流程中去做

		//正常
		if((staval=NL829_LCDCtl_new(NORMAL_TEXT, (uchar *)"正显LCD测试开始"))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_LCDCtl(0x06, 0x00000000, 0x00000000))!=NL829_SUCC||cls_show_msg("NL829若显示欢迎使用,按[ENTER];否则按[其它]")!=ENTER||cls_show_msg("若NL829的标题栏\"正显LCD测试开始\",按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}		
		//以下测试中会分两行显示时会覆盖标题,故改变测试策略,只要上面测试一下即可cls_show_msg("以下测试中,NL829的标题栏改为\"LCD测试开始\",任意键继续...");
		if((staval=NL829_LCDCtl(0x00, 0x00000000, 0x00000000))!=NL829_SUCC||cls_show_msg("NL829若显示余额0.00元,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_LCDCtl_new(NORMAL_TEXT, (uchar *)"你看到这行就是错误的了!"))!=NL829LCD_LCD_FMTERR)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;			
		}
		if((staval=NL829_LCDCtl(0x01, 0x00000001, 0x00000001))!=NL829_SUCC||cls_show_msg("NL829若显示交易成功或余额0.01元,按[ENTER];否则按[其它]")!=ENTER/*||cls_show_msg("若NL829的标题栏\"正显LCD测试开始\",按[ENTER];否则按[其它]")!=ENTER*/)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_LCDCtl_new(0x02, (uchar *)"标题栏错误!"))!=NL829LCD_LCD_FMTERR)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;			
		}
		if((staval=NL829_LCDCtl(0x02, 0x00000010, 0x00000010))!=NL829_SUCC||cls_show_msg("NL829若显示交易失败,按[ENTER];否则按[其它]")!=ENTER/*||cls_show_msg("若NL829的标题栏\"正显LCD测试开始\",按[ENTER];否则按[其它]")!=ENTER*/)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_LCDCtl(0x03, 0x00000100, 0x00000100))!=NL829_SUCC||cls_show_msg("NL829若显示余额不足及余额1.00元,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_LCDCtl(0x04, 0x99999999, 0x99999999))!=NL829_SUCC||cls_show_msg("NL829若显示交易异常或999999.99元,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_LCDCtl(0x05, 0x00000010, 0x00000010))!=NL829_SUCC||cls_show_msg("NL829若显示请刷卡及消费0.10元,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_LCDCtl_new(REVERSE_TEXT, (uchar *)"反显LCD测试结束"))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_LCDCtl(0x06, 0x00000000, 0x00000000))!=NL829_SUCC||cls_show_msg("NL829若显示欢迎使用,按[ENTER];否则按[其它]")!=ENTER||cls_show_msg("若NL829的标题栏\"反显LCD测试结束\",按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
	}

	//硬件信息设置与读取
	if(g_NL829type==DEVICE_NL829MR_PP60RF||g_NL829type==DEVICE_YH100)
	{
		//异常
		if((staval=NL829_GetHWinfo(NL829_FLAG_MACHINE+1, buf))!=NL829_ARGERR)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_SetHWinfo(NL829_FLAG_MACHINE+1, (uchar *)NL829_HWINFO))!=NL829_ARGERR)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		//正常
		//NL829_FLAG_SN
		if((staval=NL829_GetHWinfo(NL829_FLAG_SN, buf))==NL829_HWINFO_ERR)//若未下载,就下载
		{
			if((staval=NL829_SetHWinfo(NL829_FLAG_SN, (uchar *)NL829_HWINFO))!=NL829_SUCC)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
			else
			{
				if((staval=NL829_GetHWinfo(NL829_FLAG_SN, buf))!=NL829_SUCC||strcmp((char *)buf, NL829_HWINFO))
				{
					cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
					goto ERR;
				}
			}
		}
		else if(staval!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		//NL829_FLAG_MACHINE
		if((staval=NL829_GetHWinfo(NL829_FLAG_MACHINE, buf))==NL829_HWINFO_ERR)//若未下载,就下载
		{
			if((staval=NL829_SetHWinfo(NL829_FLAG_MACHINE, (uchar *)NL829_HWINFO))!=NL829_SUCC)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
			else
			{
				if((staval=NL829_GetHWinfo(NL829_FLAG_MACHINE, buf))!=NL829_SUCC||strcmp((char *)buf, NL829_HWINFO))
				{
					cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
					goto ERR;
				}
			}
		}
		else if(staval!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
	}
	else//DEVICE_NL829LCD//目前可以使用PC工具与机具交互来测试下载SN即可,本处只测试一下读取机号,商户号
	{
		//异常
		if((staval=NL829LCD_HWinfo(NL829LCD_FLAG_GETMERCHANT+1, buf))!=NL829_ARGERR)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		//正常
		//NL829LCD_FLAG_SETSN
		cls_show_msg("请使用SN下载工具测试SN下载,任意键继续其它测试...");
		//NL829LCD_FLAG_GETMACHINE
		if((staval=NL829LCD_HWinfo(NL829LCD_FLAG_GETMACHINE, buf))==NL829_SUCC)
		{
			if(cls_show_msg("取到机号:%s,正确按[ENTER];否则按[其它]", buf)!=ENTER)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
		}
		else if(staval==NL829_HWINFO_ERR)
		{
			if(cls_show_msg("未能取到机号,请确认.正确按[ENTER];否则按[其它]")!=ENTER)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
		}
		else
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		//NL829LCD_FLAG_GETMERCHANT
		if((staval=NL829LCD_HWinfo(NL829LCD_FLAG_GETMERCHANT, buf))==NL829_SUCC)
		{
			if(cls_show_msg("取到商户号:%s,正确按[ENTER];否则按[其它]", buf)!=ENTER)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
		}
		else if(staval==NL829_HWINFO_ERR)
		{
			if(cls_show_msg("未能取到商户号,请确认.正确按[ENTER];否则按[其它]")!=ENTER)
			{
				cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
				goto ERR;
			}
		}
		else
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
	}

	//RTC的设置与读取(以下用例部分地从rtc1用例移植而来)
	if(g_NL829type==DEVICE_NL829LCD)
	{
		if((staval=NL829LCD_GetRTC(old_time))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		test_rtc = TRUE;

		//设置不合理时间(BCD码,但年份值域范围不对)
		memcpy(new_time, "\x19\x81\x11\x03\x07\x17\x37\x03", 8);
		if((staval=NL829LCD_SetRTC(new_time, 8))!=NL829_ARGERR)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		//设置不合理时间(非BCD码)
		memcpy(new_time, "\x20\x0a\x11\x03\x07\x17\x37\x03", 8);
		if((staval=NL829LCD_SetRTC(new_time, 8))!=NL829_ARGERR)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		//设置不合理时间(数据域不完整)
		memcpy(new_time, "\x20\x00", 2);
		if((staval=NL829LCD_SetRTC(new_time, 2))!=NL829_ARGERR)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}

		//设置合理时间(BCD码)//支持的年份:2000-2099
		memcpy(new_time, "\x20\x30\x11\x03\x07\x17\x37\x03", 8);
		if((staval=NL829LCD_SetRTC(new_time, 8))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		memset(new_time, 0, sizeof(new_time));
		if((staval=NL829LCD_GetRTC(new_time))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		else //校验内容
		{
			if(memcmp(new_time, "\x20\x30\x11\x03", 4)||memcmp(new_time+5, "\x17\x37", 2))//年月日时分应是校验过,星期与秒不检验
			{
				cls_show_msg("line %d:测试失败(%.2x%.2x-%.2x-%.2x\n%.2x:%.2x)", __LINE__, new_time[0], new_time[1], new_time[2], new_time[3], new_time[5], new_time[6]);
				goto ERR;
			}
		}

		if((staval=NL829LCD_SetRTC(old_time, 8))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
	}

	cls_show_msg("非卡类控制命令集测试通过");
	return;
ERR:
	NL829_LEDCtl(0x0f);
	NL829_LEDCtlT(0x0f);
	if(g_NL829type==DEVICE_NL829LCD)
	{
		NL829_LCDCtl(0x06, 0x00000000, 0x00000000);
		if(test_rtc)
			NL829LCD_SetRTC(old_time, 8);
	}
	return;
}

static int test_RF_M1_KeyMode(int akeymode,int bkeymode)
{
	/*private & local definition*/
	int ATQlen = 0, SNRlen = 0, SAKlen = 0;
	uchar buf[NL829_MAXLEN_DATA] = {0}, ATQ[NL829_LEN_ATQ] = {0}, SNR[NL829_LEN_SNR] = {0}, SAK[NL829_LEN_SAK] = {0};
	ushort staval = NL829_FAIL;
	time_t oldtime = 0, diff = 0;
	uint inc = 0x01;
	
	/*process body*/
	//cls_show_msg1(2, "M1命令集测试...");//加提示以刷掉测试菜单
	//测试前置
	NL829_AntennaCtl(0xffff);//完全关闭场
	//预装A,B Key各一个,本块只使用key-A, key-B只存不装载//这里的预装动作由"	key-A"与"大流程1"之间移到最前做为前置,否则测试多卡后存储会返回30 e8(开发无法解决此问题,RC531使然)
	if((staval=NL829_M1_storekey(akeymode, 0x00, NL829_AUTHKEY))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_storekey(bkeymode, 0x00, NL829_AUTHKEY))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//考虑到测试流程的流畅性,不进行"未load就直接认证及读写"的用例(放到key-B用例中测试),因为无法获取是否load过,预期结果不好预设
	if((staval=NL829_M1_loadkey(akeymode, 0x00))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	//异常(可单独测试的异常情况放于此部分,需要前置条件的测试可以放到正常流程中扩展)
	if((staval=NL829_M1_active(0xff, 1, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;		
	}
	if((staval=NL829_M1_storekey(akeymode, 0x10, NL829_AUTHKEY))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_loadkey(akeymode, 0x10))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_auth(0xff, SNR, 0, 0))!=NL829_ERR_FRAME_STX/*NL829_ARGERR*/)//没有做寻卡操作，传入的SNR长为0，导致传入的指令长度不对，指令集在数据长度不对的情况下不返回，测试应用程序就收不到包头  modify by zhengjd
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_auth_ex(0xff, SNR, 0, 0, NL829_AUTHKEY))!=NL829_ERR_FRAME_STX/*NL829_ARGERR*/)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	cls_show_msg("请确保NL829感应区无M-1卡,按任意键继续...请等待%ds", MAXWAITTIME);
	oldtime = time(NULL);
	while((diff=time(NULL)-oldtime)<MAXWAITTIME)
	{
		cls_printf("请确保NL829感应区无M-1卡,请等待%ds", MAXWAITTIME-diff);
		if((staval=NL829_M1_active(NL829_M1_WUPA, 0/*1*/, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_TIMEOUT)//0x0000表示寻卡1次就立即返回
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
	}
	cls_show_msg("请确保NL829感应区有2张M-1卡,按任意键继续...");
	if((staval=NL829_M1_active(NL829_M1_WUPA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_MULCARD)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	//注意:M1与CPU不同,CPU出错时有一些安全要求高的场合需要下电重新寻卡,而M1无此要求.故以下的测试策略就与rf5不同,这将大大简化测试代码(出错时不必重新开始).	
	//但经实际验证发现测试卡还是要求出错后重新寻卡的!故也按rf5的做法来实现
	//基于正常流程的测试
	//key-A
	//大流程1
	cls_show_msg("请确保NL829感应区有1张M-1卡,按任意键继续...");
	//寻卡(包括 "寻卡-防冲突-选卡")
	if((staval=NL829_M1_active(NL829_M1_REQA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//认证
	if((staval=NL829_M1_auth(akeymode, SNR, SNRlen, 0x01))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//块读写及增减量
	if((staval=NL829_M1_BLKWrite(0x02, NL829_BLK02DATA_FF))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_BLKRead(0x02, buf))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else
	{
		if(memcmp(NL829_BLK02DATA_FF, buf, NL829_LEN_BLKDATA))
		{
			cls_show_msg("line %d:块读写失败", __LINE__);
			goto ERR;
		}
	}
	//增减量操作之前,块上的数据必须先符合钱包格式,否则操作会失败
	if((staval=NL829_M1_BLKInc(0x02, &inc))==NL829_SUCC)//!=MI_INCRERR)//实际返回
	{
		cls_show_msg("line %d:增减量操作应失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//需要重新下电寻卡一下
	if((staval=NL829_AntennaCtl(10))!=NL829_SUCC)//10ms后再打开天线
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//大流程2
	//寻卡(包括 "寻卡-防冲突-选卡")
	if((staval=NL829_M1_active(NL829_M1_REQA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//认证
	if((staval=NL829_M1_auth(akeymode, SNR, SNRlen, 0x01))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}	
	if((staval=NL829_M1_BLKDec(0x02, &inc))==NL829_SUCC)//!=MI_INCRERR)//实际返回
	{
		cls_show_msg("line %d:增减量操作应失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//需要重新下电寻卡一下
	if((staval=NL829_AntennaCtl(10))!=NL829_SUCC)//10ms后再打开天线
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//大流程3
	//寻卡(包括 "寻卡-防冲突-选卡")
	if((staval=NL829_M1_active(NL829_M1_REQA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//认证
	if((staval=NL829_M1_auth(akeymode, SNR, SNRlen, 0x01))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}	
	if((staval=NL829_M1_BLKWrite(0x02, NL829_BLK02DATA_ORI))!=NL829_SUCC)	
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_BLKInc(0x02, &inc))!=NL829_SUCC)
	{
		cls_show_msg("line %d:增减量操作失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_BLKInc(0x02, &inc))!=NL829_SUCC)
	{
		cls_show_msg("line %d:增减量操作失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_BLKDec(0x02, &inc))!=NL829_SUCC)
	{
		cls_show_msg("line %d:增减量操作失败(%04x)", __LINE__, staval);
		goto ERR;
	}		
	//上面三个调用的最终实际效果为+1
	if((staval=NL829_M1_BLKRead(0x02, buf))!=NL829_SUCC)
	{
		cls_show_msg("line %d:块读失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else
	{
		if(memcmp(BLK02DATA_INC1, buf, LEN_BLKDATA))
		{
			cls_show_msg("line %d:块读写失败", __LINE__);
			goto ERR;
		}			
	}
	//halt
	if((staval=NL829_M1_halt())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//key-B,使用外部认证(使用内部认证不进行load,预期失败)
	//大流程4
	//cls_show_msg("请确保NL829感应区有一张M-1卡,按任意键继续...");
	//寻卡(包括 "寻卡-防冲突-选卡")
	if((staval=NL829_M1_active(NL829_M1_REQA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_halt())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_active(NL829_M1_WUPA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_SUCC)//换种模式来激活
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//认证
#if 0	//由于测试使用的A,B key的值是一样的,且load起来后是放于共用的密钥缓冲的,故虽然未load过B,但此处仍会成功.另外,auth中mode的作用是用来使用不同的算法,与load中mode的作用不同!
	//使用内部认证不进行load,预期失败,再使用外部认证
	if((staval=NL829_M1_auth(NL829_AUTHKEY_TYPE_B, SNR, SNRlen, 0x01))!=NL829_M1_NOTAUTHERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
#endif
	if((staval=NL829_M1_auth_ex(bkeymode, SNR, SNRlen, 0x01, NL829_AUTHKEY))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//块读写及增减量(在key-A中测试过的内容不着重测试了)
	if((staval=NL829_M1_BLKWrite(0x02, NL829_BLK02DATA_ORI))!=NL829_SUCC)	
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_BLKInc(0x02, &inc))!=NL829_SUCC)
	{
		cls_show_msg("line %d:增减量操作失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_BLKInc(0x02, &inc))!=NL829_SUCC)
	{
		cls_show_msg("line %d:增减量操作失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_M1_BLKDec(0x02, &inc))!=NL829_SUCC)
	{
		cls_show_msg("line %d:增减量操作失败(%04x)", __LINE__, staval);
		goto ERR;
	}		
	//上面三个调用的最终实际效果为+1
	if((staval=NL829_M1_BLKRead(0x02, buf))!=NL829_SUCC)
	{
		cls_show_msg("line %d:块读失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else
	{
		if(memcmp(BLK02DATA_INC1, buf, LEN_BLKDATA))
		{
			cls_show_msg("line %d:块读写失败", __LINE__);
			goto ERR;
		}			
	}
	//halt
	if((staval=NL829_M1_halt())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//大流程5
	cls_show_msg("请在NL829感应区放置1张4k M1卡,任意键继续...");
	//寻卡(包括 "寻卡-防冲突-选卡")
	if((staval=NL829_M1_active(NL829_M1_WUPA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//认证/激活,此处使用外部认证(内部认证类似,但会对卡的存储寿命有所影响)
	//由于不知道4k卡的B密钥是多少,使用A密钥(经测试A密钥为全0xff)
	if((staval=NL829_M1_auth_ex(akeymode, SNR/*UID*/, SNRlen, 0xfe, NL829_AUTHKEY))!=NL829_SUCC)//认证的块在第4K内的扇区
	{
		cls_show_msg("line %d:外部认证失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//读写,考虑到测试卡寿命,仅做读操作
	if((staval=NL829_M1_BLKRead(0xfd, buf))!=NL829_SUCC)//0xfe所在扇区的块应都能读
	{
		cls_show_msg("line %d:块读失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//测试毕,完全关闭场
	if((staval=NL829_AntennaCtl(0xffff))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	//cls_show_msg("M1命令集测试通过");
	//return;
ERR:
	NL829_AntennaCtl(0xffff);//完全关闭场
	return staval;
}

static void test_RF_M1(void)
{
	/*private & local definition*/
	ushort staval = NL829_FAIL;
    
	/*process body*/
    cls_show_msg1(2, "M1命令集测试...");//加提示以刷掉测试菜单

    if((staval=test_RF_M1_KeyMode(NL829_AUTHKEY_TYPE_A,NL829_AUTHKEY_TYPE_B))!=NL829_SUCC)
    {
        cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
        goto ERR;
    }
    if((staval=test_RF_M1_KeyMode(NL829_AUTHKEY_TYPE_A1,NL829_AUTHKEY_TYPE_B1))!=NL829_SUCC)
    {
        cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
        goto ERR;
    }

    cls_show_msg("M1命令集测试通过");
        return;

ERR:
    NL829_AntennaCtl(0xffff);//完全关闭场
    return;
}

void NL829_ICSAM_powerdown_all(void)
{
	/*private & local definition*/

	/*process body*/
	NL829_ICSAM_powerdown(NL829_CARDNO_IC1);
	NL829_ICSAM_powerdown(NL829_CARDNO_SAM1);
	NL829_ICSAM_powerdown(NL829_CARDNO_SAM2);
	NL829_ICSAM_powerdown(NL829_CARDNO_SAM3);
	NL829_ICSAM_powerdown(NL829_CARDNO_SAM4);	
}

//对SAM1,SAM2逐一测试
static void test_ICSAM(void)
{
	/*private & local definition*/
	int len = 0;
	uchar buf[NL829_MAXLEN_DATA] = {0}, sta = 0x00;	
	ushort staval = NL829_FAIL, ret = 0;
	
	uchar sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
	//根据所发的卡的不同,有的可能支持下面的命令,与机具无关,与使用的测试卡有关
	//char sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 0};	
	
	/*process body*/
	if(g_NL829type==DEVICE_NL829LCD)
	{
		if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0x00)
		{
			cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
			goto ERR;
		}
		cls_show_msg("请插入IC1,任意键继续...");
		if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0x01)//应只能检测到插入IC1
		{
			cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
			goto ERR;
		}		
	}
	
	cls_show_msg1(2, "IC/SAM命令集测试...");//加提示以刷掉测试菜单
	//测试前置
	NL829_ICSAM_powerdown_all();
	
	//异常
	if((staval=NL829_ICSAM_poweron(0xf0, buf, buf+1, &len))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_APDUrw(0xf0, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_APDUrw(NL829_CARDNO_SAM1, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SAM_NOTPOWERON
	||(ret=NL829_APDUrw(NL829_CARDNO_SAM2, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SAM_NOTPOWERON)
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;
	}
	if((staval=NL829_ICSAM_powerdown(0xf0))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	
	//正常
	//SAM1
	//上电
	if((staval=NL829_ICSAM_poweron(NL829_CARDNO_SAM1, buf, buf+1, &len))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(buf[0]==NL829_CARD_T0||buf[0]==NL829_CARD_T1)
		cls_show_msg1(2, "SAM1(T%c卡)上电成功", (buf[0]==NL829_CARD_T0)?'0':'1');
	else //未知卡类型
	{
		cls_show_msg("line %d:测试失败(%02x)", __LINE__, buf[0]);
		goto ERR;
	}
	if(g_NL829type==DEVICE_NL829LCD)
	{	
	//取状态
	if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0x11)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
		goto ERR;
	}
	}
	//APDU读写
	if((staval=NL829_APDUrw(NL829_CARDNO_SAM1, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||(ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;
	}
	//下电
	if((staval=NL829_ICSAM_powerdown(NL829_CARDNO_SAM1))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//SAM2
	//上电
	if((staval=NL829_ICSAM_poweron(NL829_CARDNO_SAM2, buf, buf+1, &len))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(buf[0]==NL829_CARD_T0||buf[0]==NL829_CARD_T1)
		cls_show_msg1(2, "SAM2(T%c卡)上电成功", (buf[0]==NL829_CARD_T0)?'0':'1');
	else //未知卡类型
	{
		cls_show_msg("line %d:测试失败(%02x)", __LINE__, buf[0]);
		goto ERR;
	}
	if(g_NL829type==DEVICE_NL829LCD)
	{	
	//取状态
	if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0x21)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
		goto ERR;
	}
	}
	//APDU读写
	if((staval=NL829_APDUrw(NL829_CARDNO_SAM2, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||(ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;	
	}
	//下电
	if((staval=NL829_ICSAM_powerdown(NL829_CARDNO_SAM2))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(g_NL829type==DEVICE_NL829LCD)
	{
	//SAM3
	//上电
	if((staval=NL829_ICSAM_poweron(NL829_CARDNO_SAM3, buf, buf+1, &len))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(buf[0]==NL829_CARD_T0||buf[0]==NL829_CARD_T1)
		cls_show_msg1(2, "SAM3(T%c卡)上电成功", (buf[0]==NL829_CARD_T0)?'0':'1');
	else //未知卡类型
	{
		cls_show_msg("line %d:测试失败(%02x)", __LINE__, buf[0]);
		goto ERR;
	}
	//取状态
	if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0x41)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
		goto ERR;
	}
	//APDU读写
	if((staval=NL829_APDUrw(NL829_CARDNO_SAM3, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||(ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;	
	}
	//下电
	if((staval=NL829_ICSAM_powerdown(NL829_CARDNO_SAM3))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//SAM4
	//上电
	if((staval=NL829_ICSAM_poweron(NL829_CARDNO_SAM4, buf, buf+1, &len))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(buf[0]==NL829_CARD_T0||buf[0]==NL829_CARD_T1)
		cls_show_msg1(2, "SAM4(T%c卡)上电成功", (buf[0]==NL829_CARD_T0)?'0':'1');
	else //未知卡类型
	{
		cls_show_msg("line %d:测试失败(%02x)", __LINE__, buf[0]);
		goto ERR;
	}
	//取状态
	if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0x81)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
		goto ERR;
	}
	//APDU读写
	if((staval=NL829_APDUrw(NL829_CARDNO_SAM4, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||(ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;	
	}
	//下电
	if((staval=NL829_ICSAM_powerdown(NL829_CARDNO_SAM4))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//IC1
	//上电
	if((staval=NL829_ICSAM_poweron(NL829_CARDNO_IC1, buf, buf+1, &len))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(buf[0]==NL829_CARD_T0||buf[0]==NL829_CARD_T1)
		cls_show_msg1(2, "IC1(T%c卡)上电成功", (buf[0]==NL829_CARD_T0)?'0':'1');
	else //未知卡类型
	{
		cls_show_msg("line %d:测试失败(%02x)", __LINE__, buf[0]);
		goto ERR;
	}
	//取状态
	if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0x03)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
		goto ERR;
	}
	//APDU读写
	if((staval=NL829_APDUrw(NL829_CARDNO_IC1, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||(ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;	
	}
	//下电
	if((staval=NL829_ICSAM_powerdown(NL829_CARDNO_IC1))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	}

	//追加几张卡都上电时,getsta的测试
#if 0	//暂不支持
	if(g_NL829type==DEVICE_NL829LCD)
	{
		if((staval=NL829_ICSAM_poweron(NL829_CARDNO_SAM1, buf, buf+1, &len))!=NL829_SUCC||(ret=NL829_ICSAM_poweron(NL829_CARDNO_SAM2, buf, buf+1, &len))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, ret);
			goto ERR;
		}
		if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0x31)
		{
			cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
			goto ERR;
		}
		if((staval=NL829_ICSAM_poweron(NL829_CARDNO_SAM3, buf, buf+1, &len))!=NL829_SUCC||(ret=NL829_ICSAM_poweron(NL829_CARDNO_SAM4, buf, buf+1, &len))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, ret);
			goto ERR;
		}
		if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0xf1)
		{
			cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
			goto ERR;
		}		
		if((staval=NL829_ICSAM_poweron(NL829_CARDNO_IC1, buf, buf+1, &len))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		if((staval=NL829_ICSAM_GetSta(&sta))!=NL829_SUCC||NL829_ICSAM_GETSTA_MASK(sta)!=0xf3)
		{
			cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, sta);
			goto ERR;
		}
		NL829_ICSAM_powerdown_all();
	}
#endif

	cls_show_msg("IC/SAM命令集测试通过");
	return;
ERR://出错处理也是一种测试后置
	NL829_ICSAM_powerdown_all();
	return;
}

static int test_RF_CPU_KEY_INPUT(void)
{
	int len = 0, UIDlen = 0, ATRlen = 0;
	uchar buf[NL829_MAXLEN_DATA] = {0}, UID[NL829_LEN_UID] = {0}, ATR[256] = {0}, keybuf[NL829_LEN_KEY_INPUT]={0},tmp[21]={0};
	ushort staval = NL829_FAIL, staval1=NL829_FAIL,ret = 0;
	time_t oldtime = 0, diff = 0;

	uchar sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令

	cls_show_msg1(2, "射频卡寻卡和按键输入轮询测试...");
	cls_show_msg("请确保NL829感应区无CPU卡,按任意键继续...请等待%ds", MAXWAITTIME);
	oldtime = time(NULL);
	while((diff=time(NULL)-oldtime)<MAXWAITTIME)
	{
		cls_printf("请确保NL829感应区无CPU卡,请等待%ds", MAXWAITTIME-diff);
		if((staval=NL829CpuActiveKeyInput(0, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_TIMEOUT)//0x0000表示寻卡1次就立即返回
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
	}
	//case1  测试寻卡多卡
	cls_show_msg("请确保NL829感应区有2张A卡,按任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_MULCARD)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	
	cls_show_msg("请确保NL829感应区有2张B卡,按任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_MULCARD)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	cls_show_msg("请确保NL829感应区有A,B卡各1张,按任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_MULCARD)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//case2  正常寻卡、APDU
	//A卡正常的寻卡、APDU
	cls_show_msg("请在感应区放置一张标准A卡，任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_APDUrw(NL829_CARDNO_RFID, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||((ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC&&(ret=NL829_GETSTAVAL(buf+len-2))!=0x6d00))
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;
	}
    //B卡正常的激活、APDU
	cls_show_msg("请确保NL829感应区有1张B卡,按任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_SUCC||buf[0]!=NL829_TYPE_B)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, buf[0]);
		goto ERR;
	}
	if((staval=NL829_APDUrw(NL829_CARDNO_RFID, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||((ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC&&(ret=NL829_GETSTAVAL(buf+len-2))!=0x6d00))
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;
	}
	//case 3  测试键盘输入情况
	cls_show_msg("请将感应区的射频卡移开，任意键继续...");
	//键盘输入测试前置，将输入的字符最小值设置为0，最大值设置为21
	if((staval=PP60_cmd_N(0x00))!=NL829_PP60_SUCC||(staval1=PP60_cmd_L(0x18))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	//case3.1  输入的0个字符
	cls_show_msg("在密码键盘上输入[ENTER]，任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_SUCC||strlen((char *)keybuf)!=0)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2,"#3.1测试通过");
	//case3.2  输入6位字符
	cls_show_msg("在密码键盘上输入%s及[ENTER]，任意键继续...","123456");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_SUCC||memcmp(keybuf, "123456", strlen("123456")))
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2,"#3.2测试通过");
	//case3.3  输入21个字符
	memset(tmp,0x31,sizeof(tmp));
	cls_show_msg("在密码键盘上输入21个1及[ENTER]，任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_SUCC||memcmp(keybuf, tmp, sizeof(tmp)))
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	} 
	cls_show_msg1(2,"#3.3测试通过");
	//case3.4  输入的字符超过21个
	cls_show_msg("在密码键盘上输入24个1及[ENTER]，任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_SUCC||memcmp(keybuf, tmp, sizeof(tmp)))
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	} 
	cls_show_msg1(2,"#3.4测试通过");
         
	//case4  超时情况测试
	//case4.1  键盘输入，键盘上没有输入数字的时候，返回寻卡超时
	cls_show_msg("请将感应区中的射频卡移走，任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_TIMEOUT)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg("请将感应区中的射频卡移走，在密码键盘上输入%s后按取消键清掉输入的数字后耐心等待，任意键继续...","123456");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_TIMEOUT)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2,"#4.1测试通过");
	//case4.2  键盘输入，键盘上有输入数字的时候，返回输入超时
	cls_show_msg("请将感应区中的射频卡移走，在密码键盘上输入%s后耐心等待，任意键继续...","123456");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2,"#4.2测试通过");
	//case4.3  键盘输入的时候按下退格清掉输入的数字，感应区放入射频卡，射频卡激活、apdu功能应该正常
	cls_show_msg("请在密码键盘上输入%s后按取消键清掉输入的数字,然后在感应区放入一张标准A卡，任意键继续...","123456");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_SUCC||buf[0]!=NL829_TYPE_A)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, buf[0]);
		goto ERR;
	}
	if((staval=NL829_APDUrw(NL829_CARDNO_RFID, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||(ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;
	}
	cls_show_msg1(2,"#4.3测试通过");
	//case4.4  键盘输入按下取消退出
	cls_show_msg1(2,"键盘上按取消退出，任意键继续...");
	if((staval=NL829CpuActiveKeyInput(5, buf, UID, &UIDlen, ATR, &ATRlen,keybuf))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	cls_show_msg1(2, "射频卡寻卡和按键输入轮询测试通过...");
    return NL829_SUCC;
 ERR:
	PP60_cmd_N(0x04);
	PP60_cmd_L(0x10);
	NL829_AntennaCtl(0xffff);//完全关闭场
	return NL829_FAIL;
}

static void test_RF_CPU(void)
{
	/*private & local definition*/
	int len = 0, UIDlen = 0, ATRlen = 0, pnDatalen=0, ATQlen = 0, SNRlen = 0, SAKlen = 0;
	uchar buf[NL829_MAXLEN_DATA] = {0}, UID[NL829_LEN_UID] = {0}, ATR[256] = {0}, psDatabuf[256]={0}, ATQ[NL829_LEN_ATQ] = {0}, SNR[NL829_LEN_SNR] = {0}, SAK[NL829_LEN_SAK] = {0};
	ushort staval = NL829_FAIL, ret = 0;
	time_t oldtime = 0, diff = 0;
	
	uchar sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
	//根据所发的卡的不同,有的可能支持下面的命令,与机具无关,与使用的测试卡有关
	//char sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 0};	
	
	/*process body*/
	cls_show_msg1(2, "非接CPU卡命令集测试...");//加提示以刷掉测试菜单
	//测试前置
	NL829_AntennaCtl(0xffff);//完全关闭场
	
	//异常(可单独测试的异常情况放于此部分,需要前置条件的测试可以放到正常流程中扩展)
	if((staval=NL829_APDUrw(0xf0, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=NL829_APDUrw(NL829_CARDNO_RFID, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_NOTACTIVE)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//case1:正常寻卡防冲突选卡前的调用,应该返回失败
	if((staval=NL829_RfidTypeARats(0, &pnDatalen, psDatabuf))!=NL829_M1_NOTAGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
    
	cls_show_msg("请确保NL829感应区无CPU卡,按任意键继续...请等待%ds", MAXWAITTIME);
	oldtime = time(NULL);
	while((diff=time(NULL)-oldtime)<MAXWAITTIME)
	{
		cls_printf("请确保NL829感应区无CPU卡,请等待%ds", MAXWAITTIME-diff);
		if((staval=NL829_CPU_active(0/*1*/, buf, UID, &UIDlen, ATR, &ATRlen))!=NL829_TIMEOUT)//0x0000表示寻卡1次就立即返回
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
	}

	cls_show_msg("请确保NL829感应区有2张A卡,按任意键继续...");
	if((staval=NL829_CPU_active(5, buf, UID, &UIDlen, ATR, &ATRlen))!=NL829_MULCARD)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	
	cls_show_msg("请确保NL829感应区有2张B卡,按任意键继续...");
	if((staval=NL829_CPU_active(5, buf, UID, &UIDlen, ATR, &ATRlen))!=NL829_MULCARD)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	cls_show_msg("请确保NL829感应区有A,B卡各1张,按任意键继续...");
	if((staval=NL829_CPU_active(5, buf, UID, &UIDlen, ATR, &ATRlen))!=NL829_MULCARD)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	
	//基于正常流程的测试
	//type-A
	cls_show_msg("请确保NL829感应区有1张A卡,按任意键继续...");
	//寻卡、激活
	if((staval=NL829_CPU_active(5, buf, UID, &UIDlen, ATR, &ATRlen))!=NL829_SUCC||buf[0]!=NL829_TYPE_A)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, buf[0]);
		goto ERR;
	}
	//APDU读写
	if((staval=NL829_APDUrw(NL829_CARDNO_RFID, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||(ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		return;
	}
	//断开连接
	if((staval=NL829_CPU_discnnt(5))!=NL829_TIMEOUT)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	cls_show_msg("请将NL829感应区的A卡移开,按任意键继续...");
	if((staval=NL829_CPU_discnnt(5))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	//关闭场
	if((staval=NL829_AntennaCtl(10))!=NL829_SUCC)//10ms后再打开天线
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	//type-B
	cls_show_msg("请确保NL829感应区有1张B卡,按任意键继续...");
	//寻卡、激活
	if((staval=NL829_CPU_active(5, buf, UID, &UIDlen, ATR, &ATRlen))!=NL829_SUCC||buf[0]!=NL829_TYPE_B)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, buf[0]);
		goto ERR;
	}
	//APDU读写
	if((staval=NL829_APDUrw(NL829_CARDNO_RFID, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||((ret=NL829_GETSTAVAL(buf+len-2))!=NL829_CPU_SUCC&&(ret=NL829_GETSTAVAL(buf+len-2))!=0x6d00))
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		return;
	}
	//断开连接
	if((staval=NL829_CPU_discnnt(5))!=NL829_TIMEOUT)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	cls_show_msg("请将NL829感应区的B卡移开,按任意键继续...");
	if((staval=NL829_CPU_discnnt(5))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	//关闭场
	if((staval=NL829_AntennaCtl(10))!=NL829_SUCC)//10ms后再打开天线
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	//测试封装后Rats指令能否在M1卡寻卡防冲突和选卡的基础上激活A卡进行读写操作add by zhengjd 20141029
	//正常M1卡流程
	cls_show_msg("请在感应区放置一张标准A卡,任意键继续...");
	if((staval=NL829_M1_active(NL829_M1_REQA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//根据pssakbuf 值判断是A卡还是M卡若是A卡执行获取A卡的ATS
	if((SAK[0]&0x20)!=0x20)//bit5为1时可判断为A卡,然后执行A卡激活 apdu
	{
		cls_show_msg("line %d:测试失败(0x%02x)", __LINE__, SAK[0]);
		goto ERR;
	}

	//case2:正常测试
	if((ret=NL829_RfidTypeARats(0, &pnDatalen, psDatabuf))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if((staval=NL829_APDUrw(NL829_CARDNO_RFID, sendbuf, sizeof(sendbuf), buf, &len))!=NL829_SUCC||((ret=NL829_GETSTAVAL(buf+len-2))!=0x9000&&(ret=NL829_GETSTAVAL(buf+len-2))!=0x6d00))
	{
		cls_show_msg("line %d:测试失败(%04x, %04x)", __LINE__, staval, ret);
		goto ERR;
	}
	if(g_NL829type==DEVICE_NL829MR_PP60RF)//YH100 没有键盘,不支持该功能
	{
		if((ret=test_RF_CPU_KEY_INPUT())!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__,ret);
			return;
		}
	}

	cls_show_msg("非接CPU卡命令集测试通过");
	//return;
ERR:
	NL829_AntennaCtl(0xffff);//完全关闭场
	return;
}

//各命令的非法情况,829应不应答
static void test_abnormity(void)
{
	/*private & local definition*/
	int datalen = 0, LRCOffset = 0, appendlen = 0, subcase = 0, ATQlen = 0, SNRlen = 0, SAKlen = 0;
	ushort ret1 = NL829_FAIL;
	uchar frame_cmd[NL829_MAXLEN_FRAME*2] = {0}, *data = NULL;
	uchar resp[NL829_MAXLEN_DATA] = {0}, status[2] = {0}, append[NL829_MAXLEN_DATA] = {0};//frame_resp中的data域
	uchar sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
	uchar ATQ[NL829_LEN_ATQ] = {0}, SNR[NL829_LEN_SNR] = {0}, SAK[NL829_LEN_SAK] = {0};
	
	/*process body*/
	cls_show_msg1(2, "异常命令测试...");//加提示以刷掉测试菜单
	//测试前置
	serial_clrportbuf_all(g_devicecom);
	NL829_AntennaCtl(0xffff);//完全关闭场
	NL829_LEDCtl(0x0f);	
	
	// 1.错误的帧格式,设备忽略不应答
	//组包
	memset(frame_cmd, 0xaa, sizeof(frame_cmd));
	//发包
	if((ret1=NL829_send_frame(frame_cmd, sizeof(frame_cmd)))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	//收包
	if((ret1=NL829_recv_frame(resp, &datalen, 40/(strlen(g_devicecombps)-2)))!=NL829_ERR_FRAME_STX)//40是按1K多数据以BPS300来传输计算出的
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示	

	// 2.Data_len>512,设备忽略不应答
	//组包
	datalen = 768;
	frame_cmd[NL829_OFFSET_STX] = STX;
	frame_cmd[NL829_OFFSET_DATALEN] = datalen/256;
	frame_cmd[NL829_OFFSET_DATALEN+1] = datalen%256;
	data = frame_cmd+NL829_OFFSET_DATA;
	memcpy(data, NL829_SETHWINFO, NL829_LEN_CMD);
	strcpy((char *)data+NL829_OFFSET_ARGS, NL829_SETHWINFO_CHECK);
	data[NL829_OFFSET_ARGS+strlen(NL829_SETHWINFO_CHECK)] = 0x00;//SN
	LRCOffset = NL829_LEN_FRAMEHEAD+datalen;
	frame_cmd[LRCOffset] = MakeLRC(data, datalen);
	frame_cmd[LRCOffset+NL829_LEN_LRC] = ETX;
	//发包
	if((ret1=NL829_send_frame(frame_cmd, LRCOffset+NL829_LEN_LRC+1))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	//收包
	if((ret1=NL829_recv_frame(resp, &datalen, 40/(strlen(g_devicecombps)-2)))!=NL829_ERR_FRAME_STX)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示	

	// 3.不识别的命令,设备忽略不应答
	//组包
	datalen = NL829_pack_data((uchar *)"\xff\xff", strlen("\xff\xff"), frame_cmd);
	//发包
	if((ret1=NL829_send_frame(frame_cmd, datalen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	//收包
	if((ret1=NL829_recv_frame(resp, &datalen, 40/(strlen(g_devicecombps)-2)))!=NL829_ERR_FRAME_STX)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示	

	// 4.某些命令(主要就是指硬件信息设置与读取命令)的异常在之前没有测试到的,在此处追加测试,设备返回预期错误(如,参数错误)
	//下面仅以SN的设置与读取为例,对机号实现是一样的,不测试
	if(g_NL829type==DEVICE_NL829LCD)
	{
		if((ret1=NL829_send_frame((uchar *)"\x02\x00\x08\x31\x20\x31\x32\x33\x34\x35\x20\x03", 12))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
			goto ERR;
		}
		if((ret1=NL829_recv_frame(resp, &datalen, 40/(strlen(g_devicecombps)-2)))!=NL829_ERR_FRAME_STX)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
			goto ERR;
		}
	}
	else
	{
	data = resp;//引入指针,指向地址可以根据实际需要调整,代码改动量小且灵活,妙!
	memcpy(data, NL829_SETHWINFO, NL829_LEN_CMD);
	strcpy((char *)data+NL829_OFFSET_ARGS, "1234567");
	data[NL829_OFFSET_ARGS+strlen("1234567")] = NL829_FLAG_SN;
	strcpy((char *)data+NL829_OFFSET_ARGS+strlen("1234567")+1, NL829_SETHWINFO_CHECK);
	datalen = NL829_OFFSET_ARGS+strlen("1234567")+1+strlen(NL829_SETHWINFO_CHECK);
	if((ret1=NL829_frame_factory(data, datalen, status, append, &appendlen, 0))!=NL829_SUCC||appendlen!=NL829_SETHWINFO_APPLEN_RCV)
	{
		cls_show_msg("line %d:测试失败(%04x, %d)", __LINE__, ret1, appendlen);
		goto ERR;
	}
	if((ret1=NL829_GETSTAVAL(status))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}

	data = resp;
	memcpy(data, NL829_GETHWINFO, NL829_LEN_CMD);
	strcpy((char *)data+NL829_OFFSET_ARGS, "1234567");
	data[NL829_OFFSET_ARGS+strlen("1234567")] = NL829_FLAG_SN;
	datalen = NL829_OFFSET_ARGS+strlen("1234567")+1;
	if((ret1=NL829_frame_factory(data, NL829_GETHWINFO_DATALEN_SND, status, append, &appendlen, 0))!=NL829_SUCC||appendlen!=0)
	{
		cls_show_msg("line %d:测试失败(%04x, %d)", __LINE__, ret1, appendlen);
		goto ERR;
	}
	if((ret1=NL829_GETSTAVAL(status))!=NL829_ARGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示	

	// 5.正常命令的头前,中部,尾后有干扰数据,以beep命令为例,20161028 引入的YH100只有蓝灯,无蜂鸣器,故修改以蜂鸣器命令为例子
	//中间干扰
	if((ret1=NL829_send_frame((uchar *)"\x02\x00\x04\x31\x14\x10\x10\x25\x03", 9))!=NL829_SUCC)
	//if((ret1=NL829_send_frame((uchar *)"\x02\x00\x06\x31\x13\x07\x07\xd0\x01\xf3\x03", 11))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	if((((ret1=NL829_recv_frame(resp, &datalen, 40/(strlen(g_devicecombps)-2)))!=NL829_ERR_FRAME_STX)&&((ret1!=NL829_SUCC)||((appendlen=NL829_unpack_data(resp, datalen, status, append))!=NL829_BEEP_APPLEN_RCV)||(NL829_GETSTAVAL(status)!=NL829_ARGERR)))
	||cls_show_msg("NL829若蓝灯亮,按[ENTER];否则按[其它]")==ENTER)//发出beep声
	{
		cls_show_msg("line %d:测试失败(%04x,%d,%04x)", __LINE__, ret1, appendlen, NL829_GETSTAVAL(status));
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示	
	//头部干扰(非02开头)
	if((ret1=NL829_send_frame((uchar *)"\x03\x02\x00\x03\x31\x14\x10\x35\x03", 9))!=NL829_SUCC)
	//if((ret1=NL829_send_frame((uchar *)"\x03\x02\x00\x05\x31\x13\x07\xd0\x01\xf4\x03", 11))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	if((ret1=NL829_recv_frame(resp, &datalen, 0/*0x07d0*0x01/1000*/))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	if((appendlen=NL829_unpack_data(resp, datalen, status, append))!=NL829_LEDCTL_APPLEN_RCV/*NL829_BEEP_APPLEN_RCV*/||(ret1=NL829_GETSTAVAL(status))!=NL829_SUCC||cls_show_msg("NL829若蓝灯亮,按[ENTER];否则按[其它]")!=ENTER)//发出beep声
	{
		cls_show_msg("line %d:测试失败(%d,%04x)", __LINE__, appendlen, ret1);
		goto ERR;
	}
	NL829_LEDCtl(0x0f);	
	//尾部干扰
	if((ret1=NL829_send_frame((uchar *)"\x02\x00\x03\x31\x14\x10\x35\x03\x03", 9))!=NL829_SUCC)
	//if((ret1=NL829_send_frame((uchar *)"\x02\x00\x05\x31\x13\x07\xd0\x01\xf4\x03\x03", 11))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	if((ret1=NL829_recv_frame(resp, &datalen, 0/*0x07d0*0x01/1000*/))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	if((appendlen=NL829_unpack_data(resp, datalen, status, append))!=NL829_LEDCTL_APPLEN_RCV/*NL829_BEEP_APPLEN_RCV*/||(ret1=NL829_GETSTAVAL(status))!=NL829_SUCC||cls_show_msg("NL829若蓝灯亮,按[ENTER];否则按[其它]")!=ENTER)//发出beep声
	{
		cls_show_msg("line %d:测试失败(%d,%04x)", __LINE__, appendlen, ret1);
		goto ERR;
	}
	NL829_LEDCtl(0x0f);	
	// 6.允许指令被中断的用例(即能处理"终止状态")
	if(g_NL829type==DEVICE_NL829MR_PP60RF)//YH100 无蜂鸣器,不测试该指令
	{
		//NL829_beep
		datalen = NL829_pack_data((uchar *)"\x31\x13\xff\xff\xff", strlen("\x31\x13\xff\xff\xff"), frame_cmd);
		if((ret1=NL829_send_frame(frame_cmd, datalen))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
			goto ERR;
		}
		cls_show_msg("beep长鸣,按任意键停止鸣叫,同时蓝LED亮...");
		if((ret1=NL829_LEDCtl(0x1f))!=NL829_SUCC||cls_show_msg("NL829若停止鸣叫,同时蓝LED亮,按[ENTER];否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
			goto ERR;
		}
	}

	if(g_NL829type==DEVICE_NL829MR_PP60RF||g_NL829type==DEVICE_YH100)//YH100 仅支持蓝灯,故将绿改成蓝 0x8f改成0x1f 20161028
	{
	//NL829_CPU_active
	datalen = NL829_pack_data((uchar *)"\x32\x24\xff\xff", strlen("\x31\x13\xff\xff"), frame_cmd);
	if((ret1=NL829_send_frame(frame_cmd, datalen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg("按任意键蓝LED灭...");
	if((ret1=NL829_LEDCtl(0x0f))!=NL829_SUCC||cls_show_msg("NL829若蓝LED灭,按[ENTER];否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg("请确保NL829感应区有1张CPU卡,按任意键继续...");
	if((ret1=NL829_APDUrw(NL829_CARDNO_RFID, sendbuf, sizeof(sendbuf), resp, &datalen))!=NL829_NOTACTIVE)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg("请确保NL829感应区无CPU卡,按任意键继续...");
	//NL829_M1_active
	cls_show_msg("请确保NL829感应区有1张M-1卡,按任意键继续...");
	if((ret1=NL829_M1_active(NL829_M1_WUPA, 5, ATQ, &ATQlen, SNR, &SNRlen, SAK, &SAKlen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg("请将NL829感应区的M-1卡移开,按任意键继续...");//至此获得SNR
	datalen = NL829_pack_data((uchar *)"\x33\x21\x52\xff\xff", strlen("\x33\x21\x52\xff\xff"), frame_cmd);
	if((ret1=NL829_send_frame(frame_cmd, datalen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg("按任意键蓝LED亮...");
	if((ret1=NL829_LEDCtl(0x1f))!=NL829_SUCC||cls_show_msg("NL829若蓝LED亮,按[ENTER];否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg("请将同1张M-1卡再移入NL829感应区,按任意键继续...");
	if((ret1=NL829_M1_auth_ex(NL829_AUTHKEY_TYPE_B, SNR, SNRlen, 0x01, NL829_AUTHKEY))!=NL829_M1_NOTAGERR)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}
	cls_show_msg("按任意键蓝LED灭...");
	if((ret1=NL829_LEDCtl(0x0f))!=NL829_SUCC||cls_show_msg("NL829若蓝LED灭,按[ENTER];否则按[其它]")!=ENTER)//NL829_LEDCtl(0x0f);
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret1);
		goto ERR;
	}	
	//cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示
	}

	cls_show_msg("异常命令测试通过");
ERR:
	//测试后置
	serial_clrportbuf_all(g_devicecom);
	NL829_AntennaCtl(0xffff);//完全关闭场
	NL829_LEDCtl(0x0f);
	return;
}

//NL829_RFID_xxx函数设计说明:统一设计为有type与UID两个参数
//最初只供射频使用,之后扩展为可用于接触式SAM测试.UID在测试射频时为UID,测试SAM时用于区分SAM1还是SAM2
static ushort NL829_RFID_seek(SMART_t type, uchar *UID)
{
	/*private & local definition*/
	int UIDlen = 0, ATRlen = 0, ATQlen = 0, SAKlen = 0;
	uchar buf[NL829_MAXLEN_DATA] = {0}, ATR[256] = {0}, ATQ[NL829_LEN_ATQ] = {0}, SAK[NL829_LEN_SAK] = {0};
	ushort  staval = NL829_FAIL;

	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
		if((staval=NL829_CPU_active(5, buf, UID, &UIDlen, ATR, &ATRlen))!=NL829_SUCC||buf[0]!=(type==CPU_A?NL829_TYPE_A:NL829_TYPE_B))
		{
			//cls_show_msg1(g_keeptime, "line %d:寻卡失败(0x%04x, 0x%02x)", __LINE__, staval, buf[0]);
			return (staval!=NL829_SUCC)?staval:NL829_FAIL;			
		}
		return NL829_SUCC;
		break;
	case MIFARE_1:
		return NL829_M1_active(NL829_M1_WUPA, 5, ATQ, &ATQlen, UID, &UIDlen, SAK, &SAKlen);//使用推荐值来进行测试
		break;
	case SAM:
	case IC:	
		return NL829_ICSAM_poweron(UID[0], buf, ATR, &ATRlen);//不理会buf标识的T0 or T1
		break;
	default:
		return NL829_FAIL;
		break;
	}	
}

static ushort NL829_RFID_active(SMART_t type, uchar *UID)
{
	/*private & local definition*/

	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
		return NL829_SUCC;
		break;
	case MIFARE_1:
		//由于不知道4k卡的B密钥是多少,使用A密钥(经测试A密钥为全0xff)
		return NL829_M1_auth_ex(NL829_AUTHKEY_TYPE_A, UID, 4, 0x01, NL829_AUTHKEY);//SNR可能包含0字节，故需传递固定长度4
		break;
	case SAM:
	case IC:	
		return NL829_SUCC;
		break;
	default:
		return NL829_FAIL;
		break;
	}	
}

static ushort NL829_RFID_iccrw(SMART_t type, uchar *UID)
{
	/*private & local definition*/
	int rcvlen = 0;
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	uchar Select_1pay[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
#else
	uchar Select_1pay[] = 
	{0x00, 0xA4, 0x04, 0x00, 0x0E,
	  '1',  'P',  'A',  'Y',  '.',  'S',  'Y',  'S',  '.',  'D',  'D',  'F',  '0',  '1',
	  0x00
	};//查询CPU卡的文件目录结构,mifare-1卡不支持
#endif
	uchar out[NL829_MAXLEN_DATA] = {0};
	ushort  staval = NL829_FAIL, ret = 0;

	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
		if((staval=NL829_APDUrw(NL829_CARDNO_RFID, Select_1pay, sizeof(Select_1pay), out, &rcvlen))!=NL829_SUCC||((ret=NL829_GETSTAVAL(out+rcvlen-2))!=NL829_CPU_SUCC&&(ret=SMART_GETSW(out+rcvlen-2))!=0x6d00))
		{
			cls_show_msg1(g_keeptime, "line %d:读写失败(%04x, 0x%04x)", __LINE__, staval, ret);
			return (staval!=NL829_SUCC)?staval:NL829_FAIL;
		}
		return NL829_SUCC;
		break;
	case MIFARE_1:
#if 0	//考虑到测试卡的寿命只做读操作,换一种测试策略
		if((ret=CMD_BLKWrite(0x02, BLK02DATA_FF))!=NL829_SUCC/*MI_OK*/)
		{
			cls_show_msg1(g_keeptime, "line %d:块写失败(ret=%d)", __LINE__, ret);
			return ret;
		}
#endif
		if((staval=NL829_M1_BLKRead(0x02, out))!=NL829_SUCC/*MI_OK*/)
		{
			cls_show_msg1(g_keeptime, "line %d:块读失败(%04x)", __LINE__, staval);
			return staval;
		}
		else
		{
#if 0	//考虑到测试卡的寿命只做读操作,换一种测试策略
			if(memcmp(BLK02DATA_FF, out, LEN_BLKDATA))
#else
			if(!memcmp(gen28_bak, DATA16, NL829_LEN_BLKDATA))//首次
				memcpy(gen28_bak, out, NL829_LEN_BLKDATA);
			else if(memcmp(gen28_bak, out, NL829_LEN_BLKDATA))//其它次
#endif
			{
				cls_show_msg1(g_keeptime, "line %d:块读写失败", __LINE__);
				return NL829_FAIL;
			}
		}
		return NL829_SUCC;
		break;
	case SAM:
	case IC:
		if((staval=NL829_APDUrw(UID[0], Select_1pay, sizeof(Select_1pay), out, &rcvlen))!=NL829_SUCC||(ret=NL829_GETSTAVAL(out+rcvlen-2))!=NL829_CPU_SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:读写失败(%04x, 0x%04x)", __LINE__, staval, ret);
			return (staval!=NL829_SUCC)?staval:NL829_FAIL;
		}
		return NL829_SUCC;
		break;
	default:
		return NL829_FAIL;
		break;
	}
}

static ushort NL829_RFID_powerdown(SMART_t type, uchar *UID, ushort msec)
{
	/*private & local definition*/

	/*process body*/
	switch(type)
	{
	case CPU_A:
	case CPU_B:
	case MIFARE_1:
		return NL829_AntennaCtl(msec);
		break;
	case SAM:
	case IC:
		return NL829_ICSAM_powerdown(UID[0]);
		break;
	default:
		return NL829_FAIL;
		break;
	}	
}
#if 0  //根据开发提供的NL829MR_PP60RF程序员使用手册v1.7  (删除扫描头寄存器写指令) 扫描头设置关闭
static void set_scan(void)
{
	/*private & local definition*/
	ushort staval = NL829_FAIL;
	int plen = 0;
	uchar pvalue[1024] = {0};
	uchar setcode39FULLenable[16] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x1C, 0x21, 0x32, 0x87};
	//uchar setcode39enable[16] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x1C, 0x11, 0x30, 0x97};
	uchar setcodeMSIenable[16] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x1F, 0x09, 0xF6, 0xFD};
	
	/*process body*/
    cls_show_msg1(1, "设置扫描头寄存器中...");

	//case4:设置识读Code39码
	if((staval=NL829_SCAN_SetScan(setcodeMSIenable, 9, pvalue, &plen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	//case5:设置识读MSI码
	if((staval=NL829_SCAN_SetScan(setcode39FULLenable, 9, pvalue, &plen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}

    cls_show_msg("扫描头寄存器设置完成");
	return;
}
#endif
static void test_scan(void)
{
	/*private & local definition*/
	ushort staval = NL829_FAIL;
	int plen = 0;
	uchar pvalue[1024] = {0};
#if 0  //此指令去掉了 暂时关闭
	uchar setcode39disable[16] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x1C, 0x00, 0x32, 0x87};
	uchar setcode39enable[16] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x1C, 0x11, 0x30, 0x97};
#endif
	
	/*process body*/
    cls_show_msg1(1, "扫描头扫描指令测试...");//加提示以刷掉测试菜单

	//case1:阻塞式调用,超时未扫描后获取扫描数据应返回0x0001
	if((staval=NL829_SCAN_Doscan(0,pvalue, &plen))!=NL829SCAN_TIMEOUT)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}

	//case2:非阻塞式扫描情况下，未实际扫描直接获取扫描数据应该返回0x0001正在扫描
    if((staval=NL829_SCAN_Startscan())!=NL829_SUCC)
    {
        cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
    }
	if((staval=NL829_SCAN_GetData(pvalue, &plen))!=NL829SCAN_SCANING)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		NL829_SCAN_Stopscan();
		return;
	}
	if((staval=NL829_SCAN_Stopscan())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}

	//case3:正常调用阻塞式扫描,实际扫描后应该能够返回成功,并获取到扫描数据,默认情况下支持Code39码的扫描
	cls_printf("请在10秒内扫描Code39码");
	if((staval=NL829_SCAN_Doscan(10,pvalue, &plen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
	{
		cls_show_msg("line %d:测试失败", __LINE__);
		return;
	}
#if 0
	//case4:设置不识读Code39码,预期扫描应该超时
	if((staval=NL829_SCAN_SetScan(setcode39disable, 9, pvalue, &plen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	cls_printf("请在5秒内扫描Code39码,预期超时无法识读");
	if((staval=NL829_SCAN_Doscan(5, pvalue, &plen))!=NL829SCAN_TIMEOUT)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	
	//case5:设置识读Code39码,预期扫描应该成功
	if((staval=NL829_SCAN_SetScan(setcode39enable, 9, pvalue, &plen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	cls_printf("请在10秒内重新扫描Code39码");
	if((staval=NL829_SCAN_Doscan(10,pvalue, &plen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
	{
		cls_show_msg("line %d:测试失败", __LINE__);
		return;
	}
#endif	
	//case6:正常掉用非阻塞式扫描测试,扫描到的数据应该和预期一致
	while(1)
	{
		if((staval=NL829_SCAN_Startscan())!=NL829_SUCC)
	    {
	        cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			NL829_SCAN_Stopscan();
			return;
	    }
		if(cls_show_msg1(20,"请在20秒内进行扫描,扫描后按任意键继续,取消键退出")==ESC)
			break;
		plen=0;
		memset(pvalue,0,sizeof(pvalue));
		if((staval=NL829_SCAN_GetData(pvalue, &plen))!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			NL829_SCAN_Stopscan();
			return;
		}
		if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			NL829_SCAN_Stopscan();
			return;
		}
		if((staval=NL829_SCAN_Stopscan())!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return;
		}
	}

	//case7:非阻塞式,未调用start情况下getdata和stop,预期不出现异常
	if((staval=NL829_SCAN_GetData(pvalue, &plen))!=NL829SCAN_SCANING)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval=NL829_SCAN_Stopscan())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}

    cls_show_msg("扫描头扫描指令测试通过");
	return;
}

//新增SP10T 扫描动画控制指令 20170828 sull add
static void test_showanimation(void)
{
	ushort staval = NL829_FAIL;
	int plen = 0 ;
	uchar pvalue[1024] = {0};

	//case1:异常参数测试:ctrl设为0x03；动画张数和要显示图片总数不符(动画张数设为1，显示图片4张)；要显示的图片中有不存在的(显示图片设为(0xff)
	if((staval = NL829_Scan_Animation(0x03, 0x04,0x00,(uchar *)"\x00\x00",(uchar *)"\x01\x02\x03\x04",strlen("\x01\x02\x03\x04"))) != NL829_FAIL)
	{
		cls_show_msg("line %d:异常参数测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval = NL829_Scan_Animation(0x01, 0x01,0x00,(uchar *)"\x00\x00",(uchar *)"\x01\x02\x03\x04",strlen("\x01\x02\x03\x04"))) != NL829_ERR_ANIMATION)
	{
		cls_show_msg("line %d:异常参数测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval = NL829_Scan_Animation(0x01, 0x01, 0x00, (uchar *)"\x00\x00", (uchar *)"\xFF",strlen("\xFF"))) != NL829_NOEXIT_ANIMATION)
	{
		cls_show_msg("line %d:异常参数测试失败(%04x)", __LINE__, staval);
		return;
	}

	//case2:正常测试
	//case2.1采用阻塞式扫描，将扫描条码超时时间设为10s，开启动画，扫描过程中签名板上将显示动画
	if((staval = NL829_Scan_Animation(0x01,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_OPEN)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval = NL829_Scan_Animation(0x02,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_OPEN)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}
    cls_show_msg("请在10秒内扫描条码,扫描过程中签名板上将显示动画，任意键继续");
	if((staval=NL829_SCAN_Doscan(10,pvalue, &plen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR2;
	}
	if(cls_show_msg("扫描过程中签名板上是否显示动画?是按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}

	//case2.2:采用阻塞式扫描，将扫描条码超时时间设为10s，关闭动画，扫描过程中签名板上将不显示动画
	if((staval = NL829_Scan_Animation(0x00,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_CLOSE)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval = NL829_Scan_Animation(0x02,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_CLOSE)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	cls_show_msg("请在10秒内扫描条码,扫描过程中签名板上将不显示动画，任意键继续");	
	memset(pvalue,0,sizeof(pvalue));
	if((staval=NL829_SCAN_Doscan(10,pvalue, &plen))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	if(cls_show_msg("扫描过程中签名板上是否不显示动画?是按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	
	//case2.3:采用非阻塞式扫描，开启动画，先将每张图片显示时间设为默认(0.8s)，再设为0.4s(签名板上动画显示速度变快),再设为1.2s(签名板上动画显示速度变慢)
	if((staval = NL829_Scan_Animation(0x01,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_OPEN)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}	
	cls_show_msg("签名板上显示的动画速度为默认后变快后再变慢，任意键继续");
	if((staval=NL829_SCAN_Startscan())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}
	sleep(5);
	if((staval = NL829_Scan_Animation(0x01,0x04,0x04,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_OPEN)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR2;
	}
	sleep(5);
	if((staval = NL829_Scan_Animation(0x01,0x04,0x0f,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_OPEN)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR2;
	}
	sleep(5);
	if((staval=NL829_SCAN_Stopscan())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR2;
	}
	if(cls_show_msg("签名板上显示的动画速度是否为默认后变快后再变慢?是按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}

	//case2.4:采用非阻塞式扫描，开启动画，将动画显示时间设为5s，扫描时间设为10s，签名板上显示动画5s后不再显示动画
	if((staval = NL829_Scan_Animation(0x01,0x04,0x00,(uchar *)"\x00\x32", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_OPEN)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}
	cls_show_msg("签名板上显示动画5s后将不再显示动画，任意键继续");
	if((staval=NL829_SCAN_Startscan())!=NL829_SUCC)
    {
	    cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
    }
	sleep(10);
	if((staval=NL829_SCAN_Stopscan())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR2;
	}
	if(cls_show_msg("签名板上显示动画5s后是否不再显示动画?是按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}
	
	//case3异常测试:PP60指令规避(开启动画扫描情况下，以下PP60命令不应执行成功，采用非阻塞式扫描)
	cls_printf("规避指令测试...");
	if((staval = NL829_Scan_Animation(0x01,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_OPEN)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}
	if((staval = NL829_SCAN_Startscan()) != NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}
	sleep(1);//开发说开启扫描后会延时大概0.1s才显示动画，故加延时1s，否则直接执行下一条指令会返回成功
	if((staval = PP60_cmd_C()) != NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR2;
	}
	if((staval=PP60_cmd_D(2, NULL, 0))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR2;
	}
	if((staval = NL829_SCAN_Stopscan()) != NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR2;
	}
	if((staval = NL829_Scan_Animation(0x00,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"))) != NL829_ANIMATION_CLOSE)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR1;
	}
	cls_show_msg("扫描动画控制指令测试通过");
ERR1:
	NL829_Scan_Animation(0x00,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"));
	return;
ERR2:
	NL829_Scan_Animation(0x00,0x04,0x00,(uchar *)"\x00\x00", (uchar *)"\x01\x02\x03\x04", strlen("\x01\x02\x03\x04"));
	NL829_SCAN_Stopscan();
	return;
}


//新增SP10T 加载显示图片 20170712  jiangym add
static void test_showimage(void)
{
	ushort staval = NL829_FAIL;
	int  len = 0, i = 0, cnt = 0, num = 0, value = 0, x1 = 0, y1 = 0;
	uchar testbuf[256] = {0}, sdata[256] = {0};
	uchar x[2] = {0}, y[2] = {0}, w[2] = {0}, h[2] = {0}, cnum[2] = {0}, snum[2] = {0};
	uchar *err_para[] = {
					(uchar*) "\x00\x00", //0
					(uchar*) "\x00\x01", // 1
					(uchar*) "\x00\x08",// 2
					(uchar*) "\x01\x40",// 3
					(uchar*) "\x00\xF0",// 4
					(uchar*) "\x01\x41",// 5
					(uchar*) "\x00\xF1",// 6
					  };
	char sCUPBMP48x51[] =
	{
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X20,0X04,0X00,0X00,0X00,0X00,
	0X60,0X06,0X00,0X00,0X00,0X00,0XC7,0XE3,0X00,0X00,0X00,0X03,0X8F,0XF3,0X80,0X00,
	0X00,0X07,0X1F,0XF9,0XC0,0X00,0X00,0X0E,0X1F,0XF8,0XE0,0X00,0X00,0X1E,0X1E,0X00,
	0X70,0X00,0X00,0X3C,0X3C,0X00,0X78,0X00,0X00,0X38,0X3C,0X38,0X3C,0X00,0X00,0X78,
	0X3E,0X78,0X3E,0X00,0X00,0XF8,0X1F,0XF8,0X3E,0X00,0X00,0XF8,0X1F,0XF8,0X3F,0X00,
	0X01,0XFC,0X0F,0XF0,0X7F,0X80,0X03,0XFE,0X07,0XC0,0XFF,0X80,0X07,0XFF,0X00,0X01,
	0XFF,0XC0,0X07,0XFF,0X80,0X03,0XFF,0XE0,0X07,0XFF,0XC0,0X07,0XFF,0XE0,0X07,0XFF,
	0XE0,0X0F,0XFF,0XC0,0X03,0XFF,0XF0,0X1F,0XFF,0X80,0X01,0XFF,0XF8,0X3F,0XFF,0X00,
	0X00,0XFF,0XFC,0X7F,0XFE,0X00,0X00,0X7F,0XFE,0XFF,0XFC,0X00,0X00,0X3F,0XFE,0XFF,
	0XF8,0X00,0X00,0X1F,0XFC,0X7F,0XF0,0X00,0X00,0X0F,0XFC,0X7F,0XF0,0X00,0X00,0X0F,
	0XF8,0X3F,0XF0,0X00,0X00,0X0F,0XF8,0X1F,0XF0,0X00,0X00,0X0F,0XF8,0X1F,0XF0,0X00,
	0X00,0X1F,0XF0,0X0F,0XF8,0X00,0X00,0X3F,0XE0,0X07,0XFC,0X00,0X00,0XFF,0XC0,0X07,
	0XFE,0X00,0X00,0XFF,0X80,0X03,0XFF,0X00,0X00,0XFF,0X00,0X01,0XFF,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X07,0XFE,0X21,0X00,0X40,
	0X04,0X04,0X02,0X3F,0XE0,0XA0,0X7F,0XC5,0XFA,0X20,0X01,0X10,0X44,0X44,0X42,0X4F,
	0XC2,0X08,0X44,0X45,0XFA,0X40,0X0D,0XF6,0X44,0X44,0X42,0XCF,0XC0,0X00,0X7F,0XC4,
	0X52,0X40,0X00,0X00,0X04,0X04,0X4A,0X4F,0XC3,0XF8,0X04,0X07,0XFE,0X48,0X42,0X08,
	0X04,0X04,0X02,0X4F,0XC2,0X08,0X04,0X07,0XFE,0X48,0X43,0XF8,0X00,0X00,0X00,0X00,
	0X00,0X00, 
	};
	
	//case1:异常参数测试
	if((staval=NL829_Show_Image(err_para[1],err_para[0],err_para[3],err_para[4],err_para[0],err_para[1],testbuf,256)) !=NL829_ARGERR)	
	{
		cls_show_msg("line %d:异常参数测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval=NL829_Show_Image(err_para[0],err_para[0],err_para[5],err_para[4],err_para[0],err_para[1],testbuf,256)) !=NL829_ARGERR)	
	{
		cls_show_msg("line %d:异常参数测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval=NL829_Show_Image(err_para[0],err_para[1],err_para[3],err_para[4],err_para[0],err_para[1],testbuf,256)) !=NL829_ARGERR)	
	{
		cls_show_msg("line %d:异常参数测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval=NL829_Show_Image(err_para[0],err_para[0],err_para[3],err_para[6],err_para[0],err_para[1],testbuf,256)) !=NL829_ARGERR)	
	{
		cls_show_msg("line %d:异常参数测试失败(%04x)", __LINE__, staval);
		return;
	}
	if((staval=NL829_Show_Image(err_para[2],err_para[2],err_para[3],err_para[4],err_para[0],err_para[1],testbuf,256)) !=NL829_ARGERR)	
	{
		cls_show_msg("line %d:异常参数测试失败(%04x)", __LINE__, staval);
		return;
	}

	//case2:正常测试
	cls_printf("签名板上连续显示图片");	
	len = sizeof(sCUPBMP48x51);
	cnt = len/256;
	value = len%256;
	if(value != 0)
		num=cnt;
	else
		num=cnt-1;	
	memcpy(w,"\x00\x30",2);//48
	memcpy(h,"\x00\x33",2);//51
	for(x1=0,y1=0;x1<=320-48, y1<=240-51;x1+=7,y1+=7)
	{
		x[0]=(x1&0xffff)>>8;
		x[1]=(x1)&0xff;
		y[0]=(y1&0xffff)>>8;
		y[1]=(y1)&0xff;
		for(i=0;i<cnt;i++)
		{
			cnum[0]=(i&0xffff)>>8;
			cnum[1]=(i)&0xff;
			snum[0]=((num-i)&0xffff)>>8;
			snum[1]=(num-i)&0xff;
			memcpy(sdata,sCUPBMP48x51+256*i,256);
			if((staval=NL829_Show_Image(x,y,w,h,cnum,snum,sdata,256)) !=NL829_SUCC) 
			{	
				cls_show_msg("line %d:图片加载失败(%04x)", __LINE__, staval);
				return;
			}
		}
		if(value != 0)
		{
			cnum[0]=(cnt&0xffff)>>8;
			cnum[1]=cnt&0xff;
			snum[0]=0x00;
			snum[1]=0x00;
			memcpy(sdata,sCUPBMP48x51+256*cnt,value);
			if((staval=NL829_Show_Image(x,y,w,h,cnum,snum,sdata,value)) !=NL829_SUCC) 
			{	
				cls_show_msg("line %d:图片加载失败(%04x)", __LINE__, staval);
				return;
			}
		}	
	}
	if(cls_show_msg("签名板上是否连续显示正确的图片,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:加载显示图片测试失败", __LINE__);
		return;
	}
}
static void test_show(void)
{
	/*private & local definition*/
	int keyin = 0;
				
	/*process body*/
	while(1)
	{
		keyin = cls_show_msg("1.显示图片\n"
								 "2.显示动画\n");
		switch(keyin) 
		{
			case '1':
				test_showimage();
				break;
			case '2':
				test_showanimation();	
				break;
			case ESC:
				return;
				break;
			default:
				continue;
				break;
		 }
	}
}
	

//新增测试按键获取:开始接收按键,获取按键值,结束接收按键
static void test_get_key(void)
{
	ushort staval = NL829_FAIL;
	uchar outbuf[2] = {0};
	int outlen = 0;
	
#include "kbtable.h"

	//case1:开始接收按键,获取按键,结束按键	
	if((staval=NL829_Start_Recv_Key())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}	
	cls_show_msg("请在SP10密码键盘按下%s键后任意键继续",table[1].keyname);
	if((staval=NL829_Get_Key_Value(outbuf,&outlen ))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	if (outbuf[0] != table[1].keycode )
	{
		cls_show_msg("line %d: 获取按键测试失败(%s)", __LINE__,outbuf);
		return;
	}
	if((staval=NL829_End_Recv_Key())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	
	//case2:开始接收按键,插入其他事项,按键,获取按键,结束按键
	if((staval=NL829_Start_Recv_Key())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	//插入其他指令的测试后再进行获取按键
	test_showimage();
	//按键
	cls_show_msg("请在SP10密码键盘按下%s键后任意键继续",table[10].keyname);
	if((staval=NL829_Get_Key_Value(outbuf,&outlen ))!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}	
	if (outbuf[0] != table[10].keycode )
	{
		cls_show_msg("line %d: 获取按键测试失败(%s)", __LINE__,outbuf);
		return;
	}
	if((staval=NL829_End_Recv_Key())!=NL829_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	cls_show_msg1(3,"按键测试通过");
}
static void test_function(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while(1)
       {
		keyin = cls_show_msg("1.非卡类 2.M-1\n"
							"3.接触CPU 4.按键\n" //2.4G:此点暂时关闭,使用按键替换,后续需要再开启,20170806 jym
							"5.非接CPU 6.异常\n"
							"7.扫描头 8.显示\n");
		
		switch(keyin) 
		{
		case '1':
			test_control_cmd();
			break;
		case '2':
			test_RF_M1();
			break;
		case '3':
			if(g_NL829type!=DEVICE_YH100)
			{
				if(ENTER!=cls_show_msg("请确保已装配让所有SAM(IC1除外),[ENTER]继续..."))
					break;
				test_ICSAM();
			}
			else
				cls_show_msg("YH100不支持接该测试");
			break;
		case '4':// 2.4G就是RFSIM的一种,目前2.4G已基本不用了
			//cls_show_msg1(2, "该功能暂不支持!");
			test_get_key();
			break;
		case '5':
			test_RF_CPU();
			break;
		case '6':	
			test_abnormity();
			break;
		case '7':	
			test_scan();
			break;
		case '8':	
			test_show();
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

//设计说明:不关注功能、压力(比如在一定强度下是否成功)等,仅关注性能
//也不关注ability返回,性能值在函数运行过程中给出
static void test_ability(SMART_t type)
{
	/*private & local definition*/
#define	nTestTime	(10)	//(5)

	//int nTestTime = 5;
	int count = 0;
	float fTimeElapsed = 0.0;
	
	//此处命令应与RFID_iccrw中使用的一致
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	uchar Select_1pay[] ={'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
#else
	uchar Select_1pay[] = 
	{0x00, 0xA4, 0x04, 0x00, 0x0E,
	  '1',  'P',  'A',  'Y',  '.',  'S',  'Y',  'S',  '.',  'D',  'D',  'F',  '0',  '1',
	  0x00
	};//查询CPU卡的文件目录结构,mifare-1卡不支持
#endif

	uchar UID[NL829_LEN_UID] = {0};
	char *card[] = {"CPU_A", "CPU_B"/*, "HS_B"*/, "M-1"/*, "RFID-SIM"*/, "FELICA"};

	/*process body*/
	if(g_NL829type!=DEVICE_YH100)
	{
		//先判断是否默认BPS,以默认BPS来计算性能
		if(strcmp(g_devicecombps, "57600"))//if(g_devicecombps!=BPS57600)
		{
			cls_show_msg("请先将POS的BPS配置为57600(默认),任意键退出...");
			return;
		}
	}
	if(type!=SAM&&type!=IC)
		cls_show_msg("请在NL829感应区放置%s卡,任意键继续...", card[type]);
	else
	{
		if(ENTER!=cls_show_msg("请确保已装配所有IC/SAM,[ENTER]继续..."))
			return;
		UID[0] = NL829_select_ICSAM();
	}
	
	//寻卡
#if 0
	cls_printf("正在测试寻卡速度...\n");
	count = 0;
	lib_StartStopwatch();
	while(NL829_RFID_seek(type, UID)==NL829_SUCC)
	{
		count++;
		if((fTimeElapsed=ReadStopwatch())>nTestTime)
			break;
		if(NL829_RFID_powerdown(type, UID, 0xffff)!=NL829_SUCC)
			break;
	}
	if(count>0)//因为不能苛求在此测试场景下一定100%成功,故只计算连续成功的平均时间即可//if(fTimeElapsed>nTestTime)
		cls_show_msg("寻卡速度:%.0f秒/次", fTimeElapsed/count);
	else
	{
		cls_show_msg("line %d:寻卡中发生失败(cnt=%d, err=%x)\n", __LINE__, count, GetErrorCode());
		return;
	}
#else	//仅计算一次寻卡的时间
	cls_printf("正在测试%s速度...\n", (type!=SAM&&type!=IC)?"寻卡":"上电");
	lib_StartStopwatch();
	if(NL829_RFID_seek(type, UID)!=NL829_SUCC)//不关注成功否,由压力测试去关注
	{
		cls_show_msg("line %d:%s失败", __LINE__, (type!=SAM&&type!=IC)?"寻卡":"上电");
		goto ERR;
	}
	cls_show_msg("%s速度:%.0f秒/次", (type!=SAM&&type!=IC)?"寻卡":"上电", lib_ReadStopwatch());//时间很小,基本测不出来
#endif
	
	//激活
	if (NL829_RFID_active(type, UID)!=NL829_SUCC)//不关注成功否,由压力测试去关注
	{	
		cls_show_msg("line %d:激活失败", __LINE__);
		goto ERR;
	}
	
	//测试iccrw性能
	cls_printf("正在测试读写速度...\n");
	count = 0;
	lib_StartStopwatch();
	while(NL829_RFID_iccrw(type, UID)==NL829_SUCC)
	{
		count++;
		if((fTimeElapsed=lib_ReadStopwatch())>nTestTime)
			break;
	}
	if(fTimeElapsed>nTestTime)
		cls_show_msg("每秒读卡:%.0f字节", count*(type==MIFARE_1?NL829_LEN_BLKDATA:Select_1pay[4])/fTimeElapsed);
	else 
		cls_show_msg("line %d:读写中发生失败(cnt=%d)\n", __LINE__, count);
	
	//关闭场
	if (NL829_RFID_powerdown(type, UID, 0xffff)!=SUCC)//不关注成功否,由压力测试去关注
	{	
		cls_show_msg("line %d:%s失败", __LINE__, (type!=SAM&&type!=IC)?"关闭场":"下电");
		return;
	}
	return;
ERR:
	NL829_RFID_powerdown(type, UID, 0xffff);//退出前关闭场
	return;
}

//射频卡压力测试
//设计说明:关注在一定强度下是否仍运行正常
//对于接触/非接的压力测试,除了要判断iccrw类的接口返回成功外,还要对命令状态字(SW)或状态域进行进一步判断!!!
static void test_press(SMART_t type)
{
	/*private & local definition*/
	int succ = 0, cnt = DEFAULT_CNT_VLE, bak = 0, ret1 = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uchar UID[NL829_LEN_UID] = {0};
	ushort ret = 0;
	char *card[] = {"CPU_A", "CPU_B"/*, "HS_B"*/, "M-1"/*, "RFID-SIM"*/, "FELICA"};
	uint len = 0;

	/*process body*/
	if(type!=SAM&&type!=IC)
		cls_show_msg("请在NL829感应区放置%s卡,任意键继续...", card[type]);
	else
	{
		if(ENTER!=cls_show_msg("请确保已装配所有IC/SAM,[ENTER]继续..."))
			return;
		UID[0] = NL829_select_ICSAM();
	}
	
	cls_printf("默认压力测试次数:\n");
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default    
	if ((ret1=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret1==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	while(cnt)
	{
		if (ESC==cls_show_msg1(2, "压力测试中...\n还剩%d次(已成功%d次),ESC中断测试", cnt, succ))
			break;
		cnt--;

		//寻卡
		if((ret=NL829_RFID_seek(type, UID))!=NL829_SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:%s失败(%04x)", __LINE__, bak-cnt, (type!=SAM&&type!=IC)?"寻卡":"上电", ret);
			continue;//break;			
		}
		//激活
		if ((ret=NL829_RFID_active(type, UID))!=NL829_SUCC)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:激活失败(%04x)", __LINE__, bak-cnt, ret);
			continue;//break;
		}
		//读写
		if ((ret=NL829_RFID_iccrw(type, UID))!=NL829_SUCC)
		{	
			NL829_RFID_powerdown(type, UID, 10);
			cls_show_msg1(g_keeptime, "line %d:第%d次:读写失败(%04x)", __LINE__, bak-cnt, ret);
			continue;//break;
		}
		//关闭场
		if ((ret=NL829_RFID_powerdown(type, UID, 0xffff))!=NL829_SUCC)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:%s失败(%04x)", __LINE__, bak-cnt, (type!=SAM&&type!=IC)?"关闭场":"下电", ret);
			continue;//break;
		}
		succ++;
	}

	NL829_RFID_powerdown(type, UID, 0xffff);//退出前关闭场
	cls_show_msg("压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

//射频卡和扫描头的交叉测试
//设计说明:关注在一定强度下是否仍运行正常
//对于接触/非接的压力测试,除了要判断iccrw类的接口返回成功外,还要对命令状态字(SW)或状态域进行进一步判断!!!
static void test_rfcscan_cross(SMART_t type)
{
	/*private & local definition*/
	int succ = 0, cnt = 10, bak = 0, plen = 0, keyin = 0;
	char CntStr[8] = "10";
	uchar UID[NL829_LEN_UID] = {0}, pvalue[500] = {0};
	ushort ret = 0, ret1 = 0;
	char *card[] = {"CPU_A", "CPU_B"/*, "HS_B"*/, "M-1"/*, "RFID-SIM"*/, "FELICA"};
	uint len = 0;
	
	/*process body*/
	if(type!=SAM&&type!=IC)
		cls_show_msg("请准备好%s卡,任意键继续...", card[type]);
	else
	{
		if(ENTER!=cls_show_msg("请确保已装配所有IC/SAM,[ENTER]继续..."))
			return;
		UID[0] = NL829_select_ICSAM();
	}
	
	cls_printf("默认交叉测试次数:\n");
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default    
	if ((keyin=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||keyin==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	
	while(cnt)
	{
		//打开扫描头
		if((ret=NL829_SCAN_Startscan())!=NL829_SUCC)
	    {
			cls_show_msg1(g_keeptime, "line %d:第%d次:打开扫描头失败(%04x)", __LINE__, bak-cnt, ret);
			continue;
	    }
		
		cls_printf("请进行放置卡片或扫码操作\n还剩%d次(已成功%d次)", cnt, succ);
		cnt--;

		ret = -1;
		ret1 = -1;
		ret1=NL829_RFID_seek(type, UID);
		ret=NL829_SCAN_GetData(pvalue, &plen);

		if(ret==NL829_SUCC)
		{
			plen=0;
			memset(pvalue,0,sizeof(pvalue));
			if((ret=NL829_SCAN_GetData(pvalue, &plen))==NL829_SUCC)
			{
				if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
				{
					cls_show_msg1(g_keeptime, "line %d:第%d次:扫码结果不正确", __LINE__, bak-cnt);
					return;
				}
				succ++;
			}
		}
		else if(ret1==NL829_SUCC)
		{
			//激活
			if ((ret=NL829_RFID_active(type, UID))!=NL829_SUCC)
			{	
				cls_show_msg1(g_keeptime, "line %d:第%d次:激活失败(%04x)", __LINE__, bak-cnt, ret);
				continue;//break;
			}
			//读写
			if ((ret=NL829_RFID_iccrw(type, UID))!=NL829_SUCC)
			{	
				NL829_RFID_powerdown(type, UID, 10);
				cls_show_msg1(g_keeptime, "line %d:第%d次:读写失败(%04x)", __LINE__, bak-cnt, ret);
				continue;//break;
			}
			//关闭场
			if ((ret=NL829_RFID_powerdown(type, UID, 0xffff))!=NL829_SUCC)
			{	
				cls_show_msg1(g_keeptime, "line %d:第%d次:%s失败(%04x)", __LINE__, bak-cnt, (type!=SAM&&type!=IC)?"关闭场":"下电", ret);
				continue;//break;
			}
			cls_show_msg1(2,"卡片操作成功,请移走卡片");
			succ++;
		}
		else
			cnt++;//两个操作都没有成功情况下，修正已测试次数
		
		if((ret=NL829_SCAN_Stopscan())!=NL829_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
			return;
		}
	}

	NL829_SCAN_Stopscan();
	NL829_RFID_powerdown(type, UID, 0xffff);//退出前关闭场
	cls_show_msg("交叉测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

static void test_rw_press(SMART_t type)
{
	/*private & local definition*/
	int succ = 0, cnt = DEFAULT_CNT_VLE, bak = 0, ret1 = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uchar UID[NL829_LEN_UID] = {0};
	ushort ret = 0;
	char *card[] = {"CPU_A", "CPU_B"/*, "HS_B"*/, "M-1"/*, "RFID-SIM"*/, "FELICA"};
	uint len = 0;

	/*process body*/
	if(type!=SAM&&type!=IC)
		cls_show_msg("请在NL829感应区放置%s卡,任意键继续...", card[type]);
	else
	{
		if(ENTER!=cls_show_msg("请确保已装配所有IC/SAM,[ENTER]继续..."))
			return;
		UID[0] = NL829_select_ICSAM();
	}
	
	//测试使用次数做限制,不用时间(因为读写SAM有关时钟中断,计时有较大偏差)
	cls_printf("默认压力测试次数:\n");
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default    
	if ((ret1=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret1==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	while(cnt)
	{
		//寻卡
		if((ret=NL829_RFID_seek(type, UID))!=NL829_SUCC)
		{
			cnt--;
			if(ESC==cls_show_msg1(g_keeptime, "line %d:第%d次:%s失败(%04x)", __LINE__, bak-cnt, (type!=SAM&&type!=IC)?"寻卡":"上电", ret))
				break;
			else
				continue;//break;
		}
		//激活
		if ((ret=NL829_RFID_active(type, UID))!=NL829_SUCC)
		{	
			cnt--;
			if(ESC==cls_show_msg1(g_keeptime, "line %d:第%d次:激活失败(%04x)", __LINE__, bak-cnt, ret))
				break;
			else
				continue;//break;
		}

		while(cnt)
		{
			cls_printf("读写压力测试中...\n还剩%d次(已成功%d次),请耐心等待", cnt, succ);
			cnt--;

			//读写
			if ((ret=NL829_RFID_iccrw(type, UID))!=NL829_SUCC)
			{	
				NL829_RFID_powerdown(type, UID, 10);
				cls_show_msg1(g_keeptime, "line %d:第%d次:读写失败(%04x)", __LINE__, bak-cnt, ret);
				break;					
			}
			succ++;
		}
	}

	//关闭场
	if ((ret=NL829_RFID_powerdown(type, UID, 0xffff))!=NL829_SUCC)//测试完毕,场可以永久关闭
	{	
		cls_show_msg1(g_keeptime, "line %d:%s失败(%04x)", __LINE__, (type!=SAM&&type!=IC)?"关闭场":"下电", ret);
		//此时下电失败,仍允许报告压力测试成功率return;
	}

	cls_show_msg("读写压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

static void test_compatibility(SMART_t type)
{
	/*private & local definition*/
#if 0
	uchar UID[NL829_LEN_UID] = {0};
	uchar out[NL829_MAXLEN_DATA] = {0};
	ushort ret = 0;
#endif

	/*process body*/
	switch(type)
	{
	case MIFARE_1:// 4k M-1卡的测试
#if 0	//移入test_RF_M1中
		cls_show_msg("请在NL829感应区放置1张4k M1卡,任意键继续...");
		//寻卡
		if((ret=NL829_RFID_seek(type, UID))!=NL829_SUCC)
		{
			cls_show_msg("line %d:寻卡失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		//认证/激活,此处使用外部认证(内部认证类似,但会对卡的存储寿命有所影响)
		//由于不知道4k卡的B密钥是多少,使用A密钥(经测试A密钥为全0xff)
		if((ret=NL829_M1_auth_ex(NL829_AUTHKEY_TYPE_A, UID, 0xfe, NL829_AUTHKEY))!=NL829_SUCC)//认证的块在第4K内的扇区
		{
			cls_show_msg("line %d:外部认证失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		//读写,考虑到测试卡寿命,仅做读操作
		if((ret=NL829_M1_BLKRead(0xfd, out))!=NL829_SUCC)//0xfe所在扇区的块应都能读
		{
			cls_show_msg("line %d:块读失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		//关闭场
		if ((ret=NL829_RFID_powerdown(type, UID, 0xffff))!=NL829_SUCC)
		{	
			cls_show_msg("line %d:关闭场失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		break;
#endif
	case CPU_A:
	case CPU_B:
	case SAM:
	case IC:
	default:
		cls_show_msg("暂无兼容性测试项目!请选择其它卡类型");
		return;
		break;
	}

	cls_show_msg("兼容性测试通过");
	return;
#if 0
ERR:
	NL829_RFID_powerdown(type, UID, 0xffff);//退出前关闭场
	return;
#endif
}

static void conf_NL829(SMART_t *type)
{
	/*private & local definition*/
	int nKeyin = 0;
	
	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg("1.卡类型配置\n"
							"2.链路配置\n"
							"3.机型配置\n"
							);//"4.扫描寄存器设置\n"

		switch(nKeyin) 
		{
		case '1':
			if(cls_show_msg("提示:功能测试不必进行配置!仍要配置按[ENTER]...")!=ENTER)
				break;
			*type = select_smarttype();//这个配置对功能测试不起作用
			break;
		case '2':
			cls_show_msg("请确保POS与外设已连接!任意键继续...");
			if(conf_test_bps()!=SUCC)
				cls_show_msg("line %d:配置失败!请重启POS和外设后重新配置.", __LINE__);
			else
				cls_show_msg1(2, "链路配置成功!");
			break;
		case '3':
			g_NL829type = select_NL829();
			break;
	//	case '4':
	//		set_scan();
	//		break;
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

/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		  jennywone163 	20110620		created
*****************************************************************/
void systest36(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;
	SMART_t type = CPU_A;
	
	/*process body*/
	cls_show_msg("%s测试中...请先进行配置,按任意键开始\n", TESTITEM);
	g_NL829type = DEVICE_NL829LCD;

	while(1)
	{
		keyin = cls_show_msg("1.功能 2.性能\n"
							"3.综合压力\n"
							"4.读写压力\n"
							"5.兼容性 0.配置\n"
							"6.扫描射频交叉\n");

		memcpy(gen28_bak, DATA16, NL829_LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题		
		switch(keyin)
		{
		case '1':
			test_function();
			break;
		case '2':
			test_ability(type);
			cls_show_msg("请配置其它卡类型进行测试!任意键继续...");
			break;
		case '3':
			test_press(type);
			break;
		case '4':
			if(ENTER!=cls_show_msg("读写压力测试(本项测试不接受ESC中断测试),ENTER键继续..."))
				break;
			test_rw_press(type);
			break;
		case '5':
			test_compatibility(type);
			break;
		case '6':
			test_rfcscan_cross(type);
			break;
		case '0':
			conf_NL829(&type);
			break;
		case ESC:
			cls_show_msg("请配置其它BPS重新测试!任意键继续...");
			if(g_NL829type==DEVICE_NL829MR_PP60RF)
				cls_show_msg("请另行进行PED部分的测试!");
			return;
			break;
		default:
			continue;
			break;
		}
	}
	NDK_PortClose(g_devicecom);
	return;
}

