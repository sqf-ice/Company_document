/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 33.c
* Author 			: linwl
* version			: 
* DATE			: 20160720
* directory 		: 
* description		: 测试NDK_ScrDispImgWithOutFS是否在指定区域内显示状态栏图片
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define		MAXWAITTIME 30
#define 	TESTAPI		"NDK_ScrDispImgWithOutFS"
#define 	BATT_OFFSET		0	//电池图片的偏移地址	电池图片的宽和高分别是22 12
#define 	GPRS_OFFSET 	3960//GPRS图片的偏移地址	GPRS图片的宽和高分别是19 20
#define 	WIFI_OFFSET 	9660//WIFI图片的偏移地址	WIFI图片的宽和高分别是20 14
#define 	NET_OFFSET 		13020//NET图片的偏移地址	NET图片的宽和高分别是13 14
#define 	BATT_SIZE 		792	//电池图片的长度
#define 	GPRS_SIZE 		1140
#define 	WIFI_SIZE 		840
#define 	NET_SIZE 		546


/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr10
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        linwl         20160720  	   created
*****************************************************************/
void scr33(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0, i =0;
	uint ViewWidth = 0, ViewHeight = 0;
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//状态栏打开情况下需要获取屏幕实际视口大小作为看边界测试值,状态栏未打开情况下就是液晶的宽和高
	NDK_ScrGetViewPort(NULL, NULL, &ViewWidth, &ViewHeight);

	//测试前置
	if(cls_show_msg("是否已经下载statusicon.NLP应用程序,[是]按确认,否则按其他")!=ENTER)
	{
		//case1:图片应用未下载情况下测试应该失败
		if((ret=NDK_ScrDispImgWithOutFS(80, 200, 22, 12, BATT_OFFSET, BATT_SIZE))!=NDK_ERR)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		send_result("%s测试通过,请下载statusicon.NLP应用程序后重新测试", TESTAPI);
		return;
	}

	//case2:参数异常：unX,unY,unWidth,unHeight,offset,len分别为-1,>屏幕,unX+unWidth>屏幕,unY+unHeight>屏幕
	if((ret=NDK_ScrDispImgWithOutFS(-1,0,22,12, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret=NDK_ScrDispImgWithOutFS(0,-1,22,12, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithOutFS(0,0,-1,12, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret=NDK_ScrDispImgWithOutFS(0,0,22,-1, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret=NDK_ScrDispImgWithOutFS(0,0,GUI_GetLCDWidth()+1,12, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithOutFS(0,0,22,GUI_GetLCDHeight()+1, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithOutFS(10,0,GUI_GetLCDWidth()-9,12, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrDispImgWithOutFS(0,10,22,GUI_GetLCDHeight()-9, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if((ret=NDK_ScrDispImgWithOutFS(0,0,22,12, -1, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_ScrRefresh();
	if((ret=NDK_ScrDispImgWithOutFS(0,0,22,12, 0, -1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);

	//case3:测试参数中图片宽和高与实际图片的宽高不一致的情况,预期不应该跑飞,或有其它任何死机等异常,允许显示乱码,所以应用在打包图片文件时必须清楚地自己的图片的宽高
#if !(defined ME50NX||defined ME68) //ME50NX中width，  unheight和len一定要符合如下关系: unwidth*unheight*2=len或unwidth*unheight*3=len，与其它产品有所差异 20171113
	cls_show_msg1(3,"3秒后在20,200位置显示一张20*10乱码图片");
	if((ret=NDK_ScrDispImgWithOutFS(20, 200, 20, 10, BATT_OFFSET, BATT_SIZE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);
	//cls_show_msg1(3,"3秒后在0,0位置开始显示一张全屏乱码图片");
#endif
	if((ret=NDK_ScrDispImgWithOutFS(0, 0, ViewWidth, ViewHeight, BATT_OFFSET, BATT_SIZE))!=NDK_ERR_PARA/*NDK_OK*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);
	
	//case4:正常测试显示电池电量,从0格开始逐渐增加到3格
	cls_show_msg1(3,"3秒后在80,200位置显示电量从0增加3格");
	for(i=0;i<4;i++)
	{
		if((ret=NDK_ScrDispImgWithOutFS(80, 200, 22, 12, BATT_OFFSET+i*BATT_SIZE, BATT_SIZE))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysDelay(10);
	}
	//case5:正常测试显示无线信号,从0格开始逐渐增加到4格
	cls_show_msg1(3,"3秒后在140,200位置显示无线信号从0增强到4");
	for(i=0;i<5;i++)
	{
		if((ret=NDK_ScrDispImgWithOutFS(140, 200, 19, 20, GPRS_OFFSET+i*GPRS_SIZE, GPRS_SIZE))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysDelay(10);
	}
	//case6:正常测试显示WIFI信号,从0格开始逐渐增加到3格
	cls_show_msg1(3,"3秒后在200,200位置显示WIFI信号从0增强到3");
	for(i=0;i<4;i++)
	{
		if((ret=NDK_ScrDispImgWithOutFS(200, 200, 20, 14, WIFI_OFFSET+i*WIFI_SIZE, WIFI_SIZE))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysDelay(10);
	}
	//case7:正常测试显示G网的标志
#if defined ME68  // ME68的液晶宽高为240*320,20180416 sull modify
	cls_show_msg1(3,"3秒后在200,200位置显示G网标志");
	if((ret=NDK_ScrDispImgWithOutFS(200, 200, 13, 14, NET_OFFSET, NET_SIZE))!=NDK_OK)
#else
	cls_show_msg1(3,"3秒后在260,200位置显示G网标志");
	if((ret=NDK_ScrDispImgWithOutFS(260, 200, 13, 14, NET_OFFSET, NET_SIZE))!=NDK_OK)
#endif	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(10);
	nKeyin = cls_show_msg1(MAXWAITTIME,"显示的状态栏图片是否正常,按确认测试通过,其它键失败");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	return;
}

