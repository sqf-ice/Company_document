/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: 
* Author 			: zhengry
* version			: V1.0
* DATE			: 2016/05/31
* directory 		: 
* description		: 扫描头/摄像头综合测试
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  2016/05/31  zhengry
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define		TESTITEM	"扫描头/摄像头综合测试"
#define 	MAXTIME		10

/*----------global variables declaration("extern")--------------*/
static int 	nConfInit = 0;	 /*是否配置过了参数:0未配置 1已配置*/
static int type=0; //0表示仅支持扫描头 1表示仅支持摄像头

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
void scan_press(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, succ = 0;

	/*测试前置*/
	if(nConfInit == 0)
	{
		cls_show_msg1(g_keeptime, "参数未配置");	
		return;
	}
	
	/*process body*/
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:设备初始化失败(%d)", __LINE__, ret);
		return;
	}
#if CAMERA_ENABLE
	if(type==1)
	{
		NDK_ScanSet(CAMERA_SETTYPE_IMAGE, g_ShowImage);
		NDK_ScanSet(CAMERA_SETTYPE_SATURATION, g_Saturation);
		NDK_ScanSet(CAMERA_SETTYPE_SHARPNESS, g_Sharpness);
		NDK_ScanSet(CAMERA_SETTYPE_GAMMA, g_Contrast);
	}
#else
	if(type==0)
	{
		NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT, g_Floodlight);
		NDK_ScanSet(SCAN_SETTYPE_FOCUSLIGHT, g_Focuslight);
		NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY, g_Sensitivity);
	}
#endif
	while(1)
	{		
		if(cls_show_msg1(2,"请在%d秒内进行扫描(已扫描%d次,成功%d次),按ESC退出", MAXTIME, i+1, succ)==ESC)
			break;
		i++;
		
		if((ret=lib_doscan(g_ScanType, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
			continue;
		}
		
		succ++;
	}
#if CAMERA_ENABLE
	NDK_ScanClose();
#endif
#if SCAN_SOFT_ENABLE
	NDK_ScanExit();
#endif
	
	cls_show_msg("扫描测试完成\n总次数:%d\n成功:%d次", i, succ);
	return;
}

#if CAMERA_ENABLE
void abnormal(void)
{		
	int ret=0;

	//由于摄像头和外部的U口是接在同一个usb的hub上面,通过一个gpio来切决定是使用摄像头还是外部u口,所以同时只有一条线路是通的,只能用一个
	//测试前置
	NDK_PortClose(PORT_NUM_USB);

	//未开启摄像头扫描,USB打开应正常
	if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:打开USB失败(%d)", __LINE__, ret);
		return;
	}
	if((ret = NDK_PortClose(PORT_NUM_USB)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:关闭USB失败(%d)", __LINE__, ret);
		return;
	}

	//关闭USB后,开启摄像头应正常
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
		return;
	}
	
	if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_ERR_USB_LINE_UNCONNECT)//USB打开应失败
	{
		cls_show_msg1(g_keeptime,"line %d:打开USB失败(%d)", __LINE__, ret);
		return;
	}
	//此处区分判断
	//USB打开成功情况下，进行数据操作
	//USB打开失败情况下，不做数据操作
	NDK_ScanSet(CAMERA_SETTYPE_IMAGE, g_ShowImage);
	NDK_ScanSet(CAMERA_SETTYPE_SATURATION, g_Saturation);
	NDK_ScanSet(CAMERA_SETTYPE_SHARPNESS, g_Sharpness);
	NDK_ScanSet(CAMERA_SETTYPE_GAMMA, g_Contrast);
	if((ret=lib_doscan(g_ScanType, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
		if(g_ScanType==1)
			NDK_ScanClose();
		return;
	}
	if(g_ScanType==1)
		NDK_ScanClose();
	
	//摄像头关闭后,USB打开应正常
	if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:打开USB失败(%d)", __LINE__, ret);
		return;
	}
	NDK_PortClose(PORT_NUM_USB);
	return;
}
#else
void abnormal(void){cls_show_msg("该产品不支持摄像头异常测试");return;}
#endif

void config(void)
{
	/*private & local definition*/
	int ret=0;
	uint unLen=0;
	char CntStr[8]={"0"};
	
	/*process body*/
#if CAMERA_ENABLE
	cls_printf("请选择要测试的硬件类型(0.扫描头 1.摄像头)默认为0:");
	strcpy(CntStr, "0");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen, INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		type = 0;
	else
		type = atoi(CntStr);
	
	if(type!=1 && type!=0) 
	{
		cls_show_msg("硬件类型配置错误");	
		return;
	}
	if(type==1)
		conf_camera();
	else
		conf_scan();
#else
	cls_printf("选择扫描模式:\n0.同步(阻塞)扫描\n1.异步(非阻塞)扫描:\n");
	strcpy(CntStr, "1");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		g_ScanType = 1;
	else
		g_ScanType = atoi(CntStr);
	switch(g_ScanType)
	{
		case 0:
#if !SCAN_ENABLE
			cls_show_msg1(5,"不支持同步扫描方式,请重新选择");
#endif
			break;
		case 1:
#if !SCANASYCN_ENABLE
			cls_show_msg1(5,"不支持异步扫描方式,请重新选择");
#endif
			break;	
	}
#endif
	return;
}

void scan_ability(void)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = 10, bak = 0;
	uint plen = 0;
	uchar pvalue[500] = {0};
	time_t oldtime = 0;
	float ScanTime = 0, ScanTime0 = 0;
#if CPU5810X_ENABLE||defined ME50NX||defined ME68
	uint poutlen = 0;
	uchar poutvalue[500] = {0};
#endif
#if K21_ENABLE
	char str[32] = {0};
#endif
#if defined ME66
	uint a = 0, a1= 0;
#endif

	/*process body*/
	/*测试前置*/
	if(nConfInit == 0)
	{
		cls_show_msg1(g_keeptime, "参数未配置");	
		return;
	}
		
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:设备初始化失败(%d)", __LINE__, ret);
		return;
	}
#if CAMERA_ENABLE
	if(type==1)
	{
		NDK_ScanSet(CAMERA_SETTYPE_IMAGE, g_ShowImage);
		NDK_ScanSet(CAMERA_SETTYPE_SATURATION, g_Saturation);
		NDK_ScanSet(CAMERA_SETTYPE_SHARPNESS, g_Sharpness);
		NDK_ScanSet(CAMERA_SETTYPE_GAMMA, g_Contrast);
	}
#endif
#if SCAN_ENABLE
	if(type==0)
	{
		NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT, g_Floodlight);
		NDK_ScanSet(SCAN_SETTYPE_FOCUSLIGHT, g_Focuslight);
		NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY, g_Sensitivity);
	}
#endif

	if(g_ScanType!=0 && g_ScanType!=1)
	{
		cls_show_msg1(10,"line %d:写入参数出错(%d)", __LINE__, g_ScanType);
		return;
	}
	cls_show_msg("请准备好待扫描的图形,按任意键开始扫描");
    	bak = cnt;                                                                
	while(cnt--)
	{		
		cls_printf("第%d次扫描中...", bak-cnt);
		plen=0;
		ScanTime0 = 0;
		memset(pvalue,0,sizeof(pvalue));
#if SCANASYCN_ENABLE	  //非阻塞扫描
		if(g_ScanType)
		{
#if defined ME66
			NDK_SysReadWatch(&a1);
#else
			lib_StartStopwatch();
#endif
			if((ret=NDK_ScanStart()) != NDK_OK )
			{
				NDK_ScanStop();
				cls_show_msg1_record(FILENAME, FUNCNAME, 1, "line %d:第%d次摄像头开启扫描失败(%d)", __LINE__, bak-cnt, ret);
				continue;
			}   
			oldtime = time(NULL);
			while(1)
			{	
				if((ret=NDK_ScanState(pvalue, &plen)) == NDK_OK)
				{
#if defined ME66
					NDK_SysReadWatch(&a);
					ScanTime0 = (a - a1)/(1000*1.0);
#else
					ScanTime0 = lib_ReadStopwatch();
#endif
					//ScanTime += lib_ReadStopwatch();  //增加可以看每一次的扫描时间
					ScanTime += ScanTime0;
					succ++;
#if CPU5810X_ENABLE||defined ME50NX||defined ME68		
					memset(poutvalue,0,sizeof(poutvalue));
					if((ret=NDK_Utf8ToGbk(pvalue, plen, poutvalue, &poutlen))!=NDK_OK)
					{
						cls_show_msg1_record(FILENAME, FUNCNAME, 1,"line %d:第%d次UTF8转GBK失败(%d)", __LINE__, bak-cnt, ret);
						break;
					}
					memset(str,0,sizeof(str));
					ftos(ScanTime0,str);
					//cls_show_msg1(1,"解码成功!扫描到的长度为:%d的内容:%s", poutlen, poutvalue);
					cls_show_msg1_record(FILENAME, FUNCNAME, 1,"第%d次解码成功!扫描到的长度为:%d的内容:%s,扫码时间:%ss", bak-cnt, poutlen, poutvalue, str);
#else
					memset(str,0,sizeof(str));
					ftos(ScanTime0,str);
					//cls_show_msg1(1,"解码成功!扫描到的长度为:%d的内容:%s", plen, pvalue);	
					cls_show_msg1_record(FILENAME, FUNCNAME, 1,"第%d次解码成功!扫描到的长度为:%d的内容:%s,扫码时间:%ss", bak-cnt, plen, pvalue, str);	
#endif
					break;
				}
				NDK_SysMsDelay(10);
				if((time(NULL) - oldtime)>MAXTIME)
				{
					NDK_ScanStop();
					cls_show_msg1(1,"line %d:扫描超时", __LINE__);
					break;
				}
			}
			if((ret = NDK_ScanStop()) != NDK_OK)
			{
				cls_show_msg1(1,"line %d:关闭摄像头失败(%d)", __LINE__, ret);
				continue;
			}
		}
#endif
#if SCAN_ENABLE
		if(!g_ScanType)
		{
			lib_StartStopwatch();
			if((ret=NDK_ScanDoScan(MAXTIME, pvalue, &plen))!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,1,"line %d:第%d次阻塞式扫描失败(%d)",  __LINE__, bak-cnt, ret);
				continue;
			}
			ScanTime0 = lib_ReadStopwatch();
			ScanTime += ScanTime0;
			//ScanTime += lib_ReadStopwatch();
			succ++;
#if CPU5810X_ENABLE||defined ME50NX||defined ME68
			memset(poutvalue,0,sizeof(poutvalue));
			if((ret=NDK_Utf8ToGbk(pvalue, plen, poutvalue, &poutlen))!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,1,"line %d:第%d次UTF8转GBK失败(%d)", __LINE__, bak-cnt,ret);
				continue;
			}
			memset(str,0,sizeof(str));
			ftos(ScanTime0,str);
			//cls_show_msg1(1,"扫描到的长度为:%d的内容:%s", poutlen, poutvalue);
			cls_show_msg1_record(FILENAME, FUNCNAME, 1,"第%d次扫描到的长度为:%d的内容:%s,扫码时间:%ss", bak-cnt, poutlen, poutvalue, str);
#else
			//cls_show_msg1(1,"扫描到的长度为:%d的内容:%s", plen, pvalue);
			cls_show_msg1_record(FILENAME, FUNCNAME, 1,"第%d次扫描到的长度为:%d的内容:%s,扫码时间:%fs", bak-cnt, plen, pvalue, ScanTime0);
#endif
		}
#endif
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯总次数%d,成功次数%d\n", bak, succ);
	
#if !K21_ENABLE 
	cls_show_msg_record(FILENAME,FUNCNAME,"扫码性能:%fs/次\n", ScanTime/succ);
#else
	memset(str,0,sizeof(str));
	ftos(ScanTime/succ,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "扫码性能:%ss/次\n", str);
#endif
#if CAMERA_ENABLE
	NDK_ScanClose();
#endif	
#if SCAN_SOFT_ENABLE
	NDK_ScanExit();
#endif
	return;
}

void systest33(void)
{
	/*private & local definition*/
	int nKeyin=0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	while(1)
	{
		nKeyin=cls_show_msg("1.压力  2.异常\n"
							"3.性能 0.硬件配置\n"
							);
		switch(nKeyin)
		{
			case '1':
				scan_press();
				break;
			case '2':
				abnormal();
				break;	
			case '3':
				scan_ability();
				break;
			case '0':
				config();
				nConfInit=1;
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

