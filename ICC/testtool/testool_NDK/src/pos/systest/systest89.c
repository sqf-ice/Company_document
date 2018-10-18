/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: 
* Author 			: zhengry
* version			: V1.0
* DATE			: 2016/06/02
* directory 		: 
* description		: 摄像头/U盘交叉
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  2016/06/02  zhengry
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"摄像头/U盘交叉"
#define MAXTIME		60
#define TESTFILE   "cameratest.file"
#define	BUFFERSIZE	(1024*5) //缓冲区开小些，以免冲破堆栈限制破坏程序
#define GETSTATETIMES 30//获取状态次数

/*----------global variables declaration("extern")--------------*/
static int nConfInit = 0;	 /*是否配置过了参数:0未配置 1已配置*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
void camera_UDISK(void)
{
	/*private & local definition*/
	int i = 0, j=0, succ = 0, ret=0, timeout = SO_TIMEO, fp = 0, nCnt=0, state = 0, wrlen = 0;
	char writebuf[BUFFERSIZE] = {0}, readbuf[BUFFERSIZE] = {0};
	char rootdir[][16] ={{0}}, fname[64] = {0};
	/*process body*/		
	cls_show_msg_record(FILENAME,FUNCNAME,"请确保已安装U盘,任意键继续");	
	/*测试前置*/
	if(nConfInit == 0)
	{
		cls_show_msg1(g_keeptime, "摄像头参数未配置");
		return;
	}
	//先把存在的测试文件赋值给fname
	NDK_DiskGetRootDirName(UDISK, (char**)rootdir);//经陈蕃铭确认可以直接调用获取根目录不需调用open
	sprintf(fname, "%s/%s", rootdir[0], TESTFILE);//删除测试文件避免由于测试文件存在引起后续读写校验错误，例如前期读写过程中异常断电 

	while(1)
	{
		//保护动作
ERR:
		NDK_FsClose(fp);
		NDK_FsDel(fname);
		NDK_DiskClose(UDISK);
		
		if(cls_show_msg1(2,"正在进行第%d次%s测试(已成功%d次),按ESC退出", i+1, TESTITEM, succ)==ESC)
			break;
		i++;
		sleep(5);//每次挂断之后重新连接要等待5秒,减轻绎芯片的压力

		//摄像头操作
		if((ret=NDK_ScanInit())!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
			continue;
		}
		NDK_ScanSet(CAMERA_SETTYPE_IMAGE, g_ShowImage);
		NDK_ScanSet(CAMERA_SETTYPE_SATURATION, g_Saturation);
		NDK_ScanSet(CAMERA_SETTYPE_SHARPNESS, g_Sharpness);
		NDK_ScanSet(CAMERA_SETTYPE_GAMMA, g_Contrast);
		if((ret=lib_doscan(g_ScanType, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
			if(g_ScanType==1)
				NDK_ScanClose();
			continue;
		}
		if(g_ScanType==1)
			NDK_ScanClose();

		if((ret=NDK_DiskOpen(UDISK,timeout))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次打开U盘失败(%d)",  __LINE__, i, ret);
			continue;
		}
		//获取设备状态
		nCnt = 0;
		while(1)
		{
			if((ret=NDK_DiskGetState(UDISK,&state))!=NDK_OK)
				;
			if(state==DISKMOUNTSUCC)
				break;
			sleep(1);
			if(++nCnt>=GETSTATETIMES)
			{
				cls_show_msg1(g_keeptime,"line %d:第%d次未获取到DISKMOUNTSUCC状态", __LINE__, i);
				goto ERR;
			}
		}
		//打开文件
		if((fp=NDK_FsOpen(fname, "w"))<0)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次创建测试文件失败(%d)", __LINE__, i, fp);
			continue;
		}
		//往文件写数据
		wrlen = 1024;//
		for (j=0; j<wrlen; j++)
			writebuf[j] = rand()%256;//内容随机
		cls_printf("生成设备文件中(约%dB),请稍候...",wrlen);
		if ((ret=NDK_FsWrite(fp,writebuf,wrlen))!=wrlen)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次写测试文件失败(%d)", __LINE__, i, ret);
			continue;
		}
		//关闭文件
		if(NDK_FsClose(fp)!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次关闭测试文件失败(%d)", __LINE__, i, ret);
			continue;
		}
		//关闭设备
		if((ret=NDK_DiskClose(UDISK))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NDK_DiskClose失败(%d)", __LINE__, i, ret);
			continue;
		}
		//重新打开设备校验文件
		cls_printf("重新打开设备校验文件,请稍候...");
		if((ret=NDK_DiskOpen(UDISK,timeout))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NDK_DiskOpen失败(%d)", __LINE__, i, ret);
			continue;
		}
		//获取设备状态
		nCnt = 0;
		while(1)
		{
			if((ret=NDK_DiskGetState(UDISK,&state))!=NDK_OK)
				;
			if(state==DISKMOUNTSUCC)
				break;
			sleep(1);
			if(++nCnt>=GETSTATETIMES)
			{
				cls_show_msg1(g_keeptime,"line %d:第%d次未获取到DISKMOUNTSUCC状态", __LINE__, i);
				goto ERR;
			}
		}
		//打开文件
		if ((fp=NDK_FsOpen(fname, "r"))<0)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次创建测试文件失败(%d)", __LINE__, i, fp);
			continue;
		}
		//读文件
		if ((ret=NDK_FsRead(fp,readbuf, wrlen))!=wrlen||memcmp(writebuf, readbuf, wrlen))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次读文件失败(%d)", __LINE__, i, ret);
			continue;
		}		
		//删除文件和关闭设备
		NDK_FsClose(fp);
		NDK_FsDel(fname);
		//U盘关闭
		if((ret=NDK_DiskClose(UDISK))!=NDK_OK)//会调用umount
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NDK_DiskClose失败(%d)", __LINE__, i, ret);
			continue;
		}
		succ++;
	}
	
	cls_show_msg("%s测试完成\n总次数:%d\n成功:%d次", TESTITEM, i, succ);
	return;
}

void systest89(void)
{
	int nKeyin=0;

	cls_show_msg1(2, "%s测试中...", TESTITEM);
	while(1)
	{
		nKeyin=cls_show_msg("1.交叉测试\n"
							"0.摄像头配置\n");
		switch(nKeyin)
		{
			case '1':
				camera_UDISK();
				break;
		    case '0':
				conf_camera();
				nConfInit=1;
				break;
			case ESC:
				return;
				break;
			default:
				continue;
				break;
		}
	}

	return;
}
