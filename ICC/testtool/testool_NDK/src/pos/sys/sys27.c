/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys27.c
* Author 			: zhengry
* version			: 
* DATE				: 20150722
* directory 		: 
* description		: 本例测试NDK_SysGetRemainRAMInfo获取固件剩余RAM空间的地址和大小(仅K21支持)
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysGetRemainRAMInfo"	 

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
* history 		 		: author		date		remarks
*			  	  	  zhengry       20150722       created
*****************************************************************/
void sys27(void)//无参数无返回
{
	/*private & local definition*/
	int ret=0, ram_addr=0, ram_size=0;
	/*process body*/
	cls_printf("测试%s...", TESTAPI);  

	//case1:参数非法 
	if((ret=NDK_SysGetRemainRAMInfo(NULL, &ram_size))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGetRemainRAMInfo(&ram_addr, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
 
	//case2:获取固件剩余RAM空间的地址和大小，判断是否合理
	if((ret=NDK_SysGetRemainRAMInfo(&ram_addr, &ram_size))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(auto_flag==0 || auto_flag==2)
	{
		if(cls_show_msg("获取到固件剩余RAM空间的地址是从%d单元开始，大小为%d。请判断是否合理?是[Enter],否[其他]", ram_addr, ram_size)!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return; 
		}
	}
	else
		send_result("获取到固件剩余RAM空间的地址是从%d单元开始，大小为%d。请与实际样机做比较。", ram_addr, ram_size);

	send_result("%s测试通过", TESTAPI);
	return;	
}
