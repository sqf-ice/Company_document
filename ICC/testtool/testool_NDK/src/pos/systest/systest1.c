/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 打印综合测试
* file name		: systest1.c
* Author 			: chensj
* version			: V1.0
* DATE			: 20130204
* directory 		: 
* description		: 打印综合测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20130204 chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"打印性能,压力"
#define	MAXPAGE	(3)

#define	MAXWAITTIME	(180)	//30->180 for 针打
#define	MAXHOTTIME	(60*30)	//30min
#define	MINHOTTIME		(60*7)

#define	DEFAULT_MagePon_VLE	(0)
#define	DEFAULT_MagePon_STR	"0"
#define	DEFAULT_PW_VLE	(384)
#define	DEFAULT_PW_STR	"384"

//#define	FILL_XSIZE	(384)
//#define	FILL_YSIZE	(128)

/*------------global variables definition-----------------------*/
static int general1_slct_BMP = 1;
static int general1_MagePon = DEFAULT_MagePon_VLE;
static char general1_keybuf[12] = DEFAULT_MagePon_STR;
static int general1_nPageWidth = DEFAULT_PW_VLE;//200->384
static char general1_cReadBuf[20] = DEFAULT_PW_STR;

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static void general1_print_triangle(void)
{
	/*private & local definition*/

	/*process body*/
	print_triangle(general1_nPageWidth);
}

static void general1_print_image(void)
{
	/*private & local definition*/

	/*process body*/
	if (general1_slct_BMP==1)
		print_image(128, 64, general1_MagePon, CCB_BMP);
	else
		print_image(144, 64, general1_MagePon, CCB_BMP_144);
}

//主要测算打印速度
static void prn_ability(void)
{
	/*private & local definition*/
	float stp = 0.0;
#if K21_ENABLE
	char str[32] = {0};
#endif
#define	TRIANGLE_ID	(0)
#define	IMAGE_ID		(9)
#define	ENTER_ID		(10)
	uint nLen=0;
	PF pfunc[] = {general1_print_triangle, print_testpage, print_rand, print_guo, print_fill, print_bill, 
				print_verticalbill, print_blank, print_font, general1_print_image, print_enter, print_compress, print_landi, print_xinguodu, print_signlfont};
	char *func_info[] = {"三角块", "测试页", "随机数", "国字页", "填充单", "票据", "竖条纹", 
					"空白单", "模式字体组合", "图形打印", "回车", "压缩", "联迪单据", "新国都", "单独字体"},
	*menu[] = {"1.三角块2.测试页\n3.随机数4.打印国\n5.填充单6.票据  \n7.竖条纹8.翻页\n", 
				"1.空白单2.模式字\n3.图形打印4.回车\n5.压缩6.联迪单据\n7.新国都8.翻页\n",
				"1.单字体8.翻页\n"};
	int  line[] = {0, 55, 0, 21, 0, 41, 0,
				0, 0, 0, 0, 0, 0, 0, 0};//需要计算打印速度的,将打印行数填入,否则填0//行数换算:16点行为1行，小于16点行的为1行，大于16点行的除以16得到行数
	int casekey = 0,  page = 1, ret = 0, type = lib_getprintertype();
	
	/*process body*/
	general1_nPageWidth = (PRN_TYPE_TP==type)?MAXPRLINEWIDE_TP:((PRN_TYPE_HIP ==type)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);
	sprintf(general1_cReadBuf, "%d", general1_nPageWidth);
	//为了避免其它打印用例的影响，先清一下打印BUF并同时复位一下
	/*if (FAIL==NDK_PrnInit(g_PrnSwitch))*/
	if(FAIL==lib_initprn(g_PrnSwitch))//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		cls_show_msg("line %d: 打印机复位失败", __LINE__);
		return;
	}
	
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//避免其它用例的干扰
	{
		cls_show_msg("line %d:打印机状态异常!", __LINE__);
		return;                         
	}
	
	while(1)
	{
		NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE,1);//恢复打印模式
    		casekey = cls_show_msg(menu[page-1]);
		switch(casekey)
		{
		case ESC:
			return;
			break;
		case '8'://翻页处理
			if(page<MAXPAGE)
				page++;
			else if(page==MAXPAGE)
				page = 1;//复位
	            	continue;
	            	break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':	
			casekey = casekey-'1'+7*(page-1);//转换页 
			break;
		default://'9',ENTER等
			continue;
			break;
		}

		cls_show_msg1(1, "即将进行%s打印...\n请注意打印效果\n", func_info[casekey]);
		if(casekey==IMAGE_ID)//图形打印参数设置,特殊处理一下
		{
			if(show_msg1(2, "ESC键变更参数\n")==ESC)
			{
				cls_printf("默认图形起点:\n");
				if ((ret=lib_kbgetinput(general1_keybuf,0, sizeof(general1_keybuf)-1, &nLen,INPUTDISP_OTHER, 0,INPUT_CONTRL_LIMIT))==ESC||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
					general1_MagePon = DEFAULT_MagePon_VLE;
				else
					general1_MagePon = atoi(general1_keybuf);

				if(cls_show_msg("图形选择\n1.128\n2.144")=='1')
					general1_slct_BMP = 1;
				else
					general1_slct_BMP = 2;
			}
		}
		if(casekey==TRIANGLE_ID)//三角打印参数设置,特殊处理一下
		{
			if(show_msg1(2, "ESC键变更参数\n")==ESC)
			{
				cls_printf("默认页宽:\n");
				if ((ret=lib_kbgetinput(general1_cReadBuf, 0,sizeof(general1_cReadBuf)-1,  &nLen,INPUTDISP_OTHER, 0,INPUT_CONTRL_LIMIT))==ESC||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
					general1_nPageWidth = (PRN_TYPE_TP==type)?MAXPRLINEWIDE_TP:((PRN_TYPE_HIP==type)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);//general1_nPageWidth = DEFAULT_PW_VLE;
				else
					general1_nPageWidth = atoi(general1_cReadBuf);
			}
		}
		if(casekey==ENTER_ID)//三角打印参数设置,特殊处理一下
		{
			if(show_msg1(2, "ESC键变更参数\n")==ESC)
			{
				if ('2' == cls_show_msg("选择纵横向放大模式(默认双向):1.双向,2.单向\n"))
					NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE ,0);
				else
					NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE ,1);
			}
		}		
		lib_StartStopwatch();
		pfunc[casekey]();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg("line %d:打印机状态异常!", __LINE__);
			return;                         
		}
		stp = lib_ReadStopwatch();
#if K21_ENABLE
		memset(str,0,sizeof(str));
		ftos(stp,str);
		cls_show_msg("%s打印执行时间:%ss\n", func_info[casekey], str);
		memset(str,0,sizeof(str));
		ftos(line[casekey]/stp,str);
		if(line[casekey])
			show_msg("打印速度:%s行/s\n", str);
#else
		cls_show_msg("%s打印执行时间:%fs\n", func_info[casekey], stp);
		if(line[casekey])
			show_msg("打印速度:%f行/s\n", line[casekey]/stp);
#endif
	}
	
	//test over
	return;
}

//打印填充单
static void print_fill1(void)
{
	/*private & local definition*/
	char printbuf[(MAXPRLINEWIDE_FIP*128)/8] = {0};//缓冲最大情况下需要424*128/8的大小
	//char printbuf[(FILL_XSIZE*FILL_YSIZE)/8] = {0};
	int type = lib_getprintertype();
	int xsize = 0;

	/*process body*/
	memset(printbuf,0xff,sizeof(printbuf));
	xsize = (PRN_TYPE_TP==type)?MAXPRLINEWIDE_TP:((PRN_TYPE_HIP ==type)?MAXPRLINEWIDE_HIP:MAXPRLINEWIDE_FIP);
	if(NDK_PrnImage(xsize,128,0,printbuf)!=NDK_OK)
	//if(NDK_PrnImage(FILL_XSIZE,FILL_YSIZE,0,printbuf)!=NDK_OK)//原代码FILL_XSIZE在针打情况下有问题，根据打印机类项设置填充单的宽度值 20131122 linwl
	{
		cls_show_msg("line %d:打印填充单失败!\n", __LINE__);
		//return;//避免过热但还没取到状态时 这个函数就先报错，引起测试失败 20140124
	}

	NDK_PrnStr("\n");
	NDK_PrnStart();
	return;
}

static void prn_hot(void)
{
	/*private & local definition*/
	int status = 0, cnt = 0, i = 0;
	time_t diff = 0, oldtime = time(NULL);
	
	/*process body*/
	while(1)
	{
		cls_printf("过热测试中...");
		if((diff=time(NULL)-oldtime)>MAXHOTTIME)
		{
			cls_show_msg("line %d:测试失败!长时间未检测到过热态", __LINE__);
			return;
		}

		status = GetPrinterStatus(MAXWAITTIME);
		if(status==SUCC)
			;
		else if(status==PRN_STATUS_OVERHEAT)
		{
			//if(i < 67)//根据规范将连续打印标准由7分钟改成6米 20140113 chensj
			//if(i < 44)//20160623 建新和建卫提出新打印驱动将打印长度由至少打印6米降低到至少打印4米,可能原因是过热的报警温度降低到了70度
			//if(diff<MINHOTTIME)
			//{
			//	cls_show_msg("line %d:测试失败!短时间内不应检测到过热态,第%d轮", __LINE__,i);
			//	return;
			//}
			//else
			//{
				cls_show_msg("测试成功!已检测到过热态(历时%lds)", diff);//根据20170426，不再关注“短时间内持续打印不应过早过易报过热”的测试点；屏蔽4米内不能过热的判断语句，只关注半个小时连续打印能报过热即可
				return;
			//}
		}
		else
		{
			cls_show_msg("line %d:打印机状态异常!(status=%d)", __LINE__, status);
			return;                         
		}
		
		for(cnt=0; cnt<5; cnt++)   /* 已测试打印5个黑块长度9厘米,标准不低于6米,则要打印至少67次*/
			print_fill1();
		i++;
		if(lib_kbhit()==ESC)
			return;
	}		
}

static void prn_nopower(void)
{
	/*private & local definition*/
	int status = 0;
	time_t diff = 0, oldtime = time(NULL);
	
	/*process body*/
	while(1)
	{
		if((diff=time(NULL)-oldtime)>MAXHOTTIME)
		{
			cls_show_msg("line %d:测试失败!长时间未检测到电源异常", __LINE__);
			return;
		}

		status = GetPrinterStatus(MAXWAITTIME);
		if(status==SUCC)
			;
		else if(status==PRN_STATUS_VOLERR)
		{
			cls_show_msg("测试成功!已检测到电源异常(历时%lds)", diff);
			return;
		}
		else
		{
			cls_show_msg("line %d:打印机状态异常!(status=%d)", __LINE__, status);
			return;                         
		}
		print_bill();
		if(lib_kbhit()==ESC)
			return;
	}	
}


static void prn_Invalid(void)
{
	/*private & local definition*/
	EM_PRN_STATUS emStatus=0;

	/*process body*/
	//以下采用缺纸后重装的方案来模拟可能出现的失步
	cls_show_msg("请先确保打印机处于缺纸态,任意键继续...");
	NDK_PrnStart();
	if(NDK_PrnGetStatus(&emStatus)!=NDK_OK||emStatus!=PRN_STATUS_NOPAPER )
	{
		cls_show_msg("line %d:未检测到缺纸态!(status=%d)", __LINE__, emStatus);
		return;
	}
	cls_show_msg("请装干净的打印纸,任意键继续...");
	if(NDK_PrnGetStatus(&emStatus)!=NDK_OK||emStatus!=PRN_STATUS_OK )
	{
		cls_show_msg("line %d:未检测到IPOK态!(status=%d)", __LINE__, emStatus);
		return;
	}
	NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE, 1);
	if(rand()%2)
		print_guo();
	else
		print_font();//general1_print_triangle();
	cls_show_msg("请认真观察打印效果,以确定是否存在打印失步");
}

static void __prn_attrib_unknown(void)
{
	/*private & local definition*/
	int type = lib_getprintertype();

	/*process body*/
	NDK_PrnImage(128, 64, 0, CCB_BMP);
	NDK_PrnImage(128, 64, 0, NL_BMP);
	NDK_PrnStr("NewLand新大陆00045\n00045新大陆NewLand\n新大陆NewLand00045\n");
	NDK_PrnImage(128, 64, type==PRN_TYPE_TP?56:4, CCB_BMP);
	NDK_PrnImage(128, 64, type==PRN_TYPE_TP?56:4, NL_BMP);
	NDK_PrnStr("NewLand新大陆00045\n00045新大陆NewLand\n新大陆NewLand00045\n");
	NDK_PrnStr("\f");
	NDK_PrnStart();
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)
	{
		cls_show_msg("line %d:打印机状态异常!", __LINE__);
		return;
	}
	return;
}

static void __prn_attrib_exit(void)
{
	/*private & local definition*/

	/*process body*/
	NDK_PrnSetFont(PRN_HZ_FONT_24x24, PRN_ZM_FONT_16x16);
	NDK_PrnSetMode(PRN_MODE_ALL_DOUBLE, 1);
	NDK_PrnSetForm(8,32,16);//值不可设太大,值+图形xpos+xsize不要超过非压缩模式的最大行宽
	
	NDK_PrnStr("---设置打印属性---\n"
				"左边界: 8\n"
				"字间距: 32\n"
				"行间距: 16\n"
				"汉字字体: 24x24\n"
				"字母字体: 16x16\n"
				"压缩模式: 纵向放大横向放大双向走纸\n");
	__prn_attrib_unknown();
	NDK_PrnSetMode(PRN_MODE_NORMAL,0);//mode改变,但左边界、(行[列])间距等值应不受影响
	NDK_PrnSetForm(8,32,16);//因为setprintmode第二个参数的范围是0-7,所以要设16,必须使用新接口
	NDK_PrnStr("---设置打印属性---\n"
				"左边界: 8\n"
				"字间距: 32\n"
				"行间距: 16\n"
				"汉字字体: 24x24\n"
				"字母字体: 16x16\n"
				"压缩模式: 纵向压缩横向压缩单项走纸\n");
	__prn_attrib_unknown();
	return;
}

static void prn_attrib_exit(void)
{
	/*private & local definition*/

	/*process body*/
	__prn_attrib_exit();
	cls_show_msg("手工退出应用,再进行\"未知属性打印\",打印效果应与\"默认属性打印\"一样");//cls_show_msg("请手工退出测试应用程序,再进行\"未知属性打印\",打印效果应不变");
	return;
}

static void prn_attrib_reboot(void)
{
	/*private & local definition*/

	/*process body*/
	__prn_attrib_exit();
	cls_show_msg("任意键重启,再进行\"未知属性打印\",打印效果应与\"默认属性打印\"一样");
#if !K21_ENABLE
	NDK_SysExit(0);
#endif
	return;
}

static void prn_attrib_unknown(void)
{
	/*private & local definition*/

	/*process body*/
	NDK_PrnStr("---未知属性打印---\n");
	__prn_attrib_unknown();
	return;
}

static void prn_attrib_default(void)
{
	/*private & local definition*/
	//int type=lib_getprintertype();

	/*process body*/
	/*
	左边界：0
	字间距：0
	行间距：0
	默认汉字字体：16x16（6）
	默认字母字体：8x16（6）//8x8
	默认模式：PRN_MODE_NORMAL 横向正常、纵向正常
	*/
	NDK_PrnSetFont(PRN_HZ_FONT_16x16, PRN_ZM_FONT_8x16);
	
	NDK_PrnSetMode(PRN_MODE_NORMAL,1);//setprintmode(type==PRINTTYPE_TP?7:0, 0);
	NDK_PrnSetForm(0,0,0);
	print_Ex("---恢复默认打印属性---\n"
		"左边界: 0\n"
		"字间距: 0\n"
		"行间距: 0\n"
		"汉字字体: 16x16\n"
		"字母字体: 8x16\n"
		"压缩模式: %s\n", /*type==PRN_TYPE_TP?"纵向正常横向正常单向走纸":*/"纵向放大横向放大双向走纸");
	__prn_attrib_unknown();
	return;
}

//测试步骤:进行1或2后退出应用,此时再进应用执行3和4
static void prn_attrib(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	
	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg("1.修改后退出应用\n"
								"2.修改后重启\n"
								"3.未知属性打印\n"
								"4.默认属性打印");

		switch (nKeyin)
		{
		case '1':
			prn_attrib_exit();
			break;
		case '2':
			prn_attrib_reboot();
			break;
		case '3':
			prn_attrib_unknown();
			break;			
		case '4':
			prn_attrib_default();
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

static void prn_buf_overload(void)
{
	/*private & local definition*/
	int i = 0;
	char PrintBuf[1000/*1024*/] = {0};

	/*process body*/
	memset(PrintBuf, 'H', sizeof(PrintBuf));
	for(; i<50; i++)//不理会打印状态(不管是否正在打印),不断往打印缓冲扔数据,不送\f
	{
		if(NDK_PrnStr(PrintBuf)!=SUCC)
		{
			cls_show_msg("测试成功!已检测到打印缓冲满!(%d)", i);
			return;
		}
	}

	NDK_PrnStr("\f");
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)
	{
		cls_show_msg("line %d:打印机状态异常!", __LINE__);
		return;
	}
	cls_show_msg("line %d:未检测到打印缓冲满!请确认是否丢数据(应有5w个H)", __LINE__);
}

static void prn_abnormity(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	
	/*process body*/
	while (1)
	{
#if 0 //会影响新增的"打印属性"(会复位模式),推迟到1-3中去做
		//为了避免其它打印用例的影响，先清一下打印BUF并同时复位一下
		if (FAIL==clrprintbuf())
		{
			cls_show_msg("line %d: 打印机复位失败", __LINE__);
			return;
		}
#endif
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//避免其它用例的干扰
		{
			cls_show_msg("line %d:打印机状态异常!", __LINE__);
			return;                         
		}

		nKeyin = cls_show_msg("1.过热2.电源异常\n"
								"3.失步4.打印属性\n"
#if !K21_ENABLE //K21平台仅支持边送边打
		"5.打印缓冲满"
#endif
		);

		switch (nKeyin)
		{
		case '1':
		//	if (FAIL==NDK_PrnInit(g_PrnSwitch))//为了避免其它打印用例的影响，先清一下打印BUF并同时复位一下
			if(FAIL==lib_initprn(g_PrnSwitch))//modify 20131122 jiangym
			{
				cls_show_msg("line %d: 打印机复位失败", __LINE__);
				return;
			}
			prn_hot();
			break;
		case '2':
			if(ENTER==cls_show_msg("请确保本机已装配了低电量电池,[ENTER]继续,[其它]放弃"))
			{
				//if (FAIL==NDK_PrnInit(g_PrnSwitch))//为了避免其它打印用例的影响，先清一下打印BUF并同时复位一下
				if(FAIL==lib_initprn(g_PrnSwitch))//modify 20131122 jiangym
				{
					cls_show_msg("line %d: 打印机复位失败", __LINE__);
					return;
				}
				prn_nopower();
			}
			break;
		case '3':
			//if (FAIL==NDK_PrnInit(g_PrnSwitch))//为了避免其它打印用例的影响，先清一下打印BUF并同时复位一下
			if(FAIL==lib_initprn(g_PrnSwitch))//modify 20131122 jiangym
			{
				cls_show_msg("line %d: 打印机复位失败", __LINE__);
				return;
			}			
			prn_Invalid();//用于测试重新装纸后是否出现打印失步
			break;		
		case '4':
			prn_attrib();
			break;
#if !K21_ENABLE
		case '5':
			if(ENTER==cls_show_msg("请确保本机已装配了循环打印纸,[ENTER]继续,[其它]放弃"))
			{
				//if (FAIL==NDK_PrnInit(g_PrnSwitch))//为了避免其它打印用例的影响，先清一下打印BUF并同时复位一下
				if(FAIL==lib_initprn(0))//打印缓冲区测试要求强制关闭边送边打
				{
					cls_show_msg("line %d: 打印机复位失败", __LINE__);
					return;
				}
				prn_buf_overload();
				if(FAIL==lib_initprn(g_PrnSwitch))//测试完后恢复默认的边送边打模式
				{
					cls_show_msg("line %d: 打印机复位失败", __LINE__);
					return;
				}
			}
			break;
#endif
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

int test_print_whitebill(void)
{
	/*private & local definition*/
	int succ = 0, cnt = 1000;
	
	/*process body*/
	if(lib_initprn(g_PrnSwitch)!=NDK_OK)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机复位失败", __FILE__, __LINE__);
		return FAIL;
	}
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:打印机状态异常!", __LINE__);
		return FAIL;                       
	}

	cls_show_msg("请准备好百分之70左右的电量电池(不能接外电!)进行测试。因测试次数较大，建议采用循环打印纸测试。注意：测试过程未出现\"打印空走纸\"或\"卡顿\",则为测试通过;空走纸或卡顿出现后需要重启设备");
	while(cnt)
	{
		//保护
		cls_show_msg1(1,"测试打印出现空白单问题中...\n还剩%d次(已完成%d次)", cnt, succ);
		
		print_whitebill();
		cnt--;
		succ++;
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"打印出现空白单问题测试完成,未检测到空走纸");

}
/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		  chensj     	   20130204     	created
*					
*****************************************************************/
void systest1(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	if(auto_flag==1)
	{
		//自动化测试内容
		//打印配置
		conf_prn();
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉测试
		prn_press("打印压力", NULL);
			
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	
	while(1)
       {
              keyin = cls_show_msg("1.压力测试\n"
							"2.单项及性能\n"
							"3.异常测试\n"
							"4.配置\n"
							"6.打印空白问题");
		switch(keyin) 
		{
		case '1':
			cls_show_msg1(1, "打印压力测试中...\n");
			prn_press("打印压力", NULL);
			break;
		case '2':
			cls_show_msg1(1, "打印性能测试中...\n");
			prn_ability();
			break;
		case '3':
			cls_show_msg1(1, "打印异常测试中...\n");
			prn_abnormity();
			break;
		case '4':
			conf_prn();
			break;
		case '6':
			test_print_whitebill();
			break;
		case ESC:
			cls_show_msg("请换好的打印重新测试(注意打印效果)!任意键继续...");
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}
