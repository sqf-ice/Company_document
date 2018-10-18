/************************************************************************
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm7.c
* Author 		: jiangym	
* version		: 
* DATE			: 20130821
* directory 		: 
* description		: 测试NDK_WlGetInfo接口获取ccid,imei,imsi的功能
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
#define TESTAPI	"NDK_WlGetInfo"	
#define	MAXWAITTIME	30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm7(void)
* functional description 	: 获取无线 ccid,imei,imsi
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  				
*****************************************************************/
void wlm7(void)
{
	/*private & local definition*/
	int ret = 0;
	char pszvalue[25] = {0};
	
	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:参数异常测试:emType异常,pszvalue为NULL,unBufLen长度不够
	if((ret = NDK_WlGetInfo(WLM_INFO_CCID-1,pszvalue,sizeof(pszvalue)))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WlGetInfo(WLM_INFO_UNDEFINE,pszvalue,sizeof(pszvalue)))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WlGetInfo(WLM_INFO_CCID,NULL,sizeof(pszvalue)))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WlGetInfo(WLM_INFO_CCID,pszvalue,19))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WlGetInfo(WLM_INFO_IMSI,pszvalue,14))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WlGetInfo(WLM_INFO_IMEI,pszvalue,8))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:正常测试获取ccid
	memset(pszvalue,0,sizeof(pszvalue));
	if((ret = NDK_WlGetInfo(WLM_INFO_CCID,pszvalue,sizeof(pszvalue)))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,%s)", __LINE__, TESTAPI, ret, pszvalue);
		RETURN;
	}
	cls_show_msg1(2,"CCID=%s",pszvalue);
	if(strlen(pszvalue)!=20)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, strlen(pszvalue));
		RETURN;
	}
	//case3:正常测试获取IMSI
	memset(pszvalue,0,sizeof(pszvalue));
	if((ret = NDK_WlGetInfo(WLM_INFO_IMSI,pszvalue,sizeof(pszvalue)))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,%s)", __LINE__, TESTAPI, ret, pszvalue);
		RETURN;
	}
	cls_show_msg1(2,"imsi=%s",pszvalue);
	if(strlen(pszvalue)>15 || (is_digit_str(pszvalue))!=TRUE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, strlen(pszvalue));
		RETURN;
	}
	//case4:正常测试获取IMEI
	memset(pszvalue,0,sizeof(pszvalue));
	if((ret = NDK_WlGetInfo(WLM_INFO_IMEI,pszvalue,sizeof(pszvalue)))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,%s)", __LINE__, TESTAPI, ret, pszvalue);
		RETURN;
	}
	cls_show_msg1(2,"imei=%s",pszvalue);
	if(strlen(pszvalue)>20/*15 || (is_digit_str(pszvalue))!=TRUE*/)//NDK 4.4.1版本中文档变更:IMEI总长不超过20，由0-F组成
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, strlen(pszvalue));
		RETURN;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

