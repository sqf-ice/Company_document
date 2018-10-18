/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 综合测试
* file name		: 
* Author 			: chensj
* version			: V1.0
* DATE			: 2013/06/17
* directory 		:
* description		:SD/U盘综合测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"SD/U盘综合"
#define	TESTFILE	"test.file"
#define	TESTBIGFILE	"testbig.file"
#define	BUFFERSIZE	(1024*20) //缓冲区开小些，以免冲破堆栈限制破坏程序
#define MAXWAITTIME 10//非阻塞加载超时时间
#define GETSTATETIMES 30//获取状态次数
#define	DEFAULT_CNT_STR	"1000"	//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)
#define   ABILITYNUM	 20 //性能测试次数

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
void SD_press_mnt(int devicetype)
{
	/*private & local definition*/
	int ret = 0, cnt = 0,bak=0, succ=0, nCnt = 0, state = 0;
	char CntStr[8]=DEFAULT_CNT_STR;
	uint nLen=0;
	
	/*process body*/
	if(auto_flag==1)
	{
		bak = g_CycleTime;
	}
	else
	{
		cls_printf("默认压力测试次数:");
		if ((ret=lib_kbgetinput(CntStr,2, sizeof(CntStr)-1,&nLen, 2, 5,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
			bak = DEFAULT_CNT_VLE;
		else
			bak = atoi(CntStr);
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"请确保已插上%s,按任意键继续...", devicetype==SDDISK?"SD卡":"U盘");

	while(1)
	{
		//保护动作
		NDK_DiskClose(devicetype);
		if(cls_show_msg1(2, "总共%d次开始第%d次%s装载/卸载压力,任意键退出...", bak,cnt+1, devicetype==SDDISK?"SD卡":"U盘")!=0)
			break;
		if(cnt==bak)
			break;
		cnt++;
		
		if((ret=NDK_DiskOpen(devicetype,MAXWAITTIME))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次加载失败", __LINE__, cnt);
			continue;
		}
		nCnt = 0;
		while(1)
		{
			if((ret=NDK_DiskGetState(devicetype,&state))!=NDK_OK)
				;
			if(state==DISKMOUNTSUCC)
				break;
			sleep(1);
			if(++nCnt>=GETSTATETIMES)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次测试未获取到DISKMOUNTSUCC状态", __LINE__,cnt);
				NDK_DiskClose(devicetype);
				return;//检测30次状态,如果30次都没检测到DISKMOUNTSUCC,就报错退出
			}
		}
		if((ret=NDK_DiskClose(devicetype))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次卸载失败", __LINE__, cnt);
			continue;	
		}
		succ++;
		
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"压力测试完成!已执行:%d次,成功:%d次", cnt, succ);
	return;
}

void SD_press_wr(int devicetype)
{
	/*private & local definition*/
	char rootdir[][16] ={{0}}, fname[64] = {0}, writebuf[BUFFERSIZE] = {0}, readbuf[BUFFERSIZE] = {0};
	int ret = 0, fp = 0, loop = 0, cnt = 0, wrlen = 0, i = 0, state = 0, bak = 0, nCnt = 0, nSuccNum = 0;
	time_t oldtime = 0, diff2 = 0;
	char CntStr[8]=DEFAULT_CNT_STR;
	uint nLen=0;
	
	/*process body*/
	if(auto_flag==1)
	{
		bak = g_CycleTime;
	}
	else
	{
		cls_printf("默认压力测试次数:");
		if ((ret=lib_kbgetinput(CntStr,1, sizeof(CntStr)-1,&nLen, 2, 5,INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
			bak = DEFAULT_CNT_VLE;
		else
			bak = atoi(CntStr);
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"请确保已插上%s,按任意键继续...", devicetype==SDDISK?"SD卡":"U盘");
//测试前置:先把存在的测试文件赋值给fname
	NDK_DiskGetRootDirName(devicetype, (char **)rootdir);//经陈蕃铭确认可以直接调用获取根目录不需调用open
	sprintf(fname, "%s/%s", rootdir[0], TESTFILE);//删除测试文件避免由于测试文件存在引起后续读写校验错误，例如前期读写过程中异常断电 20140730 jiangym
BEGIN:
	while(1)
	{
		//保护动作
		NDK_FsClose(fp);
		NDK_FsDel(fname);//删除存在的测试文件
		NDK_DiskClose(devicetype);
		
		if(cls_show_msg1(1, "总共%d次开始第%d次%s读写压力,任意键退出...",bak, i+1, devicetype==SDDISK?"SD卡":"U盘")!=0)
			break;
			
		if(i==bak)//达到设置次数后退出
			break;
		i++;//测试次数计数
		
		//打开设备
		if((ret=NDK_DiskOpen(devicetype,MAXWAITTIME))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NDK_DiskOpen失败(%d)", __LINE__, i, ret);
			continue;
		}
		//获取设备状态
		nCnt = 0;
		while(1)
		{
			if((ret=NDK_DiskGetState(devicetype,&state))!=NDK_OK)
				;
		
			if(state==DISKMOUNTSUCC)
				break;
			sleep(1);
			if(++nCnt>=GETSTATETIMES)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次未获取到DISKMOUNTSUCC状态", __LINE__, i);
				goto BEGIN;
			}
		}
		//获取设备根目录
		if((ret=NDK_DiskGetRootDirName(devicetype, (char **)rootdir))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次获取根目录失败(%d)", __LINE__, i, ret);
			continue;
		}
		sprintf(fname, "%s/%s", rootdir[0], TESTFILE);
		
		if ((fp=NDK_FsOpen(fname, "w"))<0)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次创建测试文件失败(%d)", __LINE__, i, fp);
			continue;
		}
		//写文件
		cnt = rand()%1023+2;//写文件次数 最小2次
		wrlen = BUFFERSIZE-rand()%8;//一次写文件约20K   合计/*测试文件大小大约介于[40K, 20M]*/
		for (loop=0; loop<wrlen; loop++)
			writebuf[loop] = rand()%256;//内容随机
		for(loop=0; loop<cnt; loop++)
		{
			cls_printf("第%d次生成设备文件中(约%dkB),已生成(%dkB)请稍候...", i, cnt*wrlen/1024, loop*wrlen/1024);
			if ((ret=NDK_FsWrite(fp,writebuf,wrlen))!=wrlen)
			{	
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次写测试文件失败(%d)", __LINE__, i, ret);
				goto BEGIN;
			}
		}
		//关闭文件
		if(NDK_FsClose(fp)!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次关闭测试文件失败(%d)", __LINE__, i, ret);
			continue;
		}
		//关闭设备
		if((ret=NDK_DiskClose(devicetype))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NDK_DiskClose失败(%d)", __LINE__, i, ret);
			continue;
		}
		sleep(3);

		//重新打开设备校验文件
		if((ret=NDK_DiskOpen(devicetype,MAXWAITTIME))!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NDK_DiskOpen失败(%d)", __LINE__, i, ret);
			continue;	
		}
		//获取设备状态
		nCnt = 0;
		while(1)
		{
			if((ret=NDK_DiskGetState(devicetype,&state))!=NDK_OK)
				;
			if(state==DISKMOUNTSUCC)
				break;
			sleep(1);
			if(++nCnt>=GETSTATETIMES)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次未获取到DISKMOUNTSUCC状态", __LINE__, i);
				goto BEGIN;
			}
		}
		//打开文件
		if ((fp=NDK_FsOpen(fname, "r"))<0)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次创建测试文件失败(%d)", __LINE__, i, fp);
			continue;
		}
		
		//文件大小是否发生改变
		if(NDK_FsFileSize(fname, (uint *)&ret)!=NDK_OK||ret!=cnt*wrlen)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次校验文件失败(%d)", __LINE__, i, ret);
			continue;
		}
		//读文件
		cls_printf("第%d次校验U盘文件中(约%dkB),请稍候...", i, cnt*wrlen/1024);
		oldtime = time(NULL);
		for (loop=0; loop<cnt; loop++)
		{
			if ((ret=NDK_FsRead(fp,readbuf, wrlen))!=wrlen||memcmp(writebuf, readbuf, wrlen))
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次读文件失败(%d)", __LINE__, i, ret);
				goto BEGIN;
			}
			show_stopwatch(ENABLE, (diff2=time(NULL)-oldtime));
		}
		show_stopwatch(DISABLE, 0);
		//删除文件和关闭设备
		NDK_FsClose(fp);
		NDK_FsDel(fname);
		if((ret=NDK_DiskClose(devicetype))!=NDK_OK)//会调用umount
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次NDK_DiskClose失败(%d)", __LINE__, i, ret);
			continue;
		}
		nSuccNum++;//成功计数
	}

	cls_show_msg_record(FILENAME,FUNCNAME,"%s读写压力测试完成,一共%d次测试已成功%d次", devicetype==SDDISK?"SD卡":"U盘", i, nSuccNum);
	return;
}

static void SD_filesys_bigfile(int devicetype)
{
	/*private & local definition*/
	int fp = 0, writelen = 0, loop = 0, ret = 0, state = 0,cnt = 0,i = 0,j=0,m=0,n=0,nCnt,succ=0;
	char rootdir[][16] = {{0}}, fname[64] = {0}, writebuf[BUFFERSIZE] = {0},filename[4] = {0};
	ST_DISK_INFO pstInfo;

	memset(&pstInfo,0,sizeof(pstInfo));
	/*process body*/
	//保护动作
	NDK_DiskClose(devicetype);	

	cls_show_msg_record(FILENAME,FUNCNAME,"请确保已插上%s,按任意键继续...", devicetype==SDDISK?"SD卡":"U盘");
	
	if((ret=NDK_DiskOpen(devicetype,MAXWAITTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:打开%s失败", __LINE__,devicetype==SDDISK?"SD卡":"U盘");
		return;
	}

	nCnt=0;
	while(1)
	{
		if((ret=NDK_DiskGetState(devicetype,&state))!=NDK_OK)
			;	
		if(state==DISKMOUNTSUCC)
			break;
		sleep(1);
		if(++nCnt>=GETSTATETIMES)
		{
			cls_show_msg1(g_keeptime,"line %d:取%s状态失败", __LINE__,devicetype==SDDISK?"SD卡":"U盘");
			NDK_DiskClose(devicetype);
			return;
		}
	}

	if((ret=NDK_DiskGetRootDirName(devicetype, (char **)rootdir))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:DiskGetRootdirName失败(ret:%d)", __LINE__, ret);
		NDK_DiskClose(devicetype);
		return;
	}

	
BEGIN:
	while(1)
	{
		//保护动作
		NDK_FsClose(fp);
		
		i++;
		if(cls_show_msg1(3, "开始第%d次%s读写压力,任意键退出...", i, devicetype==SDDISK?"SD卡":"U盘")!=0)
			break;
		for(loop=0;loop<3;loop++)
      		sprintf(filename+loop, "%x", rand()%9);
		sprintf(fname, "%s/%s", rootdir[0], filename);

		if ((fp=NDK_FsOpen(fname, "w"))<0)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d文件打开失败",__LINE__);	
			if(++j>=GETSTATETIMES)//连续失败则退出程序，防止死循环
				goto ERR;	
			continue;
		}
		j=0;

		memset(writebuf, rand()/256, sizeof(writebuf));
		
		cnt = rand()%1023+2 , writelen = BUFFERSIZE-rand()%8;
		for (loop=0; loop<cnt; loop++)
		{
			if((ret=NDK_DiskGetInfo(rootdir[0],&pstInfo))!=NDK_OK)
			{
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d获取%s信息失败",__LINE__,devicetype==SDDISK?"SD卡":"U盘");	
				if(++m>=GETSTATETIMES)
					goto ERR;
				goto BEGIN;
			}
			m=0;
			cls_printf("%s总空间:[%u]k\n剩余空间:[%u]k\n此次写入[%d]k\n剩余[%d]k\n",devicetype==SDDISK?"SD卡":"U盘", pstInfo.unTotalSpace,pstInfo.unFreeSpace,cnt*writelen/1024,(cnt*writelen/1024-loop*writelen/1024));
		
			if ((writelen=NDK_FsWrite(fp,writebuf, sizeof(writebuf)))!=sizeof(writebuf))
			{
				cls_printf("line %d:设备已写满,请重启机器!(%d,%d)", __LINE__, loop, writelen);
				NDK_FsClose(fp);
				NDK_DiskClose(devicetype);
				while(1);//死循环,以强制用户关机
				//return;
			}
		}
		if(NDK_FsClose(fp)!=NDK_OK)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d关闭文件失败",__LINE__);
			if(++n>=GETSTATETIMES)
				goto ERR;
			NDK_FsDel(fname);
			continue;	
		}
		n=0;
		
		succ++;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"line %d:电子盘未能写满,测试失败!\n总共写文件%d次成功%d次", __LINE__,i-1,succ);
ERR:
	NDK_FsClose(fp);
	NDK_FsDel(fname);
	NDK_DiskClose(devicetype);
	return;
}

void sd_abnormal(int devicetype)
{
	/*private & local definition*/
	char rootdir[][16] ={{0}}, fname[64] = {0}, writebuf[BUFFERSIZE] = {0}, readbuf[BUFFERSIZE] = {0};
	int ret = 0, fp = 0, state = 0, nCnt = 0;
	
	/*process body*/
	cls_show_msg("请确保已插上%s,按任意键继续...", devicetype==SDDISK?"SD卡":"U盘");
	//测试前置：
	NDK_DiskClose(devicetype);
	
	//case1:验证开设备后，进入休眠再唤醒，应仍能正常地进行读写
	if((ret=NDK_DiskOpen(devicetype,MAXWAITTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:打开%s失败", __LINE__,devicetype==SDDISK?"SD卡":"U盘");
		goto END;
	}
	nCnt=0;
	while(1)
	{
		if((ret=NDK_DiskGetState(devicetype,&state))!=NDK_OK)
			;	
		if(state==DISKMOUNTSUCC)
			break;
		sleep(1);
		if(++nCnt>=GETSTATETIMES)
		{
			cls_show_msg1(g_keeptime,"line %d:取%s状态失败", __LINE__,devicetype==SDDISK?"SD卡":"U盘");
			goto END;
		}
	}
	cls_show_msg("%s,按任意键继续",devicetype==SDDISK?"请等待POS进入休眠后唤醒再":"请等待超过自动休眠时间,U盘不进休眠");//jiangym modified 20140623 U盘修改成OPEN后不进休眠，相应的修改用例

	if((ret=NDK_DiskGetRootDirName(devicetype, (char **)rootdir))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:DiskGetRootdirName失败(ret:%d)", __LINE__, ret);
		goto END;
	}
	sprintf(fname, "%s/%s", rootdir[0], TESTFILE);
	
	NDK_FsDel(fname);
	if((fp=NDK_FsOpen(fname, "w"))<0)
	{
		cls_show_msg1(g_keeptime, "line %d:创建测试文件失败(%d)", __LINE__, fp);
		goto END;
	}
	memset(writebuf, rand()/256, sizeof(writebuf));
	if((ret=NDK_FsWrite(fp,writebuf,BUFFERSIZE))!=BUFFERSIZE)
	{	
		cls_show_msg1(g_keeptime, "line %d:写测试文件失败(%d)", __LINE__, ret);
		goto END;
	}
	NDK_FsClose(fp);
	//关闭设备
	if((ret=NDK_DiskClose(devicetype))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:NDK_DiskClose失败(%d)", __LINE__, ret);
		goto END;
	}
	sleep(3);
	//重新打开设备校验文件
	if((ret=NDK_DiskOpen(devicetype,MAXWAITTIME))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:NDK_DiskOpen失败(%d)", __LINE__, ret);
		goto END;
	}
	//获取设备状态
	nCnt = 0;
	while(1)
	{
		if((ret=NDK_DiskGetState(devicetype,&state))!=NDK_OK)
			;
		if(state==DISKMOUNTSUCC)
			break;
		sleep(1);
		if(++nCnt>=GETSTATETIMES)
		{
			cls_show_msg1(g_keeptime,"line %d:未获取到DISKMOUNTSUCC状态", __LINE__);
			goto END;
		}
	}
	cls_show_msg("%s,按任意键继续",devicetype==SDDISK?"请再次等待POS进入休眠后唤醒再":"请等待超过自动休眠时间,U盘不进休眠");//jiangym modified 20140724
	//打开文件
	if((fp=NDK_FsOpen(fname, "r"))<0)
	{
		cls_show_msg1(g_keeptime, "line %d:打开文件失败(%d)", __LINE__, fp);
		goto END;
	}
	//读文件
	if ((ret=NDK_FsRead(fp, readbuf, BUFFERSIZE))!=BUFFERSIZE||memcmp(writebuf, readbuf, BUFFERSIZE))
	{
		cls_show_msg1(g_keeptime, "line %d:校验文件失败(%d)", __LINE__, ret);
		goto END;
	}
	
	cls_show_msg("%s异常测试通过",devicetype==SDDISK?"SD卡":"U盘");
END:
	NDK_FsClose(fp);
	NDK_FsDel(fname);
	NDK_DiskClose(devicetype);
	return;
}

/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		  
*****************************************************************/
void sd_press(int devicetype)
{
	/*private & local definition*/
	int keyin = '0';
	
	/*process body*/
	while (1)
	{	

		if(g_SequencePressFlag)
		{
			if(( ++keyin=='3') || (cls_show_msg1(2, "即将进行%s连续压力测试,ESC退出",TESTITEM))==ESC)
				return;
		}
		else
		{
			keyin = cls_show_msg("1.装载/卸载压力\n"
								"2.设备读写压力\n"
								"3.写满设备\n"
								);
		}
		
		switch(keyin)
		{
		case '1':
			SD_press_mnt(devicetype);
			break;
		case '2':
			SD_press_wr(devicetype);
			break;
		case '3':
			SD_filesys_bigfile(devicetype);
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

void sd_ability(int devicetype)
{
	/*private & local definition*/
	char rootdir[][16] = {{0}}, fname[64] = {0}, writebuf[BUFFERSIZE] = {0}, readbuf[BUFFERSIZE] = {0};
	int ret = 0, fp = 0, loop = 0, cnt = 50, wrlen = BUFFERSIZE, i = 0,state = 0;
	time_t oldtime = 0, diff1 = 0, diff2 = 0;
	float hanguptime = 0, hangdowntime = 0, writetime = 0, readtime = 0, readtime2 = 0, cmptime = 0;
	uint unSize = 0;

	/*process body*/
	cls_show_msg_record(FILENAME,FUNCNAME,"请确保已插上%s,按任意键继续...", devicetype==SDDISK?"SD卡":"U盘");
	
	while(1)
	{
		if(cls_show_msg1(3, "开始第%d次%s读写,按ESC退出...", i+1, devicetype==SDDISK?"SD卡":"U盘")==ESC)
			break;

		if (i++>=ABILITYNUM)
			break;
		
		//打开SD
		lib_StartStopwatch();
		if((ret=NDK_DiskOpen(devicetype,MAXWAITTIME))!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:DiskOpen失败(ret:%d)", __LINE__, ret);
			return;
		}
		oldtime=time(NULL);
		while(1)
		{
			if((ret=NDK_DiskGetState(devicetype,&state))!=SUCC)
			{
				cls_show_msg1(g_keeptime,"line %d:DiskGetState失败(ret:%d)", __LINE__, ret);
				goto ERR;
			} 
			if(state==DISKMOUNTSUCC)
				break;
			if(time(NULL)-oldtime>MAXWAITTIME)
			{
				cls_show_msg1(g_keeptime,"line %d:获取状态超时state=%d", __LINE__, state);	
				goto ERR;	
			}
			sleep(1);
		}		
		hanguptime += lib_ReadStopwatch();
		
		if((ret=NDK_DiskGetRootDirName(devicetype, (char **)rootdir))<0)
		{
			cls_show_msg1(g_keeptime,"line %d:DiskGetRootdirName失败(ret:%d)", __LINE__, ret);
			goto ERR;	
		}	
		cls_show_msg1(5, "以下测试使用%dB作为每次读写长度!", wrlen);
		sprintf(fname, "%s/%s", rootdir[0], TESTFILE);
		NDK_FsDel(fname);
		if ((fp=NDK_FsOpen(fname, "w"))<0)
		{
			cls_show_msg1(g_keeptime,"line %d:创建文件失败", __LINE__);
			goto ERR;
		}
		//写文件
		cls_printf("第%d次生测试文件中(约%dkB),请稍候...", i, cnt*wrlen/1024);
		for (loop=0; loop<wrlen; loop++)
			writebuf[loop] = rand()%256;
		
		oldtime = time(NULL);
		lib_StartStopwatch();
		for (loop=0; loop<cnt; loop++)
		{
			if ((ret=NDK_FsWrite(fp,writebuf, wrlen))!=wrlen)
			{
				cls_show_msg1(g_keeptime,"line %d:第%d次写文件失败(loop=%d, ret=%d, wrlen=%d)", __LINE__, i, loop, ret, wrlen);
				goto ERR;
			}
			show_stopwatch(ENABLE, (diff1=time(NULL)-oldtime));
		}
		writetime += lib_ReadStopwatch();
		show_stopwatch(DISABLE, 0);	
		
		if(NDK_FsClose(fp)!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:关闭文件失败", __LINE__);
			goto ERR;
		}
		if ((fp=NDK_FsOpen(fname, "r"))<0)
		{
			cls_show_msg1(g_keeptime,"line %d:打开文件失败", __LINE__);
			goto ERR;
		}
		//文件大小是否发生改变
		if ((ret=NDK_FsFileSize(fname,&unSize))!=NDK_OK||unSize!=cnt*wrlen)
		{
			cls_show_msg1(g_keeptime,"line %d:第%d次文件大小不一致(实测:%dB, 预期:%dB)", __LINE__, i, ret, cnt*wrlen);
			goto ERR;
		}
		//读文件
		cls_printf("第%d次校验文件中(约%dkB),请稍候...", i, cnt*wrlen/1024);
		
		oldtime = time(NULL);
		lib_StartStopwatch();
		for (loop=0; loop<cnt; loop++)
		{
			if (((ret=NDK_FsRead(fp,readbuf, wrlen))!=wrlen)||memcmp(writebuf, readbuf, wrlen))
			{
				cls_show_msg1(g_keeptime,"line %d:第%d次读文件失败或校验失败(loop=%d, ret=%d, wrlen=%d)", __LINE__, i, loop, ret, wrlen);
				goto ERR;
			}
			show_stopwatch(ENABLE, (diff2=time(NULL)-oldtime));
		}
		readtime += lib_ReadStopwatch();
		show_stopwatch(DISABLE, 0);
		
		lib_StartStopwatch();
		for(loop=0; loop<cnt; loop++)
		{
			memcmp(writebuf, readbuf, wrlen);
		}
		cmptime += lib_ReadStopwatch();

		readtime2=readtime-cmptime;

		NDK_FsClose(fp);
		NDK_FsDel(fname);
		
		//关闭SD
		lib_StartStopwatch();
		if((ret=NDK_DiskClose(devicetype))!=SUCC)
		{
			cls_show_msg1(g_keeptime,"line %d:DiskClose失败(ret:%d)", __LINE__, ret);
			goto ERR;
		}	
		hangdowntime += lib_ReadStopwatch();
		
	}//循环20次后计算open和close的时间和读写速度,取平均值
	if(i==ABILITYNUM+1)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"open %s时间:%fs/次,close %s时间:%fs/次",devicetype==SDDISK?"SD卡":"U盘", hanguptime/ABILITYNUM,devicetype==SDDISK?"SD卡":"U盘", hangdowntime/ABILITYNUM);
		cls_show_msg_record(FILENAME,FUNCNAME,"写时间:%fs/次,读时间:%fs/次", writetime/ABILITYNUM, readtime2/ABILITYNUM);
		cls_show_msg_record(FILENAME,FUNCNAME,"写数据速度%fkB/s,读数据速度:%fkB/s",((cnt*20*ABILITYNUM)/(writetime)), ((cnt*20*ABILITYNUM)/(readtime2)));
	}
	else
		cls_show_msg_record(FILENAME,FUNCNAME,"用户取消测试");
ERR:
	NDK_FsClose(fp);
	NDK_FsDel(fname);	
	NDK_DiskClose(devicetype);
	return;
}

void systest40(void)
{
	/*private & local definition*/
	int keyin = 0, disktype = SDDISK, ret = 0;
	
	/*process body*/
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		if(conf_disktype()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,SD/U盘配置失败,请检查配置文件",TESTITEM);
			return;
		}
		disktype=get_disktype();
		g_CycleTime = 100;

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行SD/U盘压力测试
		sd_press(disktype);

		//执行SD/U盘性能测试
		sd_ability(disktype);

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		keyin = cls_show_msg("_SD/U盘综合测试_\n"
							"1.压力 2.性能\n"
							"3.异常测试\n"
							"4.SD/U盘配置\n"
							);
		switch(keyin)
		{
		case '1':
			sd_press(disktype);
			break;
		case '2':
			//cls_show_msg("暂不支持性能测试");
			sd_ability(disktype);
			break;
		case '3':
			//cls_show_msg("暂不支持异常测试");
			sd_abnormal(disktype);
			break;
		case '4':
			if((ret=conf_disktype())!=SUCC)
			{
				cls_show_msg1(2, "设备选择失败,请重新选择", ret);
				return;
			}
			disktype=get_disktype();
			break;
		case ESC:
			cls_show_msg("请使用8GB、16GB的卡片重新进行测试!任意键继续...");
			return;
			break;
		default:
			continue;
			break;
		}
	}
}

