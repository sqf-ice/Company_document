/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest75.c
* Author 			:huangjianb
* version			: 
* DATE				:20141009
* directory 			: 
* description			: NFC/BT交叉测试
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
#define	TESTITEM		"NFC/BT交叉"

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
*			  		huangjianb		20141009	created
*****************************************************************/
static void nfc_BT(void)
{
	/*private & local definition*/
	int i = 0, succ_count = 0, ret = -1 ,nSeekCnt = 3 ,pnRecvlen = 0;
	int  rlen = 0, timeout = SO_TIMEO;
	char buf[BUFSIZE_BT] = {0}, rbuf[BUFSIZE_BT] = {0};
	Packet sendpacket;
	uchar psRecebuf[64] = {0};

	/*process body*/
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);	
	while (1)
	{
		//保护动作
		NDK_PortClrBuf(PORT_NUM_BT);
		NDK_PortClose(PORT_NUM_BT);
		
		if(cls_show_msg1(2, "正在进行第%d次%s测试(已成功%d次),按ESC退出", ++i, TESTITEM, succ_count)==ESC)
			break;
		setlinktype(BT);//构造 BT数据包
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;

		//打开蓝牙串口
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次打开蓝牙串口失败", __LINE__, i);
			continue;
		}
		if(g_pair_mode == 2 || g_pair_mode == 3)// SSP PIN和PassKey两种配对模式 需要调用配对状态查询和配对确认函数进行配对操作
		{	
			cls_show_msg("把之前已配对的蓝牙取消配对后任意键继续");
			cls_printf("手机使用ISSC与POS配对连接");
			if((ret=lib_bt_pairing(1))!=NDK_OK)
			{
				cls_show_msg("line %d:配对失败,请重新配对(%d)", __LINE__,ret);
				NDK_BTDisconnect();
				continue;
			}
		}
		else //Just Work 和 PIN Code两种配对模式可以直接操作配对
			cls_show_msg("苹果手机使用BleTest或安卓手机使用ISSC,与POS连接后(BleTest需要点开始回传),任意键继续");
		//cls_show_msg("使用手机蓝牙助手ISSC与POS连接后,任意键继续");
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, i, ret);
			continue;
		}

		//蓝牙清缓冲区
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
		{				
			cls_show_msg1(g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, i, ret);
			continue;
		}	
		//两次NFC调用之间需要有个2S的休眠时间，避免手机端来不及相应
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, i, ret);
			continue;
		}
		
		//蓝牙发送数据
		if((ret=NDK_PortWrite(PORT_NUM_BT, sendpacket.len, sendpacket.header))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次蓝牙发送失败(%d)", __LINE__, i, ret);
			continue;
		}
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, i, ret);
			continue;
		}

		//蓝牙接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if((ret=NDK_PortRead(PORT_NUM_BT,sendpacket.len, rbuf, timeout*1000, &rlen)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次蓝牙接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, rlen, sendpacket.len);
			continue;
		}
		//比较收发
		if(rlen != sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次蓝牙数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, rlen, sendpacket.len);
			continue;	
		}
		if(MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次蓝牙数据校验失败", __LINE__, i);
			continue;	
		}
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, i, ret);
			continue;
		}

		//关闭蓝牙串口
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次关闭蓝牙串口失败", __LINE__, i);
			continue;
		}
		sleep(2);
		if((ret=Nfc_Activate(psRecebuf,&pnRecvlen,nSeekCnt)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:NFC测试(第%d次)失败(%d)", __LINE__, i, ret);
			continue;
		}

		succ_count++;
	}
			
	cls_show_msg("交叉次数:%d\n成功次数:%d", i-1, succ_count);
	return;
}

void systest75(void)
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
							 "0.BT配置");
		switch(keyin)
		{
		case '1':
			nfc_BT();
			break;
		case '0':
			conf_conn_BT();
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

