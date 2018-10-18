/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc11.c
* Author 			: linwl
* version			: 
* DATE			: 20150106
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmAdapt能否对有线MODEM进行自适应
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmAdapt"
#define	MAXWAITTIME	60	//60秒
#define	PCKTMAXLEN	350

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
*			  	  	linwl      20150106	created
*****************************************************************/
void sdlc11(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0}, overload[100/*52*/] = {0};
	//EM_MDMSTATUS MdmStatus;	//mdm状态
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 
	
	//case1:异常参数测试
	//case1.1:错误的拨号类型MODEM_DIAL_TYPE_NOT应该返回NDK_ERR_PARA
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_NOT, MDMDialStr))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case1.2:错误的拨号类型MODEM_DIAL_TYPE_ASYN+1应该返回NDK_ERR_PARA
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_ASYN+1, MDMDialStr))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case1.3:拨号号码为NULL应该返回NDK_ERR_PARA
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_SDLC, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case1.4:超长号码(错误号码)应该返回NDK_ERR_PARA
	memset(overload, 0x61, sizeof(overload));
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_SDLC, overload))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:测试自适应失败情况返回NDK_ERR_MODEM_SELFADAPTFAIL
	cls_printf("测试错误号码中,请耐心等待...");
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_SDLC, "errno123"))!=NDK_ERR_MODEM_SELFADAPTFAIL)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:自适应时并机，应检测到并机NDK_ERR_MODEM_OTHERMACHINE
	cls_show_msg("请并机(接入电话并摘机)后按任意键继续");
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_SDLC, MDMDialStr))!=NDK_ERR_MODEM_OTHERMACHINE)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg("子用例测试通过,请将电话挂机后按任意键继续");
	
	//case4:测试自适应时未插电话线的情况应返回NDK_ERR_MODEM_NOLINE
	cls_show_msg("请拔下POS上的电话线后按任意键继续");
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_SDLC, MDMDialStr))!=NDK_ERR_MODEM_NOLINE)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case5:验证拔掉电话线时状态栏图标应该显示为:未接电话线 图标
	if(cls_show_msg("状态栏图标是否显示未接电话线图标,是[Enter]继续,否[其他]失败")!=ENTER)
	{
		send_result("line %d:状态栏图标应该显示未接电话线", __LINE__);
		RETURN;
	}
	cls_show_msg("子用例测试通过,请插上电话线后按任意键继续");
	
	//case6:在自适应过程中可以按取消键退出,并返回NDK_ERR_MODEM_SELFADAPTCANCEL
	cls_printf("请在看到状态栏MDM图标处在连接状态时连续按[取消]键");
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_SDLC, MDMDialStr))!=NDK_ERR_MODEM_SELFADAPTCANCEL)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(2,"子用例测试通过,按任意键继续");
	
	//case7:正常测试自适应之后应该不影响正常拨号和数据收发
	cls_printf("测试正确号码自适应中,请耐心等待...");
	if((ret=NDK_MdmAdapt(MODEM_DIAL_TYPE_SDLC, MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_MdmSdlcInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号(%s)失败(ret=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		RETURN;
	}
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s未能接通(DialNum=%s,MdmStatus=%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		RETURN;
	}
	//初始化发送缓冲区数据
	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
	update_SDLC_packet(buf, sizeof(buf)); //添加TPDU包头		
	//写
	if((ret=NDK_MdmWrite(buf, PCKTMAXLEN))!=NDK_OK)
	{
		send_result("line %d:%s写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//读                                     		
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//比较
	if((PCKTMAXLEN!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, PCKTMAXLEN, rlen);
		RETURN;
	}
	//挂起
	if(NDK_MdmHangup()!=NDK_OK)
	{
		send_result("line %d:%s挂起测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

