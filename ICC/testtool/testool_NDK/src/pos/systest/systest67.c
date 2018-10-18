/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest67.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 状态栏综合测试
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM		"状态栏综合测试"
#define	DEFAULT_CNT_STR	"100"
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)

/*------------global variables definition-----------------------*/

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
* history 		 		: author			date			remarks
*			  			jiangym 			20140623     created
*****************************************************************/
static void Sta_open_close(void)
{
	/*private & local definition*/
	int ret = -1, k = 0, bak = 0,cnt = 0, succ = 0;
	EM_STATUSBAR displayposition = STATUSBAR_POSITION_TOP;
	uint getx = 0, gety = 0, ViewWidth = 0, ViewHeight = 0;
	uint unLen = 0;
	char  CntStr[8] = DEFAULT_CNT_STR;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(&getx, &gety, &ViewWidth, &ViewHeight);

	cls_printf("默认交叉测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&unLen, 2, 5,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);


	while(1)
	{
		if(bak++==cnt)
			break;
		if(ESC==cls_show_msg1(2, "正在进行%d次%s测试,成功%d次,ESC退出", bak, TESTITEM, succ))
			break;
		//2-0-4-0-8-0-0x10-0-0x20-0-0x40-0-2&0x80-0-1-0
		for(k = 0; k < 2 ; k++)
		{
			if(k==0)
				displayposition=STATUSBAR_POSITION_TOP;//状态栏显示在顶部
			else
				displayposition=STATUSBAR_POSITION_BOTTOM;//状态栏显示在底部
			
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			//2-0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			//4-0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_BATTERY|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			//8-0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_WLSIGNAL|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
		/*	 wifi 暂时不测
			//0x10-0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_WIFI|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}

		*/
			//0x20-0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_MODEM|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}

			//0x40-0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_ETH|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			
			//0x80-0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}

			//0x80&2 -0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|STATUSBAR_DISP_DATE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}

			//1-0
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_ALL|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}	
			
			//新增模拟LED开关压力0x200-0  20180420  add 
#if  STABAR_LED_ENABLE
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_LED|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}
			NDK_ScrRefresh(); 
			if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE|displayposition))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:%s测试失败(%d)", __LINE__, TESTITEM, ret);
				goto ERR;
			}	
#endif
		}	
	succ++;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"%s测试完成,总共测试%d次成功%d次", TESTITEM, bak-1, succ);
//恢复默认设置
ERR:
	if(gety!=0)//y坐标非0说明状态栏在顶部打开
		NDK_ScrStatusbar(STATUSBAR_DISP_ALL);
	else
		NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
	NDK_ScrFullRefresh();
	return;	
}
void systest67(void)
{
	/*private & local definition*/
	int key = 0;
	
	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	if(auto_flag==1)//autotest_flag
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行状态栏开关压力
		Sta_open_close();

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{
		key = cls_show_msg("状态栏综合测试\n"
							"1.状态栏开关压力\n");
		switch(key)
		{
		case '1':
			Sta_open_close();
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

