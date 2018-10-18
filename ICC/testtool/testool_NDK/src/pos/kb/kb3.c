/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 键盘
* file name		: 3.c
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 测试NDK_KbGetInput
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  MAXWAITTIME 30
#define 	TESTAPI		"NDK_KbGetInput"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: kb3
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*
*****************************************************************/
void kb3(void)
{
	/*private & local definition*/
	int nKeyin = ENTER, ret = 0, count = 0;
	char pszBuf[200] = {0}; 
	char szbuf[200] = {0}; 
	uint unMin = 0, unMaxlen = 10, punLen = 0; 
	EM_INPUTDISP emMode = 0; 
	EM_INPUT_CONTRL emControl = 0;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:参数异常:pszBuf,punLen分别为NULL,emMode,emControl分别为-1,5
	//case1.1:pszBuf,punLen分别为NULL
	memset(pszBuf, 0, sizeof(pszBuf));
	if ((ret=NDK_KbGetInput(NULL,unMin,unMaxlen,&punLen,emMode,MAXWAITTIME,emControl))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case1.2:unMin>unMaxlen,unMaxlen为0的情况
	if ((ret=NDK_KbGetInput(pszBuf,6,5,&punLen,emMode,MAXWAITTIME,emControl))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:测试了取值INPUTDISP_NORMAL能显示字符,根据emControl参数分4种情况
	//case2.1:INPUT_CONTRL_NOLIMIT：任意ASCII码字符，输满后直接返回
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输10个字符(请混合数字,小数点,逗号,小写字母)将正常显示,输10字符后自动返回.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_NOLIMIT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punLen,strlen(pszBuf));
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case2.2:INPUT_CONTRL_LIMIT：只读数字与小数点，输满后直接返回 
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输10个字符(请混合数字,小数点)将正常显示,输10字符后自动返回.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_LIMIT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case2.3:INPUT_CONTRL_NOLIMIT_ERETURN：任意ASCII码字符，输满后等待确认键返回
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输10个字符(请混合数字,小数点,逗号,小写字母)将正常显示,输10字符后请按确认键返回.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case2.4:INPUT_CONTRL_LIMIT_ERETURN，只读数字与小数点，输满后等待确认键返回 
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输10个字符(请混合数字,小数点)将正常显示,输10字符后请按确认键返回.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_LIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case3:测试了取值INPUTDISP_PASSWD能显示'*'	,根据emControl参数分4种情况
	//case3.1:INPUT_CONTRL_NOLIMIT：任意ASCII码字符，输满后直接返回
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输10个字符(请混合数字,小数点,逗号,小写字母)将显示*号,输10字符后自动返回.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_PASSWD,MAXWAITTIME,INPUT_CONTRL_NOLIMIT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case3.2:INPUT_CONTRL_LIMIT：只读数字与小数点，输满后直接返回 
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输10个字符(请混合数字,小数点)将显示*号,输10字符后自动返回.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_PASSWD,MAXWAITTIME,INPUT_CONTRL_LIMIT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case3.3:INPUT_CONTRL_NOLIMIT_ERETURN：任意ASCII码字符，输满后等待确认键返回
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输10个字符(请混合数字,小数点,逗号,小写字母)将显示*号,输10字符后请按确认键返回.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_PASSWD,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case3.4:INPUT_CONTRL_LIMIT_ERETURN，只读数字与小数点，输满后等待确认键返回 
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输10个字符(请混合数字,小数点)将显示*号,输10字符后请按确认键返回.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_PASSWD,MAXWAITTIME,INPUT_CONTRL_LIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case4:测试了取值INPUTDISP_OTHER能显示已经从键盘上输入的数据,并且用明码显示出来,根据emControl参数分4种情况
	//case4.1:INPUT_CONTRL_NOLIMIT：任意ASCII码字符，输满后直接返回
	memset(pszBuf, 0, sizeof(pszBuf));
	count = rand()%(unMaxlen-1)+1;
	memset(pszBuf, 'a', count);
	cls_show_msg("光标前为缓冲已有字符(%s),(请混合数字,小数点,逗号,小写字母)将正常显示,输满10字符后自动返回.任意键继续...", pszBuf);
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_OTHER,MAXWAITTIME,INPUT_CONTRL_NOLIMIT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case4.2:INPUT_CONTRL_LIMIT：只读数字与小数点，输满后直接返回 
	memset(pszBuf, 0, sizeof(pszBuf));
	strcpy(pszBuf, "01234");
	cls_show_msg("光标前为缓冲已有字符(%s),(请混合数字,小数点)将正常显示,输满10字符后自动返回.任意键继续...", pszBuf);
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_OTHER,MAXWAITTIME,INPUT_CONTRL_LIMIT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case4.3:INPUT_CONTRL_NOLIMIT_ERETURN：任意ASCII码字符，输满后等待确认键返回
	memset(pszBuf, 0, sizeof(pszBuf));
	count = rand()%(unMaxlen-1)+1;
	memset(pszBuf, 'a', count);
	cls_show_msg("光标前为缓冲已有字符(%s),(请混合数字,小数点,逗号,小写字母)将正常显示,输满10字符后请按确认键返回.任意键继续...", pszBuf);
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_OTHER,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case4.4:INPUT_CONTRL_LIMIT_ERETURN，只读数字与小数点，输满后等待确认键返回 
	memset(pszBuf, 0, sizeof(pszBuf));
	strcpy(pszBuf, "01234");
	cls_show_msg("光标前为缓冲已有字符(%s),(请混合数字,小数点)将正常显示,输满10字符后请按确认键返回.任意键继续...", pszBuf);
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_OTHER,MAXWAITTIME,INPUT_CONTRL_LIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case5:测试输入的字符可以通过退格键取消
	memset(pszBuf, 0, sizeof(pszBuf));
	strcpy(pszBuf, "abcdef");
	cls_printf("请输入超过一行的任意字符后,按[BASP]改为%s,再[确认]输入\n", "10086");
#if defined IM81AZ
	cls_printf("光标前为缓冲已有字符(%s),按[BASP]改为%s,再[确认]输入\n", pszBuf, "10086");
#endif
	if ((ret=NDK_KbGetInput(pszBuf,unMin,60/*30*/,&punLen,INPUTDISP_OTHER,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)//彩屏设备30个字符不满一行,所以改成60个字符 20140611 linwl modified
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	if(strcmp("10086", pszBuf))
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//用以区别子case间相同的操作提示信息

	//case13:测试默认缓冲中存在超过1行的字符，退格键是否可以正常删除 //20150312低端平台发现当缓冲数据超过1行时退格回删无法退到上一行 linwl
	memset(pszBuf, 0, sizeof(pszBuf));
	strcpy(pszBuf, "01234567890123456789012345678901234567890123456789");
	cls_printf("请按[BASP]改为%s,再[确认]输入\n", "10086");
#if defined IM81AZ
	cls_printf("光标前为缓冲已有字符(%s),按[BASP]改为%s,再[确认]输入\n", pszBuf, "10086");
#endif
	if ((ret=NDK_KbGetInput(pszBuf,unMin,60/*30*/,&punLen,INPUTDISP_OTHER,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)//彩屏设备30个字符不满一行,所以改成60个字符 20140611 linwl modified
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	if(strcmp("10086", pszBuf))
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//用以区别子case间相同的操作提示信息

	//case12:测试输入的字符达到最大值后可以通过取消键修改,而不会直接退出
	memset(pszBuf, 0, sizeof(pszBuf));
	strcpy(pszBuf, "abcdef");
	cls_printf("请输入10个字符后,按[BASP]改为%s,再[确认]输入\n", "0123456789");
#if defined IM81AZ
	cls_printf("光标前为缓冲已有字符(%s),请再输入5个字符，按10次[BASP]改为%s,再[确认]输入\n", pszBuf, "0123456789");
#endif
	if ((ret=NDK_KbGetInput(pszBuf,10,10,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	if(strcmp("0123456789", pszBuf))
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	cls_show_msg1(2, "子用例测试通过!");//用以区别子case间相同的操作提示信息

	//case6:测试输入满后只接受确认键,不支持确认，取消和退格以外的功能键
	memset(pszBuf, 0, sizeof(pszBuf));
	strcpy(pszBuf, "0123456789");
	cls_printf("先尝试输入除[ESC][ENTER][BASP]外的键,再按[ENTER]!\n");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_OTHER,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(pszBuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}
	if (strcmp(pszBuf, "0123456789"))
	{
		send_result("line %d: getRETURNline(2)测试失败", __LINE__);
		RETURN;
	}

	//case7:测试快速按键时会不会丢键 zhengdc 20120719 add
	memset(szbuf, 0, sizeof(szbuf));
	cls_show_msg("最多输100个(请连续快速输入191919191919),将正常显示,要确认输入结束,任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(szbuf,unMin,100,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(punLen!=strlen(szbuf))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, punLen);
		RETURN;
	}

	//case13:长按住除5、0以外的任一数字键（模拟卡键），再按其它数字键,获取到的键值不应为没有按下的其它键值,
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("长按住除5、0以外的任一数字键（模拟卡键），再按其它数字键,获取到的键值不应为没有按下的其它键值,任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case8:测试ESC键返回操作失败
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_show_msg("请输少于10个字符(请混合数字,小数点,逗号,小写字母)将正常显示,输入过程中按[ESC]键.任意键继续...");
	cls_printf("请输入");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_NOLIMIT))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(1,"ESC键取消输入测试通过");
/*//函数已经返回失败，就不再关心缓冲内是否还有其他数据 20140806
	nKeyin = cls_show_msg1(MAXWAITTIME, "输入串为%s,"
	"通过按确认,否则按其它键", strlen(pszBuf)?pszBuf:"空");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
*/	
	//case9:测试超时退出，和超时时间准确性//由于超时参数不是进入函数就开始计时,而是按键最大间隔时间,故不测试超时的准确度
	memset(pszBuf, 0, sizeof(pszBuf));
	cls_printf("请输入1.3后(不按[确认]),耐心等待返回...\n");
	lib_StartStopwatch();
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_NOLIMIT))!=NDK_ERR_TIMEOUT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case10:unWaittime参数为-1时,是否会跑飞
	cls_printf("请按[确认]返回...\n");
	if ((ret=NDK_KbGetInput(pszBuf,unMin,unMaxlen,&punLen,emMode,-1,emControl))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

#if 0
	//case11:测试键盘输入回绕功能
	j = GUI_GetLCDHeight()-2*GUI_GetFontHeight();
	cls_printf("测试键盘输入回绕功能,请连续输入任意字符,输入完毕按[确认]");
	NDK_ScrGotoxy(0, j);
	if ((ret=NDK_KbGetInput(szbuf,50,200,&punLen,INPUTDISP_NORMAL,MAXWAITTIME,INPUT_CONTRL_NOLIMIT_ERETURN))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
		
	nKeyin = cls_show_msg1(MAXWAITTIME, "请确认显示结果.通过按[确认],否则按[其它]");
	if (nKeyin!=0 && nKeyin!=ENTER)
	{
		send_result("line %d: %s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
#endif
	
	send_result("%s测试通过", TESTAPI);
	return;
}

