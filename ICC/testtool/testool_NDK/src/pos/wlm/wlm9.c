/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm9.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_WLCheckSIM接口功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_WLCheckSIM"	
#define	CHECK_TIMEOUT	2000
#define	MAXWAITTIME	30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm9(void)
* functional description 			:   
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			jiangym		20160726 created
*****************************************************************/
void wlm9(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1;
	int nKeyin = 0;
	char skip = 0x00;
	
#if !(defined ME50NX||defined ME68)
	int flag_4g = 0;
	char *p=NULL;
	char szOutbuf[1024] = {0};//拨号命令会受这个缓冲区长度影响，应该是个BUG
	ST_ATCMD_PACK STatcmdpack = {0};
	EM_WLM_STATUS emStatus = 0;
	//判断是否是EC20模块，EC20开机和初始化时间较长，复位完成时间暂定20S  linying20180628 added
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CGMR;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((ret = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((p=strstr(szOutbuf,"EC20"))!=NULL)
		flag_4g = 1;
	else
		flag_4g = 0;
#endif

	if (cls_show_msg1(MAXWAITTIME, "测试%s...请先确保机具内已插入了正常SIM卡,ENTER确认", TESTAPI)==ENTER)//视为测试者已确保了插入正常卡
	{
		//case1:无线模块无关闭的函数,这里在入口处已经有初始化了  这里检测卡会返回成功
		if((ret=NDK_WLCheckSIM(NULL,CHECK_TIMEOUT))!= NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		//进行硬件复位
#if !(defined ME50NX||defined ME68) //ME50NX不支持NDK_WlModemReset 20171113
		if((ret=NDK_WlModemReset())!=NDK_OK)//插卡后无线重新上电
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		//case2:马上调用检测卡会返回GPRS未初始化
		if((ret=NDK_WLCheckSIM(NULL,CHECK_TIMEOUT))!= NDK_ERR_GPRS_UNINIT)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}	
		sleep(2);//延时2s,这个时间延时可能会影响测试结果,测试人员测试时如果有问题可以适当调整:nRet==NDK_ERR_GPRS_UNINIT要改大时间,nRet==NDK_OK要改小时间
		//case3:复位未完成时检测卡会返回系统有命令未处理
        if(flag_4g) //EC20模块很难能检测到NDK_ERR_GPRS_BUSY状态故还是获取NDK_ERR_GPRS_UNINIT
		{
			if((ret=NDK_WLCheckSIM(NULL,CHECK_TIMEOUT))!= NDK_ERR_GPRS_UNINIT)
			{
				send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}	
        	}
		else
		{
			if((ret=NDK_WLCheckSIM(NULL,CHECK_TIMEOUT))!= NDK_ERR_GPRS_BUSY)
		    {
				send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
	        }
		}
		sleep(15);//等待复位完成
#endif

		//case4:复位完成后应该能够检测到卡
		if((ret=NDK_WLCheckSIM(NULL,CHECK_TIMEOUT))!= NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}	
	}
		
	//case5:无卡情况下检测不到卡
	nKeyin = cls_show_msg1(MAXWAITTIME, "测试%s...请先确保机具内没有SIM卡(关机再拔卡),ENTER确认无卡", TESTAPI);//拔卡
	if (nKeyin==ENTER)//视为测试者已确保了无卡
	{
		if((ret=NDK_WLCheckSIM("1234",CHECK_TIMEOUT))!= NDK_ERR_NO_SIMCARD)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}	
	}
	else if (nKeyin==0)//超时
	{
		skip |= 0x01;
	}
	else
	{
		send_result("line %d:用户中断测试",  __LINE__);
		RETURN;
	}
	
	//被锁定的卡片测试
	nKeyin = cls_show_msg1(MAXWAITTIME, "测试%s...请先确保机具内已插入了被锁SIM卡(关机再插卡),ENTER确认测试", TESTAPI);//插上被锁的SIM卡
	if (nKeyin==ENTER)//视为测试者已确保了插入被锁卡
	{
		//case6:测试NDK_ERR_PIN_LOCKED,PIN码锁定(在卡锁定且用为NULL的PIN串去解锁的情况)
		if((ret=NDK_WLCheckSIM(NULL,CHECK_TIMEOUT))!= NDK_ERR_PIN_LOCKED)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}

		//case7:测试NDK_ERR_PIN,PIN码输入错误(在卡锁定且用为错误的PIN 串去解锁的情况)
		if((ret=NDK_WLCheckSIM("test",CHECK_TIMEOUT))!= NDK_ERR_PIN)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		
		//case8:测试NDK_OK,正常解锁(一般厂默认是1234),卡索的后使用正确的PIN码进行解锁应该成功
		if((ret=NDK_WLCheckSIM("1234",CHECK_TIMEOUT))!= NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}		
	}
	else if (nKeyin==0)
	{
		skip |= (0x01<<1);
	}
	else
	{
		send_result("line %d:用户中断测试", __LINE__);
		RETURN;
	}
	
	//test over 
	if (skip!=0x00)
		send_result("%s测试未进行(%d)", TESTAPI, skip);
	else
		send_result("%s测试通过", TESTAPI);
	return;		
}

