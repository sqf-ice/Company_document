/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: sdusb
* file name		: 
* Author 			: 
* version			: V1.0
* DATE			: 
* directory 		: 
* description		: 测试NDK_DiskGetRootDirName能否获取U盘根目录的名称
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
#define	TESTAPI		"NDK_DiskGetRootDirName"
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
void sdusb3(void)
{
	if(auto_flag==2)
		return;
	/*private & local definition*/
	int ret = 0, diskstate = 0;
	char rootdir[][16] = {{0}};
	int disktype = get_disktype();
	time_t oldtime;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:构造测试前置失败(ret=%d)", __LINE__, ret);
		return;	
	}	
	
	//case1:还未打开U盘设备
	if((ret=NDK_DiskGetRootDirName(disktype,(char **)rootdir))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//插入设备
	cls_show_msg1(5,"请确保已插上%s卡,按任意键继续...",(disktype==UDISK)?"U盘":"SD卡");
	if((ret=NDK_DiskOpen(disktype,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s失败(ret=%d)", __LINE__, TESTAPI, ret);
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
		if(diskstate==DISKMOUNTSUCC)//检测到设备后退出
			break;
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败超时(%d)", __LINE__, TESTAPI, diskstate);
			goto ERR;			
		}
		//usleep(100);
		NDK_SysMsDelay(1);
	}
	
	//case2:非法参数测试
	if((ret=NDK_DiskGetRootDirName(-3,(char **)rootdir))!=NDK_ERR_USDDISK_NONSUPPORTTYPE)//返回-652，NDK_ERR_USDDISK_NONSUPPORTTYPE值文档定义 不一致
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_DiskGetRootDirName(disktype,NULL))!=NDK_ERR_USDDISK_PARAM)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:正常使用
	if((ret=NDK_DiskGetRootDirName(disktype,(char **)rootdir))<0)
	{	
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	else
	{
		if(auto_flag==0||auto_flag==2)
		{
			if(cls_show_msg1(MAXWAITTIME, "取到U盘根目录名为%s.\n"
										"确认键:一致\n"
										"其它键:有误\n", rootdir[0])!=ENTER)

			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
			}
		}
		else
			send_result("取到U盘根目录名为%s.请判断是否正确", rootdir[0]);
	}
	
	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_DiskClose(disktype);
	return;
}

