/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: usb
* file name		: 
* Author 			: 
* version			: V1.0
* DATE			: 
* directory 		: 
* description		: 测试NDK_DiskGetState是否可以正确显示状态
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
#define	TESTAPI		"NDK_DiskGetState"
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
void sdusb2(void)
{
	/*private & local definition*/
	int ret = 0, ret1 = 0, diskstate = 0;
	int disktype = get_disktype();;
	time_t oldtime = 0;	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		goto ERR;			
	}
	
	//DISKDRIVERLOADFAIL=3无法测试

	//case1:测试DISKNOTOPEN:未open,应该返回DISKNOTOPEN
if(auto_flag==0)	
	cls_show_msg("请确保已插上%s,按任意键继续...",(disktype==UDISK)?"U盘":"SD卡");
if(auto_flag==0 || auto_flag==1)
{
	if((ret=NDK_DiskGetState(disktype,&diskstate))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(diskstate!=DISKNOTOPEN)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, diskstate);
		GOTOERR;
	}	
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, ret);	
		GOTOERR;		
	}
	cls_show_msg1(2,"子用例测试通过....");
	
	//case2:非法参数测试
	if((ret=NDK_DiskOpen(disktype,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
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
	if(((ret=NDK_DiskGetState(-3,&diskstate))!=NDK_ERR_USDDISK_NONSUPPORTTYPE)||((ret1=NDK_DiskGetState(disktype,NULL))!=NDK_ERR_USDDISK_PARAM))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		GOTOERR;
	}
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, ret);	
		GOTOERR;		
	}
	cls_show_msg1(2,"子用例测试通过....");
}
if(auto_flag==0 || auto_flag==2)	
{
	//case3:测试DISKTIMEOUT和DISKNOEXIT，未插卡或未插U盘，此时获取状态，SD卡返回DISKNOEXIT，U盘返回DISKMOUNTING。
	cls_show_msg("请确保未插上%s,按任意键继续...",(disktype==UDISK)?"U盘":"SD卡");
	if((ret=NDK_DiskOpen(disktype,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	oldtime=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&diskstate))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if( ( (disktype==UDISK) && (diskstate==DISKMOUNTING) )||( (disktype==SDDISK) && (diskstate==DISKNOEXIT) ))
				break;
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败超时(%d)", __LINE__, TESTAPI, diskstate);
			goto ERR;			
		}
		//usleep(100);
		NDK_SysMsDelay(1);
	}
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, ret);	
		GOTOERR;		
	}
	cls_show_msg1(2,"子用例测试通过....");
	
	//case4:不插U盘或是SD卡，但是阻塞open后，调用NDK_DiskGetState
	cls_show_msg("请确保未插上%s,按任意键继续...",(disktype==UDISK)?"U盘":"SD卡");
	if((ret=NDK_DiskOpen(disktype,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
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
		if( ( (disktype==UDISK) && (diskstate==DISKMOUNTING) )||( (disktype==SDDISK) && (diskstate==DISKNOEXIT) ))
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
	cls_show_msg1(2,"子用例测试通过....");
}
else
	send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
	
	//case5:Sd卡要测DISKTIMEOUT状态，要先确保已插SD卡，然后再测超时状态(该状态不易测，因为此时已经插sd卡，有可能返回三种状态DISKMOUNTSUCC、DISKMOUNTING、DISKTIMEOUT)?
	//U盘测DISKTIMEOUT状态，在规定时间内还打开成功此时返回DISKTIMEOUT状态.
if(auto_flag==0)
	cls_show_msg("请确保已插上%s,按任意键继续...",(disktype==UDISK)?"U盘":"SD卡");	
if(auto_flag==0 || auto_flag==1)
{
	if((ret=NDK_DiskOpen(disktype,1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	oldtime=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&diskstate))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}	
		if( ( (disktype==UDISK) && (diskstate==DISKTIMEOUT) )||( (disktype==SDDISK) && (diskstate==DISKTIMEOUT||diskstate==DISKMOUNTSUCC) ))
			break;
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败超时(%d)", __LINE__, TESTAPI, diskstate);
			goto ERR;			
		}
		//usleep(100);
		NDK_SysMsDelay(1);
	}
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, ret);	
		GOTOERR;		
	}										
	cls_show_msg1(2,"子用例测试通过.....");
}
	//case6:已插SD卡或U盘，测试获取去到SD卡和U盘状态DISKMOUNTSUCC或者DISKMOUNTING
if(auto_flag==0)	
	cls_show_msg("请确保已插上%s,按任意键继续...",(disktype==UDISK)?"U盘":"SD卡");
if(auto_flag==0 || auto_flag==1)
{
	if((ret=NDK_DiskOpen(disktype,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;		
	}
	oldtime=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&diskstate))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		} 
		if(diskstate==DISKMOUNTING)
			cls_printf("%s加载中...",(disktype==UDISK)?"U盘":"SD卡");
		if(diskstate==DISKMOUNTSUCC)
			break;
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败超时(%d)", __LINE__, TESTAPI, diskstate);
			goto ERR;					
		}		
		//usleep(100);
		NDK_SysMsDelay(1);
	}
}	
	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_DiskClose(disktype);
	return;
}

