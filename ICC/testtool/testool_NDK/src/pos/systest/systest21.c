/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest21.c
* Author 			: chenjs
* version			: 
* DATE			: 20130227
* directory 	: 
* description	: 无线/MDM交叉测试
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
#define	TESTITEM		"无线/MDM交叉"

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

static void wlm_mdm_press(LinkType *lnktype)
{
	/*private & local definition*/
	int i = 0, succ1 = 0, succ2 = 0, dialtype = 0,cnt = 0;
	Packet sendpacket;
	char buf[PACKMAXLEN] = {0};
	//float call_time = 0.0 , hangdowntime = 0.0;
	float dial = 0.0, hang = 0.0;
	
	/*process body*/
	if(cls_show_msg1(5, "选择拨号压力类型:\n[确认]普通型\n[其它]增强型")==ENTER)
		dialtype = 1;//正常拨号
	else
		dialtype = 0;//异常拨号	
	
	//初始化发包供无线与MDM共用
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	mdm_clrportbuf_all();

	if(sendpacket.forever==FALSE)  //因下面循环两次调用到update_snd_packet造成实际次数减半
	{
		cnt = sendpacket.lifecycle;
		sendpacket.lifecycle = 2*cnt;
	}
	//数据通讯
	while(1)
	{
		if(cls_show_msg1(3, "第%d次%s测试中,ESC退出...", i+1, TESTITEM)==ESC)
			break;

		//以MDM类型的包格式来作为两种通讯方式下的发包
		setlinktype(lnktype[0]);
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		setlinktype(lnktype[1]);

		//无线通讯
		//ShowSignalQuality(-1, -1);//进入无线(AT命令)测试前,先关信号显示
		//cls_show_msg1(3, "无线短链接通讯中...当前信号量:%d", getsqwirelessmodem());
		//ShowSignalQuality(0, 0);//退出无线(AT命令)测试后,要恢复信号刷新

		if(lnktype[0]==ASYN)//linwl20130510由于异步PPP和无线PPP的用户名和密码不一样，所以再测试前需要将密码改成相应的密码
		{
			PppSetUsrCfg();//需要该函数重新告诉底层PPP参数结构
			strcpy(PPPUSR,DEFAULT_PPP_USR);
			strcpy(PPPPWD,DEFAULT_PPP_PWD);
		}
		if(wireless_dial_comm(&sendpacket, RESET_NO)==SUCC)
			succ1++;
		
		//有线MDM通讯
		if(lnktype[0]==ASYN)//linwl20130510由于异步PPP和无线PPP的用户名和密码不一样，所以再测试前需要将密码改成相应的密码
			PppSetUsrCfg();
		setlinktype(lnktype[0]);//linktype = SYNC;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;		
		if(mdm_dial_comm(MDMDialStr, &sendpacket, FALSE, &dial, &hang)==SUCC)
		{
			succ2++;
		}

		if(dialtype==1)//正常拨号时才要等待
			cls_show_msg1(rand()%15+20, "模块休息中,按任意键不休息继续压力...");
	}

	cls_show_msg_record(FILENAME ,FUNCNAME ,"交叉测试%d次.无线成功%d次,MDM成功%d次.", i, succ1, succ2);

	return;
}

void systest21(void)
{
	/*private & local definition*/
	int keyin = 0;
	LinkType type[2] = {NONE, NONE};
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_conn_MDM()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,MDM配置失败,请检查配置文件",TESTITEM);
			return;
		}
		type[0] = getlinktype();
		
		if(conf_conn_WLM(TRUE)!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,WLM配置失败,请检查配置文件",TESTITEM);
			return;
		}
		type[1] = getlinktype();
		g_CycleTime = 10;
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行交叉压力测试
		wlm_mdm_press(type);
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		keyin = cls_show_msg("1.交叉压力\n"
							"0.MDM配置\n"
							"9.无线配置");
		switch(keyin)
		{
		case '1':
			wlm_mdm_press(type);
			break;
		case '0':			
			switch(conf_conn_MDM())
			{
			case SUCC:
				type[0] = getlinktype();
				cls_show_msg1(2, "MDM配置成功!");
				break;
			case FAIL:
				cls_show_msg("line %d:配置modem失败!!!\n任意键退出配置...", __LINE__);	
				break;
			case NDK_ERR_QUIT://用户放弃
			default:
				continue;
				break;
			}
			break;
		case '9':
			switch(conf_conn_WLM(TRUE))
			{
			case SUCC:
				type[1] = getlinktype();
				cls_show_msg1(2, "网络配置成功!");
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

