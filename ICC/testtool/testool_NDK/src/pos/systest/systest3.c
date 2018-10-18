/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest3.c
* Author 			: chensl
* version			: 
* DATE			: 20130124
* directory 	: 
* description	: 以太网综合测试
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
#define 	TESTITEM	"ETH性能,压力"
#define		PACKETLIFE	(50)//(1000)	//根据1.6规范进行修改 20131103 linwl
#define	DEFAULT_CNT_STR	"100"//"200"	//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(200)

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
static void create_eth_ability_packet(Packet *packet, char *buf)
{
	/*private & local definition*/
	
	/*process body*/
	memset(packet, 0, sizeof(Packet));
	packet->header = buf;//设置包头
	packet->len = packet->orig_len= PACKMAXLEN;//1024;//根据1.6规范进行修改 20131103 linwl
	packet->lifecycle = PACKETLIFE;//1000;
	packet->forever = FALSE;
	packet->IsLenRec = FALSE;
	packet->IsDataRnd = TRUE;

	return;
}

//以太网性能测试:通讯次数1000次,内容随机,长度1024
static void eth_ability(void)
{
	/*private & local definition*/
	/*SOCKET*/
	uint h_tcp = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	int timeout = SO_TIMEO;
	float commtimes = 0.0;
	int slen = 0, rlen = 0;//, bak_life = 0;
	Packet sendpacket;
#if K21_ENABLE
	char str[32] = {0};
#endif

	/*process body*/
	create_eth_ability_packet(&sendpacket, buf);
	//bak_life = sendpacket.lifecycle;
		
	//测试前置
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:NetUp失败", __LINE__);
		return;
	}
		
	if(TransUp(&h_tcp)!=SUCC)
	{
		NetDown();
		cls_show_msg_record(FILENAME ,FUNCNAME ,"line %d:TransUp失败", __LINE__);
		return;
	}
		
	while (1)
	{
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
				
		//数据通讯(收一次发一次)
		lib_StartStopwatch();
		slen = sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout);
		memset(rbuf, 0, sizeof(rbuf));
		rlen = sock_recv(h_tcp, rbuf, slen, timeout);
		commtimes+=lib_ReadStopwatch();
	}
		
	//测试后置
	TransDown(h_tcp);
	NetDown();

#if K21_ENABLE
	memset(str,0,sizeof(str));
	ftos((sendpacket.len*2/1024)*PACKETLIFE/commtimes,str);
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯速率%sKB/s", str);
#else
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯速率%fKB/s", (sendpacket.len*2/1024)*PACKETLIFE/commtimes);
#endif
	return;
}

static void eth_comm_press(void)
{
	/*private & local definition*/
	int key = '0';
	uint h_tcp = -1;
	void (*pf[3])(uint) = {send_recv_press, recv_only_press, send_only_press};
	
	/*process body*/
	while (1)
	{
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			if( ++key == '2' ) /*此处只做双向收发是因为双向和单向的后台不一样,对于2、3需要另外测试*/
			{
				return ;
			}
			cls_show_msg1(10, "自动测试仅执行双向收发压力");
		}
		else
		{
			key = cls_show_msg("_ETH收发压力_\n"
								"1.双向收发压力\n"
								"2.单向接收压力\n"
								"3.单向发送压力\n");
		}
		switch(key)
		{
		case '1':
		case '2':
		case '3':
			if(NetUp()!=SUCC)
			{
				cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
				return;
			}
			
			if(TransUp(&h_tcp)!=SUCC)
			{
				NetDown();
				cls_show_msg_record(FILENAME,FUNCNAME,"line %d:TransUp失败", __LINE__);
				return;
			}
			pf[key-'1'](h_tcp);
			TransDown(h_tcp);
			NetDown();
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
}

static void eth_cnnt_press(void)
{
	/*private & local definition*/
	int i = 0, succ_count = 0;
	uint h_tcp = 0;

	/*process body*/
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
		return;
	}
	
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次连接压力(已成功%d次),按ESC退出", i+1, succ_count)==ESC)//手动退出压力测试
			break;
		
		if(g_SequencePressFlag&&(GetCycleValue()==i))  //自动压力测试达到压力次数后退出
			break;
		
		i++;
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransUp失败", __LINE__);
			continue;
		}
		
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransDown失败", __LINE__);
			continue;
		}
		succ_count++;
	}
	
	NetDown();
	
	cls_show_msg_record(FILENAME,FUNCNAME,"通讯次数:%d,成功次数:%d", i, succ_count);
	return;
}

static void cnnt_comm_press(void)
{
	/*private & local definition*/
	int i = 0, succ_count = 0, j = 0, loop = 0, relaykey = 0;
	int slen = 0, rlen = 0, timeout = SO_TIMEO;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;
	uint  h_tcp = 0;

	/*process body*/
	if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
	{
		relaykey = cls_show_msg1(5,"压力中是否要有间隔:[ENTER]是 [其他]否")==ENTER?TRUE:FALSE;
	}
	else
	{
		relaykey = cls_show_msg("压力中是否要有间隔:[ENTER]是 [其他]否")==ENTER?TRUE:FALSE;
	}
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
		return;
	}
	
BEGIN:	
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次建链/通讯混合压力(已成功%d次),本次将收发%d回", i+1, succ_count, (loop=(rand()%10+1)))==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		
		if(relaykey==TRUE) 
		{
			if(i%5==0)
			{	
				j = rand();
				if(j%3==1)
					cls_show_msg1(120,"等待120秒");
				else if(j%3==2)
					cls_show_msg1(180,"等待180秒");
				else //if(j%3==0)
					cls_show_msg1(240,"等待240秒");
			}
		}
		
		//UP
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransUp失败", __LINE__);
			continue;
		}
		
		for(j=0; j<loop; j++)
		{
			//发送数据
			if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
			{
				TransDown(h_tcp);
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len);
				goto BEGIN;//continue;
			}
			//接收数据
			memset(rbuf, 0, sizeof(rbuf));
			if ((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
			{
				TransDown(h_tcp);
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, sendpacket.len);
				goto BEGIN;//continue;
			}
			//比较收发
			if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
			{
				TransDown(h_tcp);
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验失败", __LINE__);
				goto BEGIN;//continue;
			}
		}
		
		//DOWN
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:TransDown失败", __LINE__);
			continue;
		}
		succ_count++;
	}
	
	NetDown();

	cls_show_msg_record(FILENAME,FUNCNAME,"通讯次数:%d,成功次数:%d", i, succ_count);
	return;
}

static void eth_updown_press(void)
{
	/*private & local definition*/
	int i = 0, succ = 0, ret = -1, disflag = 0;
	uint h_tcp = 0;

	if(cls_show_msg1(5,"连接压力时是否调用NDK_EthDisConnect?是[ENTER],否[其它]")==ENTER)//默认不调用
		disflag = 1;
	/*process body*/
	while (1)
	{
		if(cls_show_msg1(2, "正在进行第%d次以太网芯片压力(已成功%d次)", i+1, succ)==ESC)
			break;
		
		if(g_SequencePressFlag&&(GetCycleValue()==i))  //自动压力测试达到压力次数后退出
			break;
		
		i++;
		
		if(NetUp()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NetUp失败", __LINE__, i);
			continue;
		}
		if(TransUp(&h_tcp)!=SUCC)//让POS与服务器实际发生一下连接,以测试以太网开关对连接的影响
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransUp失败", __LINE__, i);
			continue;
		}
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次TransDown失败", __LINE__, i);
			continue;
		}		
		if(NetDown()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NetDown失败", __LINE__, i);
			continue;
		}

		//验证新增的NDK_EthDisConnect函数在卸载以太网驱动时不会影响正常功能 20140616 linwl added
		if(disflag==1)
		{
			if((ret=NDK_EthDisConnect())!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NDK_EthDisConnect失败(%d)", __LINE__, i, ret);
				NDK_EthDisConnect();
				continue;
			}
		}
		succ++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"压力次数:%d,成功次数:%d", i, succ);
	return;
}

static void ping_press(void)
{
	/*private & local definition*/
	int ret = 0, succ = 0, i = 0, cnt = DEFAULT_CNT_VLE;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint len;

	/*process body*/
	if(g_SequencePressFlag)  
	{
		cnt = GetCycleValue();
	}
	else
	{
		cls_printf("默认压力测试次数:");
		if ((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
			cnt = DEFAULT_CNT_VLE;
		else
			cnt = atoi(CntStr);
	}
	
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
		return;
	}

	while(cnt)
	{
		if (ESC==cls_show_msg1(2, "ping压力测试中\n还剩%d次(已成功%d次),ESC中断测试", cnt, succ))
			break;
		cnt--;
		i++;

		switch(ret=NDK_NetPing(netsetting.SvrIP.Str,5))
		{
		case 0:
			succ++;
			break;
		default:
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "第%d次错误返回%d", i,ret);
			break;
		}
		
	}
	NetDown();

	cls_show_msg_record(FILENAME,FUNCNAME,"ping压力测试%d次,成功%d次", i, succ);
	return;
} 

static void eth_press(void)
{
	/*private & local definition*/
	int keyin='0';
	
	/*process body*/
	while (1)
	{
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			//对应下面的菜单数+1，菜单增加此处也要对应的增加
			if((++keyin == '6')||(cls_show_msg1(2, "即将进行%s连续压力测试,ESC退出",TESTITEM)==ESC))
				return ;
		}
		else
		{
			keyin = cls_show_msg("1.数传2.连接\n"
								"3.连接+数传\n"
								"4.建链压力\n"
								"5.ping压力");
		}
		
		switch(keyin)
		{
		case '1':
			eth_comm_press();
			break;
		case '2':
			eth_cnnt_press();
			break;
		case '3':
			cnnt_comm_press();
			break;
		case '4':
			eth_updown_press();
			break;			
		case '5':
			ping_press();
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

void systest3(void)
{
	//以下为局部变量
	int nKey = 0;

	if(auto_flag==1)//autotest_flag
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_ETH(TRUE)!=SUCC)
			return;
		
		g_CycleTime = 100;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行压力测试
		eth_press();
		//执行性能测试
		eth_ability();
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	
	//以下为人工测试函数体
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	while (1)
	{
		nKey = cls_show_msg("1.ETH压力测试\n"
							"2.ETH性能测试\n"
							"3.ETH异常测试\n"
							"0.ETH配置");
		switch(nKey)
		{
		case '1':
			eth_press();
			break;
		case '2':
			eth_ability();
			break;
		case '3':
			tcp_abnormal();//20140422 linwl 新增以太网强制挂断和协议挂断异常测试用例
			cls_show_msg("请配置未开启后台的服务器地址,应不能连通!");
			break;
		case '0':
			switch(conf_conn_ETH(TRUE))
			{
			case SUCC:
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
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

