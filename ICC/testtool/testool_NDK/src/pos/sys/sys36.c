/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys36.c
* Author 			:  
* version			: 
* DATE				:  
* directory 		: 
* description		: 测试NDK_szGetCapability能否正常获取是否支持外卡功能(仅高端支持)
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysGetCapability" 

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		     jiangym				20171229		created
*****************************************************************/
void sys36(void)
{
	//以下为局部变量
	int ret = -1;
	char caps[16] = {0};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:异常参数测试(nSizeOfCap需要>=5)        之前nSizeOfCap是>=3，现在更改为>=5， linying 20180503 modified
	if((ret=NDK_SysGetCapability(3,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGetCapability(4,caps)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case1:正常获取
	if((ret=NDK_SysGetCapability(sizeof(caps)/*5*/,caps)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(strlen(caps) >= 5)
	{
		if(caps[2]=='Y')
		{	
			// 当前版本支持外卡
			cls_show_msg1(10,"当前版本支持外卡，任意键继续");
	 	 }
	 	else
		{
			// 当前版本不支持外卡
			cls_show_msg1(10,"当前版本不支持外卡，任意键继续");
		}
		if(caps[3]=='Y')
		{	
			// 当前版本支持打印
			cls_show_msg1(10,"当前版本支持打印，任意键继续");
		 }
		else
		{
			// 当前版本不支持打印
			cls_show_msg1(10,"当前版本不支持打印，任意键继续");
		}
		if(caps[4]=='Y')
		{	
			// 当前版本支持射频
			cls_show_msg1(10,"当前版本支持射频，任意键继续");
	 	 }
	 	else
		{
			// 当前版本不支持射频
			cls_show_msg1(10,"当前版本不支持射频，任意键继续");
		}
		//新增获取是否支持IC卡和磁卡 20180509  sull add
		if(caps[5]=='Y')
		{	
			// 当前版本支持IC卡
			cls_show_msg1(10,"当前版本支持IC卡，任意键继续");
	 	 }
	 	else
		{
			// 当前版本不支持IC卡
			cls_show_msg1(10,"当前版本不支持IC卡，任意键继续");
		}
		if(caps[6]=='Y')
		{	
			// 当前版本支持磁卡
			cls_show_msg1(10,"当前版本支持磁卡，任意键继续");
	 	 }
	 	else
		{
			// 当前版本不支持磁卡
			cls_show_msg1(10,"当前版本不支持磁卡，任意键继续");
		}
	}
	else
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, strlen(caps));
		RETURN;
	}	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

