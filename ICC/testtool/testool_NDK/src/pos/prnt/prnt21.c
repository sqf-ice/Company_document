/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt21.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnSetAlignment 接口功能
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
#define TESTAPI	"NDK_PrnLoadBDFFont,NDK_PrnBDFStr,NDK_PrnSetBDF"	
#define MAXWAITTIME 60
#define NOTEXIST "BDF12345" 
#define MINXPOS		0
#define MAXXPOS		288
#define MIXLINESPACE 0
#define MAXLINESPACE 255
#define MINWORDSPACE 0
#define MAXWORDSPACE 255
#define PERSIANBDF	"persian.bdf"
#define FARSITEXBDF "farsitex.bdf"
#define BDF1020		"10x20.bdf"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static int testcase(int subcase)
{
	/*private & local definition*/
	int nRet = SUCC;
	
	/*process body*/
	switch (subcase)
	{
	case 0:
		//case1:异常测试NULL
		if((nRet = NDK_PrnLoadBDFFont(NULL))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,nRet);
			return FAIL;
		}
		
		//case2:路径 不为空但是不存在
		if((nRet = NDK_PrnLoadBDFFont(NOTEXIST))!=NDK_ERR_PATH/*NDK_ERR_PARA*/)//20130717zhengdc提交修改返回值
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,nRet);
			return FAIL;
		}		
		
		//case3:打印BDF字体中的内容为空
		if((nRet = NDK_PrnBDFStr(NULL))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,nRet);
			return FAIL;
		}

		//case4:设置BDF 字体属性异常测试
		if(NDK_PrnSetBDF(MINXPOS-1,MIXLINESPACE,MINWORDSPACE,1,1)!=NDK_ERR_PARA||NDK_PrnSetBDF(MINXPOS,MIXLINESPACE-1,MINWORDSPACE,1,1)!=NDK_ERR_PARA||NDK_PrnSetBDF(MINXPOS,MIXLINESPACE,MINWORDSPACE-1,1,1)!=NDK_ERR_PARA\
			||NDK_PrnSetBDF(MAXXPOS+1,MIXLINESPACE,MINWORDSPACE,1,1)!=NDK_ERR_PARA||NDK_PrnSetBDF(MAXXPOS,MAXLINESPACE+1,MINWORDSPACE,1,1)!=NDK_ERR_PARA||NDK_PrnSetBDF(MAXXPOS,MIXLINESPACE,MAXWORDSPACE+1,1,1)!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI);
			return FAIL;
		}	

		//case5:未加载字体先打印
		if((nRet = NDK_PrnBDFStr((ushort *)"\xfc\x00\x1c\x20\xa4\x00\xe4\x00"))!=NDK_ERR/*NDK_ERR_PARA*/)//20130717zhengdc提交修改返回值
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI, nRet);
			return FAIL;
		}
		break;
	case 1:  //case6:测试能否重复注册,且不设置
		if((nRet=NDK_PrnLoadBDFFont(PERSIANBDF))!=NDK_OK)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI, nRet);
			return FAIL;
		}
		if((nRet=NDK_PrnLoadBDFFont(BDF1020))!=NDK_OK)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI, nRet);
			return FAIL;
		}
		if((nRet=NDK_PrnBDFStr((ushort *)"\x40\x00\x41\x00\x42\x00\x43\x00\x44\x00\x45\x00"))!=NDK_OK)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI, nRet);
			return FAIL;
		}
		break;	
	case 2: //case7:正常测试
		if((nRet = NDK_PrnSetBDF(50,30,40,2,2))!=NDK_OK)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI, nRet);
			return FAIL;
		}
		if((nRet=NDK_PrnBDFStr((ushort *)"\x40\x00\x41\x00\x42\x00\x43\x00\x44\x00\x45\x00"))!=NDK_OK)//if(NDK_PrnBDFStr((ushort *)"\xfc\x00\x1c\x20\xa4\x00\xe4\x00")!=NDK_OK)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI, nRet);
			return FAIL;
		}
		break;
	default:
		return 1;  /* 返回1说明测试结束*/
	}	
	
	return SUCC;  /* 返回0子用例测试通过*/
}

/****************************************************************
* function name 	 				:void prnt21(void)
* functional description 	:加载、设置字体、打印
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt21(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int nRet=0,i=0,nRet1=0,nRet2=0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}
	
	if(cls_show_msg("请确保是第一次执行本用例,[确认]继续[其他]返回")!=ENTER)
	{
		send_result("line %d:%s用户取消测试,请退出测试程序再重新进入执行本用例",__LINE__,TESTAPI);
		return;//因case5:未加载字体先打印用例,需要在进应用程序后第一次测试,增加该提示
	}	
		
	if(cls_show_msg("请确保当前路径下存在BDF字库,[确认]继续[其他]返回")==ENTER)
	{
		if((nRet = NDK_FsExist(PERSIANBDF))!=NDK_OK||(nRet1=NDK_FsExist(FARSITEXBDF))!=NDK_OK||(nRet2=NDK_FsExist(BDF1020))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d,%d)",__LINE__,TESTAPI,nRet,nRet1,nRet2);
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

			if((nRet = testcase(i++))==SUCC) /*chensj modified 2013/06/18 testcase:有三种返回值0:一个case成功,1:测试全部通过,-1调用错误*/
			{
				NDK_PrnStart();
				//continue;
			}
			else//返回FAIL或者1则继续判断是测试完成还是测试出错
			{
				if(nRet == FAIL)//测试出错
				{
					//send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}//否则就是测试完成
				NDK_PrnStart();
				if ((nKeyin = cls_show_msg("检查打印效果,[确认]通过,[其它]失败"))!=ENTER)
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				goto prnt_error;
			}
			
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
				goto prnt_error;//return;
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
			goto prnt_error;//return;
			break;
		}	
	}

	//测试结束
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}

