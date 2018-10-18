/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs16.c
* Author 			: chensj
* version			: 
* DATE			: 20140218
* directory 		: 
* description		: 测试文件最大可用句柄数
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if defined SP610
#define  FILETEST16 "FT16"
#else
#define  FILETEST16 "/appfs/FT16"
#endif
#define	TESTAPI		"最大可用句柄数"
#if defined ME68||defined ME50NX
#define	MAXFD	10
#else
#define	MAXFD	5
#endif

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
*					chensj			20140218	created
*****************************************************************/
void fs16(void)
{
	//此案例只针对K21设计
	/*private & local definition*/
	int i = 0, countnum = 0, ret = 0;
#if defined ME68||defined ME50NX
	int fp[11] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
#else
	int fp[6] = {-1, -1, -1, -1, -1, -1};
#endif

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("%s测试中...", TESTAPI); // 屏幕提示当前测试所在模块与用例

	NDK_FsDel(FILETEST16);

	//step (2)以"w"方式打开一个特定文件
	if ((fp[0]=NDK_FsOpen(FILETEST16, "w"))<0) // 创建文件出错
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp[0]);
		GOTOERR;
	}
	
	//step (3)关闭该文件
	NDK_FsClose(fp[0]);

	//step (4)循环打开该文件直至使用最大句柄数量
	for(countnum=0; countnum<MAXFD; countnum++)
	{
		if ((fp[countnum]=NDK_FsOpen(FILETEST16, "r"))<0) // 打开文件失败
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp[countnum]);
			GOTOERR;
		}
	}
	//step (5)检验打开文件数是否为5
	if((fp[countnum]=NDK_FsOpen(FILETEST16, "r"))>=0) // 打开文件句柄失败
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp[countnum]);
		GOTOERR;
	}

	//句柄值是否介于[256, 260],ME32字库占用2个句柄值，句柄值介于[258, 262]
	for(countnum=0; countnum<MAXFD; countnum++)
	{
#if defined ME32||defined ME32THM||defined ME65||defined ME30THM
		if ((fp[countnum]<258)||(fp[countnum]>262))
		{
			send_result("line %d:句柄范围出错(fp(%d):%d)", __LINE__, countnum, fp[countnum]);
			GOTOERR;
		}
#elif defined ME68||defined ME50NX
		if ((fp[countnum]<256)||(fp[countnum]>265))
		{
			send_result("line %d:句柄范围出错(fp(%d):%d)", __LINE__, countnum, fp[countnum]);
			GOTOERR;
		}
#else
		if ((fp[countnum]<256)||(fp[countnum]>260))
		{
			send_result("line %d:句柄范围出错(fp(%d):%d)", __LINE__, countnum, fp[countnum]);
			GOTOERR;
		}
#endif
	}

	//step (6)关闭所有句柄
	for(i = 0; i<MAXFD; i++) 
	{
		if((ret=NDK_FsClose(fp[i]))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d, %d)", __LINE__, TESTAPI, fp[i], ret);
			GOTOERR;
		}
	}

	//测试清理
	if((ret=NDK_FsDel(FILETEST16))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	

	send_result("%s测试通过", TESTAPI); // 测试通过
	return;
ERR:
	for(countnum=0; countnum<SZ_ARRAY(fp); countnum++)
		NDK_FsClose(fp[countnum]);
	NDK_FsDel(FILETEST16);
	return;
}


