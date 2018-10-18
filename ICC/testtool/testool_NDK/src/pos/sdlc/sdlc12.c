/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc12.c
* Author 			: zhengry
* version			: 
* DATE			: 20160912
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmGetBps获取当前连接的波特率和NDK_MdmSdlcSingleForceInit同步拨号初始化函数（单次生效）
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmGetBps,NDK_MdmSdlcSingleForceInit"
#define	PCKTMAXLEN	350
#define	MAXWAITTIME	60	//60秒

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
static int mdm_link_com(void)
{
	int ret=0, i=0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};
	
	//初始化数据包
	memset(buf, 0, sizeof(buf));
	for(i=0; i<sizeof(buf); i++)
		buf[i] = rand()%256;
	update_SDLC_packet(buf, sizeof(buf)); //添加TPDU包头

	//是否拨号成功
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s拨号失败(DialNum:%s,ret:%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		return ret;
	}
	//判断modem状态，是否连接成功
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s modem状态未接通(DialNum:%s,MdmStatus:%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		return NDK_ERR;
	}
	//是否能正常写数据				
	if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_OK)
	{
		send_result("line %d:%s 写数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		return ret;
	}
	//是否能正常读数据
	memset(rbuf, 0, sizeof(rbuf));
	rlen = 0;//防止SDLC_read没有对rlen做清0
	if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s 读数据失败(ret=%d)", __LINE__, TESTAPI, ret);
		return ret;
	}
	//比较读写数据是否一致				
	if((PCKTMAXLEN!=rlen)||MemCmp(buf, rbuf, rlen))
	{
		send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, PCKTMAXLEN, rlen);
		return NDK_ERR;
	}
	
	return NDK_OK;
}

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	   zhengry   20160704	created
*****************************************************************/
void sdlc12(void)
{
	//以下为局部变量
	int ret = 0;
	EM_MDM_BPS bps=0;
	EM_MDM_PatchType i=0;
	EM_MDMSTATUS MdmStatus;
	
	//以下为函数体
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); 
	
	//测试前置:
	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		return;
	}
	//case1:初始化前，获取bps应该失败
	if((ret=NDK_MdmGetBps(&bps))!=NDK_ERR) 
	{
		send_result("line %d:%s测试失败(ret=%d, bps=%d)", __LINE__, TESTAPI, ret, bps);
		return;
	}

	cls_show_msg("请在控制台/mnt/hwinfo/sys.conf文件中，查看mdm中baud_freq的默认波特率并记录后按任意键继续");
	while (1)
	{           
		cls_printf("%s测试中(i=%d)...", TESTAPI, i);
		ret = NDK_MdmSdlcSingleForceInit(i, EM_MDM_BPS_2400);//此时NDK_MdmSdlcSingleForceInit设置的波特率应为非默认波特率 
		//case2: 测试EM_MDM_PatchType 类型，判断是否返回不成功	
		if(ret!=NDK_OK)
		{
			if((i >=MDM_PatchType0) && (i <=MDM_PatchType5)) //case1.1: 正确的EM_MDM_PatchType 类型(i = 0 1 2 3 4 5)，应返回成功,否则报错
			{
				send_result("line %d:%s失败(%d,%d)", __LINE__, TESTAPI, i, ret);
				RETURN;
			}
			else if(ret!=NDK_ERR_PARA)	//case1.2: 错误的EM_MDM_PatchType (i = -1 6 7)类型，应返回成功NDK_ERR_PARA
			{
				send_result("line %d:%s失败(%d,%d)", __LINE__, TESTAPI, i, ret);
				RETURN;
			}
		}
		else 
		{
			//case3: 选取各种正确EM_MDM_PatchType 类型时，继续判断是否NDK_MdmSdlcSingleForceInit起作用
			if((i >= 0) && (i <= 5))
			{
				//mdm连接
				if((ret=mdm_link_com())!=NDK_OK)
				{
					send_result("line %d:%s第%d次mdm连接失败(ret=%d)", __LINE__, TESTAPI, i, ret);
					GOTOERR;
				}
				//case4:NDK_MdmGetBps为获取实际连接的BPS
				if(((ret=NDK_MdmGetBps(&bps))!=NDK_OK) || (bps!=EM_MDM_BPS_2400)) 
				{
					send_result("line %d:%s第%d次测试失败(ret=%d, bps=%d)", __LINE__, TESTAPI, i, ret, bps);
					GOTOERR;
				}
				//是否能正常挂机
				if((ret=NDK_MdmHangup())!=NDK_OK)
				{
					send_result("line %d:%s第%d次挂断失败(ret=%d)", __LINE__, TESTAPI, i, ret);
					RETURN;
				}
				sleep(3);//等待MODEM芯片挂断,避免第一次没完全挂断就去拨号而失败(该时间和硬件芯片有关系)
			}
			else//错误EM_MDM_PatchType 类型
			{
				send_result("line %d:%s失败(%d,%d)", __LINE__, TESTAPI, i, ret);
				RETURN;
			}
		}		
		if(++i>7)  //i=6和i=7是为了验证值超出范围的情况
			break;
	}
	if(cls_show_msg("请在/mnt/hwinfo/sys.conf文件中，查看mdm中baud_freq的值不应被修改为2(EM_MDM_BPS_2400),是[ENTER],否则[其他]") != ENTER)
	{
		send_result("line %d:%s默认波特率被修改", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case5:挂断后，获取bps应该失败
	if((ret=NDK_MdmGetBps(&bps))!=NDK_ERR) 
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case6:不调用NDK_MdmSdlcSingleForceInit设置bps,正常初始化，则按照参数配置文件中的默认bps进行拨号
	if((ret=NDK_MdmSdlcInit(MDMPatchType))!=NDK_OK)
	{
		send_result("line %d:%s初始化错(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN; 
	}	
	NDK_MdmClrbuf();
	if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
	{
		send_result("line %d:%s 拨号失败(DialNum:%s,ret:%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
	{
		send_result("line %d:%s modem状态未接通(DialNum:%s,MdmStatus:%d)", __LINE__, TESTAPI, MDMDialStr, ret);
		GOTOERR;
	}
	//case7:NDK_MdmGetBps为获取实际连接的BPS，此时应为参数配置文件中的默认bps
	if(((ret=NDK_MdmGetBps(&bps))!=NDK_OK) || (bps!=EM_MDM_BPS_1200)) 
	{
		send_result("line %d:%s测试失败(ret=%d, bps=%d)", __LINE__, TESTAPI, ret, bps);
		GOTOERR;
	}
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s 挂断失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case8:测试初始化函数设置bps=9600,应能正常拨号
	if((ret = NDK_MdmSdlcSingleForceInit(MDMPatchType, EM_MDM_BPS_9600))!=NDK_OK )
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//mdm连接
	if((ret=mdm_link_com())!=NDK_OK)
	{
		send_result("line %d:%s mdm连接失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(((ret=NDK_MdmGetBps(&bps))!=NDK_OK) || (bps!=EM_MDM_BPS_9600))
	{
		send_result("line %d:%s测试失败(ret=%d, bps=%d)", __LINE__, TESTAPI, ret, bps);
		GOTOERR;
	}
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s 挂断失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(cls_show_msg("请在/mnt/hwinfo/sys.conf文件中，查看mdm中baud_freq的值不应被修改为3(EM_MDM_BPS_9600),是[ENTER],否则[其他]") != ENTER)
	{
		send_result("line %d:%s默认波特率被修改", __LINE__, TESTAPI);
		RETURN;
	}
	sleep(SLEEP_SDLC_HANGUP);//开发定位这里如果不延时会导致下个拨号返回忙状态,该问题已提交BUG,但开发不修改,波形上看需要6到8秒,综合部分挂断函数有5秒延时,所以这里也改成5秒延时
	//case9:从bps=9600变成bps=2400,应能正常拨号读写
	if((ret = NDK_MdmSdlcSingleForceInit(MDMPatchType, EM_MDM_BPS_2400))!=NDK_OK )
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//mdm连接
	if((ret=mdm_link_com())!=NDK_OK)
	{
		send_result("line %d:%s mdm连接失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR; 
	}
	//case10:NDK_MdmGetBps为获取实际连接的BPS
	if(((ret=NDK_MdmGetBps(&bps))!=NDK_OK) || (bps!=EM_MDM_BPS_2400))
	{
		send_result("line %d:%s测试失败(ret=%d, bps=%d)", __LINE__, TESTAPI, ret, bps);
		GOTOERR;
	}
	if((ret=NDK_MdmHangup())!=NDK_OK)
	{
		send_result("line %d:%s 挂断失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case11:搭建小网控，应能在只支持bps=1200上使用
	if(cls_show_msg("请搭建小网控环境,进行bps=1200测试,跳过[ESC]")!=ESC)
	{
		//重新配置小网控拨号
		if(conf_link(SYNC)!=SUCC)
		{
			send_result("line %d:%s小网控同步配置失败", __LINE__, TESTAPI);
			return;
		}
		//bps=2400或9600，预期会自动协商回落波特率,并以实际的1200波特率连接上
		//bps=1200，预期能够正常连接
		for(i=1; i<=EM_MDM_BPS_9600; i++)
		{
			if((ret = NDK_MdmSdlcSingleForceInit(MDMPatchType, i))!=NDK_OK )
			{
				send_result("line %d:%s测试失败(ret=%d, i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
			if((ret=mdm_link_com())!=NDK_OK)
			{
				send_result("line %d:%s mdm连接失败(ret=%d, i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR; 
			}
			if(((ret=NDK_MdmGetBps(&bps))!=NDK_OK) || (bps!=EM_MDM_BPS_1200))
			{
				send_result("line %d:%s测试失败(ret=%d, bps=%d, i=%d)", __LINE__, TESTAPI, ret, bps, i);
				GOTOERR;
			}
			if((ret=NDK_MdmHangup())!=NDK_OK)
			{
				send_result("line %d:%s 挂断失败(ret=%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
		}
	}

	//case12:未连接情况下获取波特率应该失败
	if((ret=NDK_MdmGetBps(&bps))!=NDK_ERR) 
	{
		send_result("line %d:%s测试失败(ret=%d, bps=%d)", __LINE__, TESTAPI, ret, bps);
		return;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_MdmHangup();
	return;
}

