/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest20.c
* Author 			: chensl
* version			: 
* DATE			: 20130124
* directory 		: 
* description		: 打印/LAN交叉
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM		"打印/LAN交叉"
#define	MAXWAITTIME	(120)	//30->120 for 针打

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
*			  		chensl		   20130124	 		created
*****************************************************************/
static void print_LAN(void)
{
	/*private & local definition*/
	int i = 0, succ_count = 0;
	uint h_tcp = -1;
	int slen = 0, rlen = 0, timeout = SO_TIMEO;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;

	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:NetUp失败", __LINE__);
		return;
	}

	//if(NDK_PrnInit(g_PrnSwitch)!=NDK_OK)//if (FAIL==clrprintbuf())
	if(lib_initprn(g_PrnSwitch)!=NDK_OK)//初始化使配置有效modify 20131122 jiangym
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机复位失败", __FILE__, __LINE__);
		return;
	}
	print_bill();
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:打印机状态异常!", __LINE__);
		return; 						
	}
	while (1)
	{
		if(cls_show_msg1(2,"正在进行第%d次%s测试(已成功%d次),按ESC退出", i+1, TESTITEM, succ_count)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
				
		//UP
		if(TransUp(&h_tcp)!=SUCC)
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:TransUp失败", __LINE__);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;						  
		}
		
		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;						  
		}
		
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, sendpacket.len);
			continue;
		}
		
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:校验失败", __LINE__);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			TransDown(h_tcp);
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;						  
		}
		
		//DOWN
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:TransDown失败", __LINE__);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;						  
		}		
		succ_count++;	
	}
	
	NetDown();
	print_bill();
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:打印机状态异常!", __LINE__);
		return; 						
	}
	cls_show_msg_record(FILENAME ,FUNCNAME ,"通讯次数:%d 成功次数:%d", i, succ_count);
	return;
}

void systest20(void)
{
	/*private & local definition*/
	int keyin = 0, tmpflag = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=0)
		SetWifiStopflag(0);//确保本用例在维持wifi连接的情况下测试
		
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		//LAN配置
		if(conf_conn_LAN()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,LAN配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		//打印配置
		conf_prn();
		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉测试
		print_LAN();
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		SetWifiStopflag(tmpflag);
		return;
	}
	while (1)
	{	
		keyin = cls_show_msg("1.交叉测试\n"
							"0.LAN配置\n"
							"9.打印配置");
		switch(keyin)
		{
		case '1':
			print_LAN();
			break;
		case '0':
			switch(conf_conn_LAN())
			{
			case SUCC:
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
		case '9':
			conf_prn();
			break;			
		case ESC:
			SetWifiStopflag(tmpflag);
			return;
			break;
		default:
			continue;
			break;
		}
	}	
	SetWifiStopflag(tmpflag);
	return;
}

