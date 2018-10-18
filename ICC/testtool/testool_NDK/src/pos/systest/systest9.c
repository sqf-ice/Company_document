/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest9.c
* Author 			: chensl
* version			: 
* DATE			: 20130131
* directory 	: 
* description	: 串口综合测试
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
#define		BPS_NUM		10
#define	MAXWAITTIME 	150	//30
#define 	PORT		PORT_NUM_COM1	//用于N900AZ蓝牙底座测试用的串口
#define	TESTITEM		"串口性能,压力"
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
//串口初始化
static int aux_init(void)
{
	/*private & local definition*/
	int port = -1, ret = -1;

	/*process body*/
	port = select_serial();
	if(conf_link(SERIAL)!=SUCC)
		return FAIL;
	if((ret = NDK_PortOpen(port, g_bps)) != NDK_OK)
	{
		cls_show_msg("line %d:初始化AUX(%d)失败\n错误码:%x\nBPS:%s", __LINE__, port, ret, g_bps);
		return FAIL;
	}	
	serial_clrportbuf_all(port);
	cls_show_msg1(2, "初始化AUX(%d)成功\n""%s 8-N-1\n", port, g_bps);
	return port;
}

//串口波特率循环初始化
static void aux_circleinit(void)
{
	/*private & local definition*/
	int i = 0, ret = -1, port = 0;
	char *bps[BPS_NUM] = {"300", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400"};

	/*process body*/
	port = select_serial();
	while (1)
	{
		if(ESC==cls_show_msg1(1, "波特率轮回中(%s)...ESC中断测试", bps[i]))
			return;
		if((ret=NDK_PortOpen(port, bps[i])) != NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:初始化串口(%d, %s)失败(ret=%d)", __LINE__, port, bps[i], ret);
			return;
		}
		i++;
		if(i == BPS_NUM)
			i = 0;
	}
	
	return;
}

//POS<---PC
static void aux_pctopos(void)
{
	/*private & local definition*/
	int ret = -1, port = -1, datalen = 0;
	long int dataSum = 0;
	char recvBuf[BUFSIZE_SERIAL] = {0}; 
	uchar chksum = 0x00;

	/*process body*/
	if ((port=aux_init()) == FAIL)
		return;
	do
	{
		if(ESC==cls_show_msg1(1, "POS<---PC\n已收到数据长度:%d,校验码:%2.2x,ESC可中断接收...", dataSum, chksum))
			break;
		if((ret=NDK_PortRead(port, sizeof(recvBuf), recvBuf, 5*1000, &datalen)) == NDK_OK)
		{
			dataSum += datalen;
			chksum^=MakeLRC((uchar *)recvBuf, datalen);
		}
	}while(datalen >= 0);
	
	cls_show_msg_record(FILENAME,FUNCNAME,"POS<---PC\n共收到数据长度:%d,校验码:%2.2x", dataSum, chksum);
	return;
}

//PC<->POS
static void aux_comm_pc(void)
{
	/*private & local definition*/
	int ret = -1, port = -1, i = 0, recvLen = 0, succconut = 0;
	char sendBuf[BUFSIZE_SERIAL] = {0}, recvBuf[BUFSIZE_SERIAL] = {0};
	Packet sendpacket;

	/*process body*/	
	if ((port=aux_init()) == FAIL)
		return;
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);
	while(1)
	{
		cls_printf("开始第%d次PC<->POS通讯(已成功%d次)", i+1, succconut);

		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		if((ret = NDK_PortClrBuf(port)) != NDK_OK)	/*清接收缓冲*/
		{			
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:测试失败(%d)", __LINE__, ret)==ESC)
				break;
			else
				continue;
		}		
		NDK_ScrDispString(0,48,"POS--->PC",0);
		NDK_ScrRefresh();
		if((ret=NDK_PortWrite(port, sendpacket.len, sendpacket.header))!=NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}
		NDK_ScrDispString(0,48,"POS<---PC",0);
		NDK_ScrRefresh();
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(port, sendpacket.len, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错%d (实际%d, 预期%d)", __LINE__, i, ret, recvLen, sendpacket.len)==ESC)
				break;
			else
				continue;
		}
		NDK_ScrDispString(0,32, "数据校验中...",0);
		NDK_ScrRefresh();
		//比较
		if(recvLen != sendpacket.len)
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i,recvLen,sendpacket.len)==ESC)
				break;
			else
				continue;
		}
		if(MemCmp(sendBuf, recvBuf, sendpacket.len))
		{
			if(cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, i)==ESC)
				break;
			else
				continue;
		}
		succconut++;//通讯成功次数增加	
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯压力测试完成,执行次数为%d,成功%d次", i, succconut);
	return;
}


//自收自发
static void aux_comm_self(void)
{
	/*private & local definition*/
	int ret = -1, port = -1, i = 0, recvLen = 0, succconut = 0;
	char sendBuf[BUFSIZE_SERIAL] = {0}, recvBuf[BUFSIZE_SERIAL] = {0};
    Packet sendpacket;

	/*process body*/
	if ((port=aux_init()) == FAIL)
		return;
	cls_show_msg("短接所选串口的23脚,任意键继续...");
	init_snd_packet(&sendpacket, sendBuf);
	set_snd_packet(&sendpacket);	
	while(1)
	{
		if(cls_show_msg1(1, "正在进行AUX(%d)第%d次通讯,按ESC退出", port, i+1)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		if((ret = NDK_PortClrBuf(port)) != NDK_OK)	/*清接收缓冲*/
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:测试失败(%d)", __LINE__, ret);
			continue;
		}
		
		//发
		if((ret=NDK_PortWrite(port, sendpacket.len, sendpacket.header))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			continue;
		}
		
		//收
		memset(recvBuf, 0, sizeof(recvBuf));
		if((ret=NDK_PortRead(port, sendpacket.len, recvBuf, MAXWAITTIME*1000, &recvLen)) != NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次接收错%d (实际%d, 预期%d)", __LINE__, i, ret, recvLen, sendpacket.len);
			continue;
		}

		//比较
		if(recvLen != sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i,recvLen,sendpacket.len);
			continue;
		}
		if(MemCmp(sendBuf, recvBuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次数据校验失败", __LINE__, i);
			continue;
		}
		succconut++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"串口自检完成(共进行%d次),成功了%d次", i, succconut);
	return;
}

int sendrecv_cmd(char *sendbuf, int sendlen, char *recbuf, uint unlen, int *reclen, int timeout)
{
	int ret=0;
	NDK_PortClrBuf(PORT);
	if((ret=NDK_PortWrite(PORT, sendlen, sendbuf)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:发送失败(%d)", __LINE__, ret);
		return ret;
	}
	if((ret = NDK_PortRead(PORT, unlen, recbuf, timeout*1000, reclen)) != NDK_OK)	
	{
		cls_show_msg1(g_keeptime, "line %d:接收失败(%d)", __LINE__, ret);
		return ret;
	}
	
	return NDK_OK;
}

void n900_btdesk(void)
{
	/*private & local definition*/
	int ret=0, len=0, len1=0, len2=0, i = 0, btflag = 0;
	char recbuf[128]={0}, old_btname[128]={0}, old_btaddr[128]={0};
	char sendbuf[256]={0};
	char modify_btname[]="BLUE NAME:TestName\r\n";
	char modify_btaddr[]="BLUE ADDR:010203040506\r\n";
	char cmd[][128]={  "AT#BV\r\n", 		   		/*0 获取软件版本信息*/
				         "AT#BM\r\n", 	   			/*1	获取蓝牙名称*/
				         "AT#BMTestName\r\n",		/*2 修改蓝牙名称为TestName*/
				         "AT#BZ\r\n",				/*3 获取蓝牙地址*/
				         "AT#BZ010203040506\r\n",	/*4 修改蓝牙地址为010203040506*/
				         "AT#BX\r\n",				/*5 退出工厂模式*/
					 "AT#BR1\r\n",				/*6 修改蓝牙波特率为9600*/
					 "AT#BR5\r\n",				/*7 修改蓝牙波特率为默认值115200*/
					 "AT#BR4\r\n",			/*8 修改蓝牙波特率为57600*/
					"AT#WZ\r\n",				/*9 获取WiFi MAC*/		
					"AT#WM\r\n",				/*10 查询WiFi AP名称*/		
					"AT#WMAP@#BTtest\r\n",	/*11 修改WiFi AP名称为AP@#BTtest*/
					"AT#WI\r\n", 				/*12查询WiFi AP IP地址*/
					"AT#WI192.168.1.11\r\n",     /*13修改WiFi AP IP地址192.168.1.11*/
					"AT#WS\r\n",				/*14查询WIFI AP 子网掩码*/
					"AT#WS255.255.255.0\r\n",	/*15修改WIFI AP 子网掩码255.255.255.0*/
					"AT#WK\r\n",				/*16查询WIFI AP 密码*/
					"AT#WK87654321\r\n",		/*17修改WIFI AP 密码为87654321*/
					//"AT#WP2",				/*修改WIFI AP加密方式为wpa2,由上层测*/
					"AT#SN\r\n",				/*18查询SN码*/
					"AT#SN0123456789012\r\n",	/*19修改SN码0123456789012*/
					"AT#PN\r\n",				/*20查询PN码*/
					"AT#PN9876543210123\r\n",	/*21修改PN码9876543210123*/
					"AT#BR7\r\n",			 	 /*22 修改蓝牙波特率为380400*/
					"AT#JH\r\n",				/*23查询机器号码*/
					"AT#JH201707050102\r\n",	/*24修改机器号码*/
					"AT#FWthis is U disk test ,write@#><,over\r\n", /*25写U盘测试文件*/
					"AT#FR\r\n",				/*26读U盘测试文件*/
					"AT#FS\r\n",				/*27上传U盘测试文件*/
					"AT#LOG\r\n",				/*28读取测试记录文件*/
					"AT#LOG0123456789\r\n",	/*29写测试记录文件*/
					"AT#RESTART\r\n",			/*30生效配置参数*/
					"AT#EZ\r\n" ,                /*31查询以太网mac地址*/
					"AT#EZ123456789012\r\n",    /*32修改以太网mac12345678901234567*/
					 };
	char wifi_mac[32] = {0},old_wifiname[256] = {0},old_wifi_ipaddr[32] = {0},old_wifimask[32] = {0},old_wifipwd[256] = {0}, old_sn[128]  = {0},old_pn[128]={0}, old_jh[32] = {0}, old_ethaddr[32] = {0};
	char modify_wifiname[] = "WIFI AP NAME:AP@#BTtest\r\n", modify_wifi_ipaddr[] = "WIFI AP IP:192.168.1.11\r\n",modify_wifimask[] = "WIFI AP MASK:255.255.255.0\r\n";
	char modify_wifipwd[] = "WIFI AP PASSWORD:87654321\r\n",modify_sn[] = "SN:0123456789012\r\n",modify_pn[] = "PN:9876543210123\r\n",modify_jh[] = "JH:201707050102\r\n", modify_ethaddr[] = "ETH MAC:123456789012\r\n";
	char udisk_str[] = "file content:\r\nthis is U disk test ,write@#><,over\r\n", udisk_wr[] ="write testfile OK\r\n", old_udisk[256] = {0}, old_record[256] = {0}, new_record[256] = {0} ;
	int reclen = 0;
	char *err_cmd[] = {
					"AT#BMTestName01234567890123456789123\r\n",    //0name超过30 command ERROR!!!\r\n
					"AT#BX123\r\n", 				//1退出工厂模式指令长度大于7command ERROR!!!\r\n
					"AT#BR0\r\n",					//2波特率设置不在其范围内 error baud rate!\r\n
					"AT#BR8\r\n",					
					"AT#BZ00130485\r\n",  			//蓝牙地址设置错误BLUE ADDR ERROR!!!\r\n					
					"AT#WZ192.192.192.1922\r\n",     //5 wifi ap ip地址错误  command ERROR!!!\r\n
					"AT#WS255.255.255.0234r\n",      //6 wifi ap 子网掩码设置错误 command ERROR!!!\r\n
					"AT#WK1234567\r\n",			//7 wifi ap 密码长度小于7  大于256的不测command ERROR!!!\r\n
					"AT#WP733\r\n",				//8 wifi ap 加密模式长度大于1  command ERROR!!!\r\n
					"AT#SN0123456789012345678901234567891201234567890123456789012345678912012345678901234567890123456789120123456789012345678901234567891234\r\n",//9sn长度大于128
					"AT#PN0123456789012345678901234567891201234567890123456789012345678912012345678901234567890123456789120123456789012345678901234567891234\r\n",//10pn长度大于128
					"AT#JH0123456789012\r\n",		//11 jh长度大于12字节,
					"AT#WP7\r\n",				//12wifi ap 加密模式错误 error wifi secmode!\r\n
					"AT#BP8\r\n",					//13修改蓝牙模式错误 error blue pair mode!\r\n
					"AT#EZ012345\r\n",              //14修改以太网mac地址错误 ETH ADDR ERROR!!!\r\n
					};
	char err_cmd_return[] = "command ERROR!!!\r\n", err_baud_rate[] = "error baud rate!\r\n", err_bt_addr[] = "BLUE ADDR ERROR!!!\r\n", err_eth_addr[] = "ETH MAC ERROR!!!\r\n";
	char err_secmode[] = "error wifi secmode!\r\n", err_bt_pairmode[] = "error blue pair mode!\r\n";
#define 	MAXTIME		5//10 修改成5s超时，加快速度

	//测试前置
	if(cls_show_msg("注意:本测试需关闭POS控制台,已关闭按任意键继续测试,未关闭按取消键退出")==ESC)
		return;
	NDK_PortClose(PORT);

	if(cls_show_msg("是否进行N910蓝牙底座测试,是[ENTER],否[其他]")==ENTER)
		btflag = 1;
	else
		btflag = 0;
	//case11:进入工厂模式
        if(btflag)
		cls_show_msg("请将POS串口1与蓝牙底座串口通过串口线相连后,长按蓝牙底座上的复位键2s后进入工厂模式");
	else
		cls_show_msg("请将POS串口1与蓝牙底座串口通过串口线相连后,上电蓝牙底座进入工厂模式后(使用工厂模式线上电),按任意键继续");
	if((ret=NDK_PortOpen(PORT, "115200,8,N,1"))!=NDK_OK)//默认情况
	{
		cls_show_msg1(g_keeptime, "line %d:打开串口失败(%d)", __LINE__, ret);
		return;
	}
	//新增发送一条指令(取版本),保证已进入工厂模式(如果存在sn,会把第一条指令当做进入工厂模式的指令)
	cls_printf("进入工厂模式中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[0]);
	memset(recbuf, 0, sizeof(recbuf));	
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, sizeof(recbuf), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}

	//case10:退出工厂模式后,发送指令应该会失败
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[5]);
	memset(recbuf, 0, sizeof(recbuf));
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("exit Production Mode!"), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}
	//发送获取软件版本信息指令应该会失败
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[0]);
	memset(recbuf, 0, sizeof(recbuf));
	NDK_PortClrBuf(PORT);
	if((ret=NDK_PortWrite(PORT, strlen(sendbuf), sendbuf)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:发送失败(%d,sendbuf=%s)", __LINE__, ret, sendbuf);
		return;
	}
	if((ret = NDK_PortRead(PORT, sizeof(recbuf), recbuf, MAXTIME*1000, &len)) == NDK_OK)	
	{
		cls_show_msg1(g_keeptime, "line %d:接收失败(%d,recbuf=%s)", __LINE__, ret, recbuf);
		return;
	}
	cls_show_msg("成功退出工厂模式,请重新上电蓝牙底座进入工厂模式后,按任意键继续");

	//新增发送一条指令(取版本),保证已进入工厂模式(如果存在sn,会把第一条指令当做进入工厂模式的指令)
	cls_printf("进入工厂模式中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[0]);
	memset(recbuf, 0, sizeof(recbuf));	
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, sizeof(recbuf), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}
	//case0:增加参数异常测试
	cls_show_msg1(3,"参数异常测试中...");
	for(i=0;i<2;i++)
	{
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, err_cmd[i]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(err_cmd_return), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(i=%d,ret=%d,sendbuf=%s,recbuf=%s)", __LINE__, i, ret, sendbuf, recbuf);
			return;
		}
		if( len!=strlen(err_cmd_return) ||  strcmp(recbuf, err_cmd_return) )
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(i=%d,len=%d,recbuf:%s)", __LINE__, i, len, recbuf);
			goto ERR;
		}
	}
	for(i=2;i<4;i++)
	{
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, err_cmd[i]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(err_baud_rate), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(i=%d,ret=%d,sendbuf=%s,recbuf=%s)", __LINE__, i, ret, sendbuf, recbuf);
			return;
		}
		if( len!=strlen(err_baud_rate) ||  strcmp(recbuf, err_baud_rate) )
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(i=%d,len=%d,recbuf:%s)", __LINE__, i, len, recbuf);
			goto ERR;
		}
	}
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, err_cmd[4]);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(err_bt_addr), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}
	if(btflag) //910蓝牙底座返回BLUE ADDR ERROR!!!
	{
		if( len!=strlen(err_bt_addr) ||  strcmp(recbuf, err_bt_addr) )
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}
	}
	else  //900蓝牙底座返回command ERROR!!!
	{
		if( len!=strlen(err_cmd_return) ||  strcmp(recbuf, err_cmd_return) )
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(i=%d,len=%d,recbuf:%s)", __LINE__, i, len, recbuf);
			goto ERR;
		}
	}
	if(btflag)
	{
		for(i=5;i<12;i++)
		{
			memset(sendbuf, 0, sizeof(sendbuf));
			strcpy(sendbuf, err_cmd[i]);
			memset(recbuf, 0, sizeof(recbuf));
			len=0;
			if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(err_cmd_return), &len, MAXTIME))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(i=%d,ret=%d,sendbuf=%s,recbuf=%s)", __LINE__, i, ret, sendbuf, recbuf);
				return;
			}
			if( len!=strlen(err_cmd_return) ||  strcmp(recbuf, err_cmd_return) )
			{
				cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(i=%d,len=%d,recbuf:%s)", __LINE__, i, len, recbuf);
				goto ERR;
			}
		}
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, err_cmd[12]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(err_secmode), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			return;
		}
		if( len!=strlen(err_secmode) ||  strcmp(recbuf, err_secmode) )
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, err_cmd[13]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(err_bt_pairmode), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			return;
		}
		if( len!=strlen(err_bt_pairmode) ||  strcmp(recbuf, err_bt_pairmode) )
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}
		//新增mac设置异常测试 sull 20180207
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, err_cmd[14]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret = sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(err_eth_addr), &len, MAXTIME)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			return;
		}
		if(len!=strlen(err_eth_addr) || strcmp(recbuf, err_eth_addr))
		{
			cls_show_msg1(g_keeptime, "line %d:异常参数测试失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}
	}

	//case1:获取软件版本信息
	cls_show_msg1(3,"获取原来蓝牙底座软件版本中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[0]);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, sizeof(recbuf), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:获取软件版本信息失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}
	if(cls_show_msg("取到的蓝牙版本为%s,请与实际对照是否正确,是[ENTER],否则[其他]", recbuf) != ENTER)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,recbuf=%s)", __LINE__, sendbuf, recbuf);
		return;
	}

	//case2:修改成一个最低波特率9600
	cls_show_msg1(3,"修改低BPS中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[6]);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:修改低BPS失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}
	if(strcmp(recbuf, "OK\r\n")) 
	{
		cls_show_msg1(g_keeptime, "line %d:修改低BPS失败(recbuf:%s)", __LINE__, recbuf);
		goto ERR;
	}
	if((ret=NDK_PortClose(PORT))!=NDK_OK) 
	{
		cls_show_msg1(g_keeptime, "line %d:关闭串口失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if((ret=NDK_PortOpen(PORT, "9600,8,N,1"))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:打开串口失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
	//case3:获取原来蓝牙名称
	cls_show_msg1(3,"获取原来蓝牙名称中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[1]);
	memset(old_btname, 0, sizeof(old_btname));
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_btname, sizeof(old_btname), &len1, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:获取原蓝牙名称失败(%d,sendbuf=%s,old_btname=%s)", __LINE__, ret, sendbuf, old_btname);
		goto ERR;
	}
	if(cls_show_msg("获取到的蓝牙名称%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_btname) != ENTER)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_btname=%s)", __LINE__, sendbuf, old_btname);
		goto ERR;
	}
	
	//case4:修改成其它名字
	cls_show_msg1(3,"修改蓝牙名称中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[2]);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_btname), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:修改蓝牙名称失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		goto ERR;
	}
	if( len!=strlen(modify_btname) ||  strcmp(recbuf, modify_btname) )
	{
		cls_show_msg1(g_keeptime, "line %d:修改蓝牙名称失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
		goto ERR;
	}
		
	//case5:修改成原名字
	cls_show_msg1(3,"修改成原蓝牙名中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	sprintf(sendbuf,"AT#BM%s",old_btname+10);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, len1, &len, MAXTIME*3))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:修改原名称失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		goto ERR;
	}
	if(len!=len1 || strcmp(recbuf, old_btname) )
	{
		cls_show_msg1(g_keeptime, "line %d:修改原名称失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
		goto ERR;
	}

	//case6:修改成一个波特率57600
	cls_show_msg1(3,"修改BPS中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[8]);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:修改BPS失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		goto ERR;
	}
	if(strcmp(recbuf, "OK\r\n")) 
	{
		cls_show_msg1(g_keeptime, "line %d:修改BPS失败(recbuf=%s)", __LINE__, recbuf);
		goto ERR;
	}
	if((ret=NDK_PortClose(PORT))!=NDK_OK) 
	{
		cls_show_msg1(g_keeptime, "line %d:关闭串口失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if((ret=NDK_PortOpen(PORT, "57600,8,N,1"))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:打开串口失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
	//case7:获取原来BT地址
	cls_show_msg1(3,"获取原来蓝牙地址中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[3]);
	memset(old_btaddr, 0, sizeof(old_btaddr));
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_btaddr, sizeof(old_btaddr), &len2, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:获取原蓝牙地址失败(%d,sendbuf=%s,old_btaddr=%s)", __LINE__, ret, sendbuf, old_btaddr);
		goto ERR;
	}
	if(cls_show_msg("获取到的蓝牙地址为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_btaddr) != ENTER)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_btaddr=%s)", __LINE__, sendbuf, old_btaddr);
		goto ERR;
	}
	
	//case8:修改成其它地址
	cls_show_msg1(3,"修改蓝牙地址中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[4]);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_btaddr), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:修改蓝牙地址失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		goto ERR;
	}
	if( len!=strlen(modify_btaddr) || strcmp(recbuf, modify_btaddr) )
	{
		cls_show_msg1(g_keeptime, "line %d:修改蓝牙地址失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
		goto ERR;
	}

	//case9:修改成原地址
	cls_show_msg1(3,"修改成原蓝牙地址中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	sprintf(sendbuf,"AT#BZ%s",old_btaddr+10);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, len2, &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:修改成原蓝牙地址失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		goto ERR;
	}
	if(len!=len2 || strcmp(recbuf, old_btaddr) )
	{
		cls_show_msg1(g_keeptime, "line %d:修改成原蓝牙地址失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
		goto ERR;
	}
	if(btflag)
	{	
	//case10:获取原来WIFI MAC
		cls_show_msg1(3,"获取WIFI MAC中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[9]);
		memset(wifi_mac, 0, sizeof(wifi_mac));
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), wifi_mac, sizeof(wifi_mac), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取WIFI MAC失败(%d,sendbuf=%s,wifi_mac=%s)", __LINE__, ret, sendbuf, wifi_mac);
			goto ERR;
		}
		if(cls_show_msg("获取到的WIFI MAC为%s,请与实际对照是否正确,是[ENTER],否则[其他]", wifi_mac) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,wifi_mac=%s)", __LINE__, sendbuf, wifi_mac);
			goto ERR;
		}
		
		//case11:获取原来WIFI AP名称
		cls_show_msg1(3,"获取原WIFI AP名称中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[10]);
		memset(old_wifiname, 0, sizeof(old_wifiname));
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_wifiname, sizeof(old_wifiname), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取原WIFI AP名称失败(%d,sendbuf=%s,old_wifiname=%s)", __LINE__, ret, sendbuf, old_wifiname);
			goto ERR;
		}
		if(cls_show_msg("获取到的WIFI AP名称为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_wifiname) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_wifiname=%s)", __LINE__, sendbuf, old_wifiname);
			goto ERR;
		}

		//case12:修改成其它WIFI AP名称
		cls_show_msg1(3,"修改WIFI AP名称中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[11]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_wifiname), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP名称失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(modify_wifiname) || strcmp(recbuf, modify_wifiname) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP名称失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}

		//case13:修改成原WIFI AP名称
		cls_show_msg1(3,"修改成原WIFI AP名称中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"AT#WM%s",old_wifiname+13);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP名称失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=reclen || strcmp(recbuf, old_wifiname) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP名称失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
			goto ERR;
		}

		//case14:修改成默认波特率115200  
		cls_show_msg1(3,"修改BPS中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[7]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改BPS失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(strcmp(recbuf, "OK\r\n")) 
		{
			cls_show_msg1(g_keeptime, "line %d:修改BPS失败(recbuf=%s)", __LINE__, recbuf);
			goto ERR;
		}
		if((ret=NDK_PortClose(PORT))!=NDK_OK) 
		{
			cls_show_msg1(g_keeptime, "line %d:关闭串口失败(%d)", __LINE__, ret);
			goto ERR;
		}
		if((ret=NDK_PortOpen(PORT, "115200,8,N,1"))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:打开串口失败(%d)", __LINE__, ret);
			goto ERR;
		}
		
		//case15:获取原来WIFI AP IP 地址
		cls_show_msg1(3,"获取原WIFI AP IP 地址中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[12]);
		memset(old_wifi_ipaddr, 0, sizeof(old_wifi_ipaddr));
		reclen = 0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_wifi_ipaddr, sizeof(old_wifi_ipaddr), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取原WIFI AP IP 地址失败(%d,sendbuf=%s,old_wifi_ipaddr=%s)", __LINE__, ret, sendbuf, old_wifi_ipaddr);
			goto ERR;
		}
		if(cls_show_msg("获取到的WIFI AP IP 地址为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_wifi_ipaddr) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_wifi_ipaddr=%s)", __LINE__, sendbuf, old_wifi_ipaddr);
			goto ERR;
		}

		//case16:修改成其它WIFI AP IP 地址
		cls_show_msg1(3,"修改WIFI AP IP 地址中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[13]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_wifi_ipaddr), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP IP 地址失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(modify_wifi_ipaddr) || strcmp(recbuf, modify_wifi_ipaddr) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP IP 地址失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}

		//case17:修改成原WIFI AP IP 地址
		cls_show_msg1(3,"修改成原WIFI AP IP 地址中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"AT#WI%s",old_wifi_ipaddr+11);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP IP 地址失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=reclen || strcmp(recbuf, old_wifi_ipaddr) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP IP 地址失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
			goto ERR;
		}
		
		 //目前只支持到最高115200的波特率
		//case18:修改成最高波特率380400  
				
		//case19:获取原来WIFI AP 子网掩码
		cls_show_msg1(3,"获取原WIFI AP子网掩码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[14]);
		memset(old_wifimask, 0, sizeof(old_wifimask));
		reclen = 0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_wifimask, sizeof(old_wifimask), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取原WIFI AP子网掩码失败(%d,sendbuf=%s,old_wifimask=%s)", __LINE__, ret, sendbuf, old_wifimask);
			goto ERR;
		}
		if(cls_show_msg("获取到的WIFI AP子网掩码为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_wifimask) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_wifimask=%s)", __LINE__, sendbuf, old_wifimask);
			goto ERR;
		}

		//case20:修改成其它WIFI AP子网掩码
		cls_show_msg1(3,"修改WIFI AP子网掩码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[15]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_wifimask), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP子网掩码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(modify_wifimask) || strcmp(recbuf, modify_wifimask) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP子网掩码失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}

		//case21:修改成原WIFI AP子网掩码
		cls_show_msg1(3,"修改成原WIFI AP子网掩码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"AT#WS%s",old_wifimask+13);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP子网掩码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=reclen || strcmp(recbuf, old_wifimask) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP子网掩码失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
			goto ERR;
		}

		//case22:获取原来WIFI AP 密码
		cls_show_msg1(3,"获取原WIFI AP密码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[16]);
		memset(old_wifipwd, 0, sizeof(old_wifipwd));
		reclen = 0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_wifipwd, sizeof(old_wifipwd), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取原WIFI AP密码失败(%d,sendbuf=%s,old_wifipwd=%s)", __LINE__, ret, sendbuf, old_wifipwd);
			goto ERR;
		}
		if(cls_show_msg("获取到的WIFI AP密码为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_wifipwd) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_wifipwd=%s)", __LINE__, sendbuf, old_wifipwd);
			goto ERR;
		}

		//case23:修改成其它WIFI AP密码
		cls_show_msg1(3,"修改WIFI AP密码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[17]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_wifipwd), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP密码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(modify_wifipwd) || strcmp(recbuf, modify_wifipwd) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP密码失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}

		//case24:修改成原WIFI AP密码
		cls_show_msg1(3,"修改成原WIFI AP密码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"AT#WK%s",old_wifipwd+17);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP密码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=reclen || strcmp(recbuf, old_wifipwd) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP密码失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
			goto ERR;
		}
		
		//case25:修改成默认波特率115200  
		cls_show_msg1(3,"修改BPS中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[7]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改BPS失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(strcmp(recbuf, "OK\r\n")) 
		{
			cls_show_msg1(g_keeptime, "line %d:修改BPS失败(recbuf=%s)", __LINE__, recbuf);
			goto ERR;
		}
		if((ret=NDK_PortClose(PORT))!=NDK_OK) 
		{
			cls_show_msg1(g_keeptime, "line %d:关闭串口失败(%d)", __LINE__, ret);
			goto ERR;
		}
		if((ret=NDK_PortOpen(PORT, "115200,8,N,1"))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:打开串口失败(%d)", __LINE__, ret);
			goto ERR;
		}
		
		//case26:获取原来的SN码
		cls_show_msg1(3,"获取原SN码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[18]);
		memset(old_sn, 0, sizeof(old_sn));
		reclen = 0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_sn, sizeof(old_sn), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取原SN码失败(%d,sendbuf=%s,old_sn=%s)", __LINE__, ret, sendbuf, old_sn);
			goto ERR;
		}
		if(cls_show_msg("获取到的SN码为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_sn) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_sn=%s)", __LINE__, sendbuf, old_sn);
			goto ERR;
		}

		//case27:修改成其它SN码
		cls_show_msg1(3,"修改SN码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[19]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_sn), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改SN码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(modify_sn) || strcmp(recbuf, modify_sn) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改SN码失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}

		//case28:修改成原WIFI AP密码
		cls_show_msg1(3,"修改成原SN码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"AT#SN%s",old_sn+3);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原SN码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=reclen || strcmp(recbuf, old_sn) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原SN码失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
			goto ERR;
		}
		
		//case29:获取原来的PN码
		cls_show_msg1(3,"获取原PN码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[20]);
		memset(old_pn, 0, sizeof(old_pn));
		reclen = 0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_pn, sizeof(old_pn), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取原PN码失败(%d,sendbuf=%s,old_pn=%s)", __LINE__, ret, sendbuf, old_pn);
			goto ERR;
		}
		if(cls_show_msg("获取到的PN码为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_pn) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_pn=%s)", __LINE__, sendbuf, old_pn);
			goto ERR;
		}

		//case30:修改成其它PN码
		cls_show_msg1(3,"修改PN码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[21]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_pn), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改PN码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(modify_pn) || strcmp(recbuf, modify_pn) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改PN码失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}

		//case31:修改成原PN码
		cls_show_msg1(3,"修改成原PN码中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"AT#PN%s",old_pn+3);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原PN码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=reclen || strcmp(recbuf, old_pn) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原PN码失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
			goto ERR;
		}

		//case32:获取原来的机号
		cls_show_msg1(3,"获取原机号中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[23]);
		memset(old_jh, 0, sizeof(old_jh));
		reclen = 0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_jh, sizeof(old_jh), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取原机号失败(%d,sendbuf=%s,old_jh=%s)", __LINE__, ret, sendbuf, old_jh);
			goto ERR;
		}
		if(cls_show_msg("获取到的机号为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_jh) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_jh=%s)", __LINE__, sendbuf, old_jh);
			goto ERR;
		}
		
		//case33:修改机号	
		cls_show_msg1(3,"修改机号中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[24]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_jh), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改机号失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(modify_jh) || strcmp(recbuf, modify_jh) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改机号失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}
		
		//case34:恢复成原来的机号
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"AT#JH%s",old_jh+3);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		cls_show_msg("%s",sendbuf);
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原机号码失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=reclen || strcmp(recbuf, old_jh) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原机号码失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
			goto ERR;
		}
		
		//case35:写U盘测试文件
		cls_show_msg("确保蓝牙底座已插入U盘,任意键继续");
		cls_show_msg1(3,"写U盘测试文件中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[25]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(udisk_wr), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:写U盘测试文件失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(udisk_wr) || strcmp(recbuf, udisk_wr) )
		{
			cls_show_msg1(g_keeptime, "line %d:写U盘测试文件失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}

		//case36:读U盘测试文件
		cls_show_msg1(3,"读U盘测试文件中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[26]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(udisk_str), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:读U盘测试文件失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(udisk_str) || strcmp(recbuf, udisk_str) )
		{
			cls_show_msg1(g_keeptime, "line %d:读U盘测试文件失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}

		//case37:上传U盘测试文件
		cls_show_msg1(3,"上传U盘测试文件中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[27]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, sizeof(recbuf), &reclen, MAXTIME+1))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:上传U盘测试文件失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(cls_show_msg("获取到的U盘测试文件内容为%s,请与实际对照是否正确,是[ENTER],否则[其他]", recbuf) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,recbuf=%s)", __LINE__, sendbuf, recbuf);
			goto ERR;
		}

		//case38:读取原测试记录文件
		cls_show_msg1(3,"读取原测试记录文件中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[28]);
		memset(old_record, 0, sizeof(old_record));
		reclen = 0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), old_record, sizeof(old_record), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:读取原测试记录文件失败(%d,sendbuf=%s,old_record=%s)", __LINE__, ret, sendbuf, old_record);
			goto ERR;
		}
		if(cls_show_msg("读取到的原测试记录文件内容为%s,请与实际对照是否正确,是[ENTER],否则[其他]", old_record) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_record=%s)", __LINE__, sendbuf, old_record);
			goto ERR;
		}

		//case39:写测试记录文件
		cls_show_msg1(3,"写测试记录文件中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[29]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:写测试记录文件失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( strcmp(recbuf, "OK\r\n") )
		{
			cls_show_msg1(g_keeptime, "line %d:写测试记录文件失败(recbuf:%s)", __LINE__, recbuf);
			goto ERR;
		}
		
		//case40:读取写后的测试记录文件
		cls_show_msg1(3,"读取写后的测试记录文件中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[28]);
		memset(new_record, 0, sizeof(new_record));
		reclen = 0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), new_record, sizeof(new_record), &reclen, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:读取原测试记录文件失败(%d,sendbuf=%s,new_record=%s)", __LINE__, ret, sendbuf, new_record);
			goto ERR;
		}
		if(cls_show_msg("读取到的新测试记录文件内容为%s,请与实际对照是否正确,是[ENTER],否则[其他]", new_record) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,new_record=%s)", __LINE__, sendbuf, new_record);
			goto ERR;
		}
#if 0  //开发确认生效配置参数命令已废弃 故屏蔽 20180208
		//case41:生效配置参数:先设置蓝牙名称,后生效
		cls_show_msg1(3,"修改WIFI AP名称中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[11]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_wifiname), &len, MAXTIME+10))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP名称失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( len!=strlen(modify_wifiname) || strcmp(recbuf, modify_wifiname) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改WIFI AP名称失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			goto ERR;
		}
		if(cls_show_msg("使用手机进行搜索,看是否能够搜索到%s的WiFi,是[ENTER],否则[其他]", modify_wifiname) == ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(应该不能搜索到修改后的WiFi AP)", __LINE__);
			goto ERR;
		}
		cls_show_msg1(3,"生效配置参数中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[30]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:生效配置参数失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( strcmp(recbuf, "OK\r\n") )
		{
			cls_show_msg1(g_keeptime, "line %d:生效配置参数失败(recbuf:%s)", __LINE__, recbuf);
			goto ERR;
		}
		if(cls_show_msg("使用手机进行搜索,看是否能够搜索到%s的WiFi,是[ENTER],否则[其他]", modify_wifiname) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(应该要能搜索到修改后的WiFi AP)", __LINE__);
			goto ERR;
		}
		//恢复成原来的WiFi AP 名称
		cls_show_msg1(3,"修改成原WIFI AP名称中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"AT#WM%s",old_wifiname+13);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP名称失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(strcmp(recbuf, old_wifiname) )
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原WIFI AP名称失败(sendbuf=%s,recbuf:%s)", __LINE__, sendbuf, recbuf);
			goto ERR;
		}
		//生效
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[30]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME+10))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:生效配置参数失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if( strcmp(recbuf, "OK\r\n") )
		{
			cls_show_msg1(g_keeptime, "line %d:生效配置参数失败(recbuf:%s)", __LINE__, recbuf);
			goto ERR;
		}
#endif
		//case42:新增获取原来的以太网mac地址  sull add  20180208
		cls_show_msg1(3,"获取原来以太网mac地址中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[31]);
		memset(old_ethaddr, 0, sizeof(old_ethaddr));
		reclen = 0;
		if((ret = sendrecv_cmd(sendbuf, strlen(sendbuf), old_ethaddr, sizeof(old_ethaddr), &reclen, MAXTIME)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:获取原以太网mac地址失败(%d,sendbuf=%s,old_ethaddr=%s)", __LINE__, ret, sendbuf, old_ethaddr);
			goto ERR;
		}
		if(cls_show_msg("获取到的以太网mac地址为%s,请与实际对照是否正确，是[ENTER]，否则[其它]",old_ethaddr) != ENTER)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(sendbuf=%s,old_ethaddr=%s)", __LINE__, sendbuf, old_ethaddr);
			goto ERR;
		}

		//case43:修改以太网mac地址
		cls_show_msg1(3,"修改以太网mac地址中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		strcpy(sendbuf, cmd[32]);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret = sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen(modify_ethaddr), &len, MAXTIME)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改以太网mac地址失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=strlen(modify_ethaddr) || strcmp(recbuf, modify_ethaddr))
		{
			cls_show_msg1(g_keeptime, "line %d:修改以太网mac地址失败(len=%d,recbuf:%s)", __LINE__, len, recbuf);
			GOTOERR;
		}

		//case44:恢复成原来的以太网mac地址
		cls_show_msg1(3,"恢复成原以太网mac地址中...");
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf, "AT#EZ%s", old_ethaddr+8);
		memset(recbuf, 0, sizeof(recbuf));
		len=0;
		if((ret = sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, reclen, &len, MAXTIME)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原以太网mac地址失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
			goto ERR;
		}
		if(len!=reclen || strcmp(recbuf, old_ethaddr))
		{
			cls_show_msg1(g_keeptime, "line %d:修改成原以太网mac地址失败(len=%d,sendbuf=%s,recbuf:%s)", __LINE__, len, sendbuf, recbuf);
			goto ERR;
		}
	}

	cls_show_msg_record(FILENAME,FUNCNAME,"蓝牙底座测试成功");
	
ERR:
	cls_show_msg1(3,"恢复蓝牙底座串口默认BPS中...");	
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, cmd[7]);
	memset(recbuf, 0, sizeof(recbuf));
	len=0;	
	sendrecv_cmd(sendbuf, sizeof(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME);
	if(strcmp(recbuf, "OK\r\n")) 
		cls_show_msg1(g_keeptime, "line %d:恢复默认BPS失败,请通过串口工具恢复默认波特率(recbuf:%s)", __LINE__, recbuf);
	NDK_PortClose(PORT);
	return;
}
void setting_bt_pairmode(void)
{
	int nkeyin = 0, ret = -1, len = 0;
	char sendbuf[128] = {0}, recbuf[128] = {0};

	cls_show_msg("请将POS串口1与蓝牙底座串口通过串口线相连后,长按蓝牙底座上的复位键2s后进入工厂模式");
	if((ret=NDK_PortOpen(PORT, "115200,8,N,1"))!=NDK_OK)//默认情况
	{
		cls_show_msg1(g_keeptime, "line %d:打开串口失败(%d)", __LINE__, ret);
		return;
	}
	//新增发送一条指令(取版本),保证已进入工厂模式(如果存在sn,会把第一条指令当做进入工厂模式的指令)
	cls_printf("进入工厂模式中...");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, "AT#BV\r\n");
	memset(recbuf, 0, sizeof(recbuf));	
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, sizeof(recbuf), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}
	cls_show_msg1(2,"请设置蓝牙配对模式");
	do
	{
		nkeyin = cls_show_msg(  "1.Just Work\n"
							"2.PIN Code\n"
							"3.SSP PIN\n"
							"4.PassKey"
							);
		switch(nkeyin)
		{	
			case '1':
				strcpy(sendbuf, "AT#BP0\r\n");
				break;
			case '2':
				strcpy(sendbuf, "AT#BP1\r\n");
				break;
			case '3':
				strcpy(sendbuf, "AT#BP2\r\n");
				break;
			case '4':
				strcpy(sendbuf, "AT#BP3\r\n");
				break;
			case ESC:
				return;
				break;
			default:
				continue;
				break;
		} 
	}while(nkeyin!=0 && nkeyin!=ESC && (nkeyin<'1'||nkeyin>'4'));
	memset(recbuf, 0, sizeof(recbuf));	
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("OK\r\n"), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:设置蓝牙配对模式失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}
	if(strcmp(recbuf, "OK\r\n")) 
	{
		cls_show_msg1(g_keeptime, "line %d:设置蓝牙配对模式失败(recbuf=%s)", __LINE__, recbuf);
		return;
	}
	cls_show_msg1(2,"设置蓝牙配对模式成功");
	//退出工厂模式	
	cls_show_msg1(2,"退出工厂模式");
	memset(sendbuf, 0, sizeof(sendbuf));
	strcpy(sendbuf, "AT#BX\r\n");
	memset(recbuf, 0, sizeof(recbuf));
	if((ret=sendrecv_cmd(sendbuf, strlen(sendbuf), recbuf, strlen("exit Production Mode!"), &len, MAXTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(%d,sendbuf=%s,recbuf=%s)", __LINE__, ret, sendbuf, recbuf);
		return;
	}
}
	
void btdesk(void)
{
	int keyin = 0;

	while(1)
	{
		  keyin = cls_show_msg("1.蓝牙底座测试\n"
						"2.设置蓝牙配对模式\n"
						);
		switch(keyin) 
		{
		case '1':
			n900_btdesk();
			break;
		case '2':
			setting_bt_pairmode();
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
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		chensl		   20130131	 		created
*****************************************************************/
void systest9(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);	
	while(1)
    {
        keyin = cls_show_msg("1.波特率轮回\n"
						"2.POS<->PC\n"
						"3.POS<---PC\n"
						"4.自收自发\n"
						"5.蓝牙底座测试");
		switch(keyin) 
		{
		case '1':
			aux_circleinit();
			break;
		case '2':
			aux_comm_pc();
			break;
		case '3':
			aux_pctopos();
			break;
		case '4':
			aux_comm_self();
			break;
		case '5':
			btdesk();
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

