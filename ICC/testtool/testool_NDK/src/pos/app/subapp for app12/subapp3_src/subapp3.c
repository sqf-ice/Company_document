/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		     : 
* Author 			:
* version			: 
* DATE			:
* directory 		: 
* description		: 针对NDK_AppStartChildProcess函数设计的子应用,使用函数NDK_AppGetDatafromParentProcess,NDK_AppChildProcessExit看父子进程之间是否能够正常通讯
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "NDK.h"
#include <fcntl.h>
/*---------------constants/macro definition---------------------*/
//#define   TESTAPI		"NDK_AppStartChildProcess"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 模块的12号用例主函数使用的子应用
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*						  jiangym			20170613   created
*****************************************************************/
void main(void)
{
	int ret = -1, data = 0, len = 0;
	char text[] = "这是子进程3";

	if(NDK_ScrInitGui() != NDK_OK)
		   return ;

	NDK_ScrAutoUpdate(0, NULL);
	NDK_ScrClrs();
	NDK_ScrPrintf("进入子进程3,任意键继续");
	NDK_ScrRefresh();
	NDK_KbGetCode(0,NULL);
	//从父进程读取子应用的参数
        if( (ret=NDK_AppGetDatafromParentProcess(&data, &len))!=NDK_OK)
	{
		NDK_ScrPrintf("line:%d从父进程读取参数失败%d",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	NDK_ScrClrs();
	NDK_ScrPrintf("收到父进程传来的数据:%d,任意键继续",data);
	NDK_ScrRefresh();
	NDK_KbGetCode(0,NULL);
	//执行完后退出,并返回数据长度为0的数据给父进程,即无数据返回
	if((ret=NDK_AppChildProcessExit(0, text,0 ))!=NDK_OK)
	{
		NDK_ScrPrintf("line:%d子应用退出失败%d",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	return;
}

