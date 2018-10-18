/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: sdusb
* file name		: 
* Author 			: 
* version			: V1.0
* DATE			: 
* directory 		: 
* description		: 测试NDK_DiskOpen能否打开U盘/SD卡,NDK_DiskClose能否关闭U盘/SD卡
 * related document	: 程序员参考手册
 *
 ************************************************************************
 * log			: 
 * Revision 1.0  
 * no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_DiskOpen,NDK_DiskClose"
#define	MAXWAITTIME	30

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
*			  		
*****************************************************************/
void sdusb1(void)
{
	/*private & local definition*/
	int ret = 0, ret1 = 0, ret2 = 0,diskstate;
	int disktype = get_disktype();
	 time_t oldtime = 0;
	 
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置 
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		goto ERR;			
	}
	
	//case1:异常测试
	if( ((ret=NDK_DiskOpen(-1,1))!=NDK_ERR_USDDISK_NONSUPPORTTYPE) || 
		((ret1=NDK_DiskOpen(disktype,-3))!=NDK_ERR_USDDISK_PARAM)  ||	//返回-652，NDK_ERR_USDDISK_NONSUPPORTTYPE值文档定义 不一致
		((ret2=NDK_DiskClose(-3))!=NDK_ERR_USDDISK_NONSUPPORTTYPE) 	//返回-652，NDK_ERR_USDDISK_NONSUPPORTTYPE值文档定义 不一致
		)
	{		
		send_result("line %d:%s测试失败(ret=%d,ret1=%d,ret2=%d)", __LINE__, TESTAPI, ret, ret1 ,ret2);
		GOTOERR;		
	}
if(auto_flag==0 || auto_flag==2)	
{
	//case2:未插U盘或SD卡检测Open成功返回,获取SD卡状态为DISKNOEXIT，U盘的状态为DISKMOUNTING或DISKTIMEOUT
	cls_show_msg("请确保未插上%s,按任意键继续...", (disktype==UDISK)?"U盘":"SD卡");
	if( (ret=NDK_DiskOpen(disktype,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);	
		GOTOERR;		
	}
	oldtime=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&diskstate))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if( ( (disktype==SDDISK) && (diskstate==DISKNOEXIT) )||( (disktype==UDISK) && (diskstate==DISKTIMEOUT||diskstate==DISKMOUNTING) ))
			break;
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败超时", __LINE__, TESTAPI);		
			goto ERR;					
		}	
		//usleep(100);
		NDK_SysMsDelay(1);
	}	
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);		
		GOTOERR;			
	}
}
else
	send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
	
	//case3:正常测试
if(auto_flag==0)	
	cls_show_msg("请确保已插上%s,按任意键继续...",(disktype==UDISK)?"U盘":"SD卡");
if(auto_flag==0 || auto_flag==1)
{
	if((ret=NDK_DiskOpen(disktype,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	
	//case4:重复调用应该成功
	if((ret=NDK_DiskOpen(disktype,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);		
		GOTOERR;		
	}
	oldtime=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&diskstate))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		} 
		if(diskstate==DISKMOUNTSUCC)
			break;	
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败超时", __LINE__, TESTAPI);		
			goto ERR;					
		}	
		//usleep(100);
		NDK_SysMsDelay(1);
	}
	if((ret=NDK_DiskClose(disktype))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;			
	}
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
}
	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_DiskClose(disktype);
	return;
}

