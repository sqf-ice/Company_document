/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 工具模块
* file name		: util12.c
* Author 			: jiangym
* version			:
* DATE				: 20130109
* directory 	:
* description	: 测试NDK_SubStr  接口功能
* related document	: NDK.chm
*
************************************************************************
* log			:
* Revision
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_SubStr"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	:
* functional description 	:
* input parameter	 	:
* output parameter	 	:
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	     jiangym         20130110  	   created
*****************************************************************/
void util12(void)
{
	//以下为局部变量
	uchar soustr[20] = {0}, objstr[7] = {0};
	int stp = 2, num = 5, ret = 0;
	int len = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例

	//case1:异常测试:各个参数为NULL情况，应返回失败
	if((ret=NDK_SubStr(NULL,stp,num,objstr,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SubStr(soustr,stp,num,NULL,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SubStr(soustr,stp,num,objstr,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:nStartPos起始位置小于1,起始位置大于串长度的情况，返回失败
	if((ret=NDK_SubStr(soustr,0,num,objstr,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SubStr(soustr,sizeof(soustr)+1,num,objstr,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:nNum截取的字符数小于等于0，返回失败
	if((ret=NDK_SubStr(soustr,1,0,objstr,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:pszSouStr为"abc234defgh",nStartPos为3，nNum为3，则截取的是c23,len = 2小于NUM 应该返回参数错误
	memset(objstr,0,sizeof(objstr));
	len = 2; //由于函数实现中若len小于num那么就返回失败
	if((ret=NDK_SubStr((uchar *)"abc234defgh",3,3,objstr,&len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case5:正常测试:pszSouStr为"abc234defgh",nStartPos为3，nNum为3，则截取的是c23
	memset(objstr,0,sizeof(objstr));
	len = 4;
	if((ret=NDK_SubStr((uchar *)"abc234defgh",3,3,objstr,&len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//cls_show_msg("obj=%s",objstr);
	if(strcmp("c23",(char *)objstr)||len!=3)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	send_result("%s测试通过", TESTAPI);
	return;
}

