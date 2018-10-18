/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 音频/无线交叉测试
* file name		: systest91.c
* Author 			: linwl
* version			: V1.0
* DATE			: 2017/03/21
* directory 		: 
* description		: 音频/无线交叉测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  2017/03/21 linwl
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"音频/无线交叉"

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
*			  		  linwl 		   	2017/03/17		created
*****************************************************************/
//编写用例注意：交叉测试要做到尽可能深度交叉!另外,也要有一定的强度。
static void wireless_voice(void)
{
	/*private & local definition*/
	int i = 0, succ = 0, timeout = SO_TIMEO, slen = 0, rlen = 0, ret = -1;
	uint h_tcp = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;
	
	/*process body*/	
	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	while(1)
	{
		//保护动作
		TransDown(h_tcp);
		NetDown();
		
		if(cls_show_msg1(2,"正在进行第%d次%s测试(已成功%d次),按ESC退出", i+1, TESTITEM, succ)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		
		sleep(10);//每次挂断之后重新连接要等待10秒,减轻绎芯片的压力
		
		if(NetUp()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:NetUp失败", __LINE__);
			continue;
		}
		if((ret=NDK_VoiceLoadFile(4, NULL)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:音频播放失败失败%d", __LINE__, ret);
			continue;
		}
		NDK_SysDelay(20);//如果没有播放完语音,就快速按键开始下一段语音播放会失败返回-320,所以增加延时
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
			continue;
		}
		if((ret=NDK_VoiceLoadFile(0, NULL)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:音频播放失败失败%d", __LINE__, ret);
			continue;
		}
		NDK_SysDelay(20);//如果没有播放完语音,就快速按键开始下一段语音播放会失败返回-320,所以增加延时
		
		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len);
			continue;
		}
		if((ret=NDK_VoiceLoadFile(2, NULL)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:音频播放失败失败%d", __LINE__, ret);
			continue;
		}
		NDK_SysDelay(20);//如果没有播放完语音,就快速按键开始下一段语音播放会失败返回-320,所以增加延时
		
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, sendpacket.len);
			continue;
		}
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:校验失败", __LINE__);
			continue;
		}
		if((ret=NDK_VoiceLoadFile(4, NULL)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:音频播放失败失败%d", __LINE__, ret);
			continue;
		}
		NDK_SysDelay(20);//如果没有播放完语音,就快速按键开始下一段语音播放会失败返回-320,所以增加延时
			
		//挂断/
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:TransDown失败", __LINE__);
			continue;
		}
		if((ret=NDK_VoiceLoadFile(0, NULL)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:音频播放失败失败%d", __LINE__, ret);
			continue;
		}
		NDK_SysDelay(20);//如果没有播放完语音,就快速按键开始下一段语音播放会失败返回-320,所以增加延时
		
		if (NetDown()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:NetDown失败", __LINE__);
			continue;
		}
		if((ret=NDK_VoiceLoadFile(2, NULL)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:音频播放失败失败%d", __LINE__, ret);
			continue;
		}
		NDK_SysDelay(20);//如果没有播放完语音,就快速按键开始下一段语音播放会失败返回-320,所以增加延时
		succ++;
	}
	
	cls_show_msg("%s测试完成\n总次数:%d\n成功:%d次", TESTITEM, i, succ);
	return;
}

void systest91(void)
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
							"0.无线配置");
		switch(keyin)
		{
		case '1':
			wireless_voice();
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

