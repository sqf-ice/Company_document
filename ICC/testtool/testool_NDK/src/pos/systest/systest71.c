/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 	 U盘/3g深交叉
* file name			: systest69.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: U盘/3g深交叉
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
#define	TESTITEM	"U盘/3g深交叉"
#define TESTFILE   "3gtest.file"
#define	BUFFERSIZE	(1024*5) //缓冲区开小些，以免冲破堆栈限制破坏程序
#define GETSTATETIMES 30//获取状态次数

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
					 	jiangym			20140808	created
*****************************************************************/	
static void UDISK_WLM(void)
{
	/*private & local definition*/
	int ret = -1, i = 0, j = 0, state = 0, nCnt = 0,succ = 0, timeout = SO_TIMEO, fp = 0, wrlen = 0 ,slen = 0, rlen = 0;
	char rootdir[][16] ={{0}}, fname[64] = {0}, writebuf[BUFFERSIZE] = {0}, readbuf[BUFFERSIZE] = {0};
	uint h_tcp = 0;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	Packet sendpacket;	

	/*process body*/		
	cls_show_msg_record(FILENAME,FUNCNAME,"请确保已安装U盘,任意键继续");	
	//测试前置:先把存在的测试文件赋值给fname
	//NDK_DiskGetRootDirName(UDISK, (char**)rootdir);//经陈蕃铭确认可以直接调用获取根目录不需调用open
	//sprintf(fname, "%s/%s", rootdir[0], TESTFILE);//删除测试文件避免由于测试文件存在引起后续读写校验错误，例如前期读写过程中异常断电 

	init_snd_packet(&sendpacket, buf);
	set_snd_packet(&sendpacket);
	
	while(1)
	{
		//保护动作
ERR:
		TransDown(h_tcp);
		NetDown();
		NDK_FsClose(fp);
		NDK_FsDel(fname);
		NDK_DiskClose(UDISK);
		
		if(cls_show_msg1(2,"正在进行第%d次%s测试(已成功%d次),按ESC退出", i+1, TESTITEM, succ)==ESC)
			break;
		if(update_snd_packet(&sendpacket)!=SUCC)
			break;
		i++;
		
		sleep(5);//每次挂断之后重新连接要等待5秒,减轻绎芯片的压力
		
		if(NetUp()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NetUp失败", __LINE__, i);
			continue;
		}
		
		if((ret=NDK_DiskOpen(UDISK,timeout))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次打开U盘失败(%d)",  __LINE__, i, ret);
			continue;
		}
		//获取设备状态
		nCnt = 0;
		while(1)
		{
			if((ret=NDK_DiskGetState(UDISK,&state))!=NDK_OK)
				;
			if(state==DISKMOUNTSUCC)
				break;
			sleep(1);
			if(++nCnt>=GETSTATETIMES)
			{
				cls_show_msg1(g_keeptime,"line %d:第%d次未获取到DISKMOUNTSUCC状态", __LINE__, i);
				goto ERR;
			}
		}
		
		if(TransUp(&h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次TransUp失败", __LINE__, i);
			continue;
		}
		
		//获取设备根目录
		if((ret=NDK_DiskGetRootDirName(UDISK, (char **)rootdir))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次获取根目录失败(%d)", __LINE__, i, ret);
			continue;
		}
		sprintf(fname, "%s/%s", rootdir[0], TESTFILE);
		
		//打开文件
		if((fp=NDK_FsOpen(fname, "w"))<0)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次创建测试文件失败(%d)", __LINE__, i, fp);
			continue;
		}
		
		//无线发送数据
		if((slen=sock_send(h_tcp, sendpacket.header, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(实际%d, 预期%d)", __LINE__, i, slen, sendpacket.len);
			continue;
		}
		
		//往文件写数据
		wrlen = 1024;//
		for (j=0; j<wrlen; j++)
			writebuf[j] = rand()%256;//内容随机
		cls_printf("生成设备文件中(约%dB),请稍候...",wrlen);
		if ((ret=NDK_FsWrite(fp,writebuf,wrlen))!=wrlen)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次写测试文件失败(%d)", __LINE__, i, ret);
			continue;
		}
		//关闭文件
		if(NDK_FsClose(fp)!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次关闭测试文件失败(%d)", __LINE__, i, ret);
			continue;
		}
		//关闭设备
		if((ret=NDK_DiskClose(UDISK))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NDK_DiskClose失败(%d)", __LINE__, i, ret);
			continue;
		}
		
		//无线接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if((rlen=sock_recv(h_tcp, rbuf, sendpacket.len, timeout))!=sendpacket.len)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次接收失败(实际%d, 预期%d)", __LINE__, i, rlen, sendpacket.len);
			continue;
		}
		//比较收发
		if (MemCmp(sendpacket.header, rbuf, sendpacket.len))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次校验失败", __LINE__, i);
			continue;
		}
		
		//重新打开设备校验文件
		cls_printf("重新打开设备校验文件,请稍候...");
		if((ret=NDK_DiskOpen(UDISK,timeout))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NDK_DiskOpen失败(%d)", __LINE__, i, ret);
			continue;
		}
		//获取设备状态
		nCnt = 0;
		while(1)
		{
			if((ret=NDK_DiskGetState(UDISK,&state))!=NDK_OK)
				;
			if(state==DISKMOUNTSUCC)
				break;
			sleep(1);
			if(++nCnt>=GETSTATETIMES)
			{
				cls_show_msg1(g_keeptime,"line %d:第%d次未获取到DISKMOUNTSUCC状态", __LINE__, i);
				goto ERR;
			}
		}
		
		//打开文件
		if ((fp=NDK_FsOpen(fname, "r"))<0)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次创建测试文件失败(%d)", __LINE__, i, fp);
			continue;
		}
		//读文件
		if ((ret=NDK_FsRead(fp,readbuf, wrlen))!=wrlen||memcmp(writebuf, readbuf, wrlen))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次读文件失败(%d)", __LINE__, i, ret);
			continue;
		}		
			
		//无线挂断
		if (TransDown(h_tcp)!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:第%d次TransDown失败", __LINE__, i);
			continue;
		}
		//删除文件和关闭设备
		NDK_FsClose(fp);
		NDK_FsDel(fname);
		
		//无线netdown
		if (NetDown()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:NetDown失败", __LINE__);
			continue;
		}	
		//U盘关闭
		if((ret=NDK_DiskClose(UDISK))!=NDK_OK)//会调用umount
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次NDK_DiskClose失败(%d)", __LINE__, i, ret);
			continue;
		}
		succ++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"%s测试完成:总次数:%d 成功:%d次", TESTITEM, i, succ);
	return;
}

void systest71(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		//无线配置
		if(conf_conn_WLM(TRUE)!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,WLM配置失败,请检查配置文件",TESTITEM);
			return;
		}
		g_CycleTime = 10;
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行U盘/3g交叉测试
		UDISK_WLM();
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{
		keyin = cls_show_msg("1.U盘/3g交叉测试\n"
							 "0.无线配置");
		switch(keyin)
		{
		case '1':
			UDISK_WLM();
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

