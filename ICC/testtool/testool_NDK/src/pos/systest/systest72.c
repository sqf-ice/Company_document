/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest72.c
* Author 			: huangjianb
* version			: 
* DATE			: 20141009
* directory 	: 
* description	: NFC/MDM交叉测试
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
#define	TESTITEM		"NFC/MDM交叉"

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
*			  		  huangjb		   20141009	 	created
*****************************************************************/
static void nfc_modem(void)
{
	/*private & local definition*/
	int icount = 0, isucc = 0, ret = -1 ,pnRecvlen = 0;
	int send_len = 0, rec_len = 0 ,nSeekCnt = 3;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	uchar psRecebuf[64] = {0};
	
  /*process body*/		
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	mdm_clrportbuf_all();

	cls_show_msg("请打开手机NFC测试工具并贴近pos...,任意键继续");
	
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", icount+1, TESTITEM, isucc)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		icount++;

		mdm_hangup();//初始化前的保护
		//初始化MODEM
		cls_printf("初始化MODEM中(第%d次)...", icount);
		if((ret=mdm_init())!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:初始化MODEM(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}
			
		//拨号
		cls_printf("MODEM拨号中(第%d次)...", icount);
		if(mdm_dial(MDMDialStr)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:MODEM拨号(第%d次)失败", __LINE__, icount);
			continue;
		}
		//两次NFC调用之间需要有个2S的休眠时间，避免手机端来不及相应
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}

		//数据通讯
		cls_printf("MODEM数据通讯中(第%d次)...", icount);
		//发送数据
		if((send_len=mdm_send(sendpacket.header, sendpacket.len))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:发送数据失败(实际%d, 预期%d)", __LINE__, send_len, sendpacket.len);
			continue;
		}
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}

		//接收数据
		if((rec_len=mdm_rev(rbuf, sendpacket.len, 20))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:接收数据失败(实际%d, 预期%d)", __LINE__, rec_len, sendpacket.len);
			continue;
		}
		//比较数据
		if(MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:数据校验失败", __LINE__);
			continue;
		}
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}

		//挂断
		cls_printf("MODEM挂断中(第%d次)...", icount);
		if(mdm_hangup()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:MODEM挂断(第%d次)失败", __LINE__, icount);
			continue;
		}
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}

		isucc++;
	}
	
	cls_show_msg("循环了%d次\n成功了%d次\n", icount, isucc);
	return;
}

void systest72(void)
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

	while (1)
	{	
		keyin = cls_show_msg("1.交叉测试\n"
							"0.MDM配置");
		switch(keyin)
		{
		case '1':
			nfc_modem();
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

