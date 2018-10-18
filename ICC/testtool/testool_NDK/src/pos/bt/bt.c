/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 蓝牙模块
* file name			: 
* Author 			: jiangym
* version			: 
* DATE				: 20140217
* directory 			: 
* description			: 扫描模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"蓝牙模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt
* functional description 	: 蓝牙模块入口主函数,可根据需要扩展
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	       jiangym			20140217		created
*****************************************************************/
void bt(PF* list, unsigned char id[])
{
	/*private & local definition*/		
	int ret = -1;
	g_UCID = 0;
	/*process body*/
	cls_show_msg1(1, "%s测试...", TESTMODULE);	

#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B)	//ME15C,ME20不支持NDK_BTSetType函数
#if K21_ENABLE
	char ph_type[3]={0x0c,0x02,0x5a};
	
	if(!g_at_enable)//BM77不支持以下两个函数不测试 20150415 jiangym
	{
		if(cls_show_msg1(5,"是否是开机立即执行本用例,是[Enter],否[其他]")==ENTER)
		{
			if((ret=NDK_BTSetType(ph_type))==NDK_OK/*NDK_ERR_IOCTL*/)//ME30MH返回-1,原先的返回-5,放宽返回值判断 20170523 modify
			{
				send_result("line %d:NDK_BTSetType测试失败(%d)", __LINE__, ret);
				return;
			}
			//开机未下patch时未open执行会失败
			if((ret=NDK_BTSetDiscoverableStatus(1))==NDK_OK/*NDK_ERR_IOCTL*/)//ME30MH返回-1,原先的返回-5,放宽返回值判断 20170523 modify 
			{
				send_result("line %d:NDK_BTSetDiscoverableStatus测试失败(%d)", __LINE__, ret);
				return;
			}	
#if BTMFI_ENABLE
			//蓝牙串口未打开 设置蓝牙是否支持ble连接返回失败
			if((ret=NDK_BTSetBleStatus(0))!=NDK_ERR_IOCTL)
			{
				send_result("line %d:测试失败(%d)", __LINE__, ret);
				return;
			}
			//蓝牙串口未打开 蓝牙记录的手机信息数目返回失败
			if((ret=NDK_BTSetMaxBondedDevices(0))!=NDK_ERR_IOCTL)
			{
				send_result("line %d:测试失败(%d)", __LINE__, ret);
				return;
			}
#endif
		}
	}

#endif
#endif
	if(conf_conn_BT()==FAIL)
		return;
	
#if BTMFI_ENABLE
	ret=NDK_MFI_CHECK();
	switch(ret)
	{
		case NDK_OK: 
			NDK_BTSetBleStatus(0);//在MFI协议下，关闭BLE模式，这样手机搜索到后会识别成MFI设备，并且该函数需在OPEN之后调用，所以移动到设置之后
			break;
		case NDK_ERR_IOCTL:
			cls_show_msg("硬件有误,请找硬件工程师确认");
			return;
			break;
		case NDK_ERR_OPEN_DEV:
			cls_show_msg("无需支持CP芯片,任意键继续测试");
			break;
		case NDK_ERR_NOT_SUPPORT:
			cls_show_msg("BM77蓝牙不支持此函数,任意键继续测试");
			break;
		default:
			cls_show_msg("未知的返回值:%d",ret);
			break;
	}
#endif

	//执行测试用例
#if STABAR_ENABLE
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|STATUSBAR_DISP_BLUETOOTH))!=NDK_OK)
	{
		send_result("状态栏显示失败!(%d)", ret);
		return;
	}
#endif
	process(list, id);
#if STABAR_ENABLE	
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	
	return;
}

