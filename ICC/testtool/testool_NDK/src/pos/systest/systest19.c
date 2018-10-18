/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest19.c
* Author 			: chenjs
* version			: 
* DATE			: 20130227
* directory 	: 
* description	:打印/MDM交叉测试
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
#define	TESTITEM		"打印/MDM交叉"
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
*			  		chenjs		   20130227	 		created
*****************************************************************/
static void print_modem(void)
{
	/*private & local definition*/
	int icount = 0, isucc = 0, ret = -1;
	int send_len = 0, rec_len = 0;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	
   	/*process body*/		
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	mdm_clrportbuf_all();

	//if(NDK_PrnInit(g_PrnSwitch)!=NDK_OK)//if (FAIL==clrprintbuf())
	if(lib_initprn(g_PrnSwitch)!=NDK_OK)//初始化使配置有效modify 20131122 jiangym
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印机复位失败", __FILE__, __LINE__);
		return;
	}
	if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//避免其它用例的干扰
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:打印机状态异常!", __LINE__);
		return;                         
	}
	
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", icount+1, TESTITEM, isucc)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		icount++;
		
		//初始化MODEM
		cls_printf("初始化MODEM中(第%d次)...", icount);
		if((ret=mdm_init())!=SUCC)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:初始化MODEM(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME , g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;                         
		}
		
		//拨号
		cls_printf("MODEM拨号中(第%d次)...", icount);
		if(mdm_dial(MDMDialStr)!=SUCC)
		{
			mdm_hangup();
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:MODEM拨号(第%d次)失败", __LINE__, icount);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			mdm_hangup();
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;                         
		}
		
		//数据通讯
		cls_printf("MODEM数据通讯中(第%d次)...", icount);
		//发送数据
		if((send_len = mdm_send(sendpacket.header, sendpacket.len))!=sendpacket.len)
		{
			mdm_hangup();
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:发送数据失败(实际%d, 预期%d)", __LINE__, send_len, sendpacket.len);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			mdm_hangup();
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;                         
		}
		
		//接收数据
		if((rec_len = mdm_rev(rbuf, sendpacket.len, 20))!=sendpacket.len)
		{
			mdm_hangup();
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:接收数据失败(实际%d, 预期%d)", __LINE__, rec_len, sendpacket.len);
			continue;
		}
		//比较数据
		if(MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			mdm_hangup();
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:数据校验失败", __LINE__);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			mdm_hangup();
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;                         
		}
		
		//挂断
		cls_printf("MODEM挂断中(第%d次)...", icount);
		if(mdm_hangup()!=SUCC)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:MODEM挂断(第%d次)失败", __LINE__, icount);
			continue;
		}
		print_bill();
		if(GetPrinterStatus(MAXWAITTIME)!=SUCC)//状态OK后,终止计时
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME ,g_keeptime, "line %d:打印机状态异常!", __LINE__);
			continue;                         
		}		
		
		isucc++;
	}
	
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试完成 循环了%d次 成功了%d次", TESTITEM, icount, isucc);
	return;
}

void systest19(void)
{
	/*private & local definition*/
	int keyin = 0;	
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		//MDM配置
		if(conf_conn_MDM()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,MDM配置失败,请检查配置文件",TESTITEM);
			return;
		}
		//打印配置
		conf_prn();

		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉测试
		print_modem();
			
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		keyin = cls_show_msg("1.交叉测试\n"
							"0.MDM配置\n"
							"9.打印配置");
		switch(keyin)
		{
		case '1':
			print_modem();
			break;
		case '0':			
			switch(conf_conn_MDM())
			{
			case SUCC:
				cls_show_msg1(2, "配置modem成功!");
				break;
			case FAIL:
				cls_show_msg("line %d:配置modem失败!!!\n任意键退出配置...", __LINE__);	
				break;
			case NDK_ERR_QUIT://用户放弃
			default:
				break;
			}
			break;
		case '9':
			conf_prn();
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

