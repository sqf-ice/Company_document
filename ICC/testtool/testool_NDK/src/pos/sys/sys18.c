/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys18.c
* Author 			: chensj
* version			: 
* DATE				: 20130718
* directory 		: 
* description		: 测试NDK_SysGetFirmwareInfo接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysGetFirmwareInfo"

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
*			  		      chensj	           20130718	 	created
*****************************************************************/
void sys18(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int nRet = 0;
	EM_SYS_FWINFO emFWinfo;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:异常测试
	if((nRet = NDK_SysGetFirmwareInfo(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}

	//case2:正常调用
	if((nRet = NDK_SysGetFirmwareInfo(&emFWinfo)) != NDK_OK||(emFWinfo!=SYS_FWINFO_PRO&&emFWinfo!=SYS_FWINFO_DEV))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, nRet, emFWinfo);
		RETURN;
	}
	send_result("固件版本为%s版本,请判断是否正确",(emFWinfo==SYS_FWINFO_PRO)?"正式":"开发");

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}
