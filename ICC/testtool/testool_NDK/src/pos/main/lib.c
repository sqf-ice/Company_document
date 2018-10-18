
/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 测试程序公共函数库
* file name		: lib.c
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20060628
* directory 		: .\SRC\main\
* description		: 
* related document	: TCR690 API测试程序 详细设计说明书.doc
* 
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>
#if ANDIROD_ENABLE
#include <dlfcn.h>
#include <android/log.h>
#endif

/*---------------constants/macro definition---------------------*/

/*------------global variables definition-----------------------*/
int g_UCID = 0;
char NowSystestName[64]={0};
int g_SequencePressFlag = 0; //综合测试 连续压力的开关  0:不进行连续压力测试  1:进行连续压力测试
int auto_flag = 1;//测试方式开关 0:表示手自动结合测试 1:表示纯自动化测试 2.表示纯手动测试
int packetnum = 0;
int g_wifistopflag = 1;//新wifi工作机制设置1:表示进行实际挂断  0:表示不进行挂断
#if ANDIROD_ENABLE
int (*cls_show_msg)(const char* msg, ...) = NULL;
int (*cls_show_msg1)(int time, const char* msg, ...) = NULL;
int (*ShowMessageBox)(char* pMsg, char cStyle, int iWaitTime) = NULL;
int (*cls_printf)(const char* msg, ...) = NULL;
int (*lib_kbgetinput)(char * pszBuf,uint unMin,uint unMaxLen,uint * punLen,EM_INPUTDISP emMode,uint unWaittime, EM_INPUT_CONTRL emControl) = NULL;
int (*lib_getkeycode)(uint sec) = NULL;
void (*send_result)(char *fmt, ...) = NULL;
int (*NDK_ScrPrintf)(const char *psFormat, ...) = NULL;
int (*NDK_ScrRefresh)(void) = NULL;
int (*NDK_ScrClrs)(void) = NULL;
int (*cls_show_msg_record)(char* filename, char* funname,const char* msg, ...) = NULL;
int (*cls_show_msg1_record)(char* filename, char* funname,int time, const char* msg, ...) = NULL;
#endif

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: send_result
* functional description 	: 
* input parameter		: 
* output parameter	 	: 
* return value			: 
* history 		 		: author		date			remarks
*			 		 zhangnw	 20060320     	created
*					 zhangnw	 20060602   	   delete 多余的int port参数,直接使用g_com
*****************************************************************/
#if !ANDIROD_ENABLE
void send_result(char *fmt, ...)
{
	/*private & local definition*/
	char tmp[512] = {0}, result[400] = {0};//zhangnw20060704 modify 256->512
	va_list argptr;
	char * p_result = result;

	/*process body*/
	va_start(argptr, fmt);
#if SPRINTF_UNSPORT
	if(t_print(&p_result, fmt, argptr)>sizeof(result)-1) 
	{
		cls_show_msg("%s, line %d:信息太长,任意键退出...", __FILE__, __LINE__);
		NDK_SysReboot();//nlexit(-1);
	}
#else
	if(vsprintf(p_result, fmt, argptr)>sizeof(result)-1) 
	{
		cls_show_msg("%s, line %d:信息太长,任意键退出...", __FILE__, __LINE__);
		NDK_SysReboot();//nlexit(-1);
	}
	va_end(argptr);
#endif 
	
	if (send_flag)
	{
		sprintf(tmp, "%12d%45c%20s\r\n", g_UCID, ' ', result);//格式可调整
#if 0//ANDIROD_ENABLE
		NDK_AZPortWrite(g_com, strlen(tmp), tmp);
#else
		NDK_PortWrite(g_com, strlen(tmp), tmp);//第二个para不能超过1000
#endif
	}
	else//不上送，后续可改为其它方式亦可，如打印机出结果
		;
	
	cls_show_msg1(2, "%d号用例:%s", g_UCID, result);
}
#endif
/****************************************************************
* function name 	 	: 
* functional description 	: 将一个被空格分隔的数字字符串转换为数组
* input parameter	 	: str: 带有数字的串;
*					  cnt: array整型数组的个数
* output parameter		:
* return value		 	: 实际转换的个数
* history 		 		: author		date		  		remarks
*				  	zhangnw 	 20060308     	 created
*				 	zhangnw 	 20060309      被get_line()处理过的串不会再出现\n，所以此处做相应修改
*					zhangnw 	 20060606   	增加代码,使得array中的数据不重复
*****************************************************************/
static int digitstr2array(char* str, unsigned char* array, int cnt)
{
	/*private & local definition*/
	unsigned int i = 0, j = 0;
	int found = FALSE;

	/*process body*/
	memset(array, 0, cnt);//memset(array,0,cnt*sizeof(int));
	while (!ISDIGIT(*str))
	{
		//调整到判断\0之后str++;
		if (*str=='\0'||str==NULL)
			return i;
		
		str++;
	}

	for (i = 0; i<cnt; i++)
	{
		for (j = 0 ; j<i ; j++)
		{
			if (array[j]==atoi(str))
			{
				found = TRUE;
				break;
			}
		}
		if (found==FALSE)
			array[i] = atoi(str);
		else
		{
			i--;
			found = FALSE;
		}

		while (ISDIGIT(*++str));
		
		if (*str=='\0')
			return ++i;
		
		while (!ISDIGIT(*str))
		{
			//调整到判断\0之后str++;
			if (*str=='\0')
				return ++i;
			
			str++;
		}
	}
	return i;
}

/****************************************************************
* function name 	 	: 
* functional description 	: 进行选择测试
* input parameter	 	: 全部忽略表示可以进行全范围的选择(此功能暂不实现);
*					  否则为只在sel_mod模块及funcpointer的函数表列中选择
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20060704 	 	created
*****************************************************************/
int select_test(unsigned char select_id[], int cnt)
{
	/*private & local definition*/
	int ret = 0;
	uint len = 0;
	char buf[512] = {0};

	/*process body*/
	memset(buf, 0, sizeof(buf));
#if 0//KB_DISABLE	//从键盘获取输入
	cls_printf("请输入用例号(用.分隔):\n");
	NDK_PortClrBuf(g_com);//clrportbuf(g_com, 1);//清接收缓冲
	NDK_PortRead(g_com, 100, buf, 6*1000, &len);//portread(g_com, 100, buf, 10);//10s->6s完成100个字符的输入
	NDK_PortWrite(g_com, len, buf);//将选择的用例回显到串口，能够明白输入了什么用例
#endif
//#else	使用封装函数lib_kbgetinput
	cls_printf("请输入用例号(用.分隔):\n");
	//if ((ret=getnumstr(buf, sizeof(buf)-1, NORMAL, 0))==QUIT||ret==TIMEOUT)//quit的话,那么select_id并没改变,?//zhangnw20110622注:getnumstr(0)的行为为两种:1.传统的永远等待;2.采用与PCI安全设计一致的达到最大超时时返回
	if ((ret=lib_kbgetinput(buf, 0, sizeof(buf)-1, &len, INPUTDISP_NORMAL, 0, /*INPUT_CONTRL_LIMIT*/INPUT_CONTRL_NOLIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		return 0;//return SUCC;/*FAIL*///zhangnw20060704 QUIT 也返回SUCC

	return digitstr2array(buf, select_id, cnt);//如果该接口返回0那么select_id并没改变,直接返回
	//return SUCC;
}

/****************************************************************
* function name 	 	: process
* functional description 	: 处理函数,可以给其它函数回调
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20060704 	 	created
*****************************************************************/
//注意:由于工具已将idX中重复的数据过滤,所以理论上,本接口不必再考虑这个问题了
//使用(u)char型作为id的类型,主要是在很多地方不用再传数组大小了(strlen可得),但这个也制约了系统模块用例数最多不可超过255
int process(PF* list, unsigned char id[])
{
	/*private & local definition*/
	int nKeyin = 0, i = 0, j = 0, cnt = 0;
	PF tmp = NULL;
	unsigned char select_id[MAXID] = {0};//其实,文档中说"编译器默认char型为是unsigned char",不过还是定义成unsigned
	int found = FALSE, sel_flag = FALSE;
	char msg[32] = {0};

	/*process body*/
	//注意:我司产品平台不能使用有关内存分配的库函数(malloc/calloc等不能使用),要事先分配
#if 0
	memset(select_id, 0, sizeof(select_id));
	memcpy(select_id, id, sizeof(id));
	cnt = sizeof(id)/sizeof(int);
#else
	cnt = strlen((char *)id);
	strcpy((char *)select_id, (char *)id);
#endif
restart:
	for (i = 0; select_id[i]!=0; i++)
	{
		for (j = 0; j<cnt; j++)
		{
			if (select_id[i]==id[j])//j对应那个用例的序号,而不是用例?
			{
				found = TRUE;
				nKeyin = lib_getkeycode(2);//nKeyin = getkeycode(2);
				switch(nKeyin)
				{
				case ESC:
					//退出整个模块的测试
					return FAIL;
					break;
				case '9':
					//这里可以增加快速选择测试
					while (1)
					{
						nKeyin = ShowMessageBox("——提示——\n进行快选测试吗?", BTN_OK|BTN_CANCEL, -1);
						if (nKeyin==BTN_OK)
						{
							sel_flag = TRUE;
							break;
						}
						else //if (nKeyin==BTN_CANCEL)
						{
							sel_flag = FALSE;
							break;
						}
					}

					if (sel_flag==TRUE)
					{
						if (select_test(select_id, MAXID)>=SUCC)
						{
							found = FALSE;//为下一轮寻找复位
							goto restart;
						}
						else
							return FAIL;
					}
					//往下执行tmp = list[j];//break;
				default: //比如,timeout返回0
					g_UCID = id[j];
					tmp = list[j];
					break;
				}

				tmp();
				//这样子不会将重复的用例滤掉//break;
			}
		}

		//没找到,出错返回
		if (found==FALSE)
		{
			sprintf(msg, "——提示——\n未找到%d号用例", select_id[i]);
			while (ShowMessageBox(msg, BTN_OK, -1)!=BTN_OK);
			//暂不直接返回,这样可以继续之后存在的id测试//return FAIL;
		}

		found = FALSE;//匹配标志复位
	}
	return SUCC;
}

/****************************************************************
* function name 	 	: 
* functional description 	: 快速翻页功能
* input parameter	 	: 
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20060704 	 	created
*****************************************************************/
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
int jump2page(int maxpage, int* curpage)
{
	/*private & local definition*/
	char page[4] = {0};
	int oldpage = *curpage, newpage = 0, cnt = 0, flag = TRUE, ret = 0;
	uint len = 0;

	/*process body*/
	while (1)
	{
		do
		{
			memset(page, 0, sizeof(page));
			if (!flag)
			{
				show_msg1(10, "\n输入不合理,请重输!\n");
				if (++cnt==3)
					return FAIL;
			}

#if 0//KB_DISABLE	//从键盘获取输入
			cls_printf("当前处在第%d页\n"
				"请输入菜单页(1---%d):\n", oldpage, maxpage);
			NDK_PortClrBuf(g_com);//清接收缓冲
			NDK_PortRead(g_com, 1, page, MAX_SIGNINT, &len);//NL829STD没有KB硬件,不允许死等	
#endif			
//#else  使用封装函数lib_kbgetinput
			cls_printf("当前处在第%d页\n"
				"请输入菜单页(1---%d):\n", oldpage, maxpage);
			if ((ret=lib_kbgetinput(page, 0, sizeof(page)-1, &len, INPUTDISP_NORMAL, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)	
				return  FAIL;
		}while (!(flag=is_digit_str(page)));

		newpage = atoi(page);
		if (newpage<1 || newpage>maxpage)
		{
			show_msg1(10, "\n输入页码越界!\n");
			flag = FALSE;
			continue;
		}
		else
		{
			*curpage = newpage;
			return SUCC;
		}
	}
}
#endif
/****************************************************************
* function name 	 	: 
* functional description 	: 自动测试
* input parameter	 	: 
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20060704 	 	created(从nlmain中提取)
*****************************************************************/
void auto_test(int* auto_flag, int* nKeyin, int* SelPage, int maxpage, const char* tcrmenu[], const signed char *skip_modid)
{
	/*private & local definition*/
	int loop = 0, modid = 0;

	/*process body*/
	//ASSERT(*auto_flag==FALSE || *auto_flag==TRUE);
	
LOOP:
	while (1)
	{
		if (*auto_flag==FALSE||*auto_flag==2)//常规测试和手动测试需要键盘输入
		{
			//显示菜单
			*nKeyin = cls_show_msg(tcrmenu[*SelPage-1]);//g_menu定义在menu1.h 中
			return;
		}
		else if (*auto_flag==TRUE)//自动测试时nKeyin的值 不由键盘提供
		{
			//step1:提示正在进行的测试项目（等待一些时间，比如允许退出自动测试）
			if (ESC==cls_show_msg1(2, "进行全自动测试中...\nESC中断全自动测试"))
			{
				*auto_flag = FALSE;
				continue;
			}

			//step2 :自动计算*nKeyin与*SelPage等
			++*nKeyin;
			if (*nKeyin=='9')
			{
				*nKeyin = '1';
				if (++*SelPage>maxpage)//作为自动测试的终止条件
				{
					//提示自动测试完毕
					while (1)
					{
						if (ShowMessageBox("——提示——\n自动测试完毕", BTN_OK, 5)==BTN_OK)//超时时间-1 修改成5秒
						{
							if(*auto_flag==1)//自动测试则重启
								NDK_SysReboot();
							
							*auto_flag = FALSE;//自动测试结束,标志复位
							break;
						}
					}

					*SelPage = 1;//复位
					continue;
				}
			}

			//zhangnw20061116 使用skip_modid来过滤不参与自动测试的测试项
			modid = (*SelPage-1)*MAXCOUNT+*nKeyin-'1';
			for (loop=0; skip_modid[loop]!=-1; loop++)//-1是skip_modID的结束标志
			{
				if (skip_modid[loop]==modid)
					goto LOOP;
			}

			return;
		}
	}
}

/*
 * Function:	   send_reporthead
 * Description:    初始化成功后向PC端发送测试报告头
 * Calls:   	   
 * Called By:      init()
 * Table Accessed: 
 * Table Updated:  
 * Input:   	   
 * Output:  	   
 * Return:  	   
 * Others:  	   zhangnw20060831 从init()中分离出来
 */
void send_reporthead(void)
{
	/*private & local definition*/
	time_t now = 0;
	struct tm *t;
	char tmp[384] = {0}, BoardVer[128] = {0}, BIOSVer[32] = {0}, machine[32] = {0};
	uint len = 0;
#if 0	//defined E90
	int adjust = 1940;
#else
	int adjust = 1900;
#endif

	/*process body*/
	if (send_flag)
	{
		time(&now);
		t = localtime(&now);

		if(NDK_OK!=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_TYPE, &len, machine))		
			strcpy(machine, "UNKNOWN");

		if(NDK_OK!=NDK_SysGetPosInfo(SYS_HWINFO_GET_BIOS_VER, &len, BIOSVer))		
			strcpy(BIOSVer, "UNKNOWN");
		
		if(NDK_OK!=NDK_SysGetPosInfo(SYS_HWINFO_GET_BOARD_VER, &len, BoardVer))		
			strcpy(BoardVer, "UNKNOWN");
		
#if 0	//zhangnw201101调整
		sprintf(tmp, REPORTHEAD, ctime(&now), SlaveCPUVer, BIOSVer, HandwareVer);
#else
		sprintf(tmp, REPORTHEAD, 
				t->tm_wday, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, t->tm_year+adjust, 
				machine, BIOSVer, BoardVer);
#endif
		while(NDK_PortTxSendOver(g_com)!=NDK_OK);//总有发完的时候//clrportbuf(g_com, 0);
#if 0//ANDIROD_ENABLE
		NDK_AZPortWrite(g_com, strlen(tmp), tmp);
#else
		NDK_PortWrite(g_com, strlen(tmp), tmp);
#endif
	}
	else
		;
}

#if !defined ME66 //ME66空间小不编译
#if PRN_ENABLE
int print_Ex(char *buf, ...)
{
	/*private & local definition*/
	int cnt = 0;
	va_list args;
	char prnbuf[MAXPRNBUFSIZE] = {0};
	char * p_prnbuf = prnbuf;
	/*process body*/
	memset(prnbuf, 0, sizeof(prnbuf));

	va_start(args, buf);
#if SPRINTF_UNSPORT
	cnt = t_print(&p_prnbuf, buf, args);
#else
	cnt = vsprintf(p_prnbuf, buf, args);
	va_end(args);
#endif 

	if (cnt>sizeof(prnbuf)-1)
	{
		cls_show_msg("%s, line %d: 待打印数据超长,任意键退出...", __FILE__, __LINE__);
		NDK_SysReboot();//nlexit(-1);
	}
	return NDK_PrnStr(prnbuf);//print(prnbuf);
}

int lib_getprinterlen(void)
{
	/*private & local definition*/
	uint unLen=0;
	char szBuf[128]={0};

	/*process body*/
	if(NDK_SysGetPosInfo(SYS_HWINFO_GET_PRN_LEN ,&unLen,szBuf)!=NDK_OK)
		return FAIL;
	return atoi(szBuf);
}

int lib_getprintertype(void)
{
	/*private & local definition*/
	uint unLen=0;
	char szBuf[128]={0};
	
	/*process body*/
	if(NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO,&unLen,szBuf)!=NDK_OK||szBuf[10]==0xFF)
		return FAIL;
	
	return (szBuf[10]>=0x81/*0x82*/)?PRN_TYPE_HIP:PRN_TYPE_TP;//针打不再区分穿孔还是摩擦,统一返回穿孔
}
#endif

//MAG
#if MAG_ENABLE
char __check_stripe_result_tk23(char *track, char *name)
{
	/*private & local definition*/

	/*process body*/
	if(track[0]==0x7f||track[0]==0x7e)
		return track[0];
	else
	{
		cls_show_msg1(2, "%s磁道数据(%d个):%s\n", name, strlen(track), track);//低端平台不支持NDK_ScrDispString(0, 0, track, 1)中的参数1，所以修改磁卡数据显示方式 20141023 linwl
		//cls_show_msg1(2, "%s磁道数据(%d个):", name, strlen(track));
//#if SCR_DISABLE
//		NDK_PortWrite(g_com, strlen(track), track);//portwrite(g_com, strlen(tips), tips);
//		NDK_PortWrite(g_com, strlen("\r\n"), "\r\n");//portwrite(g_com, strlen("\r\n"), "\r\n");
//#endif
		//NDK_ScrDispString(0, 0, track, 1);
		//NDK_ScrRefresh();
		//wait_anykey(30);
	}

	return 0x00;
}

char check_stripe_result_tk2(char *track2)
{
	/*private & local definition*/

	/*process body*/
	return __check_stripe_result_tk23(track2, "二");
}

char check_stripe_result_tk3(char *track3)
{
	/*private & local definition*/

	/*process body*/
	return __check_stripe_result_tk23(track3, "三");
}

char check_stripe_result_tk1(char *track1)
{
	/*private & local definition*/

	/*process body*/
	NDK_ScrInitGui();
	if(track1[0]==0x7f)
		return track1[0];
	else if(track1[0]==0x7e)
	{
		if(ENTER==cls_show_msg("读一磁道无数据.若真无数据或硬件不支持1磁道,请按确认,否则按其它键"))
			;
		else
			return track1[0];
	}
	else
	{
		cls_show_msg1(2, "一磁道数据(%d个):%s\n", strlen(track1), track1);
//		cls_show_msg1(2, "一磁道数据(%d个):", strlen(track1));
//#if SCR_DISABLE
//		NDK_PortWrite(g_com, strlen(track1), track1);//portwrite(g_com, strlen(tips), tips);
//		NDK_PortWrite(g_com, strlen("\r\n"), "\r\n");//portwrite(g_com, strlen("\r\n"), "\r\n");
//#endif
//		NDK_ScrDispString(0, 0, track1, 1);
//		NDK_ScrRefresh();
//		wait_anykey(30);
	}

	return 0x00;
}

#if !defined READCARD3_SUPPORT
//一个简易的readcard3函数
//使用nl_mag函数族实现一个与readcard3功能相当的接口,仅支持刷磁卡
//关于返回值的设计:当多个API有可能返回相同的错误返回值而无法区分是何处出错时,可以增加出错提示或将出错处的__LINE__作为返回值
int readcard3(/*int type, */int tracks, /*int keylen, */int waittime, char *track2, char *track3 , char *track1, int *err)
{
	/*private & local definition*/
	int nRet = FAIL, ret1 = FAIL/*, ret2 = FAIL, ret3 = FAIL*/;
	time_t oldtime = 0, diff = 0;
	uchar Swiped = 0;

	/*process body*/
	//type,keylen参数不做判断
#if defined ME15CHX ||defined ME15B|| ANDIROD_ENABLE
	NDK_MagClose();//增加个关闭动作,避免重复打开返回错误
#endif
	if((ret1=NDK_MagOpen())!=NDK_OK)
		return ret1;//FAIL;
	
#if !(defined ME15CHX||defined ME30MH||defined ME15B||defined ME32||defined N920)
	oldtime = time(NULL);
	NDK_KbFlush();
	do
	{
		if(lib_kbhit()==ESC)//if(kbhit()==ESC)
		{
			nRet = NDK_ERR_QUIT;//QUIT;
			break;
		}
			
		if(NDK_MagSwiped(&Swiped)==NDK_OK&&Swiped==TRUE)//轮询直到有磁道数据(读卡结束)
		{
			if((ret1=NDK_MagReadNormal(tracks&TK1?track1:NULL, tracks&TK2?track2:NULL, tracks&TK3?track3:NULL, err))!=NDK_OK)	
				nRet = ret1;//FAIL;
			else
				nRet = STRIPE;
			
			NDK_MagReset();//根据叶明统提供的流程在读卡之后要复位磁头且清除磁卡缓冲区数据 20140304 linwl
			break;
		}
	}while(waittime==0||(diff=time(NULL)-oldtime)<waittime);
	
	if(waittime!=0 && diff>=waittime)//超时
		nRet = NDK_ERR_TIMEOUT;//TIMEOUT;
#else
	while(1)
	{
		NDK_MagSwiped(&Swiped);
		if(Swiped==TRUE)//轮询直到有磁道数据(读卡结束)
			break;
	}
	if((ret1=NDK_MagReadNormal(tracks&TK1?track1:NULL, tracks&TK2?track2:NULL, tracks&TK3?track3:NULL, err))!=NDK_OK)
		nRet = ret1;//FAIL;
	else
		nRet = STRIPE;
#endif
	
	if((ret1=NDK_MagClose())!=NDK_OK)//与accept成对使用
		return ret1;//FAIL;

	return nRet;
}
#endif
#endif
#endif

void lib_StartStopwatch(void)
{

#if K21_ENABLE
	uint a = 0;
	NDK_SysStopWatch(&a);
#endif	

	NDK_SysStartWatch();
	return;
}

float lib_ReadStopwatch(void)
{
	/*private & local definition*/
	uint time = 0;
	
	/*process body*/
#if !K21_ENABLE
	NDK_SysStopWatch(&time);
#else
	NDK_SysReadWatch(&time);
#endif
	return time/1000.0;
}
#if defined ME30GX||defined ME32GX
int INT_ReadStopwatch(void)
{
	/*private & local definition*/
	uint time = 0;
	
	/*process body*/
	NDK_SysReadWatch(&time);
	return time/1000;
}
#endif

//无键盘产品使用键盘输入的重新使用串口上送到控制台，
//对于有键盘的还是使用原先的键盘接口函数  20141031 jiangym add  将用此函数替代原先的NDK_KbGetInput
#if !ANDIROD_ENABLE
int lib_kbgetinput(char * pszBuf,uint unMin,uint unMaxLen,uint * punLen,EM_INPUTDISP emMode,uint unWaittime, EM_INPUT_CONTRL emControl)
{
	int ret = -1;
#if KB_DISABLE
	int len = 0, i = 0; //消除警告
	char tempbuf[100] = {0};
	strcpy(tempbuf,pszBuf);
	cls_printf("%s",pszBuf);
	memset(pszBuf,0,strlen(pszBuf));
	NDK_PortClrBuf(g_com);//清接收缓冲
	
	//while(NDK_PortRead(g_com, unMaxLen, pszBuf, 3*1000, &len)==-10)//此处NDK.SO封装有问题unMaxLen 超时未读到unMaxLen长度的数据返回-10而不是和中低端一样返回实际读到的长度,已经修改,20150819linwl
	//	;
#if 0//ANDIROD_ENABLE
	NDK_AZPortRead(g_com, unMaxLen, pszBuf, 5*1000, &len);
#else
	NDK_PortRead(g_com, unMaxLen, pszBuf, 5*1000, &len);
#endif
	if(len==0)
	{
		strcpy(pszBuf,tempbuf);//如果没有输入使用默认的,对应超时的情况
		ret = NDK_ERR;
	}
	else if(len==1)
	{
		if(pszBuf[0]==0x0d||pszBuf[0]==0x1b)//这里功能键只支持ESC和ENTER，其它功能键不做处理(会设置成空)
		{
			strcpy(pszBuf,tempbuf);//如果只有一个字节且为功能键，则使用默认值
			ret = NDK_ERR;
		}
		ret = NDK_OK;
	}
	else
		ret = NDK_OK;

	for(i = 1;i<len;i++)//未避免PC键盘的功能键被读入后当成有效字符而进行去功能键处理,例如在输入蓝牙名111称后按了回车键,蓝牙名称的BtName就变成 111\r 再用%s显示时就会出现异常
	{
		if(pszBuf[i]==0x0d||pszBuf[i]==0x0a||pszBuf[i]==0x1b)
			pszBuf[i] = 0;
	}
	//cls_printf("0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x",pszBuf[0],pszBuf[1],pszBuf[2],pszBuf[3],pszBuf[4],pszBuf[5],pszBuf[6]);
	
	cls_printf("%s",pszBuf);
	*punLen = len;
#else
	ret =NDK_KbGetInput(pszBuf,unMin,unMaxLen,punLen,emMode,unWaittime,emControl);
#endif
	return ret;
}
#endif

void ftos(float f, char *str)
{ 
	sprintf(str,"%d.%06d", (int)f, (int)((f-(int)f)*1000000));
	return;
}

#if !defined ME66 //ME66不支持蓝牙 空间小不编译
#if BT_ENABLE
int lib_bt_pairing(uint unaccept)
{
	int flag = 0, ret = -1, link_status = 1;
	uint ulen = 0;
	char pkey[7] = {0};
	time_t diff = 0, oldtime = time(NULL);
	char *pairmodestr[]={"Just Work","PIN Code","SSP PIN","PassKey","Jusk Work SC"};

	//cls_show_msg1(1,"设置为%s模式",pairmodestr[g_pair_mode]);
	/*process body*/
	if(g_pair_mode == 2 || g_pair_mode == 3)//	2,3两种模式进入等待配对连接，一般配对成功就默认连接成功了
	{
		memset(pkey,0,sizeof(pkey));
		while(1)
		{
			if(cls_show_msg1(1,"%s模式,等待手机蓝牙工具发起配对连接中,取消键退出", pairmodestr[g_pair_mode])==ESC)
				return NDK_ERR_QUIT;
			if((ret=NDK_BTStatus(&link_status))!=NDK_OK)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line %d:获取连接状态失败(%d)", __LINE__,ret);
				return ret;
			}
			if(!link_status)
			{
				cls_printf("已连接成功");//两三秒延时等连接稳定后才作其他操作:例如数据收发,挂断,复位等
				sleep(3);
				NDK_BTGetPairingStatus(pkey,&flag);//清配对标志
				return NDK_OK;
			}
			if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line %d:获取配对状态失败(%d)", __LINE__,ret);
				return ret;
			}
			switch(flag)
			{
				case 0:
					if((diff=time(NULL)-oldtime)>60)
					{
						cls_show_msg_record(FILENAME,FUNCNAME,"line %d:长时间未检测到配对请求(%d)", __LINE__, flag);
						return NDK_ERR_TIMEOUT;
					}
					continue;
					break;
				case 1:
					if(pkey[0] == '\0')
					{
						cls_printf("输入手机端显示的配对码:\n");
						lib_kbgetinput(pkey, 0, 6, &ulen, INPUTDISP_PASSWD, 60, INPUT_CONTRL_LIMIT_ERETURN);
					}
					NDK_BTConfirmPairing(pkey, unaccept);
					break;
				case 2:
					cls_printf("连接成功");
					sleep(2);//两三秒延时等连接稳定后才作其他操作:例如数据收发,挂断,复位等
					return NDK_OK;
					break;
				case 3:
					cls_show_msg1(2,"配对失败,任意键继续");
					return NDK_ERR;
					break;
				default:
					cls_show_msg_record(FILENAME,FUNCNAME,"line %d:检测到未知的配对状态(%d)", __LINE__,flag);
					return NDK_ERR;
					break;
			}
		}
	}
	else//	0,1两种模式无需配对就进入等待连接4just work sc模式也走此分支
	{
		while(1)
		{
			if(cls_show_msg1(1,"%s模式,等待手机蓝牙工具发起配对连接中,取消键退出", pairmodestr[g_pair_mode])==ESC)
				return NDK_ERR_QUIT;
			if((ret=NDK_BTStatus(&link_status))!=NDK_OK)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line %d:获取连接状态失败(%d)", __LINE__,ret);
				return ret;
			}
			if(!link_status)
			{
				cls_printf("连接成功");//两三秒延时等连接稳定后才作其他操作:例如数据收发,挂断,复位等
				sleep(3);
				return NDK_OK;
			}
			if((diff=time(NULL)-oldtime)>60)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line %d:长时间未检测到连接成功状态(%d)", __LINE__, link_status);
				return NDK_ERR_TIMEOUT;
			}
		}
	}
	
	return ret;
}

//修改BT包格式前面两个字节表示长度高字节在前,长度为BCD格式且包含长度本身以及校验码,最后一个是sumcheck校验码。
void update_BT_packet(char *buf, int len)
{
	/*private & local definition*/
	int i = 0;
	
	/*process body*/
	NDK_ByteToBcd(len/100,(uchar*)buf);
	NDK_ByteToBcd(len%100,(uchar*)buf+1);
	buf[len-1]=0;
	
	for(i=0;i<len-1;i++)
		buf[len-1]+=buf[i];
	
	return;
}

//响应报文格式(POS端发送到手机APK端的数据包格式)
//STX(1个字节固定0x02)+指令数据长度(2个字节BCD格式:从指令号开始到ETX前的总长度)+指令号(2个字节固定0xff 0x0c)+指示位(1个字节)+序列号(1个字节)+响应(2个字节)+设置操作(1个字节)+data长度(2个字节)+数据data(datalen个字节)+结束位ETX(1个字节)+LRC校验位(1个字节)
//设置操作 0x01对应通知手机设置扫描时间间隔t1值，单位s，BCD码（配置，ID2数据是时间间隔 T1，占 2字节）
//设置操作 0x02通知手机设置扫描设备到连接设备的时间间隔 t2 值，单位 s，BCD码（配置，ID2数据是时间间隔 T2，占 2字节）
//设置操作 0x03通知手机断开当前连接（动作）
//设置操作 0x04通知手机停止测试（动作）
//设置操作 0x05数据回传（需回传数据）
//设置操作 0x06响应确认报文，通知手机响应收到（收到回传后确认收到）
//设置操作 0x07收到手机数据，回传数据（ID 2 为回传数据）
int bt_cmd_packet(int cmdid, char *data, int datalen, char* cmd)
{
#if 0
	int i = 0;
#endif
	if(cmdid==1||cmdid==2)//设置延时时间时输入数据只能为2个字节的时间延时
	{
		if(data==NULL||datalen!=2)
			return BTCMD_SETTIME_ERR;
	}
	if(cmd==NULL)
		return BTCMD_PARA_ERR;
	if(cmdid<1||cmdid>7)
		return BTCMD_ID_ERR;
	if(datalen<0)
		return BTCMD_DATALEN_ERR;

	memset(cmd,0,sizeof(cmd));
	//STX
	cmd[0] = 0x02;
	//报文数据长度
	NDK_ByteToBcd((datalen+9)/100,(uchar*)cmd+1);
	NDK_ByteToBcd((datalen+9)%100,(uchar*)cmd+2);
	//指令号
	cmd[3] = 0xff;
	cmd[4] = 0x0c;
	//指示位
	if(cmdid==6||cmdid==7)
		cmd[5] = 0x2f;
	else
		cmd[5] = 0x5f;
	//序列号
	if(packetnum==255)
		packetnum = 1;
	else
		packetnum = packetnum+1;
	cmd[6] = packetnum;//序列号
	//响应码
	cmd[7] = 0x30;
	cmd[8] = 0x30;
	//设置操作命令号
	cmd[9] = cmdid;
	//测试命令数据长度
	NDK_ByteToBcd((datalen)/100,(uchar*)cmd+10);
	NDK_ByteToBcd((datalen)%100,(uchar*)cmd+11);
	//数据内容
	memcpy(cmd+12,data,datalen);
	//结束符ETX
	cmd[datalen+12] = 0x03;
	//LRC
	NDK_CalcLRC((uchar*)cmd+1,datalen+12,(uchar*)cmd+datalen+13);

#if BTMFI_ENABLE//如果使用旧的蓝牙工具，需要再对数据做处理，海外版本测试需要用到
	update_BT_packet(cmd, datalen+14);
#endif

#if 0
	fprintf(stderr,"\n0x%2x命令:",cmdid);
	for(i=0;i<datalen+MIN_BTCMD_LEN;i++)
		fprintf(stderr,"%02X ",cmd[i]);
	fprintf(stderr,"\n");
#endif	
	return BTCMD_OK;
}

int bt_cmd_uppacket(char* cmd, int datalen, char *data)
{
#if 0
	int i = 0;
	fprintf(stderr,"接收到数据:");
	for(i=0;i<datalen+MIN_BTCMD_LEN;i++)
		fprintf(stderr,"%02X ",cmd[i]);
	fprintf(stderr,"\n");
#endif	

	if(data==NULL||cmd==NULL)
		return BTCMD_PARA_ERR;
	//if(cmd[6]!=packetnum+1)
	//	return BTCMD_SN_ERR;

	packetnum = cmd[6];//更新序列号
	memset(data,0,sizeof(data));
	memcpy(data,cmd+12,datalen);//读取实际测试数据
	
	return BTCMD_OK;
}

//packlen:数据长度 randflag:随机数标志
int bt_comm_once(int packlen, int randflag)
{
	int ret = 0, cmdlen=0, recvLen=0, j = 0;
	char sendbuf[BUFSIZE_BT-MIN_BTCMD_LEN]={0}, recbuf[BUFSIZE_BT-MIN_BTCMD_LEN]={0};
	char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};

	if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)//清接收缓冲
	{
		cls_show_msg1(3,"line %d:清缓冲区失败(%d)", __LINE__, ret);
		return NDK_ERR;
	}

	if(randflag == 1)
	{
		for (j=0; j<sizeof(sendbuf); j++)
			sendbuf[j]=rand()%256;
	}
	else
		memset(sendbuf, 0x38, sizeof(sendbuf));
	
	//发送数据回传命令
#if BTMFI_ENABLE
	cls_show_msg("手机端允许连接后初始化并连接pos后任意键继续");//由于MFI回环测试工具需要手动连接所以操作提示有差异
#else
	cls_show_msg1(3,"3秒后进行数据包发送接收和比较");
#endif
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x05, sendbuf, packlen, cmdsendBuf);
	cmdlen=packlen+MIN_BTCMD_LEN;//数据包更新后报文长度=数据长度+最小报文长度
	if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg1(3,"line %d:发送失败(%d)", __LINE__, ret);
		return NDK_ERR;
	}
	memset(cmdrecvBuf, 0, sizeof(cmdrecvBuf));
	if(((ret=NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, 10*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
	{
		cls_show_msg1(3,"line %d:接收错(ret=%d)(实际%d, 预期%d)", __LINE__, ret, recvLen, cmdlen);
		return NDK_ERR;
	}
	bt_cmd_uppacket(cmdrecvBuf, packlen, recbuf);
	if(memcmp(sendbuf, recbuf, packlen))
	{
		cls_show_msg1(3,"line %d:数据校验失败", __LINE__);
		return NDK_ERR;
	}
	//发送响应确认报文
	memset(cmdsendBuf, 0, sizeof(cmdsendBuf));
	bt_cmd_packet(0x06, NULL, 0, cmdsendBuf);
	if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdsendBuf))!=NDK_OK)
	{
		cls_show_msg1(3,"line %d:发送失败(%d)", __LINE__, ret);
		return NDK_ERR;
	}
	sleep(1);//增加延时确保手机能够收到06指令,再做其它操作,例如挂断

	return NDK_OK;
}
#else
int lib_bt_pairing(uint unaccept) {return NDK_ERR;}
void update_BT_packet(char *buf, int len){return;}
int bt_cmd_packet(int cmdid, char *data, int datalen, char* cmd) {return NDK_ERR;}
int bt_cmd_uppacket(char* cmd, int datalen, char *data) {return NDK_ERR;}
int bt_comm_once(int packlen, int randflag) {return NDK_ERR;}
#endif

#if !(defined ME15CHX||defined ME15B)//ME15C为节省代码空间进行条件编译 20170213 linwl
#if BT_MASTER_ENABLE
//蓝牙主模式扫描和选择要连接或配对的设备,通过参数返回测试设备的MAC地址
//输入参数dectype为连接设备的方式默认为0 默认方式，相应的扫描使用双模仿是扫描，1位单模SPP方式 2为单模BLE方式
int lib_btmaster_scan(char *pszMac, int dectype)
{
	int ret = 0,snum=0,i=0, sacntype=BT_DEVICE_TYPE_DUMO;
	ST_BT_DEV sresult[50];

	switch(dectype) 
	{
		case 0:
			sacntype = BT_DEVICE_TYPE_DUMO; /**<双模*/
			break;
		case 1:	
			sacntype = BT_DEVICE_TYPE_BREDR; /**<单模:SPP*/
			break;	
		case 2:
			sacntype = BT_DEVICE_TYPE_BLE; /**<单模:BLE*/
			break;
		default:
			sacntype = BT_DEVICE_TYPE_DUMO; /**<双模*/
			break;
	}
	
	//if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	if((ret=NDK_BTMasterScan(sacntype))!=NDK_OK)
	{
		cls_show_msg1(5, "%s,line %d:主设备扫描失败(%d)", __FILE__, __LINE__, ret);
		return ret;	
	}
	
	memset(sresult,0,sizeof(sresult));
	while(1)
	{
		if(cls_show_msg1(1,"正在扫描蓝牙从模式设备，ESC键退出")==ESC)
			break;
		if((ret=NDK_BTMasterGetScanResults(NULL,sresult,15,&snum))!=NDK_OK)
		{
			cls_show_msg("%s,line %d:主设备获取扫描结果失败(%d)", __FILE__, __LINE__, ret);
			NDK_BTMasterStopScan();
			return ret;
		}
		if (snum > 0)
		{
			for (i=0; i<snum; i++)
			{
				if(cls_show_msg("是否选择BT[%d]:%s MAC为:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x作为测试从设备,是按[确认],否则按其它", i, sresult[i].sBtName,sresult[i].sBtMac[0],sresult[i].sBtMac[1],sresult[i].sBtMac[2],sresult[i].sBtMac[3],sresult[i].sBtMac[4],sresult[i].sBtMac[5])==ENTER)
				{
					memcpy(pszMac,sresult[i].sBtMac,6);
					NDK_BTMasterStopScan();
					return NDK_OK;
				}
			}
		}
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		cls_show_msg("line %d:主设备停止扫描失败(%d)", __LINE__, ret);
		NDK_BTMasterStopScan();
		return ret;
	}
	cls_show_msg1(2, "line %d:未扫描到测试用的蓝牙从设备", __LINE__);
	return NDK_ERR_QUIT;
}

//蓝牙主模式连接配对函数:若获取已连接就返回若未连接则进行配对,配对成功即连接成功返回
int connect_pair_process(char *mac,int accept)
{
	int ret = -1, status = -1, constatus = -1;
	char skey[7]={0},pincode[7]={0};
	EM_PAIRING_MODE mode=0;
	time_t oldtime=0;
	uint unLen = 0;
	
	oldtime=time(NULL);
	while(1)
	{
		//获取连接状态,成功直接返回
		if((ret=NDK_BTMasterGetConStatus(mac,&constatus))!=NDK_OK)
		{
			cls_show_msg1(2,"%s,line %d:获取连接状态失败%d", __FILE__, __LINE__, ret);
			return ret;
		}
		if(constatus==0)
		{
			cls_show_msg1(2,"主设备与从设备已成功连接");
			return NDK_OK;
		}
		//状态是未连接则进行配对连接
		if((ret=NDK_BTMasterGetBondStatus(&mode,skey,&status))!=NDK_OK)
		{
			cls_show_msg1(2,"%s,line %d:获取配对状态失败%d", __FILE__, __LINE__, ret);
			return ret;
		}
		switch(status)
		{
			case 0:
				if(time(NULL)-oldtime>30)//30秒配对
				{
					cls_show_msg1(2,"%s,line %d:配对状态长时间为0,异常退出", __FILE__, __LINE__);
					return NDK_ERR_TIMEOUT;
				}
				continue;
				break;
			case 1:
				if(mode==1)
				{
					cls_printf("PinCode模式:请输入从设备的PIN码:");
					if ((ret=lib_kbgetinput(pincode,0, sizeof(pincode)-1, &unLen,INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
						accept = 0;
					if((ret=NDK_BTMasterBondConfirm(pincode,accept))!=NDK_OK)
					{	
						cls_show_msg1(2,"%s,line %d:确认配对函数失败(%d)", __FILE__, __LINE__, ret);
						return ret;;
					}
				}
				else if(mode==2)
				{
					if(cls_show_msg("SPP模式:[Enter]确认配对,[其他]取消配对")!=ENTER)
						accept = 0;
					if((ret=NDK_BTMasterBondConfirm(skey,accept))!=NDK_OK)
					{	
						cls_show_msg1(2,"%s,line %d:确认配对函数失败(%d)", __FILE__, __LINE__, ret);
						return ret;;
					}
				}
				else if(mode==3)
				{
					cls_show_msg("PASSKEY模式:请在从设备上输入key:%s后任意键继续",skey);
				}
				continue;
				break;
			case 2:
				cls_show_msg1(2,"配对成功");//配对成功后底层会继续连接操作，break后继续获取连接状态
				break;
			case 3:
				cls_show_msg1(2,"配对失败");
				return NDK_ERR;
				break; 
			default:
				cls_show_msg1(2,"%s,line %d:获取到未知状态(%d)", __FILE__, __LINE__, status);
				return NDK_ERR;
				break;
		}
		if(time(NULL)-oldtime>60)//配对后30秒连接
		{
			cls_show_msg1(2,"%s,line %d:连接超时,异常退出", __FILE__, __LINE__);
			return NDK_ERR_TIMEOUT;
		}
		NDK_SysMsDelay(100);//延时100ms
	}
	return NDK_OK;
}

//蓝牙主模式主动配对函数:参数accept=1表示确认配对，accept=0表示取消配对
//返回值为配对状态或函数调用状态
int lib_btmaster_pairing(int accept)
{
	int ret = -1, status = 0;
	char skey[7]={0},pincode[7]={0};
	EM_PAIRING_MODE mode=0;
	time_t oldtime=0;
	uint unLen = 0;
	
	oldtime=time(NULL);
	while(1)
	{
		if(cls_show_msg1(1,"获取配对状态中...")==ESC)
			break;
		if((ret=NDK_BTMasterGetBondStatus(&mode,skey,&status))!=NDK_OK)
		{
			return ret;
		}
		switch(status)
		{
			case 0:
				cls_printf("配对状态为0");
				if(time(NULL)-oldtime>60)
				{
					cls_show_msg1(2,"长时间未发出配对请求");
					return NDK_ERR_TIMEOUT;
				}
				continue;
				break;
			case 1:
				if(mode==1)
				{
					cls_printf("请输入从设备的PIN码:");
					if ((ret=lib_kbgetinput(pincode,0, sizeof(pincode)-1, &unLen,INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
						accept = 0;
					if((ret=NDK_BTMasterBondConfirm(pincode,accept))!=NDK_OK)
					{	
						return ret;
					}
				}
				else if(mode==2)
				{
					if(cls_show_msg("[Enter]确认配对,[其他]取消配对")!=ENTER)
						accept = 0;
					if((ret=NDK_BTMasterBondConfirm(skey,accept))!=NDK_OK)
					{	
						return ret;
					}
				}
				else if(mode==3)
				{
					cls_show_msg("请在从设备上输入key:%s后任意键继续",skey);
				}
				continue;
				break;
			case 2:
				cls_show_msg1(2,"配对成功");
				return status;
			case 3:
				cls_show_msg1(2,"配对失败");
				return status;
			default:
				cls_show_msg1(2,"获取到未知状态(%d)", status);
				return status;
				break;
		}
	}
	return NDK_OK;
}

//本函数用于获取主模式连接状态，
//pszMac表示要获取的链路的MAC，
//pnStatus表示要获取的状态1为获取未连接状态，0为获取连接状态
//返回值为函数调用结果,超时或者取消
int lib_btmaster_getconstatus(char *pszMac, int pnStatus)
{
	int ret = 0, c_status = 1;
	time_t oldtime = 0;
	oldtime=time(NULL);
	while(1)
	{
		if(cls_show_msg1(1,"获取连接状态中(%d),ESC键退出",c_status)==ESC)
			return NDK_ERR_QUIT;
		if((ret=NDK_BTMasterGetConStatus(pszMac,&c_status))!=NDK_OK)
		{
			cls_show_msg1(10,"%s,line %d:获取连接状态失败(%d)", __FILE__, __LINE__, ret);
			return ret;
		}
		if(c_status==pnStatus)
			return ret;
		if(time(NULL)-oldtime>30)
		{
			cls_show_msg1(10,"%s,line %d:长时间未连接成功", __FILE__, __LINE__); 
			return NDK_ERR_TIMEOUT;
		}
	}
}
#else
int lib_btmaster_scan(char *pszMac, int dectype){return NDK_ERR;}
int connect_pair_process(char *mac,int accept){return NDK_ERR;}
int lib_btmaster_pairing(int accept){return NDK_ERR;}
int lib_btmaster_getconstatus(char *pszMac, int pnStatus){return NDK_ERR;}
#endif
#endif

#if ANDIROD_ENABLE && !defined IM81AZ//defined N900AZ||defined N910AZ||defined N700 //需要从触摸屏上获取PIN输入的设备需要该函数初始化触屏键盘  //defined IM81||defined IM81AZ||defined N900||
int touchscrean_getnum(void)//获取在触屏机上输入的pin值
{
	int ret=0, i=0, j=0;
	uchar num[80]={0}, fun[36]={0}, numserial[11]={0};
#if 1
	int x0 = 0, x1 = 135, x2 = 270, x3 = 405, x4 = 540, y0 = 462, y1 = 568, y2 = 674, y3 = 780, y4 = 886;
#elif defined N910AZ||defined N700
	int x0 = 0, x1 = 180, x2 = 240, x3 = 360,x4 = 480,x5 = 540, x6 = 720, y0 = 0, y1 = 320, y2 = 640, y3 = 960, y4 = 1280;
#else//后续需要调整键盘坐标的在此处增加
	;
#endif
//N900分辨率保持不变不从文件去获取而是直接写死的  N910的需要去读取文件获取分辨率如果是高分辨率就改成不一样的坐标
#if 1//defined N910AZ||defined N700 //全都从文件里读取
	int fp = -1, readlen = 0;
	char touch_hr[] = {"1280x720"};//高分辨率是1280x720
	char touch_hr_850[] = {"1024x600"};//850的分辨率是 1024x600 
	char readbuf[20] = {0};
	if((fp=open("/sys/class/touchscreen/resolution", O_RDONLY))<0)
	{
		cls_show_msg1(2,"line %d: 打开文件失败(%d)", __LINE__, fp);
		return FAIL;
	}
	memset(readbuf, 0, sizeof(readbuf));
	read(fp,readbuf, sizeof(readbuf));
	//读取分辨率获取到高分辨率需转换坐标如果是低分辨率不做改变
	if(!memcmp(touch_hr,readbuf,4))//使用前四个数比较才行  //910和700一样
	{
#if defined N920
		x0 = 0; x1 = 180; x2 = 360; x3 = 540;x4 = 720;
		y0 =614; y1 = 755; y2 = 896; y3 = 1037; y4 = 1178;
#else
		x0 = 0; x1 = 180; x2 = 360; x3 = 540;x4 = 720;
		y0 = 665; y1 = 818; y2 = 972; y3 = 1126; y4 = 1280;
#endif
	}
	//增加N850的分辨率
	else if(!memcmp(touch_hr_850,readbuf,4))  //850
	{
		x0 = 0; x1 = 150; x2 = 300; x3 = 450;x4 = 600;
		y0 = 495; y1 = 609; y2 = 723; y3 = 837; y4 = 952;
	}
	else   //900
	{
		x0 = 0; x1 = 135; x2 = 270; x3 = 405;x4 = 540;
		y0 = 462;y1 = 568; y2 = 674; y3 = 780; y4 = 886;
	}
	close(fp);
#endif	
	int numInt[40] = { 
			x0, y0, x1, y1, x1, y0, x2, y1, x2, y0, x3, y1,
	
			x0, y1, x1, y2, x1, y1, x2, y2, x2, y1, x3, y2,
	
			x0, y2, x1, y3, x1, y2, x2, y3, x2, y2, x3, y3,
	
			x1, y3, x2, y4};
	int funInt[18] = { 
			0x1B, 0, x3, y0, x4, y1,
	
			0x0A, 0, x3, y1, x4, y2,
	
			0x0D, 0, x3, y2, x4, y4};
			
	for (i = 0, j = 0; i < 40; i++, j++) 
	{
		num[j] = (uchar) (numInt[i] & 0xff);
		j++;
		num[j] = (uchar) ((numInt[i] >> 8) & 0xff);
	}
	for (i = 0, j = 0; i < 18; i++, j++) 
	{
		fun[j] = (uchar) (funInt[i] & 0xff);
		j++;
		fun[j] = (uchar) ((funInt[i] >> 8) & 0xff);
	}
	memset(numserial, 0, sizeof(numserial));
	ret=NDK_SecVppTpInit(num, fun, numserial);
	if(ret==NDK_OK)
	{
		cls_printf("此时触屏机上随机数字键盘的顺序为\n%c,%c,%c \n%c,%c,%c \n%c,%c,%c \n  %c\n", numserial[0], numserial[1], numserial[2], numserial[3], numserial[4], numserial[5], numserial[6], numserial[7], numserial[8], numserial[9]);
	}
	return ret;
}
#else
int touchscrean_getnum(void){return NDK_OK;}
#endif
#endif

//不支持标准c库格式化输出函数的平台可以使用以下应用层实现的现实和输出函数
#if SPRINTF_UNSPORT
void t_printchar(char **str, int c)
{
	//putchar(c);
	
	if (str) {
		if(c == '\n') {
			**str = '\r';
			++(*str);
			**str = c;
			++(*str);
		}
		else { 
			**str = c;
			++(*str);
		}
		
	}
	else
		putchar(c);//(void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

int t_prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			t_printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		t_printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		t_printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

int t_printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return t_prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			t_printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + t_prints (out, s, width, pad);
}

int t_print( char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];
	for (; *format != 0; ++format) {		
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += t_prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += t_printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += t_printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}	
			if( *format == 'X' ) {
				pc += t_printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += t_printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += t_prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			t_printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

int t_printf(const char *format, ...)
{
        va_list args;
        
        va_start( args, format );
        return t_print( 0, format, args );
}

int t_sprintf(char *out, const char *format, ...)
{
        va_list args;
        
        va_start( args, format );
        return t_print( &out, format, args );
}

int t_snprintf( char *buf, unsigned int count, const char *format, ... )
{
        va_list args;
        
        ( void ) count;
        
        va_start( args, format );
        return t_print( &buf, format, args );
}
#endif

#if !defined ME66 //ME66 空间小不编译
#if NEW_WIFI_ENABLE
//获取Ap的连接状态函数
int lib_getwifistatus(ST_WIFI_STATUS *pstStatus)
{
	/*private & local definition*/
	int ret;
	time_t oldtime;
	ST_WIFI_STATUS status;

	/*process body*/
	oldtime=time(NULL);
	while(1)
	{
		if(time(NULL)-oldtime>60)
		{	
			cls_show_msg1(10,"%s,line %d:未获取到结束状态,超时退出(%d)",__FILE__, __LINE__,status.status);//WIFI_WLAN_STATUS_INIT,WIFI_WLAN_STATUS_LINKING为中间状态,如果长时间未获取到稳定的结束状态也视为一个异常
			return NDK_ERR;
		}
		memset(status.ssid, 0, sizeof(status.ssid));
		if((ret = NDK_WifiStatus(&status)) != NDK_OK)
		{
			cls_show_msg1(10,"%s,line %d:wifi获取状态失败(%d)",__FILE__, __LINE__,ret);
			return NDK_ERR;
		}
		if(status.status == WIFI_WLAN_STATUS_LINKING)
			cls_show_msg1(1,"正在连接SSID:%s中",status.ssid);
		if(status.status == WIFI_WLAN_STATUS_SUCC || status.status == WIFI_WLAN_STATUS_FAIL || status.status == WIFI_WLAN_STATUS_EMPTY|| status.status == WIFI_WLAN_STATUS_AUTHENTICATION_REJECTED)
			break;
		else
			NDK_SysMsDelay(10);
	}
	memcpy(pstStatus->ssid,status.ssid,sizeof(status.ssid));
	pstStatus->status=status.status;
	return NDK_OK;
}
#endif

int GetWifiStopflag(void)
{
	/*private & local definition*/
	
	/*process body*/
	return g_wifistopflag;
}

int SetWifiStopflag(int stopflag)
{
	/*private & local definition*/
	
	/*process body*/
	g_wifistopflag = stopflag;
	return g_wifistopflag;
}

#if NEW_WIFI_ENABLE||defined SP610
int lib_tcp_checkstatus(uint unFd, int maxtime)
{
	int ret=0;
	int pnStatus;
	time_t oldtime;
	oldtime=time(NULL);
	while(1)
	{
		if( (ret=NDK_TcpCheck(unFd , &pnStatus, NULL)) !=NDK_OK )
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:获取TCP状态失败(%d)", __LINE__,ret);
			return NDK_ERR;
		}
		switch(pnStatus)
		{
		case SOCKET_STATUS_CONNECTERR:
			cls_show_msg1(1,"line %d:获取到连接出错SOCKET_STATUS_CONNECTERR", __LINE__);
			//return SOCKET_STATUS_CONNECTERR;
			break;
		case SOCKET_STATUS_CONNECTING:
			cls_show_msg1(1,"正在连接服务器中...");
			break;	
		case SOCKET_STATUS_DISCONNECT:
			return SOCKET_STATUS_DISCONNECT;
			break;
		case SOCKET_STATUS_CONNECT:
			//cls_printf("连接成功");
			return SOCKET_STATUS_CONNECT;
			break;
		case SOCKET_STATUS_READY_TO_SEND:
			cls_show_msg1(1,"正在发送数据中...");
			break;
		case SOCKET_STATUS_SEND_OVER:
			return SOCKET_STATUS_SEND_OVER;
			break;
		default:
			//cls_show_msg_record(FILENAME,FUNCNAME,"line %d:检测到未知的状态(%d)", __LINE__, pnStatus);
			return pnStatus;
			break;
		}
		if(time(NULL)-oldtime>maxtime)
		{	
			//cls_show_msg_record(FILENAME,FUNCNAME,"line %d:长时间未检测到连接成功状态(%d)", __LINE__, pnStatus);
			return NDK_ERR_TCP_TIMEOUT;
		}
	}
}
#endif
#endif

#if CAMERA_ENABLE||SCAN_ENABLE||SCANASYCN_ENABLE
//asycn为1时,表示异步扫描;asycn为0时,表示同步扫描
int lib_doscan(int asycn, int maxwaittime)
{
	int ret=0;
	uint plen=0;
	uchar pvalue[500]={0};
#if CPU5810X_ENABLE||defined ME68||defined ME50NX
	uint poutlen=0;
	uchar poutvalue[500]={0};
#endif
#if defined ME66
	uint a = 0, a1 = 0;
#endif

	if(asycn!=0 && asycn!=1)
	{
		cls_show_msg1(10,"%s,line %d:写入参数出错(%d)", __FILE__, __LINE__, asycn);
		return NDK_ERR;
	}
	
	plen=0;
	memset(pvalue,0,sizeof(pvalue));
#if CPU5810X_ENABLE||defined ME68||defined ME50NX
	memset(poutvalue,0,sizeof(poutvalue));
#endif
	cls_show_msg1(2, "请准备好待扫描的图形,2秒后开始扫描.");
#if SCANASYCN_ENABLE
	if(asycn==1)
	{
		if((ret=NDK_ScanStart()) != NDK_OK )
		{
			NDK_ScanStop();
			cls_show_msg1(10,"%s,line %d:摄像头开始扫描失败(%d)", __FILE__, __LINE__, ret);
			return ret;
		}
#if defined ME66
		NDK_SysReadWatch(&a1);
#else
		lib_StartStopwatch();
#endif
		while(1)
		{	
#if defined ME66
			NDK_SysReadWatch(&a);
			if(((a-a1)/(1000*1.0))>maxwaittime)
#else			
			if(lib_ReadStopwatch()>maxwaittime)
#endif
			{
				NDK_ScanStop();
				cls_show_msg1(10,"%s,line %d:扫描超时", __FILE__, __LINE__);
				return NDK_ERR_TIMEOUT;
			}
			switch(ret=NDK_ScanState(pvalue, &plen))
			{
				case NDK_OK:
					NDK_ScanStop();
#if CPU5810X_ENABLE||defined ME68||defined ME50NX
					if((ret=NDK_Utf8ToGbk(pvalue, plen, poutvalue, &poutlen))!=NDK_OK)
					{
						cls_show_msg1(10,"%s,line %d:UTF8转GBK失败", __FILE__, __LINE__);
						return NDK_ERR;
					}
					if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", poutlen, poutvalue)!=ENTER)
#else
					if(cls_show_msg("解码成功!扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
#endif
					{
						cls_show_msg1(10,"%s,line %d:扫描内容与预期不符", __FILE__, __LINE__);
						return NDK_ERR;
					}
					return NDK_OK;
					break;
#if defined ME68||defined ME50NX||defined ME66
				case NDK_ERR:
#elif CPU5810X_ENABLE
				case NDK_ERR_READ:
#else
				case NDK_ERR_CAMERA_SCANNING:
#endif				
					continue;
					break;
				default:
					NDK_ScanStop();
					cls_show_msg1(10,"%s,line %d:扫描失败:获取到异常状态(%d)", __FILE__, __LINE__, ret);
					return ret;
					break;
			}
		}
	}
#endif	
	if(asycn==0)
	{
		if((ret=NDK_ScanDoScan(maxwaittime, pvalue, &plen))!=NDK_OK)
		{
			cls_show_msg1(10,"%s,line %d:阻塞式扫描失败(%d)", __FILE__, __LINE__, ret);
			return ret;
		}
#if CPU5810X_ENABLE||defined ME50NX||defined ME68
		if((ret=NDK_Utf8ToGbk(pvalue, plen, poutvalue, &poutlen))!=NDK_OK)
		{
			cls_show_msg1(10,"%s,line %d:UTF8转GBK失败", __FILE__, __LINE__);
			return NDK_ERR;
		}
		if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", poutlen, poutvalue)!=ENTER)
#else
		if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
#endif
		{
			cls_show_msg1(10,"%s,line %d:扫描内容与预期不符", __FILE__, __LINE__);
			return NDK_ERR;
		}
		return NDK_OK;
	}
}
#else
int lib_doscan(int asycn, int maxwaittime){return NDK_ERR_NOT_SUPPORT;}
#endif

#if  ANDIROD_ENABLE
//封装dlsym,使其在出错时能够有出错提示信息 20171027
#define  DLSYM(funcp, libhd, func) {  \
	char *dlError;\
	funcp = dlsym( libhd , #func); \
	dlError = (char *)dlerror(); \
	if(dlError || NULL == funcp){\
		__android_log_print(ANDROID_LOG_INFO, "libmain.a", "lib.c:dload_library,dlsym fail,%s,return %d !", dlError, (int)funcp);\
	}\
}
int  dload_library(void)
{
	void *handle;
	//改成直接打开库文件，不再使用绝对路径,n9003G机型和4G机型绝对路径不同 20171010 modify
	handle = dlopen ("libjnindk.so", RTLD_LAZY); 
	if (!handle) {
		__android_log_print(ANDROID_LOG_INFO, "libmain.a", "dlopen fail,return %d", (int)handle);
		return -2;
	}
//改成使用封装函数,使得在找不到对应函数时能够有出错提示20171027	
	DLSYM(cls_show_msg, handle, cls_show_msg_android);
	DLSYM(cls_show_msg1, handle, cls_show_msg1_android);
	DLSYM(ShowMessageBox, handle, ShowMessageBox_android);
	DLSYM(cls_printf, handle, cls_printf_android);
	DLSYM(lib_kbgetinput, handle, lib_kbgetinput_android);
	DLSYM(lib_getkeycode, handle,lib_getkeycode_android);
	DLSYM(send_result, handle,send_result_android);
	DLSYM(NDK_ScrPrintf, handle,NDK_ScrPrintf_android);
	DLSYM(NDK_ScrRefresh, handle,NDK_ScrRefresh_android);
	DLSYM(NDK_ScrClrs, handle,NDK_ScrClrs_android);
	DLSYM(cls_show_msg_record, handle,cls_show_msg_record_android);
	DLSYM(cls_show_msg1_record, handle,cls_show_msg1_record_android);
	dlerror();
	dlclose(handle);
	return 0;
}
#endif

