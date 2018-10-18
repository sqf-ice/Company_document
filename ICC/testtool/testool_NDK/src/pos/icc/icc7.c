/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: icc模块
* file name		: 
* Author 			:  
* version			: 
* DATE				: 	
* directory 			: 
* description		: 测试 NDK_IccSetPowerUpMode是否能够正常设置上电类型,N900 N910适用
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_IccSetPowerUpMode"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern void icpowerdown_all(void);

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: icc7
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	 		jiangym		20170905    created
*				  	 
*****************************************************************/
void icc7(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}

	//以下为局部变量
	int ret=0, pnAtrlen=0, i = 0;
	uchar buf[256]={0};
	uchar sendbuf[]={'\x00', '\x84', '\x00', '\x00', 8};
	
	//以下为函数体
	cls_printf("%s测试中...", TESTAPI);

	//测试前置
	icpowerdown_all();

	//case1:异常参数测试
	if((ret=NDK_IccSetPowerUpMode(ICC_EMV_MODE-1,1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_IccSetPowerUpMode(ICC_ISO7816_DEFAULT_MODE+1,1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_IccSetPowerUpMode(ICC_EMV_MODE,ICC_VOL_OFF))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_IccSetPowerUpMode(ICC_EMV_MODE,ICC_VOL_1P8V+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:异常设置:使用社保卡测试,设置非社保卡模式,应该上电失败
	cls_show_msg("请插入社保卡后,任意键继续...");
	if((ret=NDK_IccSetPowerUpMode(ICC_BPS_384_MODE,ICC_VOL_3V))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&pnAtrlen))!=NDK_ERR_IOCTL)
	{	
		send_result("line %d:IC卡上电失败(%d)", __LINE__, ret);
		GOTOERR;
	}

	//case3:异常设置:农行银行卡使用3V的电压,设置1.8v ,上电应该失败
	cls_show_msg("请插入电压不支持1.8V的农行银行卡后,任意键继续...");
	if((ret=NDK_IccSetPowerUpMode(ICC_EMV_MODE,ICC_VOL_1P8V))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&pnAtrlen))!=NDK_ERR_IOCTL)
	{	
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		GOTOERR;
	}

	//case3:建行IC卡正常测试:EMV模块 3V电压(不同银行卡使用的电压可能不同)
	cls_show_msg("请插入电压支持3V的建行银行卡后,任意键继续...");
	if((ret=NDK_IccSetPowerUpMode(ICC_EMV_MODE,ICC_VOL_3V))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&pnAtrlen))!=NDK_OK)
	{	
		send_result("line %d:IC卡上电失败(%d)", __LINE__, ret);
		GOTOERR;
	}
	//IC读写
	memset(buf,0,sizeof(buf));
	pnAtrlen = 0;
	if((ret=NDK_Iccrw(ICTYPE_IC,sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
	{		
		send_result("line %d:IC卡读卡失败(%d)", __LINE__, ret);
		GOTOERR;
	}
	//银行卡不支持取随机数,不同的卡返回值不同,可能返回6d00可能返回6e00
	if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)//&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6e00
	{
		send_result("line %d:IC卡取随机数失败(SW:0x%04x)", __LINE__,ret);
		GOTOERR;
	}	
	//IC下电
	if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{				
		send_result("line %d:IC卡下电失败(%d)", __LINE__,  ret);
		goto ERR;
	}
	send_result("%s测试通过", TESTAPI);		
ERR://测试后置	
	icpowerdown_all();
	return;
}

