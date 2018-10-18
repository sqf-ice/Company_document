/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: mag模块
* file name		: mag4.c
* Author 			: chenfm
* version			: 
* DATE			: 20121220
* directory 		: 
* description		: 测试NDK_MagReadNormal接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_MagReadNormal"

extern int test_mag_errcode(int *perrcard1, int *perrcard2, int *perrcard3, int *perrcard4);

/****************************************************************
* function name 	 	: void mag4(void)
* functional description 	: 读取磁卡缓冲区的1、2、3磁道的数据
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  chenfm     20121220	created
*****************************************************************/
void mag4(void)
{
	int ret=-1, ret1=-3, ret2=-3, ret3=-3, pnErrorCode=0;
	int errcard1=0, errcard2=0, errcard3=0, errcard4=0;
	char pszTk1[MAXTRACKLEN]={0}, pszTk2[MAXTRACKLEN]={0}, pszTk3[MAXTRACKLEN]={0};
	char pszTk11[MAXTRACKLEN]={0}, pszTk21[MAXTRACKLEN]={0}, pszTk31[MAXTRACKLEN]={0};
	uchar pcSwiped=-1;

	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_MagClose();

	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:未刷卡，直接读，应失败返回。
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_ERR_NOSWIPED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_MagReset();
	
	//case7:测试在进行异常测试(例如11#卡)后,在进行正常卡测试应能正常读取
#if !(defined ME15CHX||defined ME30MH||defined ME32||defined ME15B||defined N920)
	cls_show_msg("请刷任意一张异常卡后(例如11#),按[ENTER]继续..");
#else
	cls_printf("请刷任意一张异常卡后(例如11#)");
	while(1)
	{
		NDK_MagSwiped(&pcSwiped);
		if(pcSwiped==1)
			break;
	}
#endif
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pnErrorCode);
		GOTOERR;
	}
	NDK_MagReset();
	
	//case2:刷一张123磁卡，应成功返回。
#if !(defined ME15CHX||defined ME30MH||defined ME32||defined ME15B||defined N920)
	cls_show_msg("请刷123磁道卡，刷完卡按[确认]继续..");
#else
	cls_printf("请刷123磁道卡");
	while(1)
	{
		NDK_MagSwiped(&pcSwiped);
		if(pcSwiped==1)
			break;
	}
#endif
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_OK||(pnErrorCode!=0&&pnErrorCode!=3073))//ME11和ME15硬件上不支持1磁道，因此放宽限制允许返回3073 20141015linwl
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pnErrorCode);
		GOTOERR;
	} 
	//以下人工check一次很重要!可检出7e、7f等情况
#if !TK1_DISABLE	//硬件上支持1磁道才检测1磁道数据
	if((ret=check_stripe_result_tk1(pszTk1))!=0x00)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif	
	if((ret=check_stripe_result_tk2(pszTk2))!=0x00)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=check_stripe_result_tk3(pszTk3))!=0x00)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_MagReset();

	//case3: 刷另一张123磁道卡，数据应不一致。
#if !(defined ME15CHX||defined ME30MH||defined ME32||defined ME15B||defined N920)
	cls_show_msg("请刷另一张123磁道卡，刷完卡按[确认]继续..");   
#else
	cls_printf("请刷另一张123磁道卡");
	while(1)
	{
		NDK_MagSwiped(&pcSwiped);
		if(pcSwiped==1)
			break;
	}
#endif
	if((ret=NDK_MagReadNormal(pszTk11, pszTk21, pszTk31, &pnErrorCode))!=NDK_OK||(pnErrorCode!=0&&pnErrorCode!=3073))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pnErrorCode);
		GOTOERR;
	}
	//以下人工check一次很重要!可检出7e、7f等情况
#if !TK1_DISABLE	//硬件上支持1磁道才检测1磁道数据
	if((ret=check_stripe_result_tk1(pszTk11))!=0x00)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if((ret=check_stripe_result_tk2(pszTk21))!=0x00)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=check_stripe_result_tk3(pszTk31))!=0x00)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}    
	if(((ret1=strcmp(pszTk1, pszTk11))==0)&&((ret2=strcmp(pszTk2, pszTk21))==0)&&((ret3=strcmp(pszTk3, pszTk31))==0))
	{
		send_result("line %d:%s测试失败(%d,%d,%d)", __LINE__, TESTAPI, ret1, ret2, ret3);
		GOTOERR;
	}
	NDK_MagReset();
	if((ret=NDK_MagReadNormal(pszTk11, pszTk21, pszTk31, &pnErrorCode))!=NDK_ERR_NOSWIPED)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pnErrorCode);
		GOTOERR;
	}
	NDK_MagReset();

	//case4 :参数都为NULL，刷一张123磁道卡，应成功。
#if !(defined ME15CHX||defined ME30MH||defined ME32||defined ME15B||defined N920)
	cls_show_msg("请刷123磁道卡，刷完卡按[确认]继续..");
	if((ret=NDK_MagSwiped(&pcSwiped))!=NDK_OK||pcSwiped!=1)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pcSwiped);
		GOTOERR;
	}
#else
	cls_printf("请刷123磁道卡");
	while(1)
	{
		NDK_MagSwiped(&pcSwiped);
		if(pcSwiped==1)
			break;
	}
#endif
	if((ret=NDK_MagReadNormal(NULL, NULL, NULL, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_MagReset();
	
#if !(defined ME30MH||defined ME32||defined N920)
	//case5:关闭磁卡设备，刷卡，读磁卡，应失败返回。
	if((ret=NDK_MagClose())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	NDK_MagReset();
#endif

	//case6:
	//增加15张用例磁卡的专项测试for 磁卡的errcode
	//errcard1会以HEX格式显示从BIT14---BIT0分别表示15号---1号卡的正刷2磁道(bit15不用),从BIT30---BIT16分别表示15号---1号卡的正刷3磁道(bit31不用),有问题相应BIT就会置1
	//errcard2,errcard3,errcard4同理
	//eg.正刷11#卡,2磁道有问题,就是0x0800
	if(FAIL==test_mag_errcode(&errcard1, &errcard2, &errcard3, &errcard4))
	{
		if(errcard1!=0||errcard2!=0||errcard3!=0||errcard4!=0)//是按ESC略过测试的情况
		{
			send_result("%s测试存在错误(慢正:0x%08x,慢反:0x%08x,快正:0x%08x,快反:0x%08x)", TESTAPI, errcard1, errcard2, errcard3, errcard4);
			goto ERR;
		}
	}    

	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_MagClose();
	return;
}

