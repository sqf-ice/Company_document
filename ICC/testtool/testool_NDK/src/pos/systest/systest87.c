/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: 
* Author 			: zhengry
* version			: V1.0
* DATE			: 2016/06/02
* directory 		: 
* description		: 摄像头/USB从交叉
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
#define	TESTITEM	"摄像头/USB从交叉"
#define MAXTIME		60
/*----------global variables declaration("extern")--------------*/
static int nConfInit = 0;	 /*是否配置过了参数:0未配置 1已配置*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
void camera_usb(void)
{
	/*private & local definition*/
	int i = 0, j=0, succ = 0, recvLen = 0 , ret = 0, timeout = SO_TIMEO;
	char recvBuf[BUFSIZE_SERIAL+1] = {0}, sendBuf[BUFSIZE_SERIAL+1]={0};

	/*测试前置*/
	if(nConfInit == 0)
	{
		cls_show_msg1(g_keeptime, "摄像头参数未配置");
		return;
	}
	
	while(1)
	{
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
		/*if((system("lsmod | grep uvcvideo")) == 0) 
		{
			system("sudo rmmod uvcvideo");
			system("sudo rmmod bcm589x_otg");
			system("sudo rmmod dwc_common_port_lib");
		}*///卸载摄像头

		//初始化USB数据
		for(j=0;j<BUFSIZE_SERIAL;j++)
			sendBuf[j] = rand()%256;
		cls_show_msg("请将POS和PC通过USB线连接并打开AccessPort工具,任意键继续");
		NDK_PortClose(PORT_NUM_USB);
		if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:第%d次打开USB失败(%d)", __LINE__, i, ret);
			continue;
		}
		cls_show_msg("请打开AccessPort工具串口,任意键继续");
		if((ret = NDK_PortClrBuf(PORT_NUM_USB)) != NDK_OK)	//清接收缓冲
		{				
			cls_show_msg1(g_keeptime, "line %d:第%d次清USB缓冲区失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		//usb发送数据
		if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		//usb接收数据
		cls_show_msg("请将AccessPort接收到的数据复制到发送区并发送数据,任意键继续");
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB,BUFSIZE_SERIAL, recvBuf, timeout*1000, &recvLen)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, BUFSIZE_SERIAL);
			goto ERR;
		}
		//比较收发数据
		if(recvLen != BUFSIZE_SERIAL)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, recvLen, BUFSIZE_SERIAL);
			goto ERR;
		}
		if(MemCmp(sendBuf, recvBuf, BUFSIZE_SERIAL))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次USB数据校验失败", __LINE__, i);
			goto ERR;
		}
		
		//关闭USB
		succ++;
ERR:
		NDK_PortClose(PORT_NUM_USB);
		cls_show_msg("请关闭AccessPort工具串口并清空收发区数据,任意键继续"); 
	}
	
	cls_show_msg("%s测试完成\n总次数:%d\n成功:%d次", TESTITEM, i, succ);
	return;
}

void systest87(void)
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
				camera_usb();
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
