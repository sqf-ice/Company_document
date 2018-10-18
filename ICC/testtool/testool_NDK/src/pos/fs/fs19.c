/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name			: fs19.c
* Author 			: zhengry
* version			: 
* DATE				: 20150723
* directory 		: 
* description		: 本例测试K21文件系统最大文件数和最大文件大小
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if defined SP610
#define  FILETEST19 "fs19_"  
#define  FILETEST19_1 "fs19_1"  
#define  FILE_TEST1 "ak_main.data"  
#define  FILE_TEST2 "ak_main.idx" 
#else
#define  FILETEST19 "/appfs/fs19_"  //相对路径
#define  FILETEST19_1 "/appfs/fs19_1"  //相对路径
#define  FILE_TEST1 "/appfs/ak_main.data"  //相对路径
#define  FILE_TEST2 "/appfs/ak_main.idx"  //相对路径
#endif

#define TESTAPI	"最大文件数和最大文件大小"
#if defined ME31
#define MAX_FLIES_NUM 20
#define MAX_FLIES_SIZE 360		//软件限制的单个文件最大长度,单位KB
#elif defined ME30||defined ME30S||defined ME50||defined ME30MH||defined ME30GX //||defined ME62
#define MAX_FLIES_NUM 55
#define MAX_FLIES_SIZE 512
#elif defined ME50NX||defined ME68
#define MAX_FLIES_NUM 254
#define MAX_FLIES_SIZE 65535
#elif defined ME11||defined ME15
#define MAX_FLIES_NUM 27
#define MAX_FLIES_SIZE 256		
#elif defined ME15C||defined ME20
#define MAX_FLIES_NUM 17
#define MAX_FLIES_SIZE 256
#elif defined ME30THM||defined ME32THM//原来文件8K一个单位，最大文件数是11，现在改为4K一个单位，最大文件数为24
#define MAX_FLIES_NUM 24
#define MAX_FLIES_SIZE 88
#elif defined ME32
#define MAX_FLIES_NUM 22
#define MAX_FLIES_SIZE 72
#elif defined ME65
#define MAX_FLIES_NUM 27
#define MAX_FLIES_SIZE 92
#elif defined ME32GX
#define MAX_FLIES_NUM 20
#define MAX_FLIES_SIZE 95
//SP610新文件系统 文件个数上限为306个,单个文件最大大小没有限制,预期能够写满文件系统,写满后文件系统空间为0,文件空间被多个文件写满情况下,删除某个或多个文件后,预期剩余的文件内容不应该被修改
#elif defined SP610||defined ME50N||defined ME50H||defined ME50C||defined ME62
#define MAX_FLIES_NUM 306
#define MAX_FLIES_SIZE 65535	//如果软件没有限制,就设置成最大值
#endif

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
*			  	  	  linwl       20150723       created
*****************************************************************/
void fs19(void)
{
	/*private & local definition*/
	int ret=0, fp1=0, writelen=0, i=0, maxfilessize = 0;
	char writebuf1[1024] = {0};
	char fname[13]={0};
	ulong pulSpace = 0;
	
	/*process body*/
	if(auto_flag==1)//NDK_FsClearAppFile函数会删除paraconf文件，故不自动测试
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//测试前置:
	//删除可能存在的测试文件
#if 0//defined ME15C	//不支持NDK_FsClearAppFile函数的设备需打开本开关使用NDK_FsFormat函数
	if(cls_show_msg("首次进入此案例,按[Enter]进行格式化,再次进入时按[其他]开始测试")==ENTER)
	{
		cls_show_msg1(3, "3秒后将格式化电子盘...格式化后将重启,重启后再次快选进入本用例进行测试");
		if((ret=NDK_FsFormat())!=NDK_OK)
		{
			send_result("line %d:格式化电子盘失败(ret=%d)", __LINE__, ret);
			goto ERR;
		}
	}
#else
	if((ret=NDK_FsClearAppFile())!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif

	NDK_FsDel(FILE_TEST1);
	NDK_FsDel(FILE_TEST2);
	for (i=0; i<sizeof(writebuf1); i++)
		writebuf1[i]=rand()%256;
	
	//case1:创建55个文件,前55个应该成功第56个应该失败
	for(i=0; i<MAX_FLIES_NUM; i++)
	{
		sprintf(fname, "%s%d", FILETEST19, i+1);
		if ((fp1=NDK_FsOpen(fname, "w"))<0)
		{
			send_result("line %d: %s测试失败(%d)(第%d个文件)", __LINE__, TESTAPI, fp1, i+1);
			goto ERR;
		}
		//ME32不足1k的文件也是分配4K的空间，最大可创建22个文件(开发设计规定)，但实际从19个开始，因为空间不够，就无法写入1k数据了，所以只判断前18份文件写入的长度 linying modified 20171212
#if defined ME32
        if(i<18) 
	{
		if((writelen=NDK_FsWrite(fp1,writebuf1, 1024))!=1024)
		{
		    send_result("line %d: %s测试失败(%d)(第%d个文件)", __LINE__, TESTAPI, writelen, i+1);
		    NDK_FsClose(fp1);
		    goto ERR;
		}
        }
#elif defined ME32GX  //ME32GX 最大可创建20个文件但实际从15个开始，因为空间不够，就无法写入1k数据了
	if(i<14) 
	{
		if((writelen=NDK_FsWrite(fp1,writebuf1, 1024))!=1024)
		{
		    send_result("line %d: %s测试失败(%d)(第%d个文件)", __LINE__, TESTAPI, writelen, i+1);
		    NDK_FsClose(fp1);
		    goto ERR;
		}
        }
#elif defined ME65  
	if(i<23) 
	{
		if((writelen=NDK_FsWrite(fp1,writebuf1, 1024))!=1024)
		{
	   	 	send_result("line %d: %s测试失败(%d)(第%d个文件)", __LINE__, TESTAPI, writelen, i+1);
	    	NDK_FsClose(fp1);
	    	goto ERR;
		}
    	}
#else
        if((writelen=NDK_FsWrite(fp1,writebuf1, 1024))!=1024)
		{
			send_result("line %d: %s测试失败(%d)(第%d个文件)", __LINE__, TESTAPI, writelen, i+1);
			NDK_FsClose(fp1);
			goto ERR;
	    }
#endif
		NDK_FsClose(fp1);
	}
	//创建第56个文件应该失败
	sprintf(fname, "%s%d", FILETEST19, i+1);
	if ((fp1=NDK_FsOpen(fname, "w"))>=0)
	{
		send_result("line %d: %s测试失败(%d)(第%d个文件)", __LINE__, TESTAPI, fp1, i+1);
		goto ERR;
	}
#if defined ME15C||defined ME20	//ME15C删除之前所创建的文件,资源有限
	for(i=0; i<MAX_FLIES_NUM; i++)
	{
		sprintf(fname, "%s%d", FILETEST19, i+1);//构造文件名
		if ((ret=NDK_FsDel(fname))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)(第%d个文件)", __LINE__, TESTAPI, ret, i+1);
			goto ERR;
		}
	}
#else	
	//删除一个已存在的文件
	NDK_FsDel(FILETEST19_1);
#endif

	//通过将取到的硬件剩余空间和软件限制的大小进行比较,调整实际测试时最大文件大小为较小的值
	NDK_FsGetDiskSpace(1,&pulSpace);
	maxfilessize = pulSpace==0?MAX_FLIES_SIZE:((pulSpace/1024)<MAX_FLIES_SIZE)?(pulSpace/1024):MAX_FLIES_SIZE;	
	//case2:文件最大长度为512K，写入第512K+1字节应该失败
	if ((fp1=NDK_FsOpen(FILETEST19_1, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp1);
		goto ERR;
	}
	for(i=0;i<maxfilessize;i++)
	{
		if((writelen=NDK_FsWrite(fp1,writebuf1, 1024))!=1024)
		{
			if(i+1==maxfilessize)
				;//由于每个文件需要一些空间存放文件信息,所以无法写满512K,最后1K写不了1024
			else
			{
				send_result("line %d: %s测试失败(%d)(第%dK)", __LINE__, TESTAPI, writelen, i+1);
				NDK_FsClose(fp1);
				goto ERR;
			}
		}
	}
	if((writelen=NDK_FsWrite(fp1,writebuf1, 1))!=0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		NDK_FsClose(fp1);
		goto ERR;
	}
	NDK_FsClose(fp1);

	//测试通过
	send_result("%s测试通过", TESTAPI);
ERR:
#if 0//defined ME15C	//ME15C删除之前所创建的文件,资源有限
	for(i=0; i<MAX_FLIES_NUM; i++)
	{
		sprintf(fname, "%s%d", FILETEST19, i+1);
		NDK_FsDel(fname);
	}
#else
	NDK_FsClearAppFile();
#endif
	return;
}

