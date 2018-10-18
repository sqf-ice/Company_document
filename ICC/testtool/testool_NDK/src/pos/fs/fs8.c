/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs8.c
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: 测试NDK_FsGetDiskSpace接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if OVERSEAS_ENABLE	|| defined SP610
#define  FILETEST8 "FT8"
#else
#define  FILETEST8 "/appfs/FT8"
#endif
#define 	TESTAPI		"NDK_FsGetDiskSpace"
#if defined ME20||defined ME32
#define	LOOP	50  //ME20和ME32文件系统可用空间只有68K  故改小写的次数
#elif defined ME30THM||defined ME32THM
#define LOOP 15
#else
#define	LOOP	100//10 //避免开太大引起低端平台堆栈溢出
#endif
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的N号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*
*****************************************************************/
void fs8(void)
{
	/*private & local definition*/
	int fp=0, writelen=0, loop=0, ret=0;
	char writebuf[1024/**10*/]={0};//避免开太大引起低端平台堆栈溢出
	ulong spaceofdisk1=0, spaceofdisk11=0, spaceofdisk2=0, spaceofdisk21=0;
#if !(CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE)
	ulong diff = 0;
#endif
	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_FsDel(FILETEST8);
	
	memset(writebuf, 0x10, sizeof(writebuf));

	//调用NDK_FsGetDiskSpace取电子盘剩余空间和已经使用的磁盘空间
	NDK_FsGetDiskSpace(1, &spaceofdisk1);
	NDK_FsGetDiskSpace(0, &spaceofdisk2);
	if((fp=NDK_FsOpen(FILETEST8, "w"))<0)
	{
		send_result("line %d:打开文件出错(%d)", __LINE__, fp);
		GOTOERR;
	}
	//循环写入100KB数据，ME32和ME20写入50KB数据
	for(loop=0;loop<LOOP;loop++)
	{
		if((writelen=NDK_FsWrite(fp, writebuf, sizeof(writebuf)))!=sizeof(writebuf))
		{
			send_result("line %d:写文件数据出错[%d, %d]", __LINE__, loop, writelen);
			GOTOERR;
		}
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//UBI文件系统暂不支持该案例，建议5892平台关闭电子盘空间相关测试案例，20140109 郑凯提出，linwl修改
#if !(CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE)
	//case1: 再次调用NDK_FsGetDiskSpace取电子盘剩余空间
	//若相差一个文件大小,或相差一个页大小(如,128M flash的页是2KB,64M的是512B,32M的是?...).这些都视为合理预期
	NDK_FsGetDiskSpace(1, &spaceofdisk11);
	if((diff=spaceofdisk1-spaceofdisk11)!=sizeof(writebuf)*LOOP
	&&diff!=sizeof(writebuf)*LOOP+512
	&&diff!=sizeof(writebuf)*LOOP+2048
	&&diff!=sizeof(writebuf)*LOOP+4096
	&&diff!=sizeof(writebuf)*LOOP+1024
	)//若相差一个文件大小,或相差一个页大小(如,128M flash的页是2KB,64M的是512B,32M的是?...).这些都视为合理预期
	{
		send_result("line %d:取电子盘剩余空间出错[原:%ld, 现:%ld]", __LINE__, spaceofdisk1, spaceofdisk11);
		GOTOERR;
	}

	//case2:再次调用NDK_FsGetDiskSpace取已经使用的磁盘空间
	NDK_FsGetDiskSpace(0, &spaceofdisk21);
	if((diff=spaceofdisk21-spaceofdisk2)!=sizeof(writebuf)*LOOP
	&&diff!=sizeof(writebuf)*LOOP+512
	&&diff!=sizeof(writebuf)*LOOP+2048
	&&diff!=sizeof(writebuf)*LOOP+4096
	&&diff!=sizeof(writebuf)*LOOP+1024)//若相差一个文件大小,或相差一个页大小(如,128M flash的页是2KB,64M的是512B,32M的是?...).这些都视为合理预期
	{
		send_result("line %d:取电子盘已用空间出错[原:%ld, 现:%ld]", __LINE__, spaceofdisk2, spaceofdisk21);
		GOTOERR;
	}
#endif

	//case3:删除文件,删除文件后电子盘空间应该恢复到之前(文件信息应完全释放)包括剩余和已使用的都要验证恢复
	if((ret=NDK_FsDel(FILETEST8))!=NDK_OK)
	{
		send_result("line %d:删除文件失败(%d)", __LINE__, ret);
		GOTOERR;
	}
#if K21_ENABLE//目前中端产品的文件系统是基于Linux的,空间并不能立即释放,只有老85的或者低端产品这点要求才能满足20170809 凯哥解释jym修改
#if 0//!K21_ENABLE
	sync();sleep(3);//根据开发要求在获取文件系统空间大小前回收内存空间 20140915 linwl
#endif
	NDK_FsGetDiskSpace(1, &spaceofdisk11);
	if(spaceofdisk1!=spaceofdisk11)
	{
		send_result("line %d:电子盘空间未完全释放(%ld,%ld)", __LINE__, spaceofdisk1, spaceofdisk11);
		GOTOERR;
	}
	NDK_FsGetDiskSpace(0, &spaceofdisk21);
	if(spaceofdisk2!=spaceofdisk21)
	{
		send_result("line %d:电子盘空间未完全释放(%ld,%ld)", __LINE__, spaceofdisk2, spaceofdisk21);
		GOTOERR;
	}
#endif

	//case4:异常测试
	if((ret=NDK_FsGetDiskSpace(1, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:测试失败(%d)", __LINE__, ret);
		goto ERR;
	}

	// 测试通过
	send_result("%s测试通过(%ld)", TESTAPI, spaceofdisk1);
	return;
ERR:
	NDK_FsClose(fp);
	NDK_FsDel(FILETEST8);
	return;
}

