/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: 16.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试 NDK_ScrSetFontType,NDK_ScrGetFontType能否设置和获取使用的显示字体,NDK_ScrGetFontSize能否获取当前系统使用的汉字显示字体宽和高
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 10//30
#if K21_ENABLE&&!(defined SP610||defined ME62)
#define 	TESTAPI		"NDK_ScrGetFontSize"//K21仅支持NDK_ScrGetFontSize函数，其它两个函数不支持
#elif (defined SP610||defined ME62)
#define 	TESTAPI		"NDK_ScrSetFontType,NDK_ScrGetFontSize"
#else
#define 	TESTAPI		"NDK_ScrSetFontType,NDK_ScrGetFontType,NDK_ScrGetFontSize"
#endif

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr16
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
#if K21_ENABLE&&!(defined SP610||defined ME62)
void scr16(void)
{
	/*private & local definition*/
	int ret = 0;
	uint punWidth = 0, punHeight = 0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	//参数为NULL，表示不用获取该项值，所以返回成功
	if((ret=NDK_ScrGetFontSize(NULL,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	send_result("%s测试通过,该设备屏幕汉字点阵宽为:%d,高为:%d,请人工判断是否正确", TESTAPI, punWidth, punHeight);
	return;
}
//目前SP610和ME62也支持NDK_ScrSetFontType故增加测试 20180712 modify
#elif  (defined SP610||defined ME62)
void scr16(void)
{
	/*private & local definition*/
	int ret = 0;
	uint punWidth = 0, punHeight = 0;
	int i = 0, j = 0;
	uint unMaxlen = 20;
	char EnumName[][20] = {"默认字体","额外字体","用户自定义字体","24*24字体"};
	char pszS[200] = {0};	
	const char pszS3[] = "特殊字符`~!@#$%^&*() -_=+][{}:;”’|\\?/><.,"; 	
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:参数异常:emType为-1;punWidth,punHeight分别为NULL
	//参数emType为-1，应该返回失败//底层会用一个合理的类型进行操作，返回成功
	if((ret=NDK_ScrSetFontType(-1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//只支持DISPFONT_CUSTOM和DISPFONT_ASC12X24，其他枚举值设置会失败
	//修改为支持DISPFONT_CUSTOM和DISPFONT_ASC12X24和DISPFONT_EXTRA 20180820 modify
/*	if((ret=NDK_ScrSetFontType(DISPFONT_EXTRA))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}*/
	if((ret=NDK_ScrSetFontType(DISPFONT_USER))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//参数为NULL，表示不用获取该项值，所以返回成功
	if((ret=NDK_ScrGetFontSize(NULL,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:设置系统默认,并获取字体的宽和高32*32
	if((ret=NDK_ScrSetFontType(DISPFONT_CUSTOM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(punWidth != 32 || punHeight != 32)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
		GOTOERR;
	}
	
	//case7:新增设置DISPFONT_EXTRA,并获取字体的宽和高32*32  20180820 add
	if((ret=NDK_ScrSetFontType(DISPFONT_EXTRA))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(punWidth != 32 || punHeight != 32)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
		GOTOERR;
	}

	//case3:设置24*24,并获取字体的宽和高
	if((ret=NDK_ScrSetFontType(DISPFONT_ASC12X24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(punWidth != 24 || punHeight != 24)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
		GOTOERR;
	}
	
	//case4:恢复设置系统默认,并获取字体的宽和高32*32
	if((ret=NDK_ScrSetFontType(DISPFONT_CUSTOM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(punWidth != 32 || punHeight != 32)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
		GOTOERR;
	}

	if(auto_flag==0||auto_flag==2)
	{
		//case5:设置字体后，使用输入法,输入的字大小会变化的(目前额外字体和用例自定义字体会保持上一次的设置)
		if((ret=NDK_ScrSetFontType(DISPFONT_CUSTOM))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		memset(pszS, 0, sizeof(pszS));
		cls_show_msg("使用字母键切换任意输入法,输入任意字符,并判断输入栏字符是否为默认字体,任意键继续");
		if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_ENGLISH/*IME_NUMPY*/))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg1(2, "接下来切换成24*24字体");
		if((ret=NDK_ScrSetFontType(DISPFONT_ASC12X24))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		memset(pszS, 0, sizeof(pszS));
		cls_show_msg("使用字母键切换任意输入法,输入任意字符,并判断输入栏字符是否为24*24字体,任意键继续");
		if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_ENGLISH/*IME_NUMPY*/))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}	
		cls_show_msg1(2, "接下来切换成DISPFONT_EXTRA字体");
		if((ret=NDK_ScrSetFontType(DISPFONT_EXTRA))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		memset(pszS, 0, sizeof(pszS));
		cls_show_msg("使用字母键切换任意输入法,输入任意字符,并判断输入栏字符是否为DISPFONT_EXTRA字体,任意键继续");
		if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_ENGLISH/*IME_NUMPY*/))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}	
		if(cls_show_msg("输入的字符大小是否随着设置的字体大小变化,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case6:设置小中大字体后，输出特殊字符看看是否为正确字符	 
		//测试时请关注屏幕显示的结果是否为“特殊字符`~!@#$%^&*() -_=+][{}:;”’|\?/><.,”不应该出现显示错误的情况，例如将“\”显示为“￥”
		if((ret=NDK_ScrSetFontType(DISPFONT_CUSTOM))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("检查屏幕的\"%s\",显示均正确按[确认];否则按其它键.", pszS3)!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
		cls_show_msg1(2, "默认字体模式测试通过!");
		if((ret=NDK_ScrSetFontType(DISPFONT_ASC12X24))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("检查屏幕的\"%s\",显示均正确按[确认];否则按其它键.", pszS3)!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
		cls_show_msg1(2, "24*24字体模式测试通过!");
		if((ret=NDK_ScrSetFontType(DISPFONT_EXTRA))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(cls_show_msg("检查屏幕的\"%s\",显示均正确按[确认];否则按其它键.", pszS3)!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
		cls_show_msg1(2, "DISPFONT_EXTRA字体模式测试通过!");
	}
	send_result("%s测试通过", TESTAPI);
	//恢复默认设置
ERR:
	NDK_ScrSetFontType(DISPFONT_CUSTOM);
	return;
}
#else
void scr16(void)
{
	/*private & local definition*/
	int ret = 0;
	uint punWidth = 0, punHeight = 0, puncd = 0;
	EM_DISPFONT oldfonttype = 0;
	int i = 0, j = 0;
	uint unMaxlen = 20;
	char EnumName[][20] = {"默认小字体","额外中字体","额外大字体","用户自定义字体"};
	EM_DISPFONT fonttype = 0;
	char pszS[200] = {0};	
	const char pszS3[] = "特殊字符`~!@#$%^&*() -_=+][{}:;”’|\\?/><.,";//20150204新增特殊字符显示 	
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置:判断液晶屏类型,用于默认值的验证
	if((ret=NDK_ScrGetColorDepth(&puncd))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_ScrGetFontType(&oldfonttype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, oldfonttype);
		GOTOERR;
	}

	//case1:参数异常:emType为-1;punWidth,punHeight分别为NULL
	//参数emType为-1，应该返回失败//底层会用一个合理的类型进行操作，返回成功
	if((ret=NDK_ScrSetFontType(-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontType(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//参数为NULL，表示不用获取该项值，所以返回成功
	if((ret=NDK_ScrGetFontSize(NULL,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:设置系统默认,并获取字体的宽和高
	if((ret=NDK_ScrSetFontType(DISPFONT_CUSTOM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_ScrGetFontType(&fonttype))!=NDK_OK)||(fonttype!=DISPFONT_CUSTOM))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, fonttype);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	switch(puncd)
	{
		case 16://通过液晶类型区分默认字体宽和高 16为彩屏,字体为16*16
			if(punWidth != 16 || punHeight != 16)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
				GOTOERR;
			}
			break;
		case 1:
		case 4://通过液晶类型区分默认字体宽和高 1,4为黑白屏,字体为12*12
			if(punWidth != 12 || punHeight != 12)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
				GOTOERR;
			}
			break;
		default:
			send_result("line %d:%s未知的屏幕类型%d(%d,%d)", __LINE__, TESTAPI, puncd, punWidth, punHeight);
			GOTOERR;
	}
	
	//case3:设置额外尺寸(中),并获取字体的宽和高
	if((ret=NDK_ScrSetFontType(DISPFONT_EXTRA))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_ScrGetFontType(&fonttype))!=NDK_OK)||(fonttype!=DISPFONT_EXTRA))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, fonttype);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	switch(puncd)
	{
		case 16://通过液晶类型区分默认字体宽和高 16为彩屏,字体为24*24
			if(punWidth != 24 || punHeight != 24)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
				GOTOERR;
			}
			break;
		case 1:
		case 4://通过液晶类型区分默认字体宽和高 1,4为黑白屏,字体为16*16
			if(punWidth != 16 || punHeight != 16)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
				GOTOERR;
			}
			break;
		default:
			send_result("line %d:%s未知的屏幕类型%d(%d,%d)", __LINE__, TESTAPI, puncd, punWidth, punHeight);
			GOTOERR;
	}

	//case4:设置额外尺寸(大),并获取字体的宽和高
	if((ret=NDK_ScrSetFontType(DISPFONT_EXTRA_LARGE))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_ScrGetFontType(&fonttype))!=NDK_OK)||(fonttype!=DISPFONT_EXTRA_LARGE))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, fonttype);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	switch(puncd)
	{
		case 16://通过液晶类型区分默认字体宽和高 16为彩屏,字体为32*32
			if(punWidth != 32 || punHeight != 32)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
				GOTOERR;
			}
			break;
		case 1:
		case 4://通过液晶类型区分默认字体宽和高 1,4为黑白屏,字体为16*16
			if(punWidth != 16 || punHeight != 16)
			{
				send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punWidth, punHeight);
				GOTOERR;
			}
			break;
		default:
			send_result("line %d:%s未知的屏幕类型%d(%d,%d)", __LINE__, TESTAPI, puncd, punWidth, punHeight);
			GOTOERR;
	}
	
	//case5:设置用户自定义字体,并获取字体的宽和高:自定义模式暂不支持,先用额外模式支持
	if((ret=NDK_ScrSetFontType(DISPFONT_EXTRA))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_ScrGetFontType(&fonttype))!=NDK_OK)||(fonttype!=DISPFONT_EXTRA))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, fonttype);
		GOTOERR;
	}
	if((ret=NDK_ScrGetFontSize(&punWidth,&punHeight))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case6:字体设置切换,测试切换引起的跑飞
	for(i=DISPFONT_CUSTOM;i<=DISPFONT_USER;i++)
	{
		if((ret=NDK_ScrSetFontType(i))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_printf("从%s模式切换到",EnumName[i]);
		
		for(j=DISPFONT_CUSTOM;j<=DISPFONT_USER;j++)
		{
			if(i>=j)
				continue;
			
			show_msg1(2, "%s模式",EnumName[j]);		
			if((ret=NDK_ScrSetFontType(j))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}

			cls_show_msg1(2, "从%s模式切换到%s模式",EnumName[j],EnumName[i]);
			if((ret=NDK_ScrSetFontType(i))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			cls_printf("从%s模式切换到",EnumName[i]);
		}
	}
	
	if(auto_flag==0||auto_flag==2)
	{
		//case7:设置小中大字体后，使用输入法,输入的字大小会变化的
		for(i=DISPFONT_CUSTOM;i<=DISPFONT_EXTRA_LARGE;i++)
		{
			if((ret=NDK_ScrSetFontType(i))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			memset(pszS, 0, sizeof(pszS));
#if OVERSEAS_ENABLE  //海外产品不支持NDK_KbHZInput  20180427 modify
			cls_show_msg("请输入不多于20个字符(可混合数字,小数点,逗号,小写字母),并判断输入栏字符是否为%s.任意键继续...",EnumName[i]);
			if ((ret=NDK_KbGetInput(pszS,0,unMaxlen,&puncd,INPUTDISP_NORMAL,30,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
#else
			cls_show_msg("使用字母键切换任意输入法,输入任意字符,并判断输入栏字符是否为%s,任意键继续",EnumName[i]);
			if ((ret=NDK_KbHZInput(pszS,unMaxlen,IME_ENGLISH/*IME_NUMPY*/))!=NDK_OK)//海外产品不支持数拼,改成通用的英语 20180309modify
#endif
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}	
		}
		if(cls_show_msg("输入的字符大小是否随着设置的字体大小变化,是[Enter],否[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case7:设置小中大字体后，输出特殊字符看看是否为正确字符	20150204 add jiangym,
		//测试时请关注屏幕显示的结果是否为“特殊字符`~!@#$%^&*() -_=+][{}:;”’|\?/><.,”不应该出现显示错误的情况，例如将“\”显示为“￥”
		for(i=DISPFONT_CUSTOM;i<=DISPFONT_EXTRA_LARGE;i++)
		{
			if((ret=NDK_ScrSetFontType(i))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			if(cls_show_msg("检查屏幕的\"%s\",显示均正确按[确认];否则按其它键.", pszS3)!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
			}
			cls_show_msg1(2, "%s模式测试通过!",EnumName[i]);
		}
	}
	send_result("%s测试通过", TESTAPI);
	//恢复默认设置
ERR:
	NDK_ScrSetFontType(oldfonttype);
	return;
}
#endif
