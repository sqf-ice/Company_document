/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt16.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnPicture接口功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_PrnPicture"
#define TIMEOUT 30
#define MAXWAITTIME 180
#define MAXCASECNT	8

#define NOEXIST "fnnnnnn.bmp"

#define ICOTUBIAO "tubiao.ico"
#define IMAGEPATH  "" //所有图片放在当前路径下，方便打包
#define	BMPFORMAT ".bmp"
#define PNGFORMAT ".png"
#define JPGFORMAT ".jpg"

//将所有bmp、png、jpg图片进行归类整合，减少宏定义及代码 20180301
//#define GUANGFABANKPNG "guangfabank.png"
//#define ABCBANKJPG "ABC.jpg"
#define ABCBANKBMP "abclogo.bmp"
//#define MEITUANPNG     "png02.png"
//#define FONTPNG           "png03.png"
//#define CARDPNG           "png04.png"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
static int testcase(int subcase)
{
	int nRet = 0, i = 0;
	char *szBmp[]={"abclogo","bmp01","bmp02"};
	char *szPng[]={"color1", "color2", "color3", "color4", "color5", "color6", "IHDR1", "IHDR2", "IHDR3", 
		"IHDR4", "IHDR5", "IHDR6", "IHDR7", "IHDR8", "IHDR9", "IHDR10", "IHDR11", "IHDR12", "IHDR13",
		"IHDR14", "IHDR15", "other1", "other2", "other3", "other4", "other5", "other6", "other7", "other8", 
		"other9", "other10", "other11", "other12", "other13", "other14", "other15", "other16", "ysz1",
		"ysz2", "ysz3", "ysz4", "ysz5", "ysz6", "ysz7", "guangfabank"};
	char *szJpg[]={"jsyh", "kbd", "ldcr", "logo", "png", "stw", "tcd", "xdl", "ABC"};
	char szBuf[64]={0};
	char hintBuf[128]={0},hintBuf1[64]="以下打印:";
	
	switch(subcase)
	{
	case 1://case1:路径为空
		if((nRet = NDK_PrnPicture(0,NULL)) != NDK_ERR_PARA)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 2://case2:图片不存在
		NDK_FsDel(NOEXIST);
		if((nRet = NDK_PrnPicture(0,NOEXIST)) != NDK_ERR_PATH)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 3://case3:不支持的图片格式ico
		if((nRet = NDK_PrnPicture(0,ICOTUBIAO)) != NDK_ERR_DECODE_IMAGE)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 4://case4:xpos+xsize(解码后图片的宽度值) > ndk_PR_MAXLINEWIDE（正常模式为384）
		if((nRet = NDK_PrnPicture(350, ABCBANKBMP)) != NDK_ERR_PARA)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		break;
	case 5://case5:xpos+xsize(解码后图片的宽度值) > ndk_PR_MAXLINEWIDE  横向放大时为384/2
		NDK_PrnSetMode(PRN_MODE_WIDTH_DOUBLE  ,0);
		if((nRet = NDK_PrnPicture(240, ABCBANKBMP)) != NDK_ERR_PARA)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		NDK_PrnSetMode(PRN_MODE_NORMAL,0);
		break;
		
	case 6://case6:打印bmp
	#if 0 //新增了测试数据，将所有bmp图片整合到一起，此测试项包含在了下面的测试项中 20180301 sull modify
		NDK_PrnStr("---以下打印工行Logo---");
		if((nRet = NDK_PrnPicture(0,ABCBANKBMP)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
	#endif
		NDK_PrnStr("--以下打印所有bmp图片(其中彩色图片浅颜色将打印成白色，深颜色将打印成黑色)--\n");
		for(i=0; i<SZ_ARRAY(szBmp); i++)
		{
			memset(szBuf, 0, sizeof(szBuf));
			memset(hintBuf, 0, sizeof(hintBuf));
			sprintf(szBuf,"%s%s%s",IMAGEPATH,szBmp[i],BMPFORMAT);
			sprintf(hintBuf,"%s%s",hintBuf1,szBuf);
			NDK_PrnStr(hintBuf);
			if((nRet = NDK_PrnPicture(0,szBuf)) != NDK_OK)
			{
				send_result("line %d: %s测试失败(%d,%s)", __LINE__, TESTAPI,nRet,szBuf);
				return FAIL;
			}
		}
		break;
		
	case 7://case7:打印png
	#if 0 //新增了测试数据，将所有png图片整合到一起，此测试项包含在了下面的测试项中 20180301 sull modify
		NDK_PrnStr("---以下打印广发Logo---");
		if((nRet = NDK_PrnPicture(0,GUANGFABANKPNG)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		NDK_PrnStr("---以下打印美团外卖Logo(彩色图片浅颜色打印成白色,深颜色打印成黑色)---");
		if((nRet = NDK_PrnPicture(0,MEITUANPNG)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		NDK_PrnStr("---以下打印类似卷的字(彩色图片浅颜色打印成白色,深颜色打印成黑色)---");
		if((nRet = NDK_PrnPicture(0,FONTPNG)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
		NDK_PrnStr("---以下打印小汽车图标(彩色图片浅颜色打印成白色,深颜色打印成黑色)---");
		if((nRet = NDK_PrnPicture(0,CARDPNG)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
	#endif
		NDK_PrnStr("--以下打印所有png图片(其中彩色图片浅颜色将打印成白色，深颜色将打印成黑色)--\n");
		for(i=0; i<SZ_ARRAY(szPng); i++)
		{
			memset(szBuf, 0, sizeof(szBuf));
			memset(hintBuf, 0, sizeof(hintBuf));
			sprintf(szBuf,"%s%s%s",IMAGEPATH,szPng[i],PNGFORMAT);
			sprintf(hintBuf,"%s%s",hintBuf1,szBuf);
			NDK_PrnStr(hintBuf);
			if((nRet = NDK_PrnPicture(0,szBuf)) != NDK_OK)
			{
				send_result("line %d: %s测试失败(%d,%s)", __LINE__, TESTAPI,nRet,szBuf);
				return FAIL;
			}
		}
		break;
	
	case 8://case8:打印jpg
	#if 0 //新增了测试数据，将所有png图片整合到一起，此测试项包含在了下面的测试项中 20180301 sull modify
		NDK_PrnStr("---以下打印银信通Logo---");
		if((nRet = NDK_PrnPicture(0,ABCBANKJPG)) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI,nRet);
			return FAIL;
		}
	#endif
		NDK_PrnStr("--以下打印所有jpg图片(其中彩色图片浅颜色将打印成白色，深颜色将打印成黑色)--\n");
		for(i=0; i<SZ_ARRAY(szJpg); i++)
		{
			memset(szBuf, 0, sizeof(szBuf));
			memset(hintBuf, 0, sizeof(hintBuf));
			sprintf(szBuf,"%s%s%s",IMAGEPATH,szJpg[i],JPGFORMAT);
			sprintf(hintBuf,"%s%s",hintBuf1,szBuf);
			NDK_PrnStr(hintBuf);
			if((nRet = NDK_PrnPicture(0,szBuf)) != NDK_OK)
			{
				send_result("line %d: %s测试失败(%d,%s)", __LINE__, TESTAPI,nRet,szBuf);
				return FAIL;
			}
		}
		break;
	}
	
	return SUCC;
}

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:void prnt16(void)
* functional description 	:打印bmp，png等格式的图片  
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt16(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int i = 1;
	int nRet=0/*,nRet1=0,nRet2=0*/,nRet3=0/*,nRet4=0,nRet5=0,nRet6=0*/;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;
	}
	if(cls_show_msg("请确保当前路径下存在所有图片,[确认]继续[其他]返回")==ENTER)
	{
		if((nRet = NDK_FsExist(ABCBANKBMP))!=NDK_OK/*||(nRet1=NDK_FsExist(GUANGFABANKPNG))!=NDK_OK||(nRet2=NDK_FsExist(ABCBANKJPG))!=NDK_OK \*/
			||(nRet3 = NDK_FsExist(ICOTUBIAO))!=NDK_OK/*||(nRet4 = NDK_FsExist(MEITUANPNG))!=NDK_OK||(nRet5 = NDK_FsExist(FONTPNG))!=NDK_OK||(nRet6 = NDK_FsExist(CARDPNG))!=NDK_OK*/)//新增了测试数据，将所有图片整合到一起 20180301 sull modify
		{
			send_result("line %d:%s测试失败(%d%d)",__LINE__,TESTAPI,nRet,nRet3);
			goto prnt_error;
		}
	}
	else
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;
	}

	while(1)
	{
		if((nRet=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,nRet);
			goto prnt_error;
		}
		if(emStatus != PRN_STATUS_BUSY)
			show_stopwatch(DISABLE, 0);
		
		switch(emStatus)
		{
		case PRN_STATUS_BUSY:
			if(nBusy==FALSE)
			{
				nOldtime = time(NULL);//从第一次收到忙状态开始计时
				nBusy = TRUE;
			}
			if((nDiff=time(NULL)-nOldtime)>MAXWAITTIME)
			{
				send_result("line %d:打印机处于忙状态时间过长", __LINE__);
				goto prnt_error;
			}
			show_stopwatch(ENABLE, nDiff);
			continue;
			break;

		case PRN_STATUS_OK:
			nBusy=FALSE;

			switch(i)
			{
			default:
				if(testcase(i)!=SUCC)	
					goto prnt_error;
				break;
			case MAXCASECNT+1:
				//test over
				nKeyin = cls_show_msg1(MAXWAITTIME, "检查打印效果,与打印图片进行比较,按ENTER通过,其它键不通过");
				if (nKeyin!=ENTER && nKeyin!=0)//zhangnw20061031 modify
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				goto prnt_error;//return;			
				break;
			}

			NDK_PrnStart();
			i++;
			break;
		case PRN_STATUS_NOPAPER:
			cls_printf("打印机缺纸...装纸后,任意键继续,ESC放弃");
			lib_kbflush(); //清KB BUF
			while ((nKeyin=lib_kbhit())==0)
			{
				NDK_SysBeep();
				//正常装纸
				NDK_PrnFeedByPixel(10);
				sleep(2);
			}
			if (nKeyin==ESC)
			{
				send_result("line %d:打印机缺纸,用户放弃", __LINE__);
				goto prnt_error;
			}
			NDK_PrnStart();
			cls_printf("测试%s...", TESTAPI);
			break;
		case PRN_STATUS_OVERHEAT:
			send_result("line %d: 打印机过热", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_VOLERR:
			send_result("line %d: 打印机电源故障", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_DESTROYED:
			send_result("line %d: 打印机损坏", __LINE__);
			goto prnt_error;
			break;
		default:
			//未知状态，判为错
			send_result("line %d: UNKNOWN STATE(tpstatus=%d)", __LINE__, emStatus);
			goto prnt_error;
			break;
		}
		
	}
	
	//测试结束	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}

