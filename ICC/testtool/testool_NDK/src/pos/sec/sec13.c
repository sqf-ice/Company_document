/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec13.c
* Author 		: 
* version		: 
* DATE			:
* directory 		: 
* description		: 测试 NDK_SecVerifyCipherPin 接口功能
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
#define TESTAPI	"NDK_SecVerifyCipherPin"	
#define PINTIMEOUT_MAX	(200*1000)
#define PINTIMEOUT_MIN	(5*1000)

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*			                sull      20180427       created
*****************************************************************/
//只能简单的验证函数的本身,对于函数返回值不能正确验证,响应码是会根据银行卡正确的密码进行正确响应的
void sec13(void)
{
	int ret = -1, nStatus = 0;
	uchar sAtrbuf[32] = {0}, sSW[8+1] = {0};
	int nAtrLen = 0;
	ST_SEC_RSA_KEY RsaKey;
	const uchar mod[]= {
		0x96,0x92,0xF2,0xDC,0x0D,0xFE,0xA1,0x34,0xF6,0xD5,0xDA,0xF7,0x56,0x34,0xCA,0xEC,
		0xC8,0x55,0xEC,0x77,0x4F,0x4B,0xFE,0x6F,0x90,0xDD,0xD4,0x32,0xB5,0xDB,0x1C,0xDC,
		0xFB,0xAC,0x96,0x98,0x02,0xC6,0x4A,0x60,0x69,0xFD,0x26,0x7C,0x41,0xC1,0xAC,0x82,
		0xE3,0x8E,0xA2,0x54,0xF4,0xAA,0x4B,0xD0,0x9B,0x04,0x51,0x6C,0x19,0xE1,0x8A,0xC5
	};
	const uchar exp[]= {0x00,0x00,0x03};
	
	cls_printf("%s测试中...", TESTAPI);

	//测试前置:擦除所有密钥
	if((ret = NDK_SecKeyErase()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	//不同ic卡此处的密钥可能不同,这里暂且使用此密钥验证函数
	memset(&RsaKey, 0,sizeof(ST_SEC_RSA_KEY));
	RsaKey.usBits=RSA_KEY_LEN_512;
	memcpy((char *)RsaKey.sExponent, exp, sizeof(exp));
	memcpy(RsaKey.sModulus, mod, sizeof(mod));

	//case1:不初始化触屏获取屏幕按键值，则返回初始化失败
#if ANDIROD_ENABLE && !defined IM81AZ
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"0", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MIN)) != NDK_ERR_SECVP_NOT_ACTIVE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case2:参数非法:超时时间不在合法范围内
	if((ret = touchscrean_getnum()) != NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"4,5,6,7,8,9,10,11,12", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MIN-1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = touchscrean_getnum()) != NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"4,5,6,7,8,9,10,11,12", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX+1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:参数非法:pszExpPinLenIn=NULL
	if((ret = touchscrean_getnum()) != NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
	if((ret = NDK_SecVerifyCipherPin(0, NULL, &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:未插IC卡的情况下进行脱机PIN运算，应失败
	if((ret = touchscrean_getnum()) != NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"0", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MIN)) != NDK_ERR_SECVP_CUSTOMERCARDNOTPRESENT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:IC卡上电后拔掉IC卡再进行脱机PIN运算，应失败
	cls_show_msg("请插IC卡，插完卡后按任意键继续...");
	memset(sAtrbuf, 0, sizeof(sAtrbuf));
	if((ret = NDK_IccPowerUp(ICTYPE_IC, sAtrbuf, &nAtrLen)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请拔IC卡，按任意键继续...");
	if((ret = touchscrean_getnum()) != NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"4,5,12", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MIN)) != NDK_ERR_SECVP_CUSTOMERCARDNOTPRESENT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_IccPowerDown(ICTYPE_IC);

	//测试前置:IC卡上电
	cls_show_msg("请插IC卡，插完卡后按任意键继续...");
	if((ret = NDK_IccPowerUp(ICTYPE_IC, sAtrbuf, &nAtrLen)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if ANDIROD_ENABLE
	//case6:参数非法:采用EMV外的模式进行测试
	memset(sSW, 0, sizeof(sSW));
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"0", &RsaKey, sSW, ICC_EMV_MODE-1, PINTIMEOUT_MAX)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"4", &RsaKey, sSW, ICC_EMV_MODE+1, PINTIMEOUT_MAX)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case7:正常测试:在pin输入过程中不应进入休眠
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if !K21_ENABLE//低端没有自动休眠功能
	cls_printf("请等待60秒后按[确认],等待过程中不应该进入休眠");
#else
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尽快按[确认]...");
#else
	cls_printf("请尽快按[确认]...");
#endif
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"0", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !K21_ENABLE//低端没有自动休眠功能	
	if(cls_show_msg("看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif	
	cls_show_msg1(2, "子用例通过!");

	//case8:验证输入长度限制出现重复"0,0,0"预期按确认返回NDK_OK，应用发现该情况下按确认会返回失败-1121的BUG，因此导入该用例
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尽快按[确认]...");
#else
	cls_printf("请尽快按[确认]...");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"0,0,0", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case9:pin的输入长度要达到要求，才进行脱机PIN运算
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入123并确认,若无反应继续输4并确认...");
#else
	cls_printf("尽快输入123并确认,若无反应继续输4并确认...");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"4,4", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case10:支持按适当的键（如，按退格）往回删除一位先前输入的PIN码
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尝试输入1237890后,如不能输入再退格改为123456并确认...");
#else
	cls_printf("请尝试输入1237890后,如不能输入再退格改为123456并确认...");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"6", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case11:pin输入最大长度支持到12
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入123456789012并确认...");
#else
	cls_printf("尽快输入123456789012并确认...");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"12", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");

	//case12:pin输入长度为3(合法长度为0,4-12)，应失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"3", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case13:pin输入长度为13(合法长度为0,4-12)，应失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"13", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case14:支持主动退出：按适当的键（如，按取消2次）退出PIN输入状态
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入123456并按2次取消...");
#else
	cls_printf("尽快输入123456并按2次取消...");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"12", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_ERR_SECVP_GET_ESC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//按键保护
	cls_show_msg1(2, "子用例通过!不要再按[取消]");

	//case15:第三个参数传NULL时，为非阻塞模式，应提供取PIN输入过程状态及结果值的功能
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入12345并确认...");
#else
	cls_printf("尽快输入12345并确认...");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"4,5,12", &RsaKey, NULL, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !K21_ENABLE//K21平台不支持忙状态判断 	
	//验证处于PIN输入状态下设备处于忙状态，将不会自动休眠,且NDK_SysGoSuspend增加PIN忙状态返回： NDK_ERR_PIN_BUSY = -3107	
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_PIN_BUSY)
#else
	if((ret=NDK_SysGoSuspend())!=NDK_ERR_PIN_BUSY)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	do {
		ret=NDK_SecGetPinResult(sSW, &nStatus);
		if (ret != 0) {
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
			break;
		}
		if (nStatus == SEC_VPP_KEY_ENTER) { 	/*确认键按下，可返回数据*/
			break;
		} else if (nStatus == SEC_VPP_KEY_ESC) {	/*取消键按下，返回错误*/
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
			break;
		} else if (nStatus == SEC_VPP_KEY_PIN || nStatus == SEC_VPP_KEY_BACKSPACE || nStatus == SEC_VPP_KEY_CLEAR ||  nStatus == SEC_VPP_KEY_NULL) {   
			//由于本函数在存在pinblock时，界面由状态栏处理，这里对其他键不处理
			//继续循环等待
			//由于取PIN函数需要非阻塞，即使没值也要返回应用程序处理（判断是否中断退出），因此增加SEC_VPP_NULL 返回值
		} else {
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
			break;
		}
		NDK_SysMsDelay(10);//usleep(10*1000);
	} while(1);

	cls_show_msg("按任意键,[确认键]除外,应该可以马上进入休眠,休眠后请唤醒");
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	usleep(1000000);
	if((ret=NDK_SysWakeUp()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	if((ret=NDK_SysGoSuspend())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(1);
#endif
	if(cls_show_msg("看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case16:输入PIN的操作，当发生超时时，应退出输入模式，并将合适的返回值或错误码反馈给合适的对象
	memset(sSW, 0, sizeof(sSW));
	cls_printf("请不要按键,耐心等待%ds...", PINTIMEOUT_MIN/1000);
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"4", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MIN)) != NDK_ERR_SECVP_TIMED_OUT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case17:支持清空全部：按适当的键（如，按取消1次或退格）清空先前输入的全部PIN码
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入4321后,再按1次取消,再输入1234并确认...");
#else
	cls_printf("尽快输入4321后,再按1次取消,再输入1234并确认...");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"4", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case18:在不为预期输入位数时候不让确认
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入12345并确认,若无反应继续输6并按确认");
#else
	cls_printf("尽快输入12345并确认,若无反应继续输6并按确认");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"6,4", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case19:在有预期输入位数限定后应不能超过最大值
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(sSW, 0, sizeof(sSW));
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尝试输入123456789,应无法输入9,再按确认");
#else
	cls_printf("请尝试输入123456789,应无法输入9,再按确认");
#endif
	if((ret = NDK_SecVerifyCipherPin(0, (uchar*)"8,6,4", &RsaKey, sSW, 1/*ICC_EMV_MODE*/, PINTIMEOUT_MAX)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//IC卡下电
	if((ret = NDK_IccPowerDown(ICTYPE_IC)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试通过
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	NDK_IccPowerDown(ICTYPE_IC);
	return;	
}



