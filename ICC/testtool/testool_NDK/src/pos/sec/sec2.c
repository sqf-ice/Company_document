/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec2.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecGetRandom接口功能
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
#define TESTAPI	"NDK_SecGetRandom"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*			  	  	   linrq       20130228	created
*****************************************************************/
void sec2(void)
{
	int ret = 0, ret1 = 0;
	uchar ucBuf[8]={0}, ucTmp[8]={0};

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//case1:传入非法参数
	if(((ret = NDK_SecGetRandom(-1, ucBuf))!=NDK_ERR_SECCR_PARAM)
		||((ret1= NDK_SecGetRandom(1, NULL))!=NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, ret1);
		RETURN;
	}
	
	//case2:正常获取随机数
	if((ret = NDK_SecGetRandom(5, ucBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("本次开机取到的5个字节随机数为:0x%2x 0x%2x 0x%2x 0x%2x 0x%2x",ucBuf[0], ucBuf[1], ucBuf[2], ucBuf[3], ucBuf[4]);
	//case3:测试是否具有随机性
	if((ret = NDK_SecGetRandom(8, ucBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_SecGetRandom(8, ucTmp)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if(!memcmp(ucBuf, ucTmp, 8))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	send_result("%s测试通过", TESTAPI);
	return;
}

