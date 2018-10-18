/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: icc模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20130107
* directory 		: 
* description		: 测试 NDK_IccDetect获取卡片状态
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_IccDetect"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern void icpowerdown_all(void);

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: icc5
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	 	 chenfm     20130107	created
*				  	 
*****************************************************************/
void icc5(void)
{
	//以下为局部变量
	int pnSta= -1, ret= -1, sta = 0x00;
#if !K21_ENABLE||ANDIROD_ENABLE
	int pnAtrlen=0;
	uchar buf[200]={0};
	uchar sendbuf[]={'\x00', '\x84', '\x00', '\x00', 8};
#endif	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 	

	//测试前置
	icpowerdown_all();

	//case1:异常参数测试
	if((ret=NDK_IccDetect(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}			

#if IC1_ENABLE
	//case2:已插IC卡,但未上电
	if(auto_flag==0||auto_flag==2)	//手动测试时要提示,自动测试已经插着卡不需提示
	{
		cls_show_msg("请确保IC已插入,任意键继续...");
	}
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=0x01)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}
#endif

#if !K21_ENABLE//||ANDIROD_ENABLE/* 20140523 跟开发确认K21 NDK_IccDetect函数只支持取IC卡插卡状态,其它状态不支持*/
	//安卓产品后续会支持卡片不同状态检测 20180523 modify  安卓产品又不再支持 20180706 modify
#if IC1_ENABLE
	//case3:IC上电
	if((ret=NDK_IccPowerUp(ICTYPE_IC, buf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sta = sta | 0x03;
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=sta)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}
#endif	

#if SAM1_ENABLE
	if((ret=NDK_IccPowerUp(ICTYPE_SAM1, buf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sta = sta | 0x10;
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=sta)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}
	memset(buf,0,sizeof(buf));
	pnAtrlen = 0;
	if((ret=NDK_Iccrw(ICTYPE_SAM1,sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
	{						
		send_result("line %d:IC1读卡失败(%d)", __LINE__, ret);
		GOTOERR;															
	}
	if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
	{
		send_result("line %d:IC1取随机数失败(SW:0x%04x)", __LINE__,ret);
		GOTOERR;								
	}	
#endif

#if SAM2_ENABLE
	if((ret=NDK_IccPowerUp(ICTYPE_SAM2, buf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}
	sta = sta | 0x20;
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=sta)
//#if SAM_POWERON_MEANWHILE
//    ||(pnSta&0xff)!=0x33)
//#else
//	||(pnSta&0xff)!=0x23)
//#endif
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}
	memset(buf,0,sizeof(buf));
	pnAtrlen = 0;
	if((ret=NDK_Iccrw(ICTYPE_SAM2,sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
	{						
		send_result("line %d:IC1读卡失败(%d)", __LINE__, ret);
		GOTOERR;															
	}
	if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
	{
		send_result("line %d:IC1取随机数失败(SW:0x%04x)", __LINE__,ret);
		GOTOERR;								
	}
#endif
    
#if SAM3_ENABLE
	if((ret=NDK_IccPowerUp(ICTYPE_SAM3, buf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sta = sta | 0x40;
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=sta)
//#if SAM_POWERON_MEANWHILE	
//	||(pnSta&0xff)!=0x73)
//#else
//	||(pnSta&0xff)!=0x43)
//#endif	
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}	
	memset(buf,0,sizeof(buf));
	pnAtrlen = 0;
	if((ret=NDK_Iccrw(ICTYPE_SAM3,sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
	{						
		send_result("line %d:IC1读卡失败(%d)", __LINE__, ret);
		GOTOERR;															
	}
	if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
	{
		send_result("line %d:IC1取随机数失败(SW:0x%04x)", __LINE__,ret);
		GOTOERR;								
	}
#endif

#if SAM4_ENABLE
	if((ret=NDK_IccPowerUp(ICTYPE_SAM4, buf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sta = sta | 0x80;
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=sta)
//#if SAM_POWERON_MEANWHILE
//#if SAM3_ENABLE
//	||(pnSta&0xff)!=0xf3)
//#else
//	||(pnSta&0xff)!=0x73)
//#endif
//#else
//	||(pnSta&0xff)!=0x83)
//#endif	
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}
	memset(buf,0,sizeof(buf));
	pnAtrlen = 0;
	if((ret=NDK_Iccrw(ICTYPE_SAM4,sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
	{						
		send_result("line %d:IC1读卡失败(%d)", __LINE__, ret);
		GOTOERR;															
	}
	if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
	{
		send_result("line %d:IC1取随机数失败(SW:0x%04x)", __LINE__,ret);
		GOTOERR;								
	}
#endif
#endif

	//case4:下电所有卡，获取卡上下电状态，应返回1（bit0表示IC卡是否存在其他bit位都为0）		
	icpowerdown_all();
#if IC1_ENABLE
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=0x01)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}

	if(auto_flag==0||auto_flag==2)
	{
		//case5:拔出IC卡。获取卡状态应返回0（bit0应为0）
		cls_show_msg("请确保IC已移除,任意键继续...");
		if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=0x00)
		{
			send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
			goto ERR;
		}

		//测试后置
		cls_show_msg("请确保IC已插入,任意键继续...");
	}
	else
	{
		send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
	}
#endif
	send_result("%s测试通过", TESTAPI);
ERR:
	icpowerdown_all();
	return;
}

