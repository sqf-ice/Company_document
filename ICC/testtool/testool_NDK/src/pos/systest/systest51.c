/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: LCD屏幕测试
* file name			: systest51.c
* Author 			: 	chensj
* version			: 
* DATE				: 
* directory 			: 
* description			:  LCD屏幕测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20131202  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"LCD屏幕测试"
#if 1//defined SP610
#define	PNGPATH	"" //统一路径放在当前main所在路径下,打包时候可一起打包 20170915
#else
#define	PNGPATH	"/appfs/apps/share/"
#endif
#if defined SP610  //Sp610图片格式是.bin故在尾部做区别
#define	IMAGEFORMAT	".bin"
#else
#define	IMAGEFORMAT	".png"
#endif

/*------------global variables definition-----------------------*/
static int  DispTime = 1;
static int RunNum = 100;
static uint unWidth,unHeight;

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
					 chensj 		2013/12/02            created
*****************************************************************/
//配置显示每张图片的时间,0为死等,按键翻页
void Scr_Config(void)
{
	char szBuf[3]={"1"},szNumBuf[4]={"100"};
	uint unLen=0;
	int ret = 0;
	int  keyin = 0;

	while(1)
	   {
		keyin = cls_show_msg("1.图片显示时间\n"
							  "2.压力次数");
		switch(keyin) 
		{
		case '1':
			cls_printf("请输入显示每张图片的时间:");
			if((ret = lib_kbgetinput(szBuf, 1, 2, &unLen, INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			{	
				DispTime = 2;
				return;
			}
			DispTime = atoi(szBuf);
			break;
		case '2':
			cls_printf("请输入压力次数:");
			if((ret = lib_kbgetinput(szNumBuf, 1, 3, &unLen, INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			{	
				RunNum = 100;
				return;
			}
			RunNum = atoi(szNumBuf);
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

//坏点测试
void Dead_Pixel(void)
{
	//char *szPng[]={"huidu_0.png","huidu_100.png","red.png","green.png","blue.png","pink.png","cyan.png","yellow.png"};
	char *szPng[]={"huidu_0","huidu100","red","green","blue","pink","cyan","yellow"};//Sp610图片格式是.bin故在尾部做区别
	char szBuf[64]={0};
	int i=0,ret = 0,nCode=0,RealRunNum = 0;

	RealRunNum = RunNum;
	NDK_ScrClrs();
	while(RealRunNum--)
	{
		while(1)
		{
			if(i==SZ_ARRAY(szPng))
			{
				i=0;
				break;
			}
			memset(szBuf,0,sizeof(szBuf));
			sprintf(szBuf,"%s%s%s",PNGPATH,szPng[i],IMAGEFORMAT);
			if((ret = NDK_ScrDispPic(0,0,unWidth,unHeight,szBuf))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败%d", TESTITEM,__LINE__,ret);
				return;
			}
			NDK_ScrRefresh();
			if(NDK_KbGetCode(DispTime, &nCode)==NDK_OK&&nCode==ESC)
				goto END;
			i++;
		}
	}
END:
	if(cls_show_msg("在上面纯色图片切换过程中是否发现坏点,[ENTER] 是 [其他] 否")==ENTER)
	{
		cls_show_msg1(2, "%s,line %d测试失败,发现屏幕坏点", TESTITEM,__LINE__);
		return;
	}
	cls_show_msg("测试通过,未发现坏点");
	return;
}

//亮点测试
void Light_Pixel(void)
{
	//char *szPng[]={"huidu_0.png","red.png","huidu_0.png","green.png","huidu_0.png","blue.png"};
	char *szPng[]={"huidu_0","red","huidu_0","green","huidu_0","blue"};
	char szBuf[64]={0};
	int i=0,ret =0,nCode=0,RealRunNum = 0;

	RealRunNum = RunNum;
	NDK_ScrClrs();
	while(RealRunNum--)
	{
		while(1)
		{
			if(i==SZ_ARRAY(szPng))
			{
				i = 0;
				break;
			}
			memset(szBuf,0,sizeof(szBuf));
			sprintf(szBuf,"%s%s%s",PNGPATH,szPng[i],IMAGEFORMAT);
			if((ret = NDK_ScrDispPic(0,0,unWidth,unHeight,szBuf))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败%d", TESTITEM,__LINE__,ret);
				return;
			}
			NDK_ScrRefresh();
			if(NDK_KbGetCode(DispTime, &nCode)==NDK_OK&&nCode==ESC)
				goto END;
			i++;
		}
	}
END:
	if(cls_show_msg("在上面黑红黑绿黑蓝图片切换过程中是否发现其比较亮的点,[ENTER] 是 [其他] 否")==ENTER)
	{
		cls_show_msg1(2, "%s,line %d测试失败,发现屏幕亮点", TESTITEM,__LINE__);
		return;
	}
	cls_show_msg("测试通过,未发现亮点");
	return;
}

//暗点测试
void Dark_Pixel(void)
{
	//char *szPng[]={"huidu_100.png","red.png","huidu_100.png","green.png","huidu_100.png","blue.png"};
	char *szPng[]={"huidu100","red","huidu100","green","huidu100","blue"};
	char szBuf[64]={0};
	int i=0,ret = 0,nCode=0,RealRunNum = 0;

	RealRunNum = RunNum;
	NDK_ScrClrs();
	while(RealRunNum--)
	{
		while(1)
		{
			if(i==SZ_ARRAY(szPng))
			{
				i=0;
				break;
			}
			memset(szBuf,0,sizeof(szBuf));
			sprintf(szBuf,"%s%s%s",PNGPATH,szPng[i],IMAGEFORMAT);
			if((ret =NDK_ScrDispPic(0,0,unWidth,unHeight,szBuf))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败%d", TESTITEM,__LINE__,ret);
				return;
			}
			NDK_ScrRefresh();
			if(NDK_KbGetCode(DispTime, &nCode)==NDK_OK&&nCode==ESC)
				goto END;
			i++;
		}
	}
END:
	if(cls_show_msg("在上面白红白绿白蓝图片切换过程中是否发现发现比较暗的点,[ENTER] 是 [其他] 否")==ENTER)
	{
		cls_show_msg1(2, "%s,line %d测试失败,发现屏幕暗点", TESTITEM,__LINE__);
		return;
	}
	cls_show_msg("测试通过,未发现暗点");
	return;
}

//屏幕像素点测试:测试坏点，暗点，亮点
void Screen_Pixel(void)
{
	int  keyin = 0;

	/*if(cls_show_msg("请确认在%s目录下有huidu_0.png huidu_100.png red.png green.png\
		blue.png pink.png cyan.png yellow.png图片,[ESC]退出",PNGPATH)==ESC)*/
	if(cls_show_msg("请确认在%s目录下有huidu_0 huidu100 red green blue pink cyan yellow %s 图片,[ESC]退出",PNGPATH,IMAGEFORMAT)==ESC)
		return;
	
	while(1)
       {
		keyin = cls_show_msg("1.坏点测试\n"
							  "2.亮点测试\n"
							  "3.暗点测试");
		switch(keyin) 
		{
		case '1':
			Dead_Pixel();
			break;
		case '2':
			Light_Pixel();
			break;
		case '3':
			Dark_Pixel();
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

//灰度渐变测试:从0递增到100再递减到0为一轮测试
void Gray_Gradient_Test(void)
{
	//char *szPng[]={"huidu_0.png","huidu_25.png","huidu_50.png","huidu_75.png","huidu_100.png","huidu_75.png","huidu_50.png","huidu_25.png"};
	char *szPng[]={"huidu_0","huidu_25","huidu_50","huidu_75","huidu100","huidu_75","huidu_50","huidu_25"};
	char szBuf[64]={0};
	int i=0,ret = 0,nCode=0,RealRunNum = 0;
	
	RealRunNum = RunNum;
	NDK_ScrClrs();
	while(RealRunNum--)
	{
		while(1)
		{
			if(i==SZ_ARRAY(szPng))
			{
				i=0;
				break;
			}
			memset(szBuf,0,sizeof(szBuf));
			sprintf(szBuf,"%s%s%s",PNGPATH,szPng[i],IMAGEFORMAT);
			if((ret =NDK_ScrDispPic(0,0,unWidth,unHeight,szBuf))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败%d", TESTITEM,__LINE__,ret);
				return;
			}
			NDK_ScrRefresh();
			if(NDK_KbGetCode(DispTime, &nCode)==NDK_OK&&nCode==ESC)
				goto END;
			i++;
		}
	}
END:
	if(cls_show_msg("灰度渐变是否正常,[ENTER] 是 [其他] 否")!=ENTER)
	{
		cls_show_msg1(2, "%s,line %d测试失败", TESTITEM,__LINE__);
		return;
	}
	cls_show_msg("灰度渐变测试通过");
	return;
}

//灰度等级测试
void Grey_Scale_Test(void)
{
	//char *szPng[]={"huidu_h1.png","huidu_h2.png","huidu_v1.png","huidu_v2.png"};
	char *szPng[]={"huidu_h1","huidu_h2","huidu_v1","huidu_v2"};
	char szBuf[64]={0};
	int i=0,ret = 0,nCode=0,RealRunNum = 0;
	
	RealRunNum = RunNum;
	NDK_ScrClrs();
	while(RealRunNum--)
	{
		while(1)
		{
			if(i==SZ_ARRAY(szPng))
			{
				i=0;
				break;
			}
			memset(szBuf,0,sizeof(szBuf));
			sprintf(szBuf,"%s%s%s",PNGPATH,szPng[i],IMAGEFORMAT);
			if((ret =NDK_ScrDispPic(0,0,unWidth,unHeight,szBuf))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败%d", TESTITEM,__LINE__,ret);
				return;
			}
			NDK_ScrRefresh();
			if(NDK_KbGetCode(DispTime, &nCode)==NDK_OK&&nCode==ESC)
				goto END;
			i++;
		}
	}
END:
	if(cls_show_msg("灰度等级图片显示是否正常,[ENTER] 是 [其他] 否")!=ENTER)
	{
		cls_show_msg1(2, "%s,line %d测试失败", TESTITEM,__LINE__);
		return;
	}
	cls_show_msg("灰度等级图片测试通过");
	return;
}

//屏幕灰度测试
void Screen_Gray(void)
{
	int  keyin = 0;

	if(cls_show_msg("请确认在%s目录下有huidu_0 huidu_25 huidu_50 huidu_75 huidu100\
		huidu_h1 huidu_h2 huidu_v1 huidu_v2 huidu2 huidu44 %s图片,[ESC]退出",PNGPATH,IMAGEFORMAT)==ESC)
		return;
	
	while(1)
       {
		keyin = cls_show_msg("1.灰度渐变测试\n"
							  "2.灰度等级测试\n");
		switch(keyin) 
		{
		case '1':
			Gray_Gradient_Test();
			break;
		case '2':
			Grey_Scale_Test();
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

//串扰测试:测试过程中按确认键使屏幕暂停后观察屏幕黑白边界处是否存在干扰
void Cross_talk(void)
{
	int start_x1_point = 0, start_y1_point = 0, start_x_len = 0,start_y_len = 0, flag = 1, ret = 0, nCode = 0;

	if(unHeight>= unWidth)	
	{
		start_x1_point = (unWidth/2)-1;
		start_y1_point = start_x1_point;
		start_x_len = 2;
		start_y_len = unHeight-(unWidth-2);
	}	
	else
	{
		start_x1_point = (unHeight/2)-1;
		start_y1_point = start_x1_point;
		start_x_len =  unWidth-(unHeight-2);
		start_y_len = 2;
	}
	
	NDK_ScrClrs();
	NDK_ScrRefresh();
	while(1)
	{
		NDK_KbFlush();
		while(flag)
		{
			if((ret = NDK_ScrRectangle(start_x1_point,start_y1_point,start_x_len,start_y_len,RECT_PATTERNS_NO_FILL,0))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败(%d)", TESTITEM,__LINE__,ret);
				return;
			}
			NDK_ScrRefresh();
			NDK_SysMsDelay(10);
			if((start_x1_point--)==0)
			{
				flag = 0;
				++start_x1_point;
				if(NDK_KbGetCode(DispTime, &nCode)==NDK_OK&&nCode == ESC)
					goto END;
				break;
			}
			start_y1_point--;
			start_x_len+=2;
			start_y_len+=2;	
			if(NDK_KbHit(&nCode)==NDK_OK)
			{
				if(nCode==ENTER)
				{
					NDK_KbGetCode(0, NULL);
					nCode = 0;
				}
				else if(nCode == ESC)
				{
					goto END;
				}
			}
		}
		NDK_KbFlush();
		while(!flag)
		{
			if((ret = NDK_ScrRectangle(start_x1_point,start_y1_point,start_x_len,start_y_len,RECT_PATTERNS_NO_FILL,0xFFFF))!=NDK_OK)
			{
				cls_show_msg1(2, "%s,line %d测试失败%d", TESTITEM,__LINE__,ret);
				return;
			}
			NDK_ScrRefresh();
			NDK_SysMsDelay(10);
			if((start_x1_point++)==((unHeight>= unWidth)?((unWidth/2)-1):((unHeight/2)-1)))
			{
				flag = 1;
				--start_x1_point;
				if(NDK_KbGetCode(DispTime, &nCode)==NDK_OK&&nCode == ESC)
					goto END;
				break;
			}
			start_y1_point++;
			start_x_len-=2;
			start_y_len-=2;
			if(NDK_KbHit(&nCode)==NDK_OK)
			{
				if(nCode==ENTER)
				{
					NDK_KbGetCode(0, NULL);
					nCode = 0;
				}
				else if(nCode == ESC)
				{
					goto END;
				}
			}
		}
	}
END:
	if(cls_show_msg("CrossTalk测试中途按ENTER停住后边界是否清晰,[ENTER] 是 [其他] 否")!=ENTER)
	{
		cls_show_msg1(2, "%s,line %d测试失败", TESTITEM, __LINE__);
		return;
	}
	cls_show_msg("CrossTalk测试通过");
	return ;
}

//测试图案为“棋盘格”，将该画面显示12小时以上，再切换成全屏50%灰度的画面，看是否有残留“棋盘格”。
void Ghosting_Test(void)
{
	//char *szPng[]={"qipan.png","huidu_50.png"};
	char *szPng[]={"qipan","huidu_50"};
	char szBuf[64]={0};
	int i=0,ret = 0,nCode=0,RealRunNum = 0;

	if(cls_show_msg("本测试请关闭休眠开关,为避免误操作,棋盘到灰度图片按1,灰度图片到退出案例按3")==ESC)
		return;

	RealRunNum = RunNum;
	NDK_ScrClrs();
	while(1)
	{
		if(i==SZ_ARRAY(szPng))
			break;
	
		memset(szBuf,0,sizeof(szBuf));
		sprintf(szBuf,"%s%s%s",PNGPATH,szPng[i],IMAGEFORMAT);
		if((ret =NDK_ScrDispPic(0,0,unWidth,unHeight,szBuf))!=NDK_OK)
		{
			cls_show_msg1(2, "%s,line %d测试失败%d", TESTITEM,__LINE__,ret);
			return;
		}
		NDK_ScrRefresh();
	START:
		if(NDK_KbGetCode(0, &nCode)==NDK_OK&&((i==0&&nCode=='1')||(i==1&&nCode == '3')))
			;
		else
			goto START;
		i++;
	}
	
	if(cls_show_msg("从棋盘切换到灰度50是否有棋盘残影,[ENTER] 是 [其他] 否")==ENTER)
	{
		cls_show_msg1(2, "%s,line %d测试失败", TESTITEM,__LINE__);
		return;
	}
	cls_show_msg("残影测试通过");
	return;
}

//拖影测试:白色小方块移动过程中屏幕上不应该出现白色拖尾
void Ghosting_Test2(void)
{
	int start_x1_point = 0, start_y1_point = 0, len = 10, flagx = 1, flagy = 1, ret = 0, nCode = 0;

	NDK_ScrClrs();
	NDK_ScrRefresh();
	//fprintf(stderr,"xxx=%d,yyy=%d\n",GUI_GetLCDWidth(),GUI_GetLCDHeight());
	while(1)
	{
		//fprintf(stderr,"x=%d,y=%d\n",start_x1_point,start_y1_point);
		if((ret = NDK_ScrRectangle(0,0,unWidth,unHeight,RECT_PATTERNS_SOLID_FILL,0))!=NDK_OK)//全屏黑色
		{
			cls_show_msg1(2, "%s,line %d测试失败(%d)", TESTITEM,__LINE__,ret);
			return;
		}
		if((ret = NDK_ScrRectangle(start_x1_point,start_y1_point,len,len,RECT_PATTERNS_SOLID_FILL,0xffff))!=NDK_OK)//白色小正方形(模拟鼠标)
		{
			cls_show_msg1(2, "%s,line %d测试失败(%d)", TESTITEM,__LINE__,ret);
			return;
		}
		if((ret = NDK_ScrLine(start_x1_point,start_y1_point,start_x1_point+len-1,start_y1_point+len-1,0))!=NDK_OK)//白色小正方形中的黑色斜线
		{
			cls_show_msg1(2, "%s,line %d测试失败(%d)", TESTITEM,__LINE__,ret);
			//fprintf(stderr,"x=%d,y=%d\n",start_x1_point,start_y1_point);
			return;
		}

		NDK_ScrRefresh();
		NDK_SysMsDelay(6);
		//修改变向标志
		if(start_x1_point==0)
			flagx = 1;
		if(start_x1_point==(unWidth-len))
			flagx = 0;
		if(start_y1_point==0)
			flagy = 1;
		if(start_y1_point==(unHeight-len))
			flagy = 0;
		//根据变向标志修改起始坐标
		if(flagx==1&&flagy==1)//左上角->右下角移动
		{
			start_x1_point++;
			start_y1_point++;
		}
		if(flagx==0&&flagy==1)//右上角->左下角移动
		{
			start_x1_point--;
			start_y1_point++;
		}
		if(flagx==0&&flagy==0)//右下角->左上角移动
		{
			start_x1_point--;
			start_y1_point--;
		}
		if(flagx==1&&flagy==0)//左下角->右上角移动
		{
			start_x1_point++;
			start_y1_point--;
		}

		if(NDK_KbHit(&nCode)==NDK_OK)
		{
			if(nCode == ESC)
				break;//退出测试
		}
	}

	if(cls_show_msg("白色小方块移动过程中屏幕是否出现拖影,[ENTER] 是 [其他] 否")==ENTER)
	{
		cls_show_msg1(2, "%s,line %d测试失败", TESTITEM,__LINE__);
		return;
	}
	cls_show_msg("拖影测试通过");
	return;
}

void Screen_Ghosting(void)
{
	int  keyin = 0;

	if(cls_show_msg("请确认在%s目录下有huidu_50和qipan %s图片,[ESC]退出",PNGPATH,IMAGEFORMAT)==ESC)
		return;
	
	while(1)
       {
		keyin = cls_show_msg("1.Crosstalk测试\n"
							  "2.残影测试\n"
							  "3.拖影测试\n");
		switch(keyin) 
		{
		case '1':
			Cross_talk();
			break;
		case '2':
			Ghosting_Test();
			break;
		case '3':
			Ghosting_Test2();
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

void systest51(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0, ret = -1;
	
	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
#if !K21_ENABLE
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE))!=NDK_OK)
	{
		cls_show_msg("状态栏关闭失败!(%d)", ret);
		return;
	}
#endif	
//	NDK_ScrGetViewPort(NULL, NULL, &unWidth, &unHeight);
	unWidth = GUI_GetLCDWidth();
	unHeight = GUI_GetLCDHeight();
	
	while(1)
       {
		keyin = cls_show_msg("1.像素点测试\n"
							"2.灰度测试\n"
							"3.残影测试\n"
							"0.等待时间设置");
		switch(keyin) 
		{
		case '1':
			Screen_Pixel();
			break;
		case '2':
			Screen_Gray();
			break;
		case '3':
			Screen_Ghosting();
			break;
		case '0':
			Scr_Config();
			break;
		case ESC:
#if !K21_ENABLE
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_WLSIGNAL|STATUSBAR_DISP_BATTERY|STATUSBAR_DISP_DATE|STATUSBAR_DISP_TIME|STATUSBAR_DISP_ETH|STATUSBAR_DISP_MODEM|STATUSBAR_DISP_WIFI))!=NDK_OK)
			{
				cls_show_msg("状态栏显示失败!(%d)", ret);
				return;
			}
#endif
			return;
		default:
			continue;
			break;
		}
	}
	
	//test over
	return;
}
