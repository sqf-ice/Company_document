/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: 
* Author 			: zhengry
* version			: V1.0
* DATE			: 2016/06/02
* directory 		: 
* description		: 摄像头/USB主交叉交叉
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
#define	TESTITEM	"摄像头/USB主交叉"
#define MAXTIME		60
#define MAX_SIZE  1024 //BUFSIZE_SERIAL 

/*----------global variables declaration("extern")--------------*/
static int nConfInit = 0;	 /*是否配置过了参数:0未配置 1已配置*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
void camera_usb_host(void)
{
	/*private & local definition*/
	int i = 0, j=0, succ = 0, recvLen = 0 , ret=0, timeout = SO_TIMEO;
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
	
		//初始化USB数据
		for(j=0;j<MAX_SIZE;j++)
			sendBuf[j] = rand()%256;
		cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");		
		NDK_PortClose(PORT_NUM_USB_HOST);
		if((ret = NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,20")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:第%d次打开USB失败(%d)", __LINE__, i, ret);
			continue;
		}
		cls_show_msg("从模式POS打开串口,任意键继续");
		if((ret = NDK_PortClrBuf(PORT_NUM_USB_HOST)) != NDK_OK)	//清接收缓冲
		{				
			cls_show_msg1(g_keeptime, "line %d:第%d次清USB缓冲区失败(%d)", __LINE__, i, ret);
			continue;
		}
		//usb主模式发送数据
		cls_show_msg("从模式POS开启收发数据后,主pos马上任意键继续");
		if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, MAX_SIZE, sendBuf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		//主usb接收数据
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(PORT_NUM_USB_HOST,MAX_SIZE, recvBuf, timeout*1000, &recvLen)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, MAX_SIZE);
			goto ERR;
		}
		//比较收发数据
		if(recvLen != MAX_SIZE)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, recvLen, MAX_SIZE);
			goto ERR;
		}
		if(MemCmp(sendBuf, recvBuf, MAX_SIZE))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次USB数据校验失败", __LINE__, i);
			goto ERR;
		}
	
		//关闭从USB
		cls_show_msg("等待从POS退出收发数据后关闭USB串口,任意键继续");

		succ++;

		//关闭主USB
ERR:
		NDK_PortClose(PORT_NUM_USB_HOST);		
		cls_show_msg("关闭从模式POS的USB串口,任意键继续");
	}
	
	cls_show_msg("%s测试完成\n总次数:%d\n成功:%d次", TESTITEM, i, succ);
	return;
}

void systest88(void)
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
				camera_usb_host();
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
