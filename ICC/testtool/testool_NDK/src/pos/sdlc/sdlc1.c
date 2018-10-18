/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc1.c
* Author 			: chenjings
* version			: 
* DATE			: 20130106
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmSdlcInit能否对有线MODEM进行同步方式的初始化
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmSdlcInit"
#define	MAXWAITTIME	60	//60秒
#define PCKTMAXLEN 350

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
*			  	  	chenjings      20130106	created
*****************************************************************/
void sdlc1(void)
{
	//以下为局部变量
	int i = -1, j = 0, ret = 0;
	uint rlen = 0;
	char buf[512] = {0}, rbuf[512] = {0};
	
	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); 

	if((ret=NDK_MdmReset())!=NDK_OK )
	{
		send_result("line %d:%s复位失败(ret=%d)", __LINE__, TESTAPI, ret);
		return;
	}
	while (1)
	{           
		cls_printf("%s测试中(i=%d)...", TESTAPI, i);//屏幕提示当前测试所在模块与用例
		ret = NDK_MdmSdlcInit(i);
		//case1: 测试EM_MDM_PatchType 类型，判断是否返回不成功	
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
			//case2: 选取各种正确EM_MDM_PatchType 类型时，继续判断是否NDK_MdmSdlcInit起作用
			if((i >= 0) && (i <= 5))
			{
				//case2.1:是否拨号成功
				NDK_MdmClrbuf();
				if((ret=NDK_MdmDial(MDMDialStr))!=NDK_OK)
				{
					send_result("line %d:%s第%d次拨号失败(DialNum:%s,ret:%d)", __LINE__, TESTAPI, i, MDMDialStr, ret);
					GOTOERR;
				}
				//case2.2:判断modem状态，是否连接成功
				if((ret=mdm_detectCONNECT())!=MDMSTATUS_CONNECT_AFTERPREDIAL)
				{
					send_result("line %d:%s modem状态未接通(DialNum:%s,MdmStatus:%d)", __LINE__, TESTAPI, MDMDialStr, ret);
					GOTOERR;
				}
				//case2.3:初始化buf
				memset(buf, 0, sizeof(buf));
				for(j=0; j<sizeof(buf); j++)
					buf[j] = rand()%256;
				//case2.4:是否能正常写数据				
				update_SDLC_packet(buf, sizeof(buf)); //添加TPDU包头
				if((ret=NDK_MdmWrite(buf,PCKTMAXLEN))!=NDK_OK)
				{
					send_result("line %d:%s 第%d次写数据失败(ret=%d)", __LINE__, TESTAPI, i, ret);
					GOTOERR;
				}
				//case2.5:是否能正常读数据
				memset(rbuf, 0, sizeof(rbuf));
				rlen = 0;//防止SDLC_read没有对rlen做清0
				if((ret=NDK_MdmRead(rbuf, &rlen, MAXWAITTIME))!=NDK_OK)
				{
					send_result("line %d:%s 第%d次读数据失败(ret=%d)", __LINE__, TESTAPI, i, ret);
					GOTOERR;
				}
				//case2.6:比较读写数据是否一致				
				if((PCKTMAXLEN!=rlen)||MemCmp(buf, rbuf, rlen))
				{
					send_result("line %d:%s收发数据不一致(W=%d,R=%ld)", __LINE__, TESTAPI, PCKTMAXLEN, rlen);
					GOTOERR;
				}
				//case2.7:是否能正常挂机
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

	//测试结束
	NDK_MdmHangup();//挂起
	send_result("%s测试通过", TESTAPI);
	return;
	goto ERR;
ERR:
	NDK_MdmHangup();
	return;
}

