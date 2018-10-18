/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: MODEM( 异步同步)/串口综合测试
* file name		: 2.c
* Author 			: 
* version			: V1.0
* DATE			: 20060911
* directory 		: .\SRC\systest\
* description		: MODEM综合测试
* related document	: NL720程序员参考手册.doc
*				  
************************************************************************
* log			: 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/

#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	UCID					2
#define	TESTITEM				"MODEM性能,压力"

#define	MAX_PHONENUM_CNT		150
#define	MAX_PHONENUM_SIZE	16
#define	SIZEOFSEPERATOR		strlen("\r\n")
#define	phonenumfile			"phonenum.txt"
#define	LOG_NACTEST			"system2.result"


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
*			  		  panxl 		   20060911 		created
*					zhangnw		  20100601		modify
*****************************************************************/
/*
以下设计会使得无法进行含有复位操作的1次压力测试,但不考虑此情况(实际中可通过ESC中断无限次即可)
maxcnt:
=0时,表示压力次数不受maxcnt控制(仅受ESC控制);
=-1时,表示压力次数不受maxcnt控制(仅受ESC控制),还会进行复位操作;
>0时,maxcnt表示最大压力次数;
<-1时,abs(maxcnt)表示最大压力次数,还会进行复位操作;
函数返回:进行的总压力次数
*/
static int __mdm_dial_press(char *dialstr, const int maxcnt, int *isucc, float *call_time, float *hangdowntime)
{
	/*private & local definition*/
	int icount = 0, ret = -1;
	*isucc = 0;
	*call_time = *hangdowntime = 0.0;
//	float tmp_time = 0.0;
	float init_time = 0.0;
//	LinkType type = getlinktype();
//	int initsucc = 0;//记录初始化次数 added by linwl 20140606

   	/*process body*/
	while (1)
	{
		if(cls_show_msg1(5, "第%d次拨%s中(已成功%d次),按ESC退出", icount+1, dialstr, *isucc)==ESC)
			break;	
		if(maxcnt==0 || maxcnt==-1)
			;
		else if(icount==abs(maxcnt))
			break;
		icount++;

		if(maxcnt<0)
		{
			//复位MODEM
			cls_printf("复位MODEM中(第%d次)...", icount);
			mdm_reset();
		}

		if(maxcnt>=0&&icount==1)//第一次压力开始时增加一次复位操作,这样可以保证初始化是真初始化,让下一步操作能够获取到初始化的时间
		{
			//复位MODEM
			//cls_printf("复位MODEM中(第%d次)...", icount);
			mdm_reset();
		}
		
		//初始化MODEM
		cls_printf("初始化MODEM中(第%d次)...", icount);
		//if(icount==1)//只有第一次初始化时间较长，初始化一次之后再初始化底层实际上是直接返回的,所以后续初始化时间约为0
		lib_StartStopwatch();
		if((ret=mdm_init())!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:初始化MODEM(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}
		init_time+=lib_ReadStopwatch();//复位情况下 初始化时间累计统计			
//		if(icount==1)
//			tmp_time=lib_ReadStopwatch();//不复位情况下只统计第一次
//		initsucc++;//记录初始化次数 added by linwl 20140606
		
		//拨号
		cls_printf("MODEM拨%s中(第%d次)...", dialstr, icount);
		lib_StartStopwatch();
		if(mdm_dial(dialstr)!=SUCC)
		{
			mdm_hangup();
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:MODEM拨%s(第%d次)失败", __LINE__, dialstr, icount);
			continue;
		}
		*call_time += lib_ReadStopwatch();
		++*isucc;
		
		//挂断
		cls_printf("MODEM挂断中(第%d次)...", icount);
		lib_StartStopwatch();
		if(mdm_hangup()!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:MODEM挂断(第%d次)失败", __LINE__, icount);
			continue;
		}
		*hangdowntime += (lib_ReadStopwatch()-(getlinktype()==SYNC?SLEEP_SDLC_HANGUP:0));
	}

//	if(maxcnt>=0)
//		cls_show_msg("hangdowntime=%f,tmp_time=%f, isucc = %d",*hangdowntime,tmp_time*(*isucc), *isucc);
//	if(maxcnt<0)
//		cls_show_msg("hangdowntime=%f,init_time=%f, isucc = %d",*hangdowntime,(init_time/initsucc)*(*isucc), *isucc);
	
//	if(type==SYNC)//同步将初始化的操作移动到挂断中，所以挂断时间需要重新统计真实值
//	{//modified by linwl 20140606
//		if((maxcnt>=0)&&(*hangdowntime>tmp_time*(*isucc)))
//			*hangdowntime=*hangdowntime-tmp_time*(*isucc)/*icount*/;//仅第一次复位
//		if((maxcnt<0)&&(*hangdowntime>((init_time/initsucc)*(*isucc))))
//			*hangdowntime=*hangdowntime-(init_time/initsucc)*(*isucc);//挂断对应的初始化时间=成功始化时间/成功初始化次数*挂断成功的次数
//	}
	
	return icount;
}

#if 0
static int data_comm_press(void)
{
	/*private & local definition*/
	int i = 0, succ_cout = 0;
	int send_len = 0, rec_len = 0, len = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;
	float sendtime = 0.0,revtime = 0.0;

	/*process body*/
  	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	mdm_clrportbuf_all();
	
	while (1)
	{
		if(cls_show_msg1(2, "进行第%d次通讯中(已成功%d次),按ESC退出", i+1, succ_cout)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
			
		//发送数据
		lib_StartStopwatch();
		if((len=mdm_send(sendpacket.header, sendpacket.len))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:发送数据失败(实际%d, 预期%d)", __LINE__, len, sendpacket.len);
			mdm_clrportbuf_all();//出错后恢复一下,以不影响下一次测试
			continue;
		}
		sendtime+=lib_ReadStopwatch();
		send_len+=len;
		//接收数据
		lib_StartStopwatch();
		if((len=mdm_rev(rbuf, sendpacket.len, 20))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:接收数据失败(实际%d, 预期%d)", __LINE__, len, sendpacket.len);
			mdm_clrportbuf_all();
			continue;
		}
		revtime+=lib_ReadStopwatch();
		rec_len+=len;
		//比较数据
		if(MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:数据校验失败", __LINE__);
			continue;
		}
		
		succ_cout++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"Total:%d,succ:%d", i, succ_cout);
	if(sendtime!=0&&revtime!=0)//linwl 增加条件当sendtime，revtime为0时不会跑飞退出；20120607
		cls_show_msg_record(FILENAME,FUNCNAME,"发送:%fB/s,接收:%fB/s\n", send_len/sendtime, rec_len/revtime);
	return (i==succ_cout)?SUCC:FAIL;
}
#endif

static void comm_press(char *dialstr)
{
	/*private & local definition*/
	int ret = -1;
	EM_MDMSTATUS MdmStatus = 0;	//mdm状态
	LinkType type = getlinktype();
	int i = 0, succ_cout = 0, last=-1;
	int send_len = 0, rec_len = 0, len = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;
	float sendtime = 0.0,revtime = 0.0;
	
   	/*process body*/
	strcpy(MDMDialStr, dialstr);
  	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	mdm_clrportbuf_all();
	
PRE:
	if(LinkUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:创建链路失败", __LINE__);
		return;
	}
	while (1)
	{
		if(cls_show_msg1(2, "进行第%d次通讯中(已成功%d次),按ESC退出", i+1, succ_cout)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
			
		//发送数据
		lib_StartStopwatch();
		if((len=mdm_send(sendpacket.header, sendpacket.len))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:发送数据失败(实际%d, 预期%d)", __LINE__, len, sendpacket.len);
			mdm_clrportbuf_all();//出错后恢复一下,以不影响下一次测试
			if(IsContinuous(&i, &last))
			{
				LinkDown();
				goto PRE;
			}
			else
				continue;
		}
		sendtime+=lib_ReadStopwatch();
		send_len+=len;
		//接收数据
		lib_StartStopwatch();
		if((len=mdm_rev(rbuf, sendpacket.len, 20))!=sendpacket.len)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:接收数据失败(实际%d, 预期%d)", __LINE__, len, sendpacket.len);
			mdm_clrportbuf_all();
			if(IsContinuous(&i, &last))
			{
				LinkDown();
				goto PRE;
			}
			else
				continue;
		}
		revtime+=lib_ReadStopwatch();
		rec_len+=len;
		//比较数据
		if(MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:数据校验失败", __LINE__);
			mdm_clrportbuf_all();//出错后恢复一下,以不影响下一次测试
			if(IsContinuous(&i, &last))
			{
				LinkDown();
				goto PRE;
			}
			else
				continue;
		}
		
		succ_cout++;
	}
	
	if(type==SYNC)
	{
		//挂断状态测试
		
		if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_CONNECT_AFTERPREDIAL))
		{
			LinkDown();
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:SDLC_getmodemstatus发生错误%d", __LINE__,MdmStatus);
			return;
		}
	}

	if(LinkDown()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:断开链路失败", __LINE__);
		return;
	}
    if(type==SYNC)
	{
		
		if(((ret=NDK_MdmCheck(&MdmStatus))!=NDK_OK)||(MdmStatus!=MDMSTATUS_NOPREDIAL))
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:SDLC_getmodemstatus发生错误", __LINE__);
			return;
		}
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"Total:%d,succ:%d", i, succ_cout);
	if(sendtime!=0&&revtime!=0)//linwl 增加条件当sendtime，revtime为0时不会跑飞退出；20120607
		cls_show_msg_record(FILENAME,FUNCNAME,"发送:%fB/s,接收:%fB/s", send_len/sendtime, rec_len/revtime);
	return;
}

static void call_comm_press(char *dialstr)
{
	/*private & local definition*/
	int icount = 0, isucc = 0;
	float call_time = 0.0, hangdowntime = 0.0, dial = 0.0, hang = 0.0;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0};
	
   	/*process body*/		
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	mdm_clrportbuf_all();
	
	while (1)
	{
		if(cls_show_msg1(5, "第%d次拨%s通讯中(已成功%d次),按ESC退出", icount+1, dialstr, isucc)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		icount++;

		if(mdm_dial_comm(dialstr, &sendpacket, FALSE, &dial, &hang)==SUCC)
		{
			isucc++;
			call_time+=dial;
			hangdowntime+=hang;
		}
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"循环了%d次,成功了%d次", icount, isucc);
	if(isucc!=0)
		cls_show_msg_record(FILENAME,FUNCNAME,"拨%s平均时间%3.5f秒/次,挂断平均时间(含初始化)%3.5f秒/次", dialstr, call_time/isucc, hangdowntime/isucc);
	return;
}

static void call_press(char *dialstr)
{
	/*private & local definition*/
	int isucc = 0;
	float call_time = 0.0, hangdowntime = 0.0;
	
   	/*process body*/
	if(g_SequencePressFlag) 
		cls_show_msg_record(FILENAME,FUNCNAME,"循环了%d次,成功了%d次", __mdm_dial_press(dialstr, GetCycleValue(), &isucc, &call_time, &hangdowntime), isucc);
	else
		cls_show_msg_record(FILENAME,FUNCNAME,"循环了%d次,成功了%d次", __mdm_dial_press(dialstr, 0, &isucc, &call_time, &hangdowntime), isucc);
	if(isucc!=0)
		cls_show_msg_record(FILENAME,FUNCNAME,"拨%s平均时间%3.5f秒/次,挂断平均时间%3.5f秒/次", dialstr, call_time/isucc, hangdowntime/isucc);
	return;
}
static void send_result_NACtest(void)
{
	/*private & local definition*/
	int fp = 0, ret = 0, rdlen = 0;
	char result[1024] = {0};
	
	/*process body*/
	NDK_PortClose(PORT_NUM_COM1);
	cls_show_msg("请将POS的串口1连接PC,并打开PC接收工具.按任意键继续");
	if(	NDK_PortOpen(PORT_NUM_COM1, "115200") != NDK_OK )
	{
		cls_show_msg("line %d:打开串口2失败", __LINE__);
		return;
	}
	if ((fp=NDK_FsOpen(LOG_NACTEST, "r"))< NDK_OK)
	{
		cls_show_msg("line %d:打开%s失败(%d)", __LINE__, LOG_NACTEST);
		return;
	}
	while((rdlen=NDK_FsRead(fp, result,sizeof(result)))==sizeof(result))
	{
		if((ret=NDK_PortWrite(PORT_NUM_COM1, rdlen, result))!=NDK_OK)
		{
			cls_show_msg("line %d: 发送失败ret=%d", __LINE__, ret);
			NDK_FsClose(fp);
			return;
		}
	}
	if(rdlen>0)
	{
		if((ret=NDK_PortWrite(PORT_NUM_COM1,rdlen, result))!=NDK_OK)
			cls_show_msg("line %d: 发送失败ret=%d", __LINE__, ret);	
	}	
	NDK_FsClose(fp);
	NDK_PortClose(PORT_NUM_COM1);//关闭串口
	if (g_com!=INVALID_COM)//打开默认的串口
	{
		if ((ret=NDK_PortOpen(g_com, "115200,8,N,1")) != NDK_OK)
		{
			//初始化失败了,也不必send_result("line %d: 初始化串口错", __LINE__);
			cls_show_msg("line %d: 初始化串口错", __LINE__);
			return;
		}
	}
	return;
}
static int  get_allphonenum(char *allphonenum, const int maxbuflen)
{
	/*private & local definition*/
	int fp = 0, readlen = 0;
	uint phonenumfilelen = 0;
	char *pos = NULL, *start = allphonenum;
	
	/*process body*/
	//step1:读取全部电话号码文件到缓冲
	if(NDK_FsExist(phonenumfile)!=NDK_OK)
	{
		cls_show_msg("line %d:测试文件不存在!", __LINE__);
		return FAIL;
	}
	if ((fp=NDK_FsOpen(phonenumfile, "r"))<0)
	{
		cls_show_msg("line %d:打开%s失败!", __LINE__, phonenumfile);
		return FAIL;
	}
	if (NDK_FsFileSize(phonenumfile, &phonenumfilelen) != NDK_OK)
	{
		cls_show_msg("line %d:获取%s文件长度失败!", __LINE__, phonenumfile);
		return FAIL;
	}
	if(phonenumfilelen>maxbuflen-1)//不允许缓冲区长度小于文件长度
	{
		cls_show_msg("line %d:无法读出完整文件内容!", __LINE__);
		NDK_FsClose(fp);
		return FAIL;
	}
	memset(allphonenum, 0, maxbuflen);
	if ((readlen=NDK_FsRead(fp, allphonenum, maxbuflen-1)) < NDK_OK)//少读一个以确保allphonenum是串
	{
		cls_show_msg("line %d:读文件出错(%d)", __LINE__, readlen);
		NDK_FsClose(fp);
		return FAIL;
	}
	NDK_FsClose(fp);

	//step2:格式化号码缓冲(\r\n->\x00\x00)
	while(strlen(start))
	{
		if((pos=strstr(start, "\x0D\x0A"))!=NULL)
		{
			memset(pos, 0x00, SIZEOFSEPERATOR);
			if(strlen(start)>=MAX_PHONENUM_SIZE)
			{
				cls_show_msg("line %d:检测到超长号码(%s)!", __LINE__, start);
				return FAIL;	
			}
			start = pos+SIZEOFSEPERATOR;
		}
		else
			break;
	}
	
	return SUCC;
}

static int  get_phonenum(char *allphonenum, const int maxbuflen, int id, char *phonenum)
{
	/*private & local definition*/
	char *curr = allphonenum;
	
	/*process body*/
	//step1:找到指定id的号码串并拷入phonenum
	while(id>0)
	{
		if(strlen(curr))//找到一行"有效"数据
		{
			if(--id==0)
			{
				strcpy(phonenum, curr);
				
#if 0	//由调用者再判断,非法号码无非就是拔不上
				//step2:phonenum合法性判断
				if(is_digit_str(phonenum))
					return SUCC;
				else
				{
					//cls_show_msg("line %d:存在非数字号码!", __LINE__);
					return FAIL;
				}
#else
				return SUCC;
#endif
			}
		}
		curr+=strlen(curr)+1;
		if(curr+1>=allphonenum+maxbuflen)//越界
			return FAIL;//NOT FOUND
	}
	
	return FAIL;//NOT FOUND(传入的id<=0)
}

static int nac_mdm_dial_press(char *dialstr, const int maxcnt, int *isucc, float *call_time, float *hangdowntime)
{
	/*private & local definition*/
	int icount = 0, ret = -1;
	*isucc = 0;
	*call_time = *hangdowntime = 0.0;
//	float tmp_time = 0.0;
   	/*process body*/
	while (1)
	{
		if(cls_show_msg1(5, "第%d次拨%s中(已成功%d次),按ESC退出", icount+1, dialstr, *isucc)==ESC)
			break;	
		if(maxcnt==0 || maxcnt==-1)
			;
		else if(icount==abs(maxcnt))
			break;
		icount++;

		if(maxcnt<0)
		{
			//复位MODEM
			cls_printf("复位MODEM中(第%d次)...", icount);
			mdm_reset();
		}
		//初始化MODEM
		cls_printf("初始化MODEM中(第%d次)...", icount);
		if((ret=mdm_init())!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:初始化MODEM(第%d次)失败(%d)", __LINE__, icount, ret);
			continue;
		}
		//拨号
		cls_printf("MODEM拨%s中(第%d次)...", dialstr, icount);
		lib_StartStopwatch();
		if(mdm_dial(dialstr)!=SUCC)
		{
			mdm_hangup();
			cls_show_msg1(3, "line %d:MODEM拨%s(第%d次)失败", __LINE__, dialstr, icount);
			continue;
		}

		*call_time += lib_ReadStopwatch();
		++*isucc;
		//挂断
		cls_printf("MODEM挂断中(第%d次)...", icount);
		lib_StartStopwatch();
		if(mdm_hangup()!=SUCC)
		{
			cls_show_msg1(3, "line %d:MODEM挂断(第%d次)失败", __LINE__, icount);
			continue;
		}
		*hangdowntime += (lib_ReadStopwatch()-(getlinktype()==SYNC?SLEEP_SDLC_HANGUP:0));
	}
	
	return icount;
}


static void NAC_test(void)
{
	/*private & local definition*/
	char allphonenum[MAX_PHONENUM_CNT*MAX_PHONENUM_SIZE] = {0};//用于存放全部电话号码
	char phonenum[MAX_PHONENUM_SIZE] = {0};
	int icount = 0, isucc = 0;//拨号次数和成功次数
	float call_time = 0.0, hangdowntime = 0.0;//拨号时间和挂断时间
	char resultbuf[128] = {0};//用于存放测试结果
	int fp = 0, index = 0, nKeyin = 0;
	
	/*process body*/
	//对于已存在结果文件,要求要删除后才允许进入下一步
	if(NDK_FsExist(LOG_NACTEST)==NDK_OK)
	{
		nKeyin = cls_show_msg("检测到旧LOG,请选择:\n"
						"1.导出 2.删除\n");
		switch(nKeyin)
		{
		case '1':
			send_result_NACtest();
			return;
			break;
		case '2':
			NDK_FsDel(LOG_NACTEST);
			return;
			break;
		case ESC:
			return;
			break;
		default:
			break;
		}
	}

	//读取电话号码文件到格式化缓冲
	if(get_allphonenum(allphonenum, sizeof(allphonenum))!=SUCC)
		return;

	if((fp=NDK_FsOpen(LOG_NACTEST, "w"))<0)
	{
		cls_show_msg("line %d:创建%s失败!", LOG_NACTEST);
		return;
	}
	//每次取一个号码进行拔号/挂断的压力测试,并将每次的测试结果记录到文件
	while(get_phonenum(allphonenum, sizeof(allphonenum), ++index, phonenum)!=FAIL)
	{
		icount = nac_mdm_dial_press(phonenum, 5/*10*/, &isucc, &call_time, &hangdowntime);//为了避免话费消耗，减小到5次 20140901 linwl
		if(isucc!=0)
			sprintf(resultbuf, "%s拨%d次 成功%d次.拨号:%2.3fs/次,挂断:%2.3fs/次\r\n", phonenum, icount, isucc, call_time/isucc, hangdowntime/isucc);
		else
			sprintf(resultbuf, "%s拨%d次 成功%d次.\r\n", phonenum, icount, isucc);

		if(cls_show_msg1(5, resultbuf)==ESC)
			break;
		if(NDK_FsWrite(fp, resultbuf, strlen(resultbuf))!=strlen(resultbuf))
		{
			cls_show_msg("line %d:写%s失败!", __LINE__, LOG_NACTEST);
			break;
		}
	}
	NDK_FsClose(fp);

	//上送测试结果
	if(cls_show_msg("测试完成,是否上送结果?\n[确认]是[其它]否")==ENTER)
		send_result_NACtest();

	return;
}

static void reset_call_press(char *dialstr)
{
	/*private & local definition*/
	int isucc = 0;
	float call_time = 0.0, hangdowntime = 0.0;

   	/*process body*/
	if(g_SequencePressFlag) 
		cls_show_msg_record(FILENAME,FUNCNAME,"循环了%d次,成功了%d次", __mdm_dial_press(dialstr, -(GetCycleValue()), &isucc, &call_time, &hangdowntime), isucc);
	else
		cls_show_msg_record(FILENAME,FUNCNAME,"循环了%d次,成功了%d次", __mdm_dial_press(dialstr, -1, &isucc, &call_time, &hangdowntime), isucc);
	if(isucc!=0)
		cls_show_msg_record(FILENAME,FUNCNAME,"拨%s平均时间%3.5f秒/次,挂断平均时间%3.5f秒/次\n", dialstr, call_time/isucc, hangdowntime/isucc);
	return;
}

static void modem_press(void)
{
	/*private & local definition*/
	int keyin = '0';
	void (*pfunc[])(char *) = {comm_press, call_press, call_comm_press, reset_call_press};
	
	/*process body*/
	cls_show_msg1(g_keeptime, "请确保已有电话接入POS电话口(但不摘机),任意键继续...");
				
	while (1)
	{
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			if( ++keyin == '5') //对应下面的菜单数+1，菜单增加此处也要对应的增加
			{
				cls_show_msg_record(FILENAME ,FUNCNAME ,"%s连续压力测试结束\n",TESTITEM);
				return ;
			}
			if(cls_show_msg1(2, "即将进行连续压力测试,ESC退出")==ESC)
				return;
		}
		else
		{
			keyin = cls_show_msg("1.通讯压力\n" 
							"2.拨号压力\n"
							"3.通讯拨号压力\n"
							"4.复位拨号压力\n");
		}
		
		switch (keyin)
		{
		case '1':
		case '2':
		case '3':
		case '4':
			pfunc[keyin-'1'](MDMDialStr);
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

static void modem_ability(void)
{
	/*private & local definition*/
	int keyin = 0;
	void (*pfunc[])(char *) = {call_press, reset_call_press, call_press};
	
	/*process body*/
	while(1)
       {
              keyin = cls_show_msg("1.同步MODEM简易\n"
							"2.同步MODEM高级\n"
							"3.异步MODEM\n"
							"4.NAC\n");
		switch(keyin) 
		{
		case '1':
		case '2':
			if(getlinktype()!=SYNC)
			{
				cls_show_msg1(5, "MODEM类型配置错误!\n请先进行配置...");
				return;
			}
			pfunc[keyin-'1'](MDMDialStr);
			break;
		case '3':
			if(getlinktype()!=ASYN)
			{
				cls_show_msg1(5, "MODEM类型配置错误!\n请先进行配置...");
				return;
			}
			pfunc[keyin-'1'](MDMDialStr);
			break;
		case '4':
			NAC_test();
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

//modem异常测试: 测试先进行两次非法号码拨号,再进行一次正确号码拨号看最后一次是否能拨通
static void modem_abnormal(void)
{
	/*private & local definition*/
	int icount = 0, ret = -1;
	char dialstr1[10]={"5840"},dialstr2[10]={"7540"}; /*modem正常拨号范围#1~#8 */
	char dialstr[10]={0};
	
   	/*process body*/
	cls_printf("modem异常测试中");
	while (1)
	{
		if(++icount==4)
			break;

		//初始化MODEM
		cls_printf("初始化MODEM中(第%d次)...", icount);
		if((ret=mdm_init())!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:初始化MODEM(第%d次)失败(%d)", __LINE__, icount, ret);
			return;
		}
		//拨号
		memset(dialstr,0,sizeof(dialstr));
		if(icount==1)
			strcpy(dialstr,dialstr1);
		else if(icount==2)
			strcpy(dialstr,dialstr2);
		else
			strcpy(dialstr,linksetting.WMDialStr);
		
		cls_printf("MODEM拨%s中(第%d次)...", dialstr, icount);
		if(mdm_dial(dialstr)!=SUCC)
		{
			if(icount==3)
			{
				mdm_hangup();
				cls_show_msg1(g_keeptime, "line %d:MODEM异常测试失败", __LINE__);
				return;
			}
			mdm_hangup();
			continue;
		}

		//挂断
		cls_printf("MODEM挂断中(第%d次)...", icount);
		mdm_hangup();
		break;	
	}
	cls_show_msg("MODEM异常测试通过,任意键继续");
}

//验证进行MDM同步/异步初始化切换后应该能够正常拨号
static void modem_abnormal2(void)
{
	/*private & local definition*/
	char dialstr[10]={0};
	LinkType type = getlinktype();
	int i = 0, ret = -1;
	
   	/*process body*/
	cls_printf("modem异常测试中");
	if(type==ASYN)
		NDK_MdmSdlcInit(MDMPatchType);//测试异步拨号前进行同步初始化
	else if(type==SYNC)
	{
		NDK_MdmSdlcInit(MDMPatchType);
		NDK_MdmAsynInit(MDMPatchType);//测试同步拨号前进行异步初始化
	}
	for(i=0; i<2; i++)
	{
		cls_printf("初始化MODEM中...");
		if((ret=mdm_init())!=SUCC)
		{
			cls_show_msg("line %d:初始化MODEM失败(%d)异常测试失败", __LINE__, ret);
			return;
		}
		strcpy(dialstr,linksetting.WMDialStr);
		if(mdm_dial(dialstr)!=SUCC)
		{
			cls_show_msg("line %d:MODEM拨%s失败异常测试失败", __LINE__, dialstr);
			mdm_hangup();
			return;
		}
		//挂断
		cls_printf("MODEM挂断中...");
		if(mdm_hangup()!=SUCC)
		{
			cls_show_msg("line %d:MODEM挂断失败异常测试失败", __LINE__);
			return;
		}
	}
	cls_show_msg("MODEM异常测试通过,任意键继续");
}

void  MDM_abnormal(void)
{
	int keyin = 0;
	
	while(1)
	{
         keyin = cls_show_msg("1.非法号码拨号\n"
							"2.同步异步切换");
		switch(keyin)
		{
		case '1':
			modem_abnormal();
			break;
		case '2':
			modem_abnormal2();
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
void systest2(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_MDM()!=SUCC)
			return;
		g_CycleTime = 100;
		
		//自动化测试内容
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		
		//执行压力测试
		modem_press();

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	
	while(1)
	{
		keyin = cls_show_msg("_MODEM综合测试_\n"
							"1.压力  2.性能\n"
							"3.MOEM异常测试\n"
							"0.MODEM配置");
		switch(keyin)
		{
		case '1':
			modem_press();
			break;
		case '2':
			modem_ability();
			break;
		case '3':
			MDM_abnormal();
			break;
		case '0':
			switch(conf_conn_MDM())
			{
			case SUCC:
				break;
			case FAIL:
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

