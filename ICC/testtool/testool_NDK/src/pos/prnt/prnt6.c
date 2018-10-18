/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt6.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnGetType接口功能
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
#define TESTAPI	"NDK_PrnGetType"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void prnt6(void)
* functional description 	: 获取本打印机类型 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt6(void)
{
	//以下为局部变量
	EM_PRN_TYPE emType;
	//int nRet;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	
	//case1:测试NDK_PrnGetType能否获取打印机类型:TP、HIP、FIP未知类型
	emType = lib_getprintertype();
	switch (emType)
	{
	case PRN_TYPE_TP:
		cls_printf("类型:热敏打印");
		break;
	case PRN_TYPE_HIP:
		cls_printf( "类型:穿孔针打");
		break;
	case PRN_TYPE_FIP:
		cls_printf("类型:摩擦针打");
		break;
	default:
		cls_printf("未能获取打印机类型");
		break;
	}
	if (show_msg( "\n检查打印机类型是否匹配,ENTER通过,其它键不通过")!=ENTER)//zhangnw20061031 modify
	{
		send_result("%s测试失败", TESTAPI);
		return;
	}

#if 0 //接口已去除
	//case2:入参为空
	if((nRet = NDK_PrnGetType(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
		return;
	}
#endif
	
	//测试结束	
	send_result("%s测试通过", TESTAPI);
	return;
}

