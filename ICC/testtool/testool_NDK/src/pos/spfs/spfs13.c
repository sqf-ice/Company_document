/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name		: spfs13.c
* Author 			: linying
* version			: 
* DATE			: 20180829
* directory 		: 
* description		: 测试文件最大可用句柄数
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/

#define  FILETEST16 "/appfs/FT16"
#define	TESTAPI		"最大可用句柄数"
#define	MAXFD	5


/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的16号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*					        linying		   20180829     	created
*****************************************************************/
void spfs13(void)
{
	//此案例只针对K21设计
	/*private & local definition*/
	int i = 0, countnum = 0, ret = 0;
	int fp[6] = {-1, -1, -1, -1, -1, -1};

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("%s测试中...", TESTAPI); // 屏幕提示当前测试所在模块与用例

	NDK_SP_FsDel(FILETEST16);

	//step (2)以"w"方式打开一个特定文件
	if ((fp[0]=NDK_SP_FsOpen(FILETEST16, "w"))<0) // 创建文件出错
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp[0]);
		GOTOERR;
	}
	
	//step (3)关闭该文件
	NDK_SP_FsClose(fp[0]);

	//step (4)循环打开该文件直至使用最大句柄数量
	for(countnum=0; countnum<MAXFD; countnum++)
	{
		if ((fp[countnum]=NDK_SP_FsOpen(FILETEST16, "r"))<0) // 打开文件失败
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp[countnum]);
			GOTOERR;
		}
	}
	//step (5)检验打开文件数是否为5
	if((fp[countnum]=NDK_SP_FsOpen(FILETEST16, "r"))>=0) // 打开文件句柄失败
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp[countnum]);
		GOTOERR;
	}

	//句柄值是否介于[256, 260],ME32字库占用2个句柄值，句柄值介于[258, 262]
	for(countnum=0; countnum<MAXFD; countnum++)
	{
		if ((fp[countnum]<256)||(fp[countnum]>260))
		{
			send_result("line %d:句柄范围出错(fp(%d):%d)", __LINE__, countnum, fp[countnum]);
			GOTOERR;
		}
	}

	//step (6)关闭所有句柄
	for(i = 0; i<MAXFD; i++) 
	{
		if((ret=NDK_SP_FsClose(fp[i]))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d, %d)", __LINE__, TESTAPI, fp[i], ret);
			GOTOERR;
		}
	}

	//测试清理
	if((ret=NDK_SP_FsDel(FILETEST16))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	

	send_result("%s测试通过", TESTAPI); // 测试通过
	return;
ERR:
	for(countnum=0; countnum<SZ_ARRAY(fp); countnum++)
		NDK_SP_FsClose(fp[countnum]);
	NDK_SP_FsDel(FILETEST16);
	return;
}


