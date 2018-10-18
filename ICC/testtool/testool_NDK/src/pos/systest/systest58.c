/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	蓝牙/取电池电量
* file name			: systest58.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: 蓝牙/取电池电量测试
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
#define	TESTITEM	"蓝牙/取电池电量"
#define MAXWAITTIME		10
#define	DEFAULT_CNT_STR	"10"
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)
//#define MAX_SIZE BUFSIZE_BT//1024

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
					 	jiangym			20140228	created
*****************************************************************/	
static void bt_getpowervol(void)
{
	int ret = 0, bak = 0,cnt = 0, succ = 0, recvLen = 0, j = 0, cmdlen =0;
	uint unLen = 0;
	char sendBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0}, recvBuf[BUFSIZE_BT-MIN_BTCMD_LEN] = {0};//原始数据
	char cmdBuf[BUFSIZE_BT] = {0};// cmdrecvBuf[BUFSIZE_BT] = {0}, ;//加入报文头后的命令
	char  CntStr[8] = DEFAULT_CNT_STR;
	unsigned int unVol = 300;

	if(g_SequencePressFlag) 
	{
		bak = cnt = GetCycleValue();  //连续压力测试，需要手工输入循环次数的都直接设置默认值
	}
	else
	{
		cls_printf("默认交叉测试次数:\n");
		if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			bak = cnt = DEFAULT_CNT_VLE;
		else
			bak = cnt = atoi(CntStr);
	}
	cls_show_msg1(g_keeptime,"手持设备请确保只通过电池供电,不连接外接电源,没有连接串口线USB线等,任意键继续...");
	if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
	{
		cls_show_msg1(g_keeptime, "line %d:%s失败(ret=%d,vol=%d)", __LINE__, TESTITEM, ret, unVol);
		return;
	}
	for(j=0; j<sizeof(sendBuf); j++)
		sendBuf[j]=rand()%256;
	NDK_PortClose(PORT_NUM_BT);
	while(cnt)
	{
		if(ESC==cls_show_msg1(2, "%s中...\n还剩%d次(已成功%d次)(电池电量:%d),按ESC退出", TESTITEM, cnt, succ, unVol&0x7fffffff))//	value & 0x7fffffff
			break;	
		cnt--;
		//打开蓝牙串口
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL)) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次打开蓝牙串口失败", __LINE__, bak-cnt);
			NDK_PortClose(PORT_NUM_BT);
			continue;
		}
		if(g_btdiscoverflag)
		{
			if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:蓝牙开启广播失败(%d)", __LINE__, ret);
				continue;
			}
		}
		//取电池电量
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(ret=%d,vol=%d)", __LINE__, bak-cnt, TESTITEM, ret, unVol);
			NDK_PortClose(PORT_NUM_BT);
			continue;
		}
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME, FUNCNAME, g_keeptime, "line %d:第%d次配对连接失败(%d)", __LINE__, bak-cnt, ret);
			NDK_BTDisconnect();
			NDK_PortClose(PORT_NUM_BT);
			continue;
		}
#if BTMFI_ENABLE
		cls_show_msg("手机端允许后进行初始化连接pos后任意键继续");
#endif
		//清缓冲区
		if((ret = NDK_PortClrBuf(PORT_NUM_BT)) != NDK_OK)
		{				
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次清缓冲区失败(%d)", __LINE__, bak-cnt, ret);
			goto DISCONNECT;
		}
		//取电池电量
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(ret=%d,vol=%d)", __LINE__, bak-cnt, TESTITEM, ret, unVol);
			goto DISCONNECT;
		}
		
		//cls_printf("发送数据包中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x05, sendBuf, sizeof(sendBuf), cmdBuf);
		cmdlen = sizeof(sendBuf)+MIN_BTCMD_LEN;
		if((ret=NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, bak-cnt, ret);
			goto DISCONNECT;
		}
		//取电池电量
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(ret=%d,vol=%d)", __LINE__, bak-cnt, TESTITEM, ret, unVol);
			goto DISCONNECT;
		}
		//cls_printf("接收数据包中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		if(((ret=NDK_PortRead(PORT_NUM_BT,cmdlen, cmdBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)||(recvLen != cmdlen))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, bak-cnt, ret, recvLen, cmdlen);
			bt_comm_once(12,0);
			goto DISCONNECT;
		}
		memset(recvBuf, 0, sizeof(recvBuf));
		bt_cmd_uppacket(cmdBuf, sizeof(sendBuf), recvBuf);
		if(memcmp(sendBuf, recvBuf, sizeof(sendBuf)))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, bak-cnt);
			goto DISCONNECT;
		}
		//取电池电量
		if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s失败(ret=%d,vol=%d)", __LINE__, bak-cnt, TESTITEM, ret, unVol);
			goto DISCONNECT;
		}
		//发送响应确认报文
		cls_printf("发送响应数据报文中...");
		memset(cmdBuf, 0, sizeof(cmdBuf));
		bt_cmd_packet(0x06, NULL, 0, cmdBuf);
		if((ret=NDK_PortWrite(PORT_NUM_BT, MIN_BTCMD_LEN, cmdBuf))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, bak-cnt, ret);
			goto DISCONNECT;
		}
		succ++;
		
DISCONNECT:
		//断开蓝牙连接(低端需要此函数来断开连接,中端调用不影响)
		cls_show_msg1(1, "POS端挂断蓝牙连接中...");
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次断开蓝牙连接失败", __LINE__, bak-cnt);
			NDK_PortClose(PORT_NUM_BT);
			continue;
		}
		//关闭蓝牙串口
		if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭蓝牙串口失败", __LINE__, bak-cnt);
			continue;
		}
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"%s完成,已执行次数为%d,成功%d次", TESTITEM, bak-cnt, succ);
	return;
}

void systest58(void)
{
	/*private & local definition*/
	int ret = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_BT()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,BT配置失败,请检查配置文件",TESTITEM);
			return;
		}
		g_CycleTime = 10;
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);	
		//执行交叉测试
		bt_getpowervol();
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		ret = cls_show_msg("1.交叉压力\n"
							"0.蓝牙配置");
		
		switch(ret)
		{
		case '1':
			bt_getpowervol();
			break;
		case '0':
			conf_conn_BT();
#if BTMFI_ENABLE
			NDK_BTSetBleStatus(0);//在MFI协议下，关闭BLE方式
#endif
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


