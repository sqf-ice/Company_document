/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	USB主/3g深交叉
* file name			: systest69.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: USB主/3g深交叉
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20131202  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"USB主/3g深交叉"
#define MAX_SIZE  1024 //BUFSIZE_SERIAL 

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
* history 				: author			date			remarks
					 	jiangym			20140808	created
*****************************************************************/	
static void HOST_USB_WLM(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, j = 0, succ = 0, timeout = SO_TIMEO, slen = 0, rlen = 0, recvLen = 0 ;
	char recvBuf[MAX_SIZE+1] = {0}, sendBuf[MAX_SIZE+1]={0};
	uint h_tcp = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;

	/*process body*/
	//初始化无线和USB数据
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	for(j=0;j<MAX_SIZE;j++)
		sendBuf[j] = rand()%256;
	cls_show_msg("请将主POS和从POS通过USB线连接,任意键继续");		
	
	while(1)
	{
		//保护动作
		NDK_PortClose(PORT_NUM_USB_HOST);
		TransDown(h_tcp);
		NetDown();
		
		if(cls_show_msg1(2,"正在进行第%d次%s测试(已成功%d次),按ESC退出", i+1, TESTITEM, succ)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		
		sleep(5);//每次挂断之后重新连接要等待5秒,减轻绎芯片的压力
		
		if(NetUp()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NetUp失败", __LINE__, i);
			continue;
		}
		//主模式打开USB串口
		if((ret = NDK_PortOpen(PORT_NUM_USB_HOST, "115200,8,N,1,20")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:第%d次打开USB失败(%d)", __LINE__, i, ret);
			continue;
		}
		cls_show_msg("从模式POS打开串口,任意键继续");
		
		//无线
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次TransUp失败", __LINE__, i);
			goto ERR;
		}

		//主USB
		if((ret = NDK_PortClrBuf(PORT_NUM_USB_HOST)) != NDK_OK)	/*清接收缓冲*/
		{				
			cls_show_msg1(g_keeptime, "line %d:第%d次清USB缓冲区失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		
		//无线发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(实际%d, 预期%d)", __LINE__, i, slen, sendpacket.len);
			goto ERR;
		}
		
		//usb主模式发送数据
		cls_show_msg("从模式POS开启收发数据后,主pos马上任意键继续");
		if((ret=NDK_PortWrite(PORT_NUM_USB_HOST, MAX_SIZE, sendBuf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			goto ERR;
		}
		
		//无线接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次无线接收失败(实际%d, 预期%d)", __LINE__, i, rlen, sendpacket.len);
			goto ERR;
		}
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次无线数据校验失败", __LINE__, i);
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
			
		//无线挂断
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:第%d次TransDown失败", __LINE__, i);
			goto ERR;
		}
		
		//关闭从USB
		cls_show_msg("等待从POS退出收发数据后关闭USB串口,任意键继续");
			
		if (NetDown()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NetDown失败", __LINE__, i);
			continue;
		}
		//关闭主USB
		NDK_PortClose(PORT_NUM_USB_HOST);		
		
		succ++;
		continue;
	ERR:
		cls_show_msg("关闭从模式POS的USB串口,任意键继续");
	}
	
	cls_show_msg("%s测试完成\n总次数:%d\n成功:%d次", TESTITEM, i, succ);
	return;
}

void systest70(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	cls_show_msg("从POS运行systest32,打开客户端操作,任意键继续");
	
	while (1)
	{
		keyin = cls_show_msg("1.USB主/3g交叉测试\n"
							"0.无线配置");
		switch(keyin)
		{
		case '1':
			HOST_USB_WLM();
			break;
		case '0':
			switch(conf_conn_WLM(TRUE))
			{
			case SUCC:
				cls_show_msg1(2, "网络配置成功!");
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
				//return;
				break;
			case NDK_ERR_QUIT://用户放弃
			default:
				//return;
				break;
			}
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

