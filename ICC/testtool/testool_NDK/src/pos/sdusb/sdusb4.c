/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: usb
* file name		: 
* Author 			: 
* version			: V1.0
* DATE			: 
* directory 		: 
* description		: 测试NDK_DiskGetInfo能否正确获取信息
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
#define	TESTAPI		"NDK_DiskGetInfo"
#define	TESTFILE	"test.file"
#define	BUFFERSIZE	(1024*20)
#define MAXWAITTIME 30

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
void sdusb4(void)
{
	if(auto_flag==2)
		return;
	/*private & local definition*/
	char rootdir[][16] = {{0}}, fname[64] = {0}, writebuf[BUFFERSIZE] = {0}, readbuf[BUFFERSIZE] = {0};
	int ret = 0, fp = 0, loop = 0,state = 0, cnt = rand()%1023+2 /*测试文件大小大约介于[40K, 20M]*/, wrlen = BUFFERSIZE-rand()%8;
	time_t oldtime = 0, diff1 = 0, diff2 = 0;
	uint punSize ;
	ST_DISK_INFO pstInfo;
//	ST_DISK_INFO diskinfo;
//	ST_DISK_INFO  *pstInfo = &diskinfo;//=(ST_DISK_INFO *)malloc(sizeof(ST_DISK_INFO));
	int disktype = get_disktype();
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//测试前置保护
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:构造测试前置失败(ret=%d)", __LINE__, ret);
		return;	
	}
	
	//case1,不打开直接调用DiskGetInfo
	if((ret=NDK_DiskGetInfo(rootdir[0],&pstInfo))!=NDK_ERR)
	{		
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//插入设备
	cls_show_msg1(3,"请确保已插上%s,按任意键继续...",(disktype==UDISK)?"U盘":"SD卡");
	if((ret=NDK_DiskOpen(disktype,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:%s失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	oldtime=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&state))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		} 
		if(state==DISKMOUNTSUCC)
			break;
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败超时", __LINE__, TESTAPI);	
			goto ERR;	
		}
		//usleep(100);
		NDK_SysMsDelay(1);
	}
	if((ret=NDK_DiskGetRootDirName(disktype,(char **)rootdir))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:非法参数测试
	if((ret=NDK_DiskGetInfo(NULL,&pstInfo))!=NDK_ERR_USDDISK_PARAM)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_DiskGetInfo(rootdir[0],NULL))!=NDK_ERR_USDDISK_PARAM)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:正常操作
	if((ret=NDK_DiskGetInfo(rootdir[0],&pstInfo))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	else
	{
		if(auto_flag==0||auto_flag==2)
		{
			if(cls_show_msg1(MAXWAITTIME, "剩余空间:[%u]k,总空间:[%u]k.\n"
										"确认键:一致\n"
										"其它键:有误\n", pstInfo.unFreeSpace,pstInfo.unTotalSpace)!=ENTER)

			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				GOTOERR;		
			}
		}
		else
			send_result("剩余空间:[%u]k,总空间:[%u]k.请判断是否正确", pstInfo.unFreeSpace,pstInfo.unTotalSpace);
	}

	//case3,进行读写操作，然后验证空间大小
	cls_show_msg1(5, "以下测试使用%dB作为每次读写长度!", wrlen);
	sprintf(fname, "%s/%s", rootdir[0], TESTFILE);
	NDK_FsDel(fname);
	if ((fp=NDK_FsOpen(fname, "w"))<0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	cls_printf("生成%s文件中(约%dkB),请稍候...",(disktype==UDISK)?"U盘":"SD卡", cnt*wrlen/1024);
	for (loop=0; loop<wrlen; loop++)
		writebuf[loop] = rand()%256;
	oldtime = time(NULL);
	for (loop=0; loop<cnt; loop++)
	{
		if ((ret=NDK_FsWrite( fp,writebuf, wrlen))!=wrlen)
		{
			NDK_FsClose(fp);
			NDK_FsDel(fname);
			send_result("line %d:%s测试失败(cnt=%d, loop=%d, ret=%d, wrlen=%d)", __LINE__, TESTAPI, cnt, loop, ret, wrlen);
			GOTOERR;
		}
		show_stopwatch(ENABLE, (diff1=time(NULL)-oldtime));
	}
	show_stopwatch(DISABLE, 0);

	if(NDK_FsClose(fp)!=NDK_OK)
	{
		NDK_FsDel(fname);
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;		
	}
	if((ret=NDK_DiskClose(disktype))!=NDK_OK)
	{
		send_result("line %d:UsbDiskClose失败(ret:%d)", __LINE__, ret);
		GOTOERR;
	}
	Beep(3);//写完成的提示音
	cls_show_msg1(3,"按任意键继续...");
	if((ret=NDK_DiskOpen(disktype,MAXWAITTIME))!=NDK_OK)
	{
		send_result("line %d:UsbDiskOpen失败(ret:%d)", __LINE__, ret);
		GOTOERR;
	}
	oldtime=time(NULL);
	while(1)
	{
		if((ret=NDK_DiskGetState(disktype,&state))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		} 
		if(state==DISKMOUNTSUCC)
			break;
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			send_result("line %d:%s测试失败超时", __LINE__, TESTAPI);	
			goto ERR;	
		}
		//usleep(100);
		NDK_SysMsDelay(1);
	}
	if ((fp=NDK_FsOpen(fname, "r"))<0)
	{
		NDK_FsDel(fname);
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	//文件大小是否发生改变
	if ((NDK_FsFileSize(fname, &punSize))!=NDK_OK)
	{	
		NDK_FsClose(fp);
		NDK_FsDel(fname);
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	//fprintf(stderr,"%s %d %d\n",__FUNCTION__,__LINE__,*punSize);
	if ((punSize)!=cnt*wrlen)
	{	
		NDK_FsClose(fp);
		NDK_FsDel(fname);
		send_result("line %d:%s测试失败(实测:%dB, 预期:%dB)", __LINE__, TESTAPI, ret, cnt*wrlen);
		GOTOERR;
	}
	cls_printf("校验U盘文件中(约%dkB),请稍候...", cnt*wrlen/1024);
	oldtime = time(NULL);
	for (loop=0; loop<cnt; loop++)
	{
		if ((ret=NDK_FsRead( fp,readbuf, wrlen))!=wrlen||memcmp(writebuf, readbuf, wrlen))
		{
			NDK_FsClose(fp);
			NDK_FsDel(fname);
			send_result("line %d:%s测试失败(cnt=%d, loop=%d, ret=%d, wrlen=%d)", __LINE__, TESTAPI, cnt, loop, ret, wrlen);
			debug_buf("发送缓冲", writebuf, wrlen);
			debug_buf("接收缓冲", readbuf, ret);
			cls_show_msg("请保存数据并关闭串口,任意键继续...");//防止过早进入UsbDiskClose后,也将USB调试信息记录下来产生干扰
			GOTOERR;
		}
		show_stopwatch(ENABLE, (diff2=time(NULL)-oldtime));
	}
	show_stopwatch(DISABLE, 0);
	if((ret=NDK_DiskGetInfo(rootdir[0],&pstInfo))!=NDK_OK)
	{
		NDK_FsClose(fp);
		NDK_FsDel(fname);
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	else
	{
		if(auto_flag==0||auto_flag==2)
		{
			if(cls_show_msg1(MAXWAITTIME, "剩余空间:[%u]k,总空间:[%u]k.\n"
										"确认键:一致\n"
										"其它键:有误\n", pstInfo.unFreeSpace,pstInfo.unTotalSpace)!=ENTER)

			{
				NDK_FsClose(fp);
				NDK_FsDel(fname);
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);		
				goto ERR;
			}
		}
		else
			send_result("剩余空间:[%u]k,总空间:[%u]k.请判断是否正确", pstInfo.unFreeSpace,pstInfo.unTotalSpace);
	}

	//test over
	send_result("%s测试通过(写%dkB/s,读%dkB/s)", TESTAPI, (cnt*wrlen)/(1024*diff1), (cnt*wrlen)/(1024*diff2));
	NDK_FsClose(fp);
	NDK_FsDel(fname);
ERR:
	show_stopwatch(DISABLE, 0);
	NDK_DiskClose(disktype);
	return;
}

