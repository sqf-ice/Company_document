/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys30.c
* Author 			: zhengry
* version			: 
* DATE				: 20150827
* directory 		: 
* description		: 测试NDK_SysGetEvent获取K21端辅CPU通知事件
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysGetEvent"	 
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
*			  	  	      zhengry       20150827       created
*****************************************************************/
void sys30(void)
{
	/*private & local definition*/
	int ret=0, len=0;
	uint event=0;
	uchar out_data[128]={0};
	
	/*process body*/
	if(auto_flag==1)
	{
		//send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);  

	cls_show_msg("请确保开机首次运行该用例,按任意键继续...");  
	//case1:参数错误
	if((ret=NDK_SysGetEvent(NULL, &len, out_data))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGetEvent(&event, NULL, out_data))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGetEvent(&event, &len, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常测试:构造K21端安全触发，获取辅CPU通知事件与实际是否一致。
 	memset(out_data, 0, sizeof(out_data));
	cls_show_msg("请构造安全触发,任意键继续...");
	if((ret=NDK_SysGetEvent(&event, &len, out_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(event!=SYS_EVENT_TAMPER|| len<=0|| len>128||(out_data[0]==0&&out_data[1]==0&&out_data[2]==0&&out_data[3]==0))//安全触发返回寄存器值为4个字节,如果触发则某个字节肯定不为0
	{
		send_result("line %d:%s获取安全触发事件失败(%d,%d,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, event, len, out_data[0], out_data[1], out_data[2], out_data[3]);
		RETURN;
	} 

	//case4:SYS_EVENT_REBOOT事件暂时无法构造
	
	//case3:正常测试:空事件
	memset(out_data, 0, sizeof(out_data));
	if((ret=NDK_SysGetEvent(&event, &len, out_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(event!=SYS_EVENT_NONE|| len!=0|| strlen(out_data)!=0)
	{
		send_result("line %d:%s获取空事件失败(%d,%d,%s)", __LINE__, TESTAPI, event, len, out_data);
		RETURN;
	}

 	send_result("%s测试通过", TESTAPI);
	return;	
}

