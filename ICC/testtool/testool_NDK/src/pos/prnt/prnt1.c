/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt1.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnGetStatus接口功能
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
#define TESTAPI	"NDK_PrnGetStatus"	
#define MAXWAITTIME 90

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void prnt1(void)
* functional description 	: 获取打印机状态
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt1(void)
{
	//以下为局部变量
	int nRet=0;
	int nKeyin=0;
	EM_PRN_STATUS  emStatus=0;
	time_t old = 0, diff = 0;
	
	//以下为函数体
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//测试前置:
	cls_show_msg("请确保有纸后按任意键继续...");
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;
	}

	//case5:初始化后检查状态应为OK态
	if((nRet = NDK_PrnGetStatus(&emStatus)) != NDK_OK ||emStatus != PRN_STATUS_OK)
	{
		send_result("line %d:%s测试失败(%d, %d)",__LINE__,TESTAPI,nRet,emStatus);
		goto prnt_error;
	}
	

	//case1:让打印机无纸,检测状态	
	cls_show_msg("确认打印机缺纸后,按任意键开始");
	//NDK_PrnStart();
	if((nRet = NDK_PrnGetStatus(&emStatus)) != NDK_OK ||emStatus != PRN_STATUS_NOPAPER)
	{
		send_result("line %d:%s测试失败(%d, %d)",__LINE__,TESTAPI,nRet,emStatus);
		goto prnt_error;
	}
#if 0//ANDIROD_ENABLE  根据秋云跟开发沟通结果此处不会返回忙的状态 20170623
	//新增NDK_PrnStr函数返回打印机状态的用例 20170220 linwl
	if((nRet = NDK_PrnStr("不应该打印出本行汉字,如果打印出则表示测试失败\n"))!=PRN_STATUS_NOPAPER)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
		goto prnt_error;
	}
#endif
	
	//装不足10cm的纸，在打印过程中检测缺纸
	nKeyin = cls_show_msg1(MAXWAITTIME, "装不足10cm的纸,任意键继续.ESC放弃");
	if (nKeyin==0)
		;
	else if (nKeyin==ESC)
	{
		send_result("line %d:用户放弃测试", __LINE__);
		return;
	}
	else
	{
		NDK_PrnStr ("打印测试页1:将打印1---120行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
			11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
			31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n\
			51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n\
			71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n81\n82\n83\n84\n85\n86\n87\n88\n89\n90\n\
			91\n92\n93\n94\n95\n96\n97\n98\n99\n100\n101\n102\n103\n104\n105\n106\n107\n108\n109\n110\n\
			111\n112\n113\n114\n115\n116\n117\n118\n119\n120\n");

		NDK_PrnStart();
		sleep(5);//休眠一下,保证5秒内不取打印中间非稳定状态
		if((nRet = NDK_PrnGetStatus(&emStatus))!=NDK_OK || emStatus != PRN_STATUS_NOPAPER)
		{
			send_result("line %d:%s测试失败(%d,%d)",__LINE__,TESTAPI,nRet,emStatus);
			goto prnt_error;
		}
#if ANDIROD_ENABLE
		//新增NDK_PrnStr函数返回打印机状态的用例 20170220 linwl
		if((nRet = NDK_PrnStr("不应该打印出本行汉字,如果打印出则表示测试失败\n"))!=PRN_STATUS_NOPAPER)
		{
			send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
			goto prnt_error;
		}
#endif	
	}
	
	//case2:装好纸,检测状态
	cls_show_msg("确认打印机装好纸后,按任意键开始");
	if((nRet = NDK_PrnGetStatus(&emStatus)) != NDK_OK||emStatus!=PRN_STATUS_OK) 
	{
		send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
		goto prnt_error;
	}
	
	//case3:打印机处于过热状态(留压力测试做)
	
	
	//case4:打印机忙
	if(g_PrnSwitch==1)//开启边送边打
	{
		NDK_PrnStr ("打印测试页1:将打印1---50行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
			11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
			31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n");

#if 0
		if((nRet = NDK_PrnStart()) != NDK_OK)
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,nRet);
			goto prnt_error;
		}
#endif		

		if(NDK_PrnGetStatus(&emStatus) != NDK_OK || emStatus != PRN_STATUS_BUSY)
		{
			send_result("line %d:%s测试失败(%d,%d)",__LINE__,TESTAPI,nRet,emStatus);
			goto prnt_error;
		}
#if 0//ANDIROD_ENABLE //根据秋云跟开发沟通结果此处不会返回忙的状态20180111
		//新增NDK_PrnStr函数返回打印机状态的用例 20170220 linwl
		if((nRet = NDK_PrnStr("不应该打印出本行汉字,如果打印出则表示测试失败\n"))!=PRN_STATUS_BUSY)
		{
			send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
			goto prnt_error;
		}
#endif		
		old = time(NULL);
		while(1)
		{
			if(NDK_PrnGetStatus(&emStatus) != NDK_OK )
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			if(emStatus == PRN_STATUS_OK)
				break;
			else if((diff=time(NULL)-old)>60)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			show_stopwatch(ENABLE, diff);
		}
		show_stopwatch(DISABLE, 0);
	}

	//case5:把POS放置冰箱中低温环境下应该能获取到状态PRN_STATUS_DESTROYED
	//低温环境比较难构造,机型不同,需要的低温不同,故改成拆除打印机来测20180119 
	if(cls_show_msg("是否进行拆除打印机测试,[确认]测试,[其他]取消")==ENTER)
	{
		cls_show_msg("把打印机拆除后,按任意键开始");
		if((nRet = NDK_PrnGetStatus(&emStatus)) != NDK_OK ||emStatus != PRN_STATUS_DESTROYED)
		{
			send_result("line %d:%s测试失败(%d, %d)",__LINE__,TESTAPI,nRet,emStatus);
			goto prnt_error;
		}
	}
	
	send_result("%s测试通过", TESTAPI);	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}

