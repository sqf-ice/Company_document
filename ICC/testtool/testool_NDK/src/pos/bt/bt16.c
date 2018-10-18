/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt16.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_BTMfiParamSet是否可以正确设置 相应的mfi参数
* related document	: NDK.chm
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define   TESTAPI		"NDK_BTMfiParamSet"
#define  MAXLEN  63
#define  MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt16
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20150318		created
*****************************************************************/
void bt16(void)
{
	/*private & local definition*/
	int ret = -1, i = 0;	  
	char ab_len_para[MAXLEN+2] = {"abnormal_length_param_1234567890_abnormal_length_param_1234567890"};
	char outbuf[16] = {0};
	char *IphoneApp[] = {"CardEase Mobile", 
					    "NEWLAND", 
					    "NLBluetoothTest"}; //测试的三个iphone App 
	/* factory参数目前无作用可随意设,但是在显示界面特殊字符和中文会显示为null   */
	/*factory影响手机配对时请求打开app时候的显示名字可以不支持中文*/
	char *factory[] =  {"`~!@#$%^&*() -_=+][{}:;”’|\\?/><.,", //特殊字符 
					    "NEWLAND",//英文  
					    "NLBluetoothTest"};//正常英文  
	char *bundleid[] = {"com.creditcall.CardEaseMobile",
						"com.newland.mpos",
						"com.newland.inhouse.NLBluetoothTest"};
	char *protocol[] = {"com.thyron",
						"com.newland.payment",
						"com.ne.payment"};
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//未打开蓝牙进行设置应该返回失败 
	/*此处存在bug,会返回成功,因此必须直接退出,否则影响后面的测试效果*/
	if((ret=NDK_BTMfiParamSet(factory[2],protocol[2],bundleid[2]))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(outbuf,0,sizeof(outbuf));
	if((ret=NDK_BTGetLocalName(outbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, outbuf);
		GOTOERR;
	}

	//case1:参数设置为NULL 应该返回失败
	if((ret=NDK_BTMfiParamSet(NULL,protocol[1],bundleid[1]))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTMfiParamSet(factory[1],NULL,bundleid[1]))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTMfiParamSet(factory[1],protocol[1],NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:参数设置长度超过64字节 应该返回失败
	if((ret=NDK_BTMfiParamSet(ab_len_para,protocol[1],bundleid[1]))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTMfiParamSet(factory[1],ab_len_para,bundleid[1]))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTMfiParamSet(factory[1],protocol[1],ab_len_para))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	/******由于 case4会设置成功  影响了case5(是测默认情况不设置)的测试故提到前面*******/
	//case 5:正常测试:测试 没有设置为默认的NLBluetoothTest(机器重启后不保存之前设置,故不设置使用默认的)
	//不设置默认的factory为NEWLAND	提示界面有"NLBluetoothTest"想与NEWLAND "蓝牙名称"连接
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("此时手机出现\"NLBluetoothTest\"想与NEWLAND\"%s\"通信,是[Enter]并允许连接,否[其他]",outbuf)!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//cls_show_msg("NLBluetoothTest连接上%s后任意键继续",outbuf);
	if((ret=bt_comm_once(12,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_BTDisconnect())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(g_pair_mode==2||g_pair_mode==3) 
	{
		cls_show_msg("在手机设置中把已配对的蓝牙设备忽略此设备后任意键继续");
	}
	
	/*此处存在bug,case3 case4不会返回失败均能够设置成功*/
	/*case3 case4开发解释:只要参数格式没有问题，不做具体参数是否正确，参数是否正确由应用自行把控*/
	//case3:id或者协议设置不是规定的应该要返回参数失败----改为返回成功
	//id使用特殊字符
	if((ret=NDK_BTMfiParamSet(factory[2],protocol[1],factory[0]))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//协议使用特殊字符
	if((ret=NDK_BTMfiParamSet(factory[2],factory[0],bundleid[1]))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:异常情况:设置id与协议不一致时应该要返回失败---改为返回成功
	if((ret=NDK_BTMfiParamSet(factory[2],protocol[0],bundleid[2]))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:分别设置三种模式进行测试
	for(i=0;i<3;i++)
	{
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_BTMfiParamSet(factory[i],protocol[i],bundleid[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		cls_show_msg("注意手机端弹框的显示内容并进行判断,任意键继续");
		if(cls_show_msg("手机是否显示\"%s\"想与%s\"%s\"通信,是[Enter],否[其他]",IphoneApp[i],factory[i],outbuf)!=ENTER)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if(g_pair_mode==2||g_pair_mode==3) 
		{
			cls_show_msg("在手机设置中把已配对的蓝牙设备忽略此设备后任意键继续");
		}
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	//设置默认的MFI参数
	NDK_BTMfiParamSet(factory[2],protocol[2],bundleid[2]);
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return;
}


