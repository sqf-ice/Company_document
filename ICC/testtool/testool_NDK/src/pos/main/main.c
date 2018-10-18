
/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: TCR端未产品测试程序主框架
* file name		: main.c
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20060312
* directory 		: .\SRC\main\
* description		: 
* related document	: TCR690 API测试程序 详细设计说明书.doc
* 
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "apitest.h"	/*所有为测试API而写的程序的声明均放于此处*/
#include "version.h"
#if defined GUI_SUPPORT	//TBD
#include "guicore.h"
#include "widget.h"
#endif

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
#if defined GUI_SUPPORT
extern void setuserscreen(int x, int y, int w, int h, image_t * img);
#endif

#if (DEBUGLEVEL==COVERAGE)
extern void com_end(void);//zhangnw20061108 add
#endif

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	: 
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20121203	 	porting from HOME version
*****************************************************************/
static int init(void)
{
	/*private & local definition*/
	time_t now = 0;
	char tmp[64] = {0};
	int nKeyin = 0, KeyValid = FALSE;

	/*process body*/
	//E90是使用setlocale(LC_ALL, "C");进行初始化
#if defined _CTYPE_INIT_SUPPORT
	_ctype_init();//zhangnw20060704 add for 初始化标准C库(lingo建议在initusr.s中增加)
#endif
	//安卓产品初始化调用动态库
#if ANDIROD_ENABLE
	dload_library();
#endif
	//memset(g_pps, 0x00, sizeof(g_pps));//zhangnw20110609add 所有卡的PPS均初始为0(即1倍速)
	init_linksetting();
#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME66)	//ME15C为节省应用空间不编译不用的函数
	init_netsetting();
#endif
	//zhangnw20060526 add 为整个TCR测试用例统一初始化一个种子
	srand(time(&now));
#if defined ME11||defined ME15||defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME65||defined ME66//ME15出现了测试方式选择不显示的问题,需增加时间延时等待USB串口打开完成后才能正常显示 20151019 linwl
	sleep(1);
#endif
	//自动化测试标志选择
	nKeyin = '1';
	nKeyin = cls_show_msg1(5,"_测试方式_\n"
							"0.手自动结合\n"
							"1.纯自动测试\n"
							"2.纯手动测试");
	switch(nKeyin)
	{
	case '0':
		auto_flag = 0;
		break;
	case '1':
		auto_flag = 1;
		break;
	case '2':
		auto_flag = 2;
		break;
	default:
		auto_flag = 1;
		break;
	}
	
	//zhangnw20060410 add for select com
	while (!KeyValid)
	{
#if COMNUM==3
#if defined GP710
	nKeyin = cls_show_msg1(2, "选择上送串口:\n①手机串口1\n②座机串口1\n③座机串口2\n");
#else
	nKeyin = cls_show_msg1(2, "选择上送串口:\n①串口1 ②串口2\n③串口3\n");
#endif
#elif COMNUM==2
		nKeyin = cls_show_msg1(2, "选择上送串口:\n①串口1 ②串口2\n0 不上送\n");
#elif COMNUM==0
		nKeyin = cls_show_msg1(2, "选择上送串口:\n①USB串口 \n0 不上送\n");
#else //默认只有一个串口
#if defined ME15C
		nKeyin = cls_show_msg1(2, "选择上送串口:\n①串口2 \n0 不上送\n");
#else
		nKeyin = cls_show_msg1(2, "选择上送串口:\n①串口1 \n0 不上送\n");
#endif
#endif

		switch (nKeyin)
		{		
		case 0://默认上送  20150506未支持24小时不间断测试 修改默认按照菜单1的物理串口或USB串口上送测试结果
		case ENTER:
		case '1':
#if COMNUM>0
			KeyValid = TRUE;
#if defined ME15C
			g_com = PORT_NUM_COM2;
#else
			g_com = PORT_NUM_COM1;		
#endif
			NDK_PortOpen(g_com, "115200,8,N,1");//initaux(g_com, BPS115200, DB8|STOP1|NP);
#else
			KeyValid = TRUE;//无普通串口的产品可以支持从USB串口上送测试结果
			g_com = PORT_NUM_USB;
			NDK_PortOpen(g_com, "115200,8,N,1");
#endif
			break;
#if COMNUM>1	//zhangnw 20060801 modify
		case '2':
			KeyValid = TRUE;
			g_com = PORT_NUM_COM2;
			NDK_PortOpen(g_com, "115200,8,N,1");
			break;
#endif
#if COMNUM>2
		case '3':
			KeyValid = TRUE;
			g_com = PORT_NUM_COM3;
			NDK_PortOpen(g_com, "115200,8,N,1");
			break;
#endif
		case ESC:
			KeyValid = TRUE;
			return FAIL;
			break;	
		//case 0://默认不上送
		case '0':
			KeyValid = TRUE;
#if defined NL829STD||defined IM81||defined SP10		//虽不上送,但需要为屏幕显示重定向到串口做初始化准备
			g_com = PORT_NUM_COM1;
			NDK_PortOpen(g_com, "115200,8,N,1");	
#elif defined ME15C  //ANDIROD_ENABLE||defined N900||
			g_com = PORT_NUM_COM2;
			//NDK_AZPortOpen(g_com, "115200,8,N,1");
#elif defined ME11||defined ME15||defined ME20||defined ME15CHX||defined ME15B||defined ME65||defined ME66
			if(COMNUM==0)
				g_com = PORT_NUM_USB;//ME11 只有USB串口，所以将键盘输入和屏幕显示定向到USB串口 
			else
				g_com = PORT_NUM_COM1;//测试USB，使用物理串口
			//NDK_PortOpen(g_com, NULL);//打开USB串口位置移动到函数入口处，否则ME11 USB串口无法正常工作
			//sleep(5);
#else
			g_com = INVALID_COM;//zhangnw20061207 add 不干扰COM测试时,使用g_com来做一些判断
#endif
			send_flag = FALSE;
			break;
		default:
			continue;
			break;
		}
		
		/*Start of zhangnw on 2006-12-15 9:30 1.6.6*/
#if DEBUGLEVEL==COVERAGE
		if (PORT_NUM_COM1==g_com)//if (AUX1==g_com) // 1 为测试覆盖率中使用的口(vlgtchk.c中定为1)
		{
			KeyValid = FALSE;//上送结果口与上送覆盖数据口冲突
			cls_show_msg("送结果口与送覆盖率数据口冲突!");
			return FAIL;
		}
#endif
		/*End of zhangnw on 2006-12-15 9:30 1.6.6*/
	}//zhangnw20060830 根据新手册调整

	//往PC送测试报告头
	if (g_com!=INVALID_COM) //zhangnw20061215 add
	{
		strcpy(tmp, "请打开串口工具\n以接收测试数据");//信息从简,以显示最少的屏幕为考量
		
#if 0
		while (BTN_OK!=ShowMessageBox(tmp, BTN_OK, -1));//这里可以预留一个自动烤机的后门
#else
		cls_show_msg1(1, tmp);
#endif
	}
#if !ANDIROD_ENABLE
	send_reporthead();
#endif
	return SUCC;
}

/****************************************************************
* function name 	 	: send_modtitle
* functional description 	: 
* input parameter		: 
* output parameter	 	: 
* return value			: 
* history 		 		: author		date			remarks
*			 		 zhangnw	 20060419     	created for 简化func.h而提取出该函数
*					 zhangnw	 20060602   	   delete 多余的int port参数,直接使用g_com
*					 zhangnw		 20060704		修改函数参数(int row,int col)->(int cnt)
*****************************************************************/
static void send_modtitle(int cnt)
{
	/*private & local definition*/
#include "testmod.h" //这个文件要放在/*private & local definition*/的最后

	/*process body*/
	if (send_flag)
	{
		while(NDK_PortTxSendOver(g_com)!=NDK_OK);//总有发完的时候//clrportbuf(g_com, 0);
#if 0//ANDIROD_ENABLE
		NDK_AZPortWrite(g_com, strlen(testmod[cnt]), testmod[cnt]);
#else
		NDK_PortWrite(g_com, strlen(testmod[cnt]), testmod[cnt]);
#endif
	}
	else
		;
}

#if defined GUI_SUPPORT
void * Dorefresh(void * arg)
{
	do{
//		change_font(FONT_DOT_16);
		widget_refresh((widget_t *)arg,0,320-22,240,22);
//		change_font(FONT_DOT_24);//还原
		sleep(2);
	}while(1);
}

void thread_to_refresh_statusbar(widget_t * win)
{
	pthread_t dorefresh;
	pthread_create(&dorefresh, NULL, Dorefresh, (void *)win);
	pthread_detach(dorefresh);
}

void change_user_view(void)
{
	extern guicore_t sys;

	sys.view.h-=22;
	statusbar_disp(0x0c,0);
}

static widget_t * create_windows(void)
{
	widget_t *win = NULL, *panel = NULL;
	
	if ((win=window_new(WIN_PROP_NONE, NULL, NULL))==NULL)//if ((win=window_new(WIN_PROP_TITLE_STRING, "API测试程序", NULL))==NULL)
		return NULL;
	if ((panel=panel_new(win, NULL))==NULL)
	{
		window_destroy(win);
		return NULL;
	}

	change_user_view();
	thread_to_refresh_statusbar(win);
	return win;
}
#endif


#include "menu.h" //zhangnw20060417 modify
#include "list.h"	//zhangnw20111124将两个文件合并为一个文件
#define  SETCASE "/appfs/setcase.txt" 

//使用该配置文件
#if CONFFLAG
#define	MAXMODNUM	256 //最大支持256个模块，每个模块256个用例 
static int changecase(void)
{
	//生成器根据选择的用例生成的文件	
	int fp = 0;
	uint size = 0;
	int ret = 0, len = 0;
	char readbuf[4096] = {0};
	int i = 0, j = 0, k = 0;
	
	if(((ret=NDK_FsFileSize(SETCASE,&size))!=NDK_OK)||(size>sizeof(readbuf)))//获取SETCASE文件大小
	{
		cls_show_msg("%s line:%d配置文件大小异常(ret=%d,size=%d)",__FILE__, __LINE__, ret, size);
		return FAIL;
	}
	if((fp=NDK_FsOpen(SETCASE, "r"))<0) //打开配置文件，并读取配置信息
	{
		cls_show_msg("%s line:%d打开配置文件失败(%d)",__FILE__, __LINE__, fp);
		return FAIL;
	}
	if((len=NDK_FsRead(fp, readbuf, size))!=size)
	{
		cls_show_msg("%s line:%d读取配置文件失败(%d)",__FILE__, __LINE__, len);
		return FAIL;
	}

#if 1
	//下面三组变量要在list.h中存在，如果SP60.INI中没有配置这么多的用例，则需要相应的减少下列变量的数量
	char** funstr[MAXMODNUM] = {funcstr0, funcstr1, funcstr2, funcstr3, funcstr4, funcstr5,
								funcstr6, funcstr7, funcstr8, funcstr9, funcstr10, funcstr11,
								funcstr12, funcstr13, funcstr14, funcstr15, funcstr16, funcstr17,
								funcstr18, funcstr19, funcstr20, funcstr21, funcstr22,NULL};

	PF* func[MAXMODNUM] = {func0, func1, func2, func3, func4, func5, 
							func6, func7, func8, func9, func10, func11,
							func12, func13, func14, func15, func16, func17,
							func18, func19, func20, func21, func22,NULL};
	
	uchar* id[MAXMODNUM] = {id0, id1, id2, id3, id4, id5,
							id6, id7, id8, id9, id10, id11,
							id12, id13, id14, id15, id16, id17,
							id18, id19, id20, id21, id22,NULL};
#else
	char** funstr[3] = {funcstr0,NULL};
	PF* func[3] = {func0,NULL};
	uchar* id[3] = {id0,NULL};
#endif
	
	for(k=0;k<MAXMODNUM;k++)
	{	
		if(funstr[k]==NULL)//达到模块上限数时退出
			break;
		j = 0;
		for(i=0;i<MAXMODNUM;i++)
		{
			if(funstr[k][i]==NULL)//达到用例上限数时退出
				break;
			//cls_show_msg("k=%d,i=%d,j=%d,funstr=%s",k,i,j,funstr[k][i]);
			if(strstr(readbuf,funstr[k][i]))//在配置文件中查找测试项
			{
				func[k][j] = func[k][i];
				id[k][j] = id[k][i];
				j++;
			}
		}
		func[k][j] = NULL;
		id[k][j] = 0;
	}
	
	NDK_FsClose(fp);
	return SUCC;
}
#endif
/****************************************************************
* function name 	 	: nlmain
* functional description 	: 
* input parameter		:
* output parameter	 	: 
* return value			:
* history 		 		: author		date			remarks
*			 		 zhangnw	 	20121203     	porting from HOME version
*****************************************************************/
int nlmain(void)
{
	/*private & local definition*/
	int nKeyin = '0';//zhangnw20060704 add for 自动测试(默认), auto_flag = TRUE
	int SelPage = 1, maxpage = 1;//zhangnw20060419 add
#if defined GUI_SUPPORT
	widget_t * win = NULL;
#endif

	/*process body*/
#if defined ME15CHX||defined ME15B
	int handle = 0;
	handle = NDK_LedOpen("blue");
	NDK_LedOn(handle);
#endif
#if defined ME66
	int handle = 0;
	handle = NDK_LedOpen("cam");
	NDK_LedOn(handle);
#endif
#if defined ME11||defined ME15||defined ME15C||defined ME20
	NDK_LedStatus(LED_RFID_BLUE_FLICK);//增加LED打开操作,帮助无液晶的设备确认程序运行成功
#endif
	NDK_SysBeep();
#if defined ME11||defined ME15||defined ME20||defined ME15CHX||defined ME15B||defined ME65||defined ME66//在函数开始处直接打开USB串口，否则USB串口无法正常工作，该问题需要开发确认
	if(COMNUM==0)
		NDK_PortOpen(PORT_NUM_USB, NULL);
	else	
		NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1");//测试USB串口,打开物理串口还有3个地方需要改:lib.h中COMNUM的定义,layer.c中的init_linksetting,main.c的第127行 
#elif 0//ANDIROD_ENABLE 
	NDK_AZPortOpen(PORT_NUM_COM2, "115200,8,N,1");
#elif defined ME15C//ME15C使用USB转串口对应的是COM2  //defined N900||
	NDK_PortOpen(PORT_NUM_COM2, "115200,8,N,1");
//#elif defined IM81AZ
//	NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1");
#else
	//其它无法将信息显示到液晶的设备，选择输入输出串口须在本处设置
#endif

#if !SCR_DISABLE||defined SP10	//没有液晶的设备不进行界面初始化操作，不支持显示
#if defined GUI_SUPPORT
    if ((win=create_windows())==NULL)
		return FAIL;

	window_show(win);
	set_autoupdate(1);//设置自动刷新界面,若不调也可以,默认就是1
	setuserscreen(0, 0, COLU_NUMS+1, LINE_NUMS+1, NULL);
#else
	if(NDK_ScrInitGui()!=NDK_OK)
		return FAIL;
#if !K21_ENABLE
	if(NDK_ScrAutoUpdate(0, NULL)!=NDK_OK)//NDK_ScrAutoUpdate(1, NULL)是实时刷新,大屏幕时可能会闪屏
		return FAIL;
#endif
	//测试环境使用统一的可显区域(为LCD尺寸)
	//以下信息格式:高*宽
	//同时普屏使用默认英文字体为12*6,汉字12*12;小字体(英文)为8*8;另可扩展到英文16*16,汉字16*8
	//同时彩屏使用默认英文字体为16*8,汉字16*16;小字体(英文)为16*8;另可扩展到英文24*24,汉字24*12
#if !ANDIROD_ENABLE
	if((GUI_GetLCDSize()!=NDK_OK)||(GUI_GetFontSize()!=NDK_OK))
		return FAIL;
#endif
#if !K21_ENABLE
	if(NDK_ScrSetViewPort(0, 0, GUI_GetLCDWidth(), GUI_GetLCDHeight())!=NDK_OK)
		return FAIL;
#endif
#endif
#endif
	if (init()!=SUCC)
	{
#if 0 //zhangnw20061016 封掉提示
		while(BTN_OK !=ShowMessageBox("测试环境初始化失败!", BTN_OK, -1));
#endif
#if defined ME11||defined ME15||defined ME20||defined ME15CHX||defined ME15B||defined ME65||defined ME66//保护措如果初始化失败退出时要关闭USB串口，否则会引起PC工具问题
		if(COMNUM==0)
			NDK_PortClose(PORT_NUM_USB);
		else
			NDK_PortClose(PORT_NUM_COM1);//测试USB串口,关闭物理串口
#elif 0//ANDIROD_ENABLE
		NDK_AZPortClose(PORT_NUM_COM2);
#elif defined ME15C //defined N900||
		NDK_PortClose(PORT_NUM_COM2);
//#elif defined IM81AZ
//		NDK_PortClose(PORT_NUM_COM1);
#else
		//其它无法将信息显示到液晶的设备，选择输入输出串口须在本处设置
#endif
		return FAIL;
	}

#if CONFFLAG
	//判断配置文件是否存在，如果存在则按照配置文件修改函数列表
	if(NDK_FsExist(SETCASE)==NDK_OK)
	{
		if(changecase()!=SUCC)
		{
			cls_show_msg("%s line:%d配置文件异常，请重新下载正确的配置文件",__FILE__, __LINE__);
			return FAIL;
		}
	}
	else//如果配置文件不存在则,提示继续还是退出
	{
		if(cls_show_msg("%s line:%d配置文件不存在[确认键]继续,[其它]退出",__FILE__, __LINE__)!=ENTER)
			return FAIL;
	}	
#endif

	//确定TCRMENU 的最大页
	maxpage = sizeof(tcrmenu)/sizeof(tcrmenu[0]);
	while (1)
	{
		auto_test(&auto_flag, &nKeyin, &SelPage, maxpage, tcrmenu, skip_modID);

		switch (nKeyin)
		{
			//建议这里的函数全部采用无参形式的
#include "func.h"
			break;//zhangnw20060704 add 保护,case 1--8没有break掉的在这里也会break

#if !(defined ME15CHX||defined ME15B||defined ME66)	//ME15C资源有限无需翻页
			//下面对翻页的处理也可以改为循环翻页
#if defined IM81||defined IM81AZ
		case K_ZMK:
#endif
		case DOWNKEY:/*下翻↓*/
			if (SelPage==maxpage)
				while (BTN_OK!=ShowMessageBox("已到末页", BTN_OK, -1));
			else
				SelPage++;
			
			break;
#if defined IM81||defined IM81AZ
		case DOT:
#endif
		case UPKEY:/*上翻↑*/
			if (SelPage==1)
				while (BTN_OK!=ShowMessageBox("已到首页", BTN_OK, -1));
			else
				SelPage--;
			
			break;
#endif
			/*
			* 由于产品键盘存在差异,所以应尽量选择几款产品都有的键
			* 如,数字键0-9等 1-8已被作为选择数字了
			*/
		case '0'://留作一个后门,比如用于自动测试
			if(ShowMessageBox("——提示——\n进行自动测试吗?", BTN_OK | BTN_CANCEL, -1)==BTN_OK)
			{
				auto_flag = TRUE;
				//nKeyin = '0';//zhangnw20060704 add for 自动测试时的初始值
			}
			break;
#if !(defined ME15CHX||defined ME15B||defined ME66)	//ME15C资源有限无需翻页		
		case '9'://zhangnw20060704 add for 快速翻页
			jump2page(maxpage, &SelPage);
			break;
#endif
		case ESC://cancel
			if(ShowMessageBox("——警告——\n返回四图标界面吗?", BTN_OK | BTN_CANCEL, -1)==BTN_OK)
			{
#if defined ME15CHX||defined ME15B||defined ME66
				NDK_LedOff(handle);
				NDK_LedClose(handle);
#endif				
#if  0//ANDIROD_ENABLE
				NDK_AZPortClose(g_com);
#else
				NDK_PortClose(g_com);
#endif
#if defined ME11||defined ME15||defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME66
				sleep(5);//为了留有时间关闭控制台,退到下载界面后可以下载
#endif
				return FAIL;//shutdown();
			}
			break;
		case 8:/*PC键盘退格键*/
		case BACKSPACE:/*←*/
			if(ShowMessageBox("——警告——\n进行软重启吗?", BTN_OK | BTN_CANCEL, -1)==BTN_OK)
			{
#if defined ME15CHX||defined ME15B||defined ME66
				NDK_LedOff(handle);
				NDK_LedClose(handle);
				NDK_PortClose(g_com);
				sleep(1);
#endif	
				NDK_SysReboot();//nlexit(0);
			}

			break;

#if !(defined IM81||ANDIROD_ENABLE)
		case DOT://预留
			if(ShowMessageBox("——警告——\n进行软关机吗?", BTN_OK | BTN_CANCEL, -1)==BTN_OK)
			{
#if defined ME15CHX||defined ME15B||defined ME66
				NDK_LedOff(handle);
				NDK_LedClose(handle);
				sleep(1);
#endif	
				NDK_SysShutDown();
			}
			break;
#endif
	
		case ENTER://zhangnw20061215 add ENTER响应上送覆盖率
#if DEBUGLEVEL==COVERAGE //zhangnw20061027 add for 覆盖率测试(固定使用AUX1)vlgtchk.c中写死了PIPE_STD(为1)
			if(ShowMessageBox("打开串口工具接收\n覆盖率数据", BTN_OK | BTN_CANCEL, -1)==BTN_OK)
			{
				NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1");//initaux(AUX1, BPS115200, DB8|STOP1|NP);
				com_end();
			}
#else
			if(ShowMessageBox("——警告——\n要退出测试程序吗?", BTN_OK | BTN_CANCEL, -1)==BTN_OK)
#if K21_ENABLE
			{
#if defined ME15CHX||defined ME15B||defined ME66
				NDK_LedOff(handle);
				NDK_LedClose(handle);
#endif
				return FAIL;//linwl20130603
			}
#else
			{
				if(ShowMessageBox("——警告——\n要重启测试程序吗?", BTN_OK | BTN_CANCEL, -1)==BTN_OK)
					NDK_SysExit(0x52);//20151025应用新增需求:参数传入0x52实现退出应用程序且不显示4图标界面重启应用程序的功能
				else
					NDK_SysExit(0);
			}
#endif
#endif
			break;
		default ://后期扩展??
			break;
		}
	}
}


#if 0
#include<stdio.h>

void main(void)
{
	NDK_ScrClrs();
	NDK_ScrDispString(2, 2, "app test 4\r\n", 0);
	NDK_KbGetCode(0, NULL);
	return;
}
#endif


