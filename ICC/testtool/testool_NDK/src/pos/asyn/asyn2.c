/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: asyn模块
* file name		: asyn2.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/asyn
* description		: 测试NDK_MdmDial 能否进行建立异步连接的发起动作，即asyn拨号
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmDial"
#define	MAXWAITTIME	60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	chenjings      20130114	created
*****************************************************************/
void asyn2(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	EM_MDMSTATUS MdmStatus;	//mdm状态
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0}, overload[100] = {0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用
	//初始化buf
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
		
	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		return;
	}
	
	//case1:未初始化拨号失败情况
#if 0 //20141202 reset函数功能修改直接返回NDK_OK后屏蔽本用例 
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_ERR_MODEM_INIT_NOT)
	{
		send_result("line %d:%s未初始化情况返回错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	//做初始化
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:错误号码(拔号成功,但不能建立连接,应异步检测到无载波)
	if((ret=NDK_MdmDial("123456"))!=NDK_OK)
	{
		send_result("line %d:%s拨号失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	lib_StartStopwatch();
	while(1)
	{
		if((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)//未能正确获取modem状态
		{
			send_result("line %d:%s检测状态失败(ret=%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		else if(MdmStatus!=MDMSTATUS_NORETURN_AFTERPREDIAL)// 判断modem状态是否改变
		{
			if((MdmStatus==MDMSTATUS_MS_BUSY )){// 判断modem状态是否为忙音
				NDK_MdmHangup();
				break;
			}
			else {
				send_result("line %d:%s猫状态异常(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, MdmStatus);
				goto ERR;
			}
		}
		else if(lib_ReadStopwatch()>MAXWAITTIME)//60 s 超时未检测到忙音
		{
			send_result("line %d:%s猫检测状态超时(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, MdmStatus);
			goto ERR;
		}
	}
	
	//case3:拨号参数错，参数为NULL
	if((ret=NDK_MdmDial(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s拨号应失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:测试超长(最大输入字节数47)
	memset(overload, 0x61, sizeof(overload));//20141212根据景水反馈 MDM底层修改拨号缓冲长度允许最大长度为80
	if((ret=NDK_MdmDial(overload))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s拨号应失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:测试拨号时并机，应检测到并机
	cls_show_msg("请并机(接入电话并摘机)后按任意键继续");
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_ERR_MODEM_OTHERMACHINE)//此时拨号应该不成功
	{
		send_result("line %d:%s并机状态下拨号应失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("子用例测试通过,请将电话挂机后按任意键继续");
	
#if 0	
	//case6:在成功拔号后，进行并机，再进行数据发送，应检测到并机；
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_MdmClrbuf();		
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号(%s)失败(ret=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s猫未能拨通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	cls_show_msg("请并机(接入电话并摘机)后按任意键继续");
	if((ret=NDK_MdmWrite(buf,sizeof(buf)))!=NDK_ERR_MODEM_OTHERMACHINE)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);			
		GOTOERR;
	}
	cls_show_msg("子用例测试通过,请将电话挂机后按任意键继续");
	NDK_MdmHangup();
#endif
	
	//case7:测试未插电话线的情况	
	cls_show_msg("请拔下POS上的电话线后按任意键继续");
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错误(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_ERR_MODEM_NOLINE)//此时拨号应该不成功
	{
		send_result("line %d:%s并机状态下拨号应失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case9:验证拔掉电话线时状态栏图标应该显示为:未接电话线 图标 20140618  jiangym
	if(cls_show_msg("状态栏图标是否显示未接电话线图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应该显示未接电话线", __LINE__);
		GOTOERR;
	}
	//case10:验证拔掉电话线时调用挂断函数后状态栏图标应该显示为:未接电话线 图标 20140623 jiangym
	NDK_MdmHangup();
	if(cls_show_msg("挂断后是否显示未接电话线图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应该显示未接电话线", __LINE__);
		GOTOERR;
	}
	cls_show_msg("子用例测试通过,请插上电话线后按任意键继续");
	
	//case8:正确拨号
	if((ret=NDK_MdmAsynInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case11:验证插电话线时且拨号成功前图标显示为:已接电话线 增加状态栏图标的判断20140618  jiangym 由于未调用拨号函数MDM无法实时更新MDM状态，所以此处状态仍然是未接线
	if(cls_show_msg("状态栏图标是否显示未接电话线图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应该显示已接电话线", __LINE__);
		GOTOERR;
	}
	NDK_MdmClrbuf();		
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号(%s)失败(ret=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s猫未能拨通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	//case12:验证拔掉成功后挂断前状态图标显示:正在通讯图标 增加状态栏图标的判断20140618  jiangym
	if(cls_show_msg("状态栏图标是否显示正在通讯图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应该显示正在通讯", __LINE__);
		GOTOERR;
	}
#if 0		
	NDK_MdmClrbuf();//异步MODEM拨号后连接数据收发前如果不调用该函数，异步串口会有遗留数据影响测试结果所以暂时增加该函数
#endif
	if((ret=NDK_MdmWrite(buf,sizeof(buf)))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);			
		GOTOERR;
	}
	memset(rbuf, 0, sizeof(rbuf));
	rlen = sizeof(buf)+1;//期望读取到的数据长度要大于等于实际能读取到的数据长度。
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((sizeof(buf)!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, sizeof(buf), rlen);
		GOTOERR;
	}
	//case13:验证拔掉成功后挂断前状态图标显示:正在通讯图标 增加状态栏图标的判断20140618  jiangym
	if(cls_show_msg("状态栏图标是否显示正在通讯图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应显示正在通讯", __LINE__);
		GOTOERR;
	}
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s挂起失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case14:验证插电话线时且挂断后图标显示为:已接电话线 增加状态栏图标的判断20140618  jiangym
	if(cls_show_msg("状态栏图标是否显示已接电话线图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应该显示已接电话线", __LINE__);
		GOTOERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
	goto ERR;
ERR:
	NDK_MdmHangup();
	return;
}

