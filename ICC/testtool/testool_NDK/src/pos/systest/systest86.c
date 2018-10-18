/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: 
* Author 			: zhengry
* version			: V1.0
* DATE			: 2016/05/30
* directory 		: 
* description		: 摄像头/WLM深交叉
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  2016/05/30  zhengry
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"摄像头/WLM深交叉"
#define MAXTIME		60
/*----------global variables declaration("extern")--------------*/
static int nConfInit[2] = {0};	 /*是否配置过了参数:0未配置 1已配置*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
void camera_wlm(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, succ = 0, timeout = SO_TIMEO, slen = 0, rlen = 0;
	uint h_tcp = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;

	/*测试前置*/
	if(nConfInit[0] == 0)
	{
		cls_show_msg1(g_keeptime, "Wlm参数未配置");
		return;
	}
	if(nConfInit[1] == 0)
	{
		cls_show_msg1(g_keeptime, "Scan参数未配置");	
		return;
	}
	
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
		if((ret=NDK_ScanInit())!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
			goto ERR;
		}
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
			goto ERR;
		}
	    NDK_ScanSet(CAMERA_SETTYPE_IMAGE, g_ShowImage);
		NDK_ScanSet(CAMERA_SETTYPE_SATURATION, g_Saturation);
		NDK_ScanSet(CAMERA_SETTYPE_SHARPNESS, g_Sharpness);
		NDK_ScanSet(CAMERA_SETTYPE_GAMMA, g_Contrast);

		//发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, sendpacket.len);
			goto ERR;
		}
		if((ret=lib_doscan(g_ScanType, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
			goto ERR;
		}
		
		//接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, sendpacket.len);
			goto ERR;
		}
		if((ret=lib_doscan(g_ScanType, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
			goto ERR;
		}

		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:校验失败", __LINE__);
			goto ERR;
		}
		if((ret=lib_doscan(g_ScanType, MAXTIME))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
			goto ERR;
		}
			
		//挂断/
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:TransDown失败", __LINE__);
			goto ERR;
		}
		if(g_ScanType==1)
		{
			if((ret=NDK_ScanClose())!=NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:扫描失败(%d)", __LINE__, ret);
				goto ERR;
			}
		}
		if (NetDown()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:NetDown失败", __LINE__);
			continue;
		}

		succ++;
	ERR:
		if(g_ScanType==1)
			NDK_ScanClose();
	}
	
	cls_show_msg("%s测试完成\n总次数:%d\n成功:%d次", TESTITEM, i, succ);
	return;
}

void systest86(void)
{
	int nKeyin=0;

	cls_show_msg1(2, "%s测试中...", TESTITEM);
	while(1)
	{
		nKeyin=cls_show_msg("1.交叉测试\n"
							"9.无线配置\n"
							"0.摄像头配置\n");
		switch(nKeyin)
		{
			case '1':
				camera_wlm();
				break;
			case '9':
				switch(conf_conn_WLM(TRUE))
				{
				case SUCC:
					cls_show_msg1(2, "网络配置成功!");
					nConfInit[0]=1;
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
		    case '0':
				conf_camera();
				nConfInit[1]=1;
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

