/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm2.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_WlModemGetSQ接口功能
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
#define TESTAPI	"NDK_WlModemGetSQ"	
#define	MAXWAITTIME	30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm4(void)
* functional description 	: 获取无线MODEM信号强度，取到的信号强度，取到的值 0-31 为成功，99 为未知,-1 为失败 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void wlm2(void)
{
	/*private & local definition*/
	int nRet = 0, sq = 0;
	LinkType type = getlinktype();

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:异常测试
	if((nRet = NDK_WlModemGetSQ(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}	

	//case2:正常测试
	if((nRet = NDK_WlModemGetSQ(&sq))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, nRet, sq);
		RETURN;
	}
	if(((type==GPRS||type==CDMA) && (sq>SQ_2G_MAX||sq<SQ_2G_MIN))
	||(type==TD && (sq>SQ_3G_MAX||sq<SQ_3G_MIN)))
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, sq);
		RETURN;
	}
	else
	{
		nRet = show_msg1(MAXWAITTIME, "取到信号%d,正确按确认,否则按其它键", sq);
		if(nRet==0||nRet==ENTER)
		{
			send_result("%s测试通过(信号强度:%d)", TESTAPI, sq);
			RETURN;
		}
		else
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			return;
		}
	}
}

