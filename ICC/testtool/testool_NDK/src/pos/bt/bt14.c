/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt14.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTGetPairingStatus,NDK_BTConfirmPairing蓝牙是否能够正确获取蓝牙配对状态并确认配对
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTGetPairingStatus,NDK_BTConfirmPairing"
#define   MAXWAITTIME		30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt14
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20141118		created
*****************************************************************/
void bt14(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = 0, flag = 0;
	uint len = 0;
	char pkey[7] = {0};
	time_t oldtime = 0, diff = 0;
	
	/*process body*/
	//BLE只有just work 模式,在平常测试中验证即可
	if(cls_show_msg("测试%s...如果是苹果手机6s以下版本测试,此案例不测,[ESC]返回,[其他]继续", TESTAPI)==ESC)
	{
		send_result("%s测试,苹果手机6s以下版本不支持just work以外的模式,用户取消测试", TESTAPI);
		return;
	}
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//case1:BM77蓝牙模块不支持，返回NDK_ERR_NOT_SUPPORT
	if(g_at_enable)//BM77不支持
	{
		if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_ERR_NOT_SUPPORT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret=NDK_BTConfirmPairing(pkey,1))!=NDK_ERR_NOT_SUPPORT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		cls_show_msg1(10,"此蓝牙模块不支持%s函数", TESTAPI);
		return;
	}

#if !K21_ENABLE
	//case2:蓝牙串口未打开 获取配对状态失败 确认配对也失败
	if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_BTConfirmPairing(pkey,1))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//ME30MH蓝牙需要开启广播
	if(g_btdiscoverflag)
	{
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	//case3:配对模式不是2 3模式的会返回失败
#if !BTPCI_ENABLE
	NDK_BTSetPairingMode(PAIRING_MODE_JUSTWORK);
	if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTConfirmPairing(pkey,1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_BTSetPairingMode(PAIRING_MODE_PINCODE);//设置pin code模式
	if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTConfirmPairing(pkey,1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case4:异常测试:参数错误
	NDK_BTSetPairingMode(PAIRING_MODE_SSP);//设置正确的模式
	if((ret=NDK_BTGetPairingStatus(NULL,&flag))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTGetPairingStatus(pkey,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTConfirmPairing(NULL,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:未连接上获取状态返回0,(文档描述其他状态均返回0)
	if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(flag)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	//未配对时调用确认配对返回失败
	if((ret=NDK_BTConfirmPairing(pkey,1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//调整测试case的顺序:先做连接失败的再执行连接成功的，可以省去取消配对的动作
	//case8:ssp模式下能够正常收到主设备（手机）端配对请求，并取消配对后应该返回配对失败
	NDK_BTSetPairingMode(PAIRING_MODE_SSP);//设置SSP模式
#if BTPCI_ENABLE
	cls_show_msg("手机端在设置中把之前配对的蓝牙取消后任意键继续"); //不管是PCI认证版本还是国内版本 苹果手机如果设置ssp模式测试后 这边都需要进行取消配对
#endif	
	cls_printf("SSP PIN模式:使用手机蓝牙工具与POS进行连接,手机端出现配对码后取消配对");
	oldtime = time(NULL);
	while(1)
	{
		if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		NDK_SysMsDelay(50);
		switch(flag)
		{
			case 0:
				if((diff=time(NULL)-oldtime)>60)
				{
					send_result("line %d:长时间未检测到配对请求(%d)", __LINE__, flag);
					goto ERR;
				}
				continue;
				break;
			case 1:
				//测试正确的pkey进行取消配对
				if((ret=NDK_BTConfirmPairing(pkey, 0))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					goto ERR;
				}
				continue;
				break;
			case 2:
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
				break;
			case 3:
				cls_show_msg1(5,"取消配对成功,子用例测试通过");
				break;
			default:
				send_result("line %d:%s测试失败:获取到未知状态(%d)", __LINE__, TESTAPI, flag);
				goto ERR;
				break;
		}
		if(flag==3)
			break;
	}
	//case9: 取消配对情况下获取的状态值0(文档描述其他状态均返回0)
	if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(flag)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	//断开连接
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:ssp模式下能够正常收到主设备（手机）端配对请求，并接受配对后能够返回配对成功
	cls_printf("SSP PIN模式:使用手机蓝牙工具与POS进行连接,手机端出现配对码后确认配对");
	oldtime = time(NULL);
	while(1)
	{
		if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		switch(flag)
		{
			case 0:
				if((diff=time(NULL)-oldtime)>60)
				{
					send_result("line %d:长时间未检测到配对请求(%d)", __LINE__, flag);
					goto ERR;
				}
				continue;
				break;
			case 1:
				//测试正确的pkey进行接受配对
				if((ret=NDK_BTConfirmPairing(pkey, 1))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					goto ERR;
				}
				continue;
				break;
			case 2:
				cls_show_msg1(5,"接受配对成功,子用例测试通过");
				break;
			case 3:
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
				break;
			default:
				send_result("line %d:%s测试失败:获取到未知状态(%d)", __LINE__, TESTAPI, flag);
				goto ERR;
				break;
		}
		if(flag==2)
			break;
	}
	
	//case7: 已连接的情况下获取的状态值0(文档描述其他状态均返回0)
	if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(flag)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	//断开连接
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case11:PassKey模式下能够正常收到主设备（手机）端配对请求，并输入正确key和取消配对后应该返回配对失败
	//cls_show_msg("把之前配对成功的蓝牙取消配对后,任意键继续");	
	NDK_BTSetPairingMode(PAIRING_MODE_PASSKEY);//设置PassKey模式
	cls_printf("PassKey模式:使用手机蓝牙工具与POS进行连接,手机端出现配对码");
	oldtime = time(NULL);
	while(1)
	{
		if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		switch(flag)
		{
			case 0:
				if((diff=time(NULL)-oldtime)>60)
				{
					send_result("line %d:长时间未检测到配对请求(%d)", __LINE__, flag);
					goto ERR;
				}
				continue;
				break;
			case 1:
				if(pkey[0]!='\0')
				{
					send_result("line %d:%s测试失败(%c)", __LINE__, TESTAPI, pkey[0]);
					goto ERR;
				}
				cls_printf("请正确输入手机端显示的配对码:\n");
				if((ret=lib_kbgetinput(pkey, 0, 6, &len, INPUTDISP_PASSWD, 0, INPUT_CONTRL_LIMIT_ERETURN)) == NDK_ERR || ret == NDK_ERR_TIMEOUT)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto ERR;
				}
				if((ret=NDK_BTConfirmPairing(pkey, 0))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					goto ERR;
				}
				continue;
				break;
			case 2:
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
				break;
			case 3:
				cls_show_msg1(5,"取消配对成功,子用例测试通过");
				break;
			default:
				send_result("line %d:%s测试失败:获取到未知状态(%d)", __LINE__, TESTAPI, flag);
				goto ERR;
				break;
		}
		if(flag==3)
			break;
	}
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case12:PassKey模式下能够正常收到主设备（手机）端配对请求，并输入错误key和接受配对后应该返回配对失败
	//cls_show_msg("把之前配对成功的蓝牙取消配对后,任意键继续");	
	cls_printf("PassKey模式:使用手机蓝牙工具与POS进行连接,手机端出现配对码");
	oldtime = time(NULL);
	while(1)
	{	
		if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		switch(flag)
		{
			case 0:
				if((diff=time(NULL)-oldtime)>60)
				{
					send_result("line %d:长时间未检测到配对请求(%d)", __LINE__, flag);
					goto ERR;
				}
				continue;
				break;
			case 1:
				if(pkey[0]!='\0')
				{
					send_result("line %d:%s测试失败(%c)", __LINE__, TESTAPI, pkey[0]);
					goto ERR;
				}
				cls_printf("请错误的输入手机端显示的配对码:\n");
				if((ret=lib_kbgetinput(pkey, 0, 6, &len, INPUTDISP_PASSWD, 0, INPUT_CONTRL_LIMIT_ERETURN)) == NDK_ERR || ret == NDK_ERR_TIMEOUT)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto ERR;
				}
				if((ret=NDK_BTConfirmPairing(pkey, 1))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					goto ERR;
				}
				continue;
				break;
			case 2:
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
				break;
			case 3:
				cls_show_msg1(5,"配对失败的子用例测试通过");
				break;
			default:
				send_result("line %d:%s测试失败:获取到未知状态(%d)", __LINE__, TESTAPI, flag);
				goto ERR;
				break;
		}
		if(flag==3)
			break;
	}

	//case10:PassKey模式下能够正常收到主设备（手机）端配对请求，并输入正确key和接受配对后应该返回配对成功
	//cls_show_msg("把之前配对成功的蓝牙取消配对后,任意键继续");	
	cls_printf("PassKey模式:使用手机蓝牙工具与POS进行连接,手机端出现PASSKEY");
	oldtime = time(NULL);
	while(1)
	{
		if((ret=NDK_BTGetPairingStatus(pkey,&flag))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		switch(flag)
		{
			case 0:
				if((diff=time(NULL)-oldtime)>60)
				{
					send_result("line %d:长时间未检测到配对请求(%d)", __LINE__, flag);
					goto ERR;
				}
				continue;
				break;
			case 1:
				if(pkey[0]!='\0')
				{
					send_result("line %d:%s测试失败(%c)", __LINE__, TESTAPI, pkey[0]);
					goto ERR;
				}
				cls_printf("请正确输入手机端显示的配对码:\n");
				if((ret=lib_kbgetinput(pkey, 0, 6, &len, INPUTDISP_PASSWD, 0, INPUT_CONTRL_LIMIT_ERETURN)) == NDK_ERR || ret == NDK_ERR_TIMEOUT)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto ERR;
				}
				if((ret=NDK_BTConfirmPairing(pkey, 1))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
					goto ERR;
				}	
				continue;
				break;
			case 2:
				cls_show_msg1(5,"接受配对成功,子用例测试通过");
				break;
			case 3:
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
				break;
			default:
				send_result("line %d:%s测试失败:获取到未知状态(%d)", __LINE__, TESTAPI, flag);
				goto ERR;
				break;
		}
		if(flag==2)
			break;
	}
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	send_result("%s测试通过", TESTAPI);	
ERR:
	NDK_BTSetPairingMode(g_pair_mode);
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

