/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 键盘
* file name		: 4.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_KbHZInput能否进行函数输入
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define 	TESTAPI		"NDK_KbHZInput"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: kb4
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*****************************************************************/
void kb4(void)
{
	/*private & local definition*/
	int ret = 0, nKeyin = 0, i = 0;	
	char pszS[200] = {0},szmessage[200]={"请验证支持在输入过程中和输入完字符后按退格键修改功能"};
	char szmessage2[200]={"请验证支持在输入过程中按取消键退出"};
	uint unMaxlen = 20;
	EM_IME emMethod = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//case1:参数异常:pszS为NULL
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(NULL,unMaxlen,emMethod))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//"NDK_ERR_MACLLOC 内存空间不足" 此返回值构造不出来 20140922 jiangym add
	
	//case13:对不支持手写输入法的测试手写输入法会返回NDK_ERR_NOT_SUPPORT   20140922 jiangym add
	if(cls_show_msg("此处需要使用不支持手写输入法的机器测试,[ESC]跳过,[其它]键继续")!=ESC)
	{
		memset(pszS, 0, sizeof(pszS));
		if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_HANDWRITE))!=NDK_ERR_NOT_SUPPORT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
	}
	
	//case2:测试数拼输入法IME_NUMPY
	cls_show_msg1(3,"测试数拼输入法,请输入任意汉字,如,\"仡\"(yi)\"单人旁月\"(wo)");///added 20130905支持GB18030能输入仴
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_NUMPY))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);

	//case3:测试英文输入法IME_ENGLISH
	cls_show_msg1(3,"测试英文输入法,请输入任意英文,如,abcdef");
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_ENGLISH))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);
	
	//case4:测试数字输入法IME_NUM
	cls_show_msg1(3,"测试数字输入法,请输入任意数字,如,\"4\"");
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_NUM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);

#if !defined ME50//支持汉字输入,可以支持数拼、数字、英文输入法
	//case5:测试笔画输入法IME_BIHUA
	cls_show_msg1(3,"测试笔画输入法,请输入任意汉字,如,\"林\"(12341234)");
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_BIHUA))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case11:测试笔画输入法IME_BIHUA,无效输入情况
	cls_show_msg1(10,"请按[1横2竖3撇4捺5折]以外的按键,如输入6,7,8,9,*,F1,F2,F3,F4均为无效按键,再输入\"林\"(12341234)");
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_BIHUA))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strcmp(pszS,"林"))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, pszS);
		RETURN;
	}
	
	//case6:测试区位输入法IME_QUWEI
	cls_show_msg1(3,"测试区位输入法,如,\"抖\"(2222)");
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_QUWEI))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);

	//case11:测试内码输入法IME_GBK
	//cls_show_msg1(3,"测试内码输入法,请输入任意汉字,\"剑\"(bda3)");
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_GBK))!=NDK_ERR_PARA/*NDK_OK*/)//NDK5.0已不支持内码,故返回失败20140822 jiangym
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);

	if(cls_show_msg("是否支持手写输入法,是按[确认],否则按[其它]")==ENTER)
	{
		//case12:测试手写输入法IME_HANDWRITE
		cls_show_msg1(3,"测试手写输入法,如,\"我\"");
		memset(pszS, 0, sizeof(pszS));
		if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_HANDWRITE))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);
	}
#endif

	//case7:测试EM_IME之外的其他值，则该函数默认激活数拼输入法
	cls_show_msg1(3,"测试数拼输入法,请输入任意汉字,如,\"仡\"(yi)");
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_MAXNUM+1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);

	//case8:测试在增加开启输入法的情况下，按数字键“0”出现标点符号，按方向键进行左右移动选择符号，按确认键返选定符
	cls_show_msg1(5,"测试标点输入,请按0键,并可按[菜单]翻页,方向键移动选择符号,按确认键返选定符");
	memset(pszS, 0, sizeof(pszS));
	if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_MAXNUM+1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);

	//case9: 支持对输入外码的回删以进行修改的功能（无论什么输入法）；
	for(i=0;i<IME_MAXNUM;i++)
	{
		memset(pszS,0,sizeof(pszS));
		switch(i)
		{
			case 0:
				strcpy(pszS,"数拼测试,请输入任意汉字,如,\"仡\"(yi),");
				strcat(pszS,szmessage);
				break;
			case 1:
				strcpy(pszS,"英文测试,请输入任意英文,如,abcdef,");
				strcat(pszS,szmessage);
				break;
			case 2:
				strcpy(pszS,"数字测试,请输入任意数字,如,\"4\",");
				strcat(pszS,szmessage);
				break;
			case 3:
				strcpy(pszS,"笔画测试,请输入任意汉字,如,\"林\"(12341234),");
				strcat(pszS,szmessage);
				break;
			case 4:
				strcpy(pszS,"区位测试,请输入任意汉字,\"抖\"(2222),");
				strcat(pszS,szmessage);
				break;
			case 5://NDK5.0已经不支持内码
				//strcpy(pszS,"内码测试,请输入任意汉字,\"剑\"(bda3),");
				//strcat(pszS,szmessage);
				continue/*break*/;
			case 6:
				strcpy(pszS,"手写测试,请输入任意汉字,\"我\",");
				strcat(pszS,szmessage);
				break;
			default:
				break;
		}
		cls_show_msg1(MAXWAITTIME,pszS);
		memset(pszS,0,sizeof(pszS));
		if ((ret=NDK_KbHZInput(pszS,unMaxlen,i))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		cls_show_msg1(MAXWAITTIME, "您输入的是%s,任意键继续测试...", pszS);
	}

	//case10:支持在输入外码过程中按ESC直接退出输入状态（无论什么输入法，尤其关注与汉字相关的输入法）
	for(i=0;i<IME_MAXNUM;i++)
	{
		memset(pszS,0,sizeof(pszS));
		switch(i)
		{
			case 0:
				strcpy(pszS,"数拼测试,请输入任意汉字,如,\"仡\"(yi),");
				strcat(pszS,szmessage2);
				break;
			case 1:
				strcpy(pszS,"英文测试,请输入任意英文,如,abcdef,");
				strcat(pszS,szmessage2);
				break;
			case 2:
				strcpy(pszS,"数字测试,请输入任意数字,如,\"4\",");
				strcat(pszS,szmessage2);
				break;
			case 3:
				strcpy(pszS,"笔画测试,请输入任意汉字,如,\"林\"(12341234),");
				strcat(pszS,szmessage2);
				break;
			case 4:
				strcpy(pszS,"区位测试,请输入任意汉字,\"抖\"(2222),");
				strcat(pszS,szmessage2);
				break;
			case 5: //NDK5.0已经不支持内码
				//strcpy(pszS,"内码测试,请输入任意汉字,\"剑\"(bda3),");
				//strcat(pszS,szmessage2);
				continue/*break*/;
			case 6:
				strcpy(pszS,"手写测试,请输入任意汉字,\"我\",");
				strcat(pszS,szmessage2);
				break;
			default:
				break;
		}
		cls_show_msg1(MAXWAITTIME,pszS);
		memset(pszS,0,sizeof(pszS));
		if ((ret=NDK_KbHZInput(pszS,unMaxlen,i))!=NDK_ERR_QUIT/*NDK_OK*/)//应用需求NDK_KbHZInput在按ESC键取消输入时，NDK接口要返回NDK_ERR_QUIT linwl 20140102
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
	}

	//测试结束
	nKeyin = cls_show_msg1(MAXWAITTIME, "按确认键测试通过,否则按其它键");	
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	
	//zhangnw20100128 add,通过黑盒测试测出字库或操作字库代码的问题(以往经验:1.字库不全;2.代码错误).
	GUI_DispStringEx(0, 0, "提示:另请使用[自检]中的[输入法]对拼音,笔画等做全部可能组合的尝试输入!", TEXT_ATTRIBUTE_REVERSE);
	show_msg1(MAXWAITTIME, "任意键继续...");
	return;
}

