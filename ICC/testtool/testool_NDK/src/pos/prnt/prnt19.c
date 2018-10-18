/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt19.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnFontRegister,NDK_PrnSetUsrFont接口功能
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
#define TESTAPI	"NDK_PrnFontRegister,NDK_PrnSetUsrFont"	
#define MAXWAITTIME 70
#define HANZI12 "hzk12"
#define ZIMU12 "asc12"
#define NOTEXIST "hzk90" 

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static int func12(char m[], ST_PRN_RECMSG *p)
{
	p->nOffset = (94*((m[0]-0xa0)-1) +(m[1]-0xa0)-1)*24;
	p->nFontByte = 24;
	return 1;
}

static int funcasc12(char m[], ST_PRN_RECMSG *p)
{
	p->nOffset = (m[0]-32)*12;
	p->nFontByte = 12;
	return 1;
}

/****************************************************************
* function name 	 				:void prnt19(void)
* functional description 	:注册字体打印
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt19(void)
{
	//以下为局部变量
	ST_PRN_FONTMSG stMsg,stMsg2,stMsg3;
	int nRet = 0, nRet1 = 0;
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}

	if(cls_show_msg("请确保当前路径下存在ASC12、HZK12字库,[确认]继续[其他]返回")==ENTER)
	{
		if((nRet = NDK_FsExist(HANZI12))!=NDK_OK||(nRet1=NDK_FsExist(ZIMU12))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d%d)",__LINE__,TESTAPI,nRet,nRet1);
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

			//case1:注册参数为空
			if((nRet = NDK_PrnFontRegister(NULL))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}	
			
			//case2: 字库名或字库所在路径 为空
			memset( &stMsg, 0, sizeof(ST_PRN_FONTMSG));
			stMsg.nNum = 0;
			stMsg.w = 12;
			stMsg.h = 12;
			stMsg.nDirection = 0;
			stMsg.nIsHZ = 1;
			stMsg.pszName = NULL;
			stMsg.func=func12;
			if((nRet = NDK_PrnFontRegister(&stMsg))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}	
			
			//case3:字库名或字库所在路径 不为空但是不存在
			memset( &stMsg, 0, sizeof(ST_PRN_FONTMSG));
			stMsg.nNum = 0;
			stMsg.w = 12;
			stMsg.h = 12;
			stMsg.nDirection = 0;
			stMsg.nIsHZ = 1;
			stMsg.pszName = NOTEXIST;
			stMsg.func=func12;
			if((nRet = NDK_PrnFontRegister(&stMsg))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}
			
			//case4:注册号非0-7
			memset( &stMsg, 0, sizeof(ST_PRN_FONTMSG));
			stMsg.nNum =16;
			stMsg.w = 12;
			stMsg.h = 12;
			stMsg.nDirection = 0;
			stMsg.nIsHZ = 1;
			stMsg.pszName = HANZI12;
			stMsg.func=func12;
			if((nRet = NDK_PrnFontRegister(&stMsg))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}
			
			//case5:Direction 非0-1
			memset( &stMsg, 0, sizeof(ST_PRN_FONTMSG));
			stMsg.nNum = 0;
			stMsg.w = 12;
			stMsg.h = 12;
			stMsg.nDirection = 8;
			stMsg.nIsHZ = 1;
			stMsg.pszName = HANZI12;
			stMsg.func=func12;
			if((nRet = NDK_PrnFontRegister(&stMsg))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}
			
			//case6:获取依移地址和占用字节数的函数为空
			memset( &stMsg, 0, sizeof(ST_PRN_FONTMSG));
			stMsg.nNum = 0;
			stMsg.w = 12;
			stMsg.h = 12;
			stMsg.nDirection = 0;
			stMsg.nIsHZ = 1;
			stMsg.pszName = HANZI12;
			stMsg.func=NULL;
			if((nRet = NDK_PrnFontRegister(&stMsg))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}
			
			//case7:注册的字库是汉字字库还是ASCII字库，1:HZ 0:ZM    非0-1
			memset( &stMsg, 0, sizeof(ST_PRN_FONTMSG));
			stMsg.nNum = 0;
			stMsg.w = 12;
			stMsg.h = 12;
			stMsg.nDirection = 0;
			stMsg.nIsHZ = 8;
			stMsg.pszName = HANZI12;
			stMsg.func=func12;
			if((nRet = NDK_PrnFontRegister(&stMsg))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}

			//case8: 正常注册字库stMsg2,stMsg3
			//case8.1:
			memset( &stMsg2, 0, sizeof(ST_PRN_FONTMSG));
			stMsg2.nNum = 0;
			stMsg2.w = 12;
			stMsg2.h = 12;
			stMsg2.nDirection = 0;
			stMsg2.nIsHZ = 1;
			stMsg2.pszName = HANZI12;
			stMsg2.func=func12;
			if((nRet = NDK_PrnFontRegister(&stMsg2))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}
			
			//case8.2:
			memset( &stMsg3, 0, sizeof(ST_PRN_FONTMSG));
			stMsg3.nNum = 1;
			stMsg3.w = 8;
			stMsg3.h = 12;
			stMsg3.nDirection = 0;
			stMsg3.nIsHZ = 0;
			stMsg3.pszName = ZIMU12;
			stMsg3.func=funcasc12;
			if((nRet = NDK_PrnFontRegister(&stMsg3))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;	
			}

			//case8.3:0,1互换,验证重复注册
			stMsg2.nNum = 1;//hz
			stMsg3.nNum = 0;//zm
			if((nRet=NDK_PrnFontRegister(&stMsg2))!=NDK_OK||(nRet1=NDK_PrnFontRegister(&stMsg3))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d,%d)",__LINE__,TESTAPI, nRet, nRet1);
				goto prnt_error;	
			}
		
			//case9:验证"该设置后会覆盖NDK_PrnSetFont中设定的字体"
			//先设置系统字体并打印附打印信息
			//再设置用户字体并打印
			NDK_PrnSetFont(PRN_HZ_FONT_24x24C,PRN_ZM_FONT_16x32);
			NDK_PrnStr("系统字体:[24x24C]汉字;系统字体:[16x32]Zm\n");			
			NDK_PrnSetUsrFont(0);
			NDK_PrnStr("系统字体:[24x24C]汉字;用户字体:[12x8]Zm\n");
			NDK_PrnSetUsrFont(1);
			NDK_PrnStr("用户字体:[12x12]汉字;用户字体:[12x8]Zm\n");
			NDK_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x8);
			NDK_PrnStr("系统字体:[16x16]汉字;系统字体:[8x8]Zm\n");	
			NDK_PrnStart();

			nKeyin = cls_show_msg1(MAXWAITTIME, "检查打印效果(是否根据预设字体打印),[确认]有,[其它]没有");
			if (nKeyin==ENTER||nKeyin==0)
				send_result("%s测试通过", TESTAPI);
			else
				send_result("%s测试失败", TESTAPI);
			goto prnt_error;
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

