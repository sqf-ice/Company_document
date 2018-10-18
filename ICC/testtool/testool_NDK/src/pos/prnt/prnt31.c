/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt31.c
* Author 		: 
* version		: 
* DATE			: 
* directory 		: 
* description		: 模拟自检打印流程
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
#define TESTAPI	"模拟自检打印流程"	
#define MAXWAITTIME 180

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void prnt31(void)
* functional description 	: 模拟自检打印流程
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 					:  author		date		remarks
*			  	  	  				 sull            20180712        created
*****************************************************************/
//模拟自检打印流程，复现在打印失败的情况后再次打印的打印效果异常，打印驱动缓存异常导致的bug
static int onTest(void)
{
	int ret = 0;
	char buf[2000] = {0};
	
	if((ret=NDK_PrnInit(1)) != NDK_OK)
		return ret;
	if((ret=NDK_PrnSetForm(0, 0, 4)) != NDK_OK)
		return ret;
	//设置成中文24*24，英文12*24C的字体，打印中途缺纸后，缓存异常的主要步骤
	if((ret=NDK_PrnSetFont(1,18)) != NDK_OK)
		return ret;
	//打印内容
	sprintf(buf, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
	         "                                            测试样单 \n\n", " - - - - - - - - - - - - - - -\n",
		     "商户名称：              签购单 \n\n", "商户编号：         01010045678 \n\n",
		     "批次号：         55667894561235\n\n", "商户订单号：          098767890\n\n",
		     "交易类型：                 消费\n\n", "交易日期：  2015/06/29 16:27:21\n\n",
		     "参考号:\n", "               1234567890123456\n\n", "应收金额：\n", "                  RMB  10000.00\n\n", 
		     "实收金额：\n", "                   RMB  10000.00\n\n", "持卡人签名：\n\n\n", "备注：         \n\n");
	if((ret = NDK_PrnStr(buf)) != NDK_OK)
		return ret;
	if((ret = NDK_PrnStart()) != NDK_OK)
		return ret;
	return NDK_OK;
}

void prnt31(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}

	//以下为局部变量
	int nRet=0, i=0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;
	}
	
	cls_show_msg("本用例模拟自检打印流程,共打印两联单,请使用空白打印纸观察打印效果是否异常;\n在第一联单据打印过程中,请手动打开pos打印盖子构造缺纸状态;\n装纸完成后第二联请正常打印,关注第二联打印效果,按任意键继续测试");

	//case1:进行脚本打印测试
	for(i=0; i<2; i++)
	{
		cls_printf("打印第%d联单据...", i+1);
		nRet = onTest();
		switch(nRet)
		{
			case NDK_OK:
				break;
			case 2:
				cls_show_msg("打印机缺纸,请装纸...完成点任意键继续");
				break;
			default:
				send_result("line %d:第%d次:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
				goto prnt_error;
				break;
		}
	}
	if(cls_show_msg("打印效果与预期提示语一致?是[ENTER],否[其它]") != ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto prnt_error;
	}
	send_result("%s测试通过", TESTAPI);
	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);	
	return;
}

