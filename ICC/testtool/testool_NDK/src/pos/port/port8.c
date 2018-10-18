/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 串口模块
* file name		: 
* Author 			: liny
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 测试ME66A指令功能
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define		TESTAPI		"ME66A指令测试"
#define WAITTIME 30
#define		MAXWAITTIME 10//接收等待时间

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
*			  				liny		          20180730    	 		created
*****************************************************************/
void port8(void)
{
	//以下为局部变量
	int ret = 0,nkeyin=0;
	//应用发送每条指令后面都会跟回车换行
    	char sendbuf[200]={0}, recbuf[200]={0},expected[]={"<STX><0009><SET><00><01><00><ETX><5E>\x0d\x0a"},expected1[]={"<STX><0114><SET><00><01><00><ECHO=1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890><ETX><6D>\x0d\x0a"};
	int len = 0;

	//以下为函数体
	//测试前置:关闭串口
	if((ret=NDK_PortClose(PORT_NUM_USB_HOST))!=NDK_OK)
	{
		send_result("line %d:串口关闭失败(%d)", __LINE__, ret);
		GOTOERR;
   	}
	
	//case1:设置音量为1
	cls_show_msg("请连接扫码器,任意键继续");
	if ((ret=NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:串口打开失败(ret:%d)", __LINE__, ret);
		GOTOERR;
	}
	if((ret = NDK_PortClrBuf(PORT_NUM_USB_HOST)) != NDK_OK)	/*清接收缓冲*/
	{				
		cls_show_msg("line %d:清缓冲区失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB_HOST);
		return;	
    }
	memset(sendbuf, 0, sizeof(sendbuf));
	memcpy(sendbuf, "<STX><0015><SET><00><00><VOLUME=1><ETX><56>", sizeof(sendbuf));
	//主模式发送指令
	if((ret = NDK_PortWrite(PORT_NUM_USB_HOST, strlen(sendbuf), sendbuf))!=NDK_OK)
	{
		send_result("line %d:串口写失败(ret:%d)", __LINE__,  ret);
		GOTOERR;
	}
	if((nkeyin=cls_show_msg1(WAITTIME,"请确认开机首次运行此case,音量变小,语音播报内容为:音量1.是按确认键,否则按其它键"))!=ENTER)
	{
		send_result("line %d:语音播报内容异常", __LINE__);
		GOTOERR;
	}
	memset(recbuf, 0, sizeof(recbuf));
	//接收指令
	if((ret = NDK_PortRead(PORT_NUM_USB_HOST,sizeof(recbuf),recbuf,MAXWAITTIME*1000,&len))!=NDK_OK)
	{
		send_result("line %d:串口读失败(ret:%d)", __LINE__, ret);
		GOTOERR;
	}
	/*for(i=0; i<strlen(recbuf);i++)
        {
            fprintf(stderr,"%d:[%02x] ",i,recbuf[i] );
        }
		send_result("\n");
		for(i=0; i<sizeof(expected);i++)
        {
            fprintf(stderr,"%d:[%02x] ",i,expected[i] );
        }*/
	if(memcmp(recbuf,expected,strlen(recbuf)))
	{
		send_result("line %d:与预期返回数据不一致(%d,%d)", __LINE__, len, sizeof(expected));
		GOTOERR;
	}
		
	//case2:	长字节发送
	memset(sendbuf, 0, sizeof(sendbuf));
	memcpy(sendbuf, "<STX><0081><SET><00><00><VOICE=00521 2 3 4 5 6 7 8 9 0ABCDEFGHIJKLMNOPQRSTUVWXYZ中国福建福州马尾+=_><ETX><48>", sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_USB_HOST, strlen(sendbuf), sendbuf))!=NDK_OK)
	{
		send_result("line %d:串口写失败(ret:%d)", __LINE__,  ret);
		GOTOERR;
	}
	if((nkeyin=cls_show_msg1(WAITTIME,"请确认语音播报内容为:1 2 3 4 5 6 7 8 9 0ABCDEFGHIJKLMNOPQRSTUVWXYZ中国福建福州马尾+=_.是按确认键,否则按其它键"))!=ENTER)
	{
		send_result("line %d:语音播报内容异常", __LINE__);
		GOTOERR;
	}
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB_HOST,sizeof(recbuf),recbuf,MAXWAITTIME*1000,&len))!=NDK_OK)
	{
		send_result("line %d:串口读失败(ret:%d)", __LINE__, ret);
		GOTOERR;
	}
	if(memcmp(recbuf,expected,strlen(recbuf)))
	{
		send_result("line %d:与预期返回数据不一致(%d,%d)", __LINE__, len, sizeof(expected)-1);
		GOTOERR;
	}
	
	//case3:短字节发送
	//关闭LED灯
	memset(sendbuf, 0, sizeof(sendbuf));
	memcpy(sendbuf, "<STX><0014><SET><00><00><LED=OFF><ETX><6C>", sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_USB_HOST, strlen(sendbuf), sendbuf))!=NDK_OK)
	{
		send_result("line %d:串口写失败(ret:%d)", __LINE__,  ret);
		GOTOERR;
	}
	if((nkeyin=cls_show_msg1(WAITTIME,"请确认LED灯是否关闭.是按确认键,否则按其它键"))!=ENTER)
	{
		send_result("line %d:LED灯关闭失败", __LINE__);
		GOTOERR;
	}
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB_HOST,sizeof(recbuf),recbuf,MAXWAITTIME*1000,&len))!=NDK_OK)
	{
		send_result("line %d:串口读失败(ret:%d)", __LINE__, ret);
		GOTOERR;
	}
	if(memcmp(recbuf,expected,strlen(recbuf)))
	{
		send_result("line %d:与预期返回数据不一致(%d,%d)", __LINE__, len, sizeof(expected)-1);
		GOTOERR;
	}
	//开启LED灯
	memset(sendbuf, 0, sizeof(sendbuf));
	memcpy(sendbuf, "<STX><0013><SET><00><00><LED=ON><ETX><25>", sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_USB_HOST, strlen(sendbuf), sendbuf))!=NDK_OK)
	{
		send_result("line %d:串口写失败(ret:%d)", __LINE__,  ret);
		GOTOERR;
	}
	if((nkeyin=cls_show_msg1(WAITTIME,"请确认LED灯是否开启.是按确认键,否则按其它键"))!=ENTER)
	{
		send_result("line %d:LED灯关闭失败", __LINE__);
		GOTOERR;
	}
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB_HOST,sizeof(recbuf),recbuf,MAXWAITTIME*1000,&len))!=NDK_OK)
	{
	    send_result("line %d:串口读失败(ret:%d)", __LINE__, ret);
		GOTOERR;
	}
	if(memcmp(recbuf,expected,strlen(recbuf)))
	{
		send_result("line %d:与预期返回数据不一致(%d,%d)", __LINE__, len, sizeof(expected)-1);
		GOTOERR;
	}
	
	//case4:回显测试
	memset(sendbuf, 0, sizeof(sendbuf));
	memcpy(sendbuf, "<STX><0112><SET><00><00><ECHO=1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890><ETX><68>", sizeof(sendbuf));
	if((ret = NDK_PortWrite(PORT_NUM_USB_HOST, strlen(sendbuf), sendbuf))!=NDK_OK)
	{
		send_result("line %d:串口写失败(ret:%d)", __LINE__,  ret);
		GOTOERR;
	}
	memset(recbuf, 0, sizeof(recbuf));
	if((ret = NDK_PortRead(PORT_NUM_USB_HOST,sizeof(recbuf),recbuf,MAXWAITTIME*1000,&len))!=NDK_OK)
	{
		send_result("line %d:串口读失败(ret:%d)", __LINE__, ret);
		GOTOERR;
	}
	if(memcmp(recbuf,expected1,strlen(recbuf)))
	{
		send_result("line %d:与预期返回数据不一致(%d,%d)", __LINE__, len, sizeof(expected1)-1);
		GOTOERR;
	}
	if((ret=NDK_PortClose(PORT_NUM_USB_HOST))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
	    GOTOERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	if (g_com!=INVALID_COM)
	{
		if ((ret=NDK_PortOpen(g_com, "115200,8,N,1")) != NDK_OK)
		{
			cls_show_msg("line %d: 初始化串口错", __LINE__);
			return;
		}
	}
	return;
}


