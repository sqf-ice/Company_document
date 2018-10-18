/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt3.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTSetLocalName,NDK_BTGetLocalName是否可以正常设置获取蓝牙的本地名称
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  TESTAPI		"NDK_BTSetLocalName,NDK_BTGetLocalName"
#if BTMFI_ENABLE||defined ME15CHX||defined ME30MH||defined ME30GX||defined ME15B||defined ME32||defined ME32GX||defined ME65||defined ME30S
#define  MAXNAME 		23
#elif defined ME30THM   //同方微蓝牙名称长度由原来的15位改为31位 20180820
#define  MAXNAME 		31
#else
#define  MAXNAME 		15
#endif

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt3
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt3(void)
{
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}

	/*private & local definition*/ 
	char *inbuf2[] ={"`~!\"@$#[^%]",  /* " 需要有个\"  */
					 "*() -_=+|\\&",  /* \ 需要有个\\  */
					 "{}:;?/><.,",	  /*单引号暂时不支持*/
					 "ne#w;l@",		  /*特殊字符+英文*/
					 "新b@大#t陆",	  /*特殊字符+英文+中文*/
					 "蓝牙"};  		  /*纯中文*/
#if BTMFI_ENABLE||defined ME15CHX ||defined ME30MH||defined ME30GX||defined ME15B||defined ME32||defined ME32GX||defined ME65||defined ME30S
	char inbuf1[MAXNAME+1] = {"newlandcompany012345678"}, abnorname[MAXNAME+2] = {"newlandcomputers12345678"};//海外版本蓝牙名称最大长度为23, ME15CHX等后续产品均按照海外标准要求来实现
#elif defined ME51
	char inbuf1[MAXNAME+1] = {"newlandcompany012345678"}, abnorname[MAXNAME+3] = {"newlandcomputers012345678"};//ME51 V1.4.0固件版本蓝牙支持最大字节数24
#elif defined ME30THM
	char inbuf1[MAXNAME+1] = {"newlandcompany0123456789newland"}, abnorname[MAXNAME+2] = {"newlandcomputers123456789newland"};//ME30同方微最大支持31字节
#else
	char inbuf1[MAXNAME+1] = {"newlandcompany0"}, abnorname[MAXNAME+2] = {"newlandcomputers"};//国内版本为15
#endif
	char inbuf[MAXNAME+1] = {"newland"}, outbuf[MAXNAME+1] = {0}, oldbuf[MAXNAME+1] = {0};
	int ret = -1, i = 0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

#if !K21_ENABLE
	//case1:蓝牙串口未打开进行设置获取应该失败
	if((ret=NDK_BTSetLocalName(inbuf))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//测试前置:打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if(g_at_enable)//如果是BM77模块进行以下操作，支持AT命令模式的BT模块情况相同
	{	
		//case3:未进入命令模式设置 获取失败
		if((ret=NDK_BTGetLocalName(outbuf))!=NDK_ERR_TIMEOUT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR3;
		}
		if((ret=NDK_BTSetLocalName(inbuf))!=NDK_ERR_TIMEOUT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR3;
		}
	}
	
	
	//测试前置:进入命令模式后才能进行获取默认的蓝牙本地名称
	if((ret=NDK_BTEnterCommand())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR2;
	}
	memset(oldbuf,0,sizeof(oldbuf));
	if((ret=NDK_BTGetLocalName(oldbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, oldbuf);
		goto ERR2;
	}

	//case 4:异常测试
	//case 4.1:name=NULL的情况
	if((ret=NDK_BTSetLocalName(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTGetLocalName(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case 4.2:name的大小超出范围最多支持15个字节 //海外版本最大支持23字节
	if((ret=NDK_BTSetLocalName(abnorname))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:name支持特殊字符测试例如:!@#$%^<>等符号
	for(i=0;i<SZ_ARRAY(inbuf2);i++)
	{
		if((ret=NDK_BTSetLocalName(inbuf2[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//蓝牙外挂flash的新方案增加用例测试重新open后蓝牙名称变成旧名称的问题 20160728 linwl
		NDK_PortClose(PORT_NUM_BT);
		NDK_PortOpen(PORT_NUM_BT, NULL);
		NDK_BTEnterCommand();
		memset(outbuf,0,sizeof(outbuf));
		if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(strcmp(outbuf, inbuf2[i]))
		{
			send_result("line %d:%s测试失败(%s,%s)", __LINE__, TESTAPI, outbuf, inbuf2[i]);
			GOTOERR;
		}
	}
	
	//case6:连续两次设置，应该都成功，获取的时候应该获取到第2次设置的值，设置可以选择先设置比较长的，再设置一个短的
	if((ret=NDK_BTSetLocalName(inbuf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTSetLocalName(inbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(outbuf, inbuf))
	{
		send_result("line %d:%s测试失败(%s,%s)", __LINE__, TESTAPI, outbuf, inbuf);
		GOTOERR;
	}
	
	//case 7:正常流程//改成使用长字节设置获取比较 20180820 modify
	if((ret=NDK_BTSetLocalName(inbuf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(outbuf, inbuf1))
	{
		send_result("line %d:%s测试失败(%s,%s)", __LINE__, TESTAPI, outbuf, inbuf);
		goto ERR;
	}

	/*
	//case8:蓝牙名字的设置是否支持中文"蓝牙"  同时使用手机进行搜索应能搜索到
	if((ret=NDK_BTSetLocalName(inbuf3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(outbuf, inbuf3))
	{
		send_result("line %d:%s测试失败(%s,%s)", __LINE__, TESTAPI, outbuf, inbuf3);
		GOTOERR;
	}
	//退出命令模式BM77才能搜到蓝牙
	NDK_BTExitCommand();
	cls_show_msg("使用手机蓝牙进行搜索名为\"蓝牙\"的pos,搜索到后继续");
	if(cls_show_msg("手机搜索到的pos蓝牙名称是否为\"蓝牙\",是[Enter],否[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}*/
	
	//测试后置
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_BTEnterCommand();
	NDK_BTSetLocalName(oldbuf);
ERR2:
	NDK_BTExitCommand();
ERR3:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}


