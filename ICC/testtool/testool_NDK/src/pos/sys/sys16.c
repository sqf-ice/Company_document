/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys16.c
* Author 			: linw
* version			: 
* DATE				: 20130109
* directory 		: 
* description		: 本例测试NDK_SysGetPosInf读取pos硬件信息接口功能是否正确
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysGetPosInfo"	
#define MAXWAITTIME 5
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/*函数描述:获取无线模块类型
	参数cin
	0xFF :没有无线模块
	0x02 :MC39 
	0x03 :sim300 
	0x06 :M72 
	高位为0x80表示CDMA模块
	0x81 :dtgs800 
	0x82 :dtm228c 
	返回:输出无线类型
*/
static char * WireType(char cin)
{
	switch(cin)
	{
	case 0xff:
		return ("无");
	case 0x02:
		return ("GPRS+MC39");
	case 0x03:
		return ("GPRS+sim300");
	case 0x06:
		return ("GPRS+M72");
	case 0x81:
		return ("CDMA+dtgs800");
	case 0x82:
		return ("CDMA+dtm228c");
   	case 0x85://GP720 新增 20131128
		return ("CDMA+ce910");
	case 0x07://SP60 新增 20140117
		return ("GPRS+BGS2");
	case 0x08://SP60 新增 20140117
		return ("GPRS+G610");
	case 0x83://AC35平台新增 20180627 
		return ("GPRS+Ac35");
	default:
		return ("未知");
	}
}

/*函数描述:获取磁头模块类型
	参数cin
	0x01 :giga
	0x02 :mesh 
	0x03 :magtek 
	0x04 :Idtech-dualmag 
	0x10 :ADC软解码
	
	返回:输出磁头类型
*/
static char * MagHeadType(char cin)
{
	switch(cin)
	{
		case 0xff:
			return ("无");
			break;
		case 0x01:
			return ("giga");
			break;
		case 0x02:
			return ("mesh");
			break;
		case 0x03:
			return ("magtek");
			break;
		case 0x04:
			return ("Idtech-dualmag");
			break;
		case 0x10:
			return ("ADC软解码");
			break;
		case 0x11:
			return ("GPIO");
			break;
		default:
			return ("未知");
			break;
	}
}

/*函数描述:获取蓝牙模块类型
	参数cin
	0xFF:没有蓝牙模块
	0x01 :BM77
	0x02 :AT24 
	0x03 :AP6210B 
	0x04 :ALLTEC20706 
	0x05:YC1021
	0x06:DA14681
	
	返回:输出蓝牙类型
*/
static char *BtType(char cin)
{
	switch(cin)
	{
		case 0xff:
			return ("无");
			break;
#if K21_ENABLE
		case 0x01:
			return ("BM77");
			break;
		case 0x02:
			return ("AT24");
			break;
		case 0x03:
			return ("AP6210B");
			break;
		case 0x04:
			return ("ALLTEC20706");
			break;
		case 0x05:
			return ("YC1021");
			break;
		case 0x06:
			//return ("DA14681");
			return ("TC35661");
			break;
#else
		case 0x01:
				return ("SD8787");
				break;
			case 0x02:
				return ("AP6210");
				break;
			case 0x03:
				return ("AP6210B");
				break;
			case 0x04:
				return ("AP6212");
				break;
			case 0x05:
				return ("AP6236");
				break;
			case 0x06:
				return ("AP6255");
				break;
#endif
		default:
			return ("未知");
			break;
	}
}

/*函数描述:获取扫描模块类型
	参数cin
	0xFF:没有蓝牙模块
	0x01 :EM1300
	0x02 :EM3000 
	0x03 :SE955 
	0x04 :UE966 
	0x05 :EM3095
	0x06 :EM1365
	0x06 :EM1365
	0x07 :E20390
	0x08 :GC0312
	返回:输出蓝牙类型
*/
static char *ScanType(char cin)
{
	switch(cin)
	{
		case 0xff:
			return ("无");
			break;
		case 0x01:
			return ("EM1300");
			break;
		case 0x02:
			return ("EM3000");
			break;
		case 0x03:
			return ("SE955");
			break;
		case 0x04:
#if K21_ENABLE 
			return ("0390");
#else
			return ("UE966");
#endif
			break;
		case 0x05:
#if defined ME50NX||defined ME68
			return ("AC35");
#else
			return ("EM3095");
#endif
			break;
		case 0x06:
			return ("EM1365");
			break;
		case 0x07:
			return ("E20390");
			break;
		case 0x08:
			return ("GC0312");
			break;
		default:
			return ("未知");
			break;
	}
}
/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	        linw         20130109	created
*****************************************************************/
void sys16(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int ret = -1;
	unsigned int unLen=0;
	char sBuf[128]={0};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//测试前置

	//case1: 异常测试，错误参数，psBuf为空指针
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_KEY_COUNT, &unLen, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2: 异常测试，错误参数，emFlag大于SYS_HWINFO_GET_KEY_COUNT
	if((ret=NDK_SysGetPosInfo(20, &unLen, sBuf)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
#if 1	//文档已修正,对此情况说明为正常(允许)
	//case3: 异常测试，错误参数，punLen为空指针
	//ME31不支持获取POS按键次数，所以用获取机器类型来代替测试这个CASE
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_TYPE/*SYS_HWINFO_GET_KEY_COUNT*/, NULL, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	
	//case4: 正常读取pos机器类型
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_TYPE, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("机器类型:%s,请与样机配置文件进行比较",sBuf);


	//case5: 正常读取pos硬件信息
	memset(sBuf,0xff,sizeof(sBuf)-1);
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case5.1:无线模块
	send_result("无线模块:%s,请与样机配置文件进行比较",WireType(sBuf[0]));
	
	//case5.2射频模块
	send_result("射频模块:%s,请与样机配置文件进行比较",\
		sBuf[1]==0xff?"无":(sBuf[1]==0x01?"RC531":(sBuf[1]==0x02?"PN512":(sBuf[1]==0x03?"AS3911":(sBuf[1]==0x04?"RC663":(sBuf[1]==0x05?"MH1608":"未知"))))));
	
	//case5.3磁卡模块
	send_result("磁卡模块:%s,请与样机配置文件进行比较",MagHeadType(sBuf[2]));
	
	//case5.4扫描头模块
	send_result("扫描头模块:%s,请与样机配置文件进行比较",ScanType(sBuf[3]));
	
	//case5.5外接密码键盘
	send_result("外接密码键盘:%s,请与样机配置文件进行比较",sBuf[4]==0xff?"不支持":(sBuf[4]==0x01?"支持":"未知"));

	//case5.6串口个数
	send_result("串口个数:%s,请与样机配置文件进行比较",sBuf[5]==0xff?"无":(sBuf[5]==0x01?"1":(sBuf[5]==0x02?"2":(sBuf[5]==0x03?"3":"未知"))));
	
	//case5.7是否支持USB
	send_result("支持USB:%s,请与样机配置文件进行比较",sBuf[6]==0xff?"否":(sBuf[6]==0x01?"是":"未知"));
	
	//case5.8是否支持MODEM模块
	send_result("支持MODEM:%s,请与样机配置文件进行比较",sBuf[7]==0xff?"否":(sBuf[7]==0x01?"是":"未知"));
	
	//case5.9是否支持wifi模块
	send_result("支持wifi:%s,请与样机配置文件进行比较",sBuf[8]==0xff?"否":(sBuf[8]==0x01?"是USI WM-G-MR-09":(sBuf[8]==0x02?"是ESP8266":(sBuf[8]==0x03?"是99":"未知"))));
	
	//case5.10是否支持以太网 pinfo[9]:是否支持以太网 0xFF :否 0x01 :dm9000 0x02 :bcm589xcore //根据4.4.3版本NDK文档修改 linwl20131029
	send_result("支持以太网:%s,请与样机配置文件进行比较",sBuf[9]==0xff?"否":(sBuf[9]==0x01?"dm9000":(sBuf[9]==0x02?"bcm589xcore":"未知")));
	
	//case5.11打印模块
	send_result("打印模块:%s,请与样机配置文件进行比较",sBuf[10]==0xff?"无":((sBuf[10]>0 && sBuf[10]<0x80)?"热敏":((sBuf[10]>0x80&&sBuf[10]<0xfe)?"针打":"未知")));
	
	//case5.12是否支持触屏 0xFF :否 0x01:ts_2046 0x02:589x_ts //根据4.4.3版本NDK文档修改 linwl20131029
	send_result("支持触屏:%s,请与样机配置文件进行比较",sBuf[11]==0xff?"否":(sBuf[11]==0x01?"ts_2046":(sBuf[11]==0x02?"589x_ts":"未知")));
	
	//case5.13是否有射频LED灯
	send_result("射频LED灯:%s,请与样机配置文件进行比较",sBuf[12]==0xff?"否":(sBuf[12]==0x01?"是":"未知"));
	
	//case5.14是否有蓝牙
	//send_result("是否有蓝牙:%s,请与样机配置文件进行比较",sBuf[13]==0xff?"否":(sBuf[13]==0x01?"BM77":(sBuf[13]==0x02?"AT24":(sBuf[13]==0x03?"AP6210B":(sBuf[13]==0x04?"ALLTEC20706":(sBuf[13]==0x05?"YC1021":"未知"))))));	
	send_result("蓝牙模块:%s,请与样机配置文件进行比较",BtType(sBuf[13]));
	
	//case5.15是否有NFC
	send_result("是否有NFC:%s,请与样机配置文件进行比较",sBuf[14]==0xff?"否":(sBuf[14]==0x01?"是":"未知"));

	//case5.16是否有国密芯片
	send_result("是否有国密芯片:%s,请与样机配置文件进行比较",sBuf[15]==0xff?"否":(sBuf[15]==0x01?"THK88":(sBuf[13]==0x02?"CCM":"未知")));

	//case6:取bios版本信息 
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BIOS_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("bios版本信息:%s,请判断是否正确",sBuf);
	
	
#if	1//  //开发说K21没有传统机器的序列号只有机器号 ，新版本都要有
	//case7:取机器序列号
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_USN, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("机器序列号:%s,请判断是否正确",sBuf);	
#endif

	//case8:取机器机器号
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_PSN, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("机器机器号:%s,请判断是否正确",sBuf);

	//case9:取主板号
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BOARD_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("主板号:%s,请判断是否正确",sBuf);

	//case10:取pos刷卡总数
	memset(sBuf,0,sizeof(sBuf));
#if K21_ENABLE//K21平台不支持取磁卡刷卡次数
	ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_CREDITCARD_COUNT, &unLen, sBuf);
	if((ret!=NDK_ERR_PARA)&&(ret!=NDK_ERR_NOT_SUPPORT))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
#else
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_CREDITCARD_COUNT, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("刷卡总数:%s,请判断是否正确",sBuf);
#endif

#if PRN_ENABLE
	//case11:取pos打印总长度
	memset(sBuf,0,sizeof(sBuf));
#if K21_ENABLE//K21平台不支持取pos打印总长度
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_PRN_LEN, &unLen, sBuf)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#else
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_PRN_LEN, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("打印总长度:%smm,请判断是否正确",sBuf);
#endif
#endif

	//case12:取pos机开机运行时间
	memset(sBuf,0,sizeof(sBuf));
#if K21_ENABLE//K21平台不支持取开机运行时间
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_RUNTIME, &unLen, sBuf)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
#else
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_RUNTIME, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("开机运行时间:%s,请判断是否正确",sBuf);
#endif

	//case13:取pos机按键次数 
	memset(sBuf,0,sizeof(sBuf));
#if K21_ENABLE//K21平台不支持取按键次数 
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_KEY_COUNT, &unLen, sBuf)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
#else
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_KEY_COUNT, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("按键次数:%s,请判断是否正确",sBuf);
#endif

	//case14:取CPU 类型 5892芯片还是ZA9L芯片  add by chensj 2013/06/24
	memset(sBuf,0,sizeof(sBuf));
#if K21_ENABLE//K21平台不支持取取CPU 类型
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_CPU_TYPE, &unLen, sBuf)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#else
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_CPU_TYPE, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("CPU 类型为:%s,请判断是否正确",sBuf);
#endif

#if !(defined ME15CHX||defined ME30MH||defined ME15B||defined ME66)
	//case15:取POS机的BOOT版本号
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BOOT_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("POS机的BOOT版本号为:%s,请判断是否正确",sBuf);
#endif	

#if !K21_ENABLE
	//case16:取POS机的BIOS的补丁版本号
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_PATCH_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("POS机的BIOS补丁号为:%s,请判断是否正确",sBuf);

	//case16:取公钥版本信息
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_PUBKEY_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("公钥版本信息为:%s,请判断是否正确",sBuf);
#endif

#if defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME62//ME50NX没有PCI固件版本号
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_PCIFW_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("PCI固件版本号为:%s,请判断是否正确",sBuf);
#endif

#if OVERSEAS_ENABLE    //海外版本是获取TID
	//case17:获取终端 编号0到9的数字组成共10位
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_TID, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("终端 编号为:%s,请判断是否正确",sBuf);
#else //中低端POS获取TUSN   
	//case17:获取TUSN信息
#if !defined ME66 //ME66不支持获取TUSN
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_TUSN, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("TUSN为:%s,请判断是否正确",sBuf);
#endif
#endif

	//低端新增获取wifi固件版本号 20180503 sull add
#if NEW_WIFI_ENABLE
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_WIFIFW_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("WIFI固件版本号为:%s,请判断是否正确",sBuf);
#endif

	//安卓产品新增获取非接芯片型号  20180503 sull add
#if ANDIROD_ENABLE
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_RFID_CHIP_TYPE, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("非接芯片型号为:%s,请判断是否正确",sBuf);
#endif

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

