/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt6.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTStatus是否能够获取到正确的蓝牙连接状态
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTStatus"
#define   MAXWAITTIME		30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt6
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20140217		created
*****************************************************************/
void bt6(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = 0, status= 0, flag = 0, j = 0, accept = 1;
	uint ulen = 0;
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

	//case1:未打开蓝牙串口返回失败
#if !K21_ENABLE
	if((ret=NDK_BTStatus(&status))!=NDK_ERR_OPEN_DEV)
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
	//进入命令模式
	if((ret=NDK_BTEnterCommand())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:异常测试,参数为NULL
	if((ret=NDK_BTStatus(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//正常测试判断status,分别对四种模式进行测试
	//case3: 进入命令模式,返回1(未连接)
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	//PCI 认证版本不支持just work模式 故增加条件判断
#if JUSTWORK_ENABLE
	//case4:设置配对模式0(Just Work)
	NDK_BTSetPairingMode(PAIRING_MODE_JUSTWORK);
	//退出命令模式,进入数据模式,根据实际情况返回status
	if((ret=NDK_BTExitCommand())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//cls_show_msg("把先前已配对的POS取消,取消后按任意键继续");
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	cls_show_msg("just work模式:手机蓝牙工具重新扫描连接POS的蓝牙:%s,当提示连接上时继续",BtName);
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case8:NDK_BTDisconnect之后取状态应该返回未连接状态
	sleep(2);
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
#endif

#if !JUSTWORK_FLAG
//新增pincode模式标志位
#if PINCODE_ENABLE
	//case5:设置配对模式为1(Pin Code)
	//cls_show_msg("把先前已配对的POS取消后,按任意键继续");
	NDK_BTEnterCommand();//进入命令模式
	NDK_BTSetPairingMode(PAIRING_MODE_PINCODE);//设置
	NDK_BTExitCommand();//退出命令模式
	cls_show_msg("pin code模式:手机蓝牙工具扫描连接POS的蓝牙:%s,出现pin对话框时点取消,当提示未连接时继续",BtName);
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	cls_show_msg("pin code模式:手机蓝牙工具重新扫描连接POS的蓝牙:%s,出现pin对话框,输入错误的pin,当显示未连接时继续",BtName);
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	cls_show_msg("pin code模式:手机蓝牙工具重新扫描连接POS的蓝牙:%s,出现pin对话框,输入正确的pin:%s,当提示连接上时继续",BtName,BtPin);
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
	{
		send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
		GOTOERR;
	}
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if(!g_at_enable)
	{
		//case6:设置配对模式为2(SSP)
#if BTPCI_ENABLE
		cls_show_msg("把苹果手机设置中已配对的POS取消后,按任意键继续");
#endif		
		NDK_BTEnterCommand();
		NDK_BTSetPairingMode(PAIRING_MODE_SSP);//设置SSP模式
		NDK_BTExitCommand();
		for(j=0;j<2;j++)
		{
			if(j==0)
				cls_printf("SSP PIN模式:手机蓝牙串口与POS进行连接,当出现SSP配对对话框时,手机取消配对");
			else
				cls_printf("SSP PIN模式:手机蓝牙串口与POS进行重新连接,当出现SSP配对对话框时,手机确认配对");
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
							send_result("line %d:%s长时间未检测到配对请求(%d)", __LINE__, TESTAPI, flag);
							goto ERR;
						}
						break;
					case 1:
						//j=0时取消配对 j=1时确认配对
						if((ret=NDK_BTConfirmPairing(pkey, j))!=NDK_OK)
						{
							send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
							goto ERR;
						}
						break;
					case 2://配对成功
						cls_show_msg("配对成功,任意键继续");
						if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
						{
							send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
							goto ERR;
						}
						if((ret=bt_comm_once(12,0))!=NDK_OK)
						{
							send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
							goto ERR;
						}
						if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
						{
							send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
							goto ERR;
						}
						break;
					case 3://配对失败
						if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
						{
							send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
							goto ERR;
						}
						break;
					default:
						send_result("line %d:%s测试失败:获取到未知状态(%d)", __LINE__, TESTAPI, flag);
						goto ERR;
						break;
				}
				if(flag ==2 || flag == 3)
					break;//退出while继续执行
			}
		}
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		
		//case7:设置配对模式为3(PassKey)	
		//cls_show_msg("把先前已配对的POS取消后,按任意键继续");
		NDK_BTEnterCommand();
		NDK_BTSetPairingMode(PAIRING_MODE_PASSKEY);//设置PassKey模式
		NDK_BTExitCommand();
		for(j=0;j<3;j++)
		{
			cls_printf("pass key模式:使用手机蓝牙工具与POS进行连接");
			oldtime = time(NULL);
			while(1)
			{
				accept = 1;
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
							send_result("line %d:%s长时间未检测到配对请求(%d)", __LINE__, TESTAPI, flag);
							goto ERR;
						}
						break;
					case 1://收到手机配对请求
						if(pkey[0]!='\0')
						{
							send_result("line %d:%s测试失败(%c)", __LINE__, TESTAPI, pkey[0]);
							goto ERR;
						}
						if(j==0)
							cls_printf("POS取消配对");
						else if(j==1)
							cls_printf("POS请输入错误的配对码:\n");
						else			
							cls_printf("POS请正确输入手机端显示的配对码:\n");
						if((ret=lib_kbgetinput(pkey, 0, 6, &ulen, INPUTDISP_PASSWD, 0, INPUT_CONTRL_LIMIT_ERETURN)) == NDK_ERR || ret == NDK_ERR_TIMEOUT)
						{
							accept = 0;
						}		
						if((ret=NDK_BTConfirmPairing(pkey,accept))!=NDK_OK)
						{
							send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
							goto ERR;
						}	
							
						break;
					case 2://配对成功
						cls_show_msg("配对成功,任意键继续");
						if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
						{
							send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
							goto ERR;
						}
						if((ret=bt_comm_once(12,0))!=NDK_OK)
						{
							send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
							goto ERR;
						}
						if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
						{
							send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
							goto ERR;
						}
						break;
					case 3://配对失败
						if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
						{
							send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
							goto ERR;
						}
						break;
					default:
						send_result("line %d:%s测试失败:获取到未知状态(%d)", __LINE__, TESTAPI, flag);
						goto ERR;
						break;
				}
				if(flag ==2 || flag == 3)
					break;//退出while继续执行
			}
		}
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}

		//安卓手机无just work sc模式;BLE4.2以上协议的just work and sc模式有配对对话框,可取消可确认
#if JUSTWORK_SC_ENABLE
		if(cls_show_msg("如果是安卓手机测试,请按[ESC]返回,苹果6s以上手机测试按[其他]继续")==ESC)
		{ 
			send_result("%s测试,安卓手机和苹果手机6s以下版本手机不支持just work and sc模式,用户取消测试", TESTAPI);
			GOTOERR;
		}
		else
		{
			NDK_BTEnterCommand();//进入命令模式
			NDK_BTSetPairingMode(PAIRING_MODE_JUSTWORK_SC);//设置
			NDK_BTExitCommand();//退出命令模式
			cls_show_msg("just work sc模式:手机蓝牙工具扫描连接POS的蓝牙:%s,出现配对对话框时点取消,当提示未连接时继续",BtName);
			if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=1)
			{
				send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
				GOTOERR;
			}
			cls_show_msg("just work sc模式:手机蓝牙工具重新扫描连接POS的蓝牙:%s,出现配对对话框时点确认,当显示连接上时继续",BtName);
			if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
			{
				send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
				GOTOERR;
			}
			if((ret=bt_comm_once(12,0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			if((ret=NDK_BTStatus(&status))!=NDK_OK||status!=0)
			{
				send_result("line %d:%s测试失败(ret=%d,status=%d)", __LINE__, TESTAPI, ret, status);
				GOTOERR;
			}
			if((ret=NDK_BTDisconnect())!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
		}
#endif
	}	
#endif

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_BTEnterCommand();
	NDK_BTSetPairingMode(g_pair_mode);//恢复配置中的设置模式
	NDK_BTExitCommand();
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

