/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: icc模块
* file name		: 
* Author 			:  
* version			: 
* DATE				: 	
* directory 			: 
* description		: 测试 NDK_IccGetProtocol是否能够正常获取卡使用的传输协议,N900 N910适用
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_IccGetProtocol"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern void icpowerdown_all(void);

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: icc7
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	 		jiangym		20170905    created
*				  	 
*****************************************************************/
void icc8(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}

	//以下为局部变量
	int ret = 0, pnAtrlen = 0;
	uchar buf[256] = {0};
	int protocol = 0;
	
	//以下为函数体
	cls_printf("%s测试中...", TESTAPI);

	//测试前置
	icpowerdown_all();

	//case1:异常参数测试
	if((ret=NDK_IccGetProtocol(ICTYPE_IC-1,&protocol))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_IccGetProtocol(ICTYPE_M_1_64+1,&protocol))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//这里会跑飞可以 先屏蔽,否则后面用例无法进行
	if((ret=NDK_IccGetProtocol(ICTYPE_IC,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:正常测试:获取IC卡的传输协议,获取的值多数都为0
	cls_show_msg("请插入IC卡后,任意键继续...");
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&pnAtrlen))!=NDK_OK)
	{	
		send_result("line %d:IC卡上电失败(%d)", __LINE__, ret);
		GOTOERR;
	}
	if((ret=NDK_IccGetProtocol(ICTYPE_IC,&protocol))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(protocol)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, protocol);
		GOTOERR;
	}
	//IC下电
	if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{				
		send_result("line %d:IC卡下电失败(%d)", __LINE__,  ret);
		GOTOERR;
	}

	//case:正常测试 传输协议返回1的情况目前无卡,故先不测,后续有卡可再新增用例

	//case3:异常测试:使用memory卡测试,返回失败
	cls_show_msg("请确保已插入at24c类型的memory卡,任意键继续...");
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_M_1,buf,&pnAtrlen))!=NDK_OK)
	{	
		send_result("line %d:IC卡上电失败(%d)", __LINE__, ret);
		GOTOERR;
	}
	if((ret=NDK_IccGetProtocol(ICTYPE_M_1,&protocol))!=/*NDK_ERR_IOCTL*/NDK_ERR_PARA)//开发说除了IC 卡和SAM卡均返回-6
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//IC下电
	if ((ret=NDK_IccPowerDown(ICTYPE_M_1))!=NDK_OK)
	{				
		send_result("line %d:IC卡下电失败(%d)", __LINE__,  ret);
		goto ERR;
	}

	send_result("%s测试通过", TESTAPI);		
ERR://测试后置	
	icpowerdown_all();
	return;
}

