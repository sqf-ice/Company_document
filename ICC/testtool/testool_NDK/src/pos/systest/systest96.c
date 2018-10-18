/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: 96.c
* Author 			: linying
* version			: V1.0
* DATE			: 20180828
* directory 		: 
* description		: 高端文件系统综合测试
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  20130117 chenfm
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"K21_fs性能,压力"
#define	FILETESTG8 "/appfs/FTG8"
#define	BUFFERSIZE	(1024)
#define	BUFLEN		(1024)

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
*					       linying			20180828   	created
*****************************************************************/
//设计说明:关注在一定强度下是否仍运行正常
static void filesys_press_wr(void)
{
	/*private & local definition*/
	char writebuf2[BUFLEN] = {0}, readbuf2[BUFLEN] = {0};
	int fp = 0, writelen = 0, readlen = 0,succ = 0;
	uint unLen = 0;
#define	DEFAULT_CNT_STR	"3000"	//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(1000)
	int ret = -1, cnt = DEFAULT_CNT_VLE, bak = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	
	/*process body*/
	//创建,读写,关闭,删除压力
	NDK_SP_FsDel(FILETESTG8);//保护动作

	cls_printf("默认测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);
	
	memset(writebuf2, rand()/256/*0x44*/, sizeof(writebuf2));
	
	while(cnt)
	{
		if (ESC==cls_show_msg1(2, "单文件读写压力中...\n还剩%d次,ESC中断测试", cnt))
			break;
		cnt--;
			
		if ((fp=NDK_SP_FsOpen(FILETESTG8, "w"))<0)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:%s第%d次:创建文件失败(%d)", __LINE__,TESTITEM, bak-cnt, fp);
			continue;//return; 20171106修改:执行人员要求统计测试成功率
			
		}
		if ((writelen= NDK_SP_FsWrite(fp,writebuf2, BUFLEN))!=BUFLEN)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:写文件失败(%d)", __LINE__, bak-cnt,writelen);
			NDK_SP_FsClose(fp);
			NDK_SP_FsDel(FILETESTG8);
			continue;//return;
		}
		if ((ret=NDK_SP_FsClose(fp))!=NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:关闭文件失败(%d)", __LINE__, bak-cnt, ret);
			NDK_SP_FsDel(FILETESTG8);
			continue;//return;
		}
		if ((fp=NDK_SP_FsOpen(FILETESTG8, "r"))<0)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:打开文件失败(%d)", __LINE__, bak-cnt, fp);
			NDK_SP_FsDel(FILETESTG8);
			continue;//return;
		}
		memset(readbuf2, 0, sizeof(readbuf2));
		if ((readlen=NDK_SP_FsRead(fp,readbuf2, BUFLEN))!=BUFLEN)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:读文件失败(%d)", __LINE__, bak-cnt, readlen);
			NDK_SP_FsClose(fp);
			NDK_SP_FsDel(FILETESTG8);
			continue;//return;
		}
		if (memcmp(writebuf2, readbuf2, sizeof(writebuf2)))
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:校验错", __LINE__, bak-cnt);
			NDK_SP_FsClose(fp);
			NDK_SP_FsDel(FILETESTG8);
			continue;//return;
		}		
		if ((ret=NDK_SP_FsClose(fp))!=NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:关闭文件失败(%d)", __LINE__, bak-cnt, ret);
			NDK_SP_FsDel(FILETESTG8);
			continue;//return;
		}
		if ((ret=NDK_SP_FsDel(FILETESTG8))!=NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:删除文件失败(%d)", __LINE__, bak-cnt, ret);
			NDK_SP_FsDel(FILETESTG8);
			continue;//return;
		}
		succ++;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"单文件读写压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

static void filesys_press_bigfile(void)
{
	/*private & local definition*/
	int fp = 0, writelen = 0, readlen = 0, cnt = 0, cnt2 = 0, loop = 0,  filelen1 = 0;
	time_t oldtime = 0;
	float fTimeElapsed = 0;
	char writebuf[BUFFERSIZE] = {0}, readbuf[BUFFERSIZE] = {0};
	ulong discspace1 = 0, discspace2 = 0;
	char str[32] = {0};
	uint filelen = 0;

	//定义单个扇区大小，单位K
#define BLOCK_SIZE 4

	//定义每个产品的单个文件最大大小(安卓平台未知)
#define MAX_FLIE_SIZE 240


	/*process body*/
	//  step	(1) 高端产品不支持取文件空间
	cnt = MAX_FLIE_SIZE;
	filelen1 = cnt*sizeof(writebuf);//先算出超大文件的预计长度
/*
#if !K21_ENABLE||defined SP610||defined ME20
	sync();sleep(3);
	NDK_FsGetDiskSpace(1,&discspace1);
	cnt = discspace1/sizeof(writebuf)+1;//多加1的目的是让下面一定会写满电子盘,以达到边界测试的效果
#elif defined ME15C	//ME15C ME20 最大只支持67KB 
	cnt = 66; 
#elif defined ME11||defined ME15 //ME11 最大支持128KB
	cnt =118; 
#elif defined ME30||defined ME30MH||defined ME50||defined ME50N//||defined SP610	//ME30 最大支持512KB
	cnt = 511; 
#else
	cnt = 240;	//IM81,ME31最大支持256KB，不支持NDK_FsGetDiskSpace,最大可写入有效数据243K+597字节故此处改成240次,每次1K
#endif
*/
	cls_printf("超大文件测试中...正在写入约%dK的超大文件,请耐心等待", cnt);
	NDK_SP_FsDel(FILETESTG8);
	
	// step	(2) 以"w"方式打开一特定文件
	if ((fp= NDK_SP_FsOpen(FILETESTG8, "w"))<0)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:创建超大文件失败", __LINE__);
		goto ERR;
	}

	// step	(3) 写入240KB数据(随机数)至该文件中
	// step	(4) 循环至步骤2,创建超大文件
	memset(writebuf, rand()/256/*0x24*/, sizeof(writebuf));
	oldtime = time(NULL);
	lib_StartStopwatch();
	for (loop=0; loop<cnt; loop++)
	{
		if ((writelen=NDK_SP_FsWrite(fp,writebuf, sizeof(writebuf)))!=sizeof(writebuf))
		{
			if((loop+1)!=cnt)
			{
				cls_show_msg_record(FILENAME,FUNCNAME, "line %d:预期应该能写约%dK数据，实际写第%dK时出错(%d)", __LINE__, cnt, loop+1, writelen);
				goto ERR;
			}
			else
			{
				cls_show_msg1(2, "电子盘已写满(%d)", writelen);//写满时返回-1,同时实际上会写部分数据进去(写满)
				filelen1 = sizeof(writebuf)*loop;//修正超大文件的预计长度
				cnt = loop;//修正cnt,此时cnt=实际的XXK数据
			}
		}
		show_stopwatch(ENABLE, time(NULL)-oldtime);//zhangnw20110715add
	}
	fTimeElapsed = lib_ReadStopwatch();
	show_stopwatch(DISABLE, 0);

	// step	(5) 关闭该文件
	if (NDK_SP_FsClose(fp)!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME, "line %d:关闭文件出错", __LINE__);
		goto ERR;
	}

	// step	(6) 以"r"方式打开该文件
	if ((fp= NDK_SP_FsOpen(FILETESTG8, "r"))<0)
	{
		cls_show_msg_record(FILENAME,FUNCNAME, "line %d:打开超大文件失败", __LINE__);
		goto ERR;
	}

	// step	(7) 取得文件长度
	NDK_SP_FsFileSize(FILETESTG8,&filelen);
	if (filelen<filelen1)
	{
		cls_show_msg_record(FILENAME,FUNCNAME, "line %d:文件长度异常(%d,%d)", __LINE__, filelen, filelen1);
		goto ERR;
	}

	// step	(8) 读出文件数据并进行校验
	for (loop=0; loop<cnt; loop++)
	{
		memset(readbuf, 0, sizeof(readbuf));
		if ((readlen=NDK_SP_FsRead(fp,readbuf, sizeof(readbuf)))!=sizeof(readbuf))
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次读文件出错(%d)", __LINE__, loop+1, readlen);
			goto ERR;
		}
		if (memcmp(writebuf, readbuf, sizeof(writebuf)))
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次校验错", __LINE__, loop+1);
			goto ERR;
		}
	}
	if(filelen>filelen1)//if(writelen!=sizeof(writebuf))//对尾部残留的不足sizeof(writebuf)个的数据进行校验
	{
		memset(readbuf, 0, sizeof(readbuf));
		if ((readlen=NDK_SP_FsRead(fp,readbuf, filelen-filelen1))!=(filelen-filelen1))
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:读文件出错(%d,%d)", __LINE__, readlen, filelen-filelen1);
			goto ERR;
		}
		if (memcmp(writebuf, readbuf, (filelen-filelen1)))
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:校验错", __LINE__);
			goto ERR;
		}
	}

	// step	(9) 关闭并删除该文件
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETESTG8);

	// step	(10)高端产品不支持取文件剩余空间
	ftos(fTimeElapsed,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "超大文件测试通过(创建大小为%dKB文件耗时%ss)", (cnt*sizeof(writebuf)+writelen)/1024, str);
	return;
ERR:
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETESTG8);
	return;
}

static void filesys_press_multifiles(void)
{
	/*private & local definition*/
	int fp[5] ={-1, -1, -1, -1, -1}, ret = -1;
	int countnum = 0, i = 0, j = 0, cnt = 0/*, filelen[5] = {0}*/;
	int  writelen = 0, readlen = 0;
	char writebuf[BUFLEN] = {0}, readbuf[BUFLEN] = {0}, tempbuf[BUFLEN] = {0};
	char filename[5][20] ={"/appfs/FTG8A", "/appfs/FTG8B", "/appfs/FTG8C", "/appfs/FTG8D", "/appfs/FTG8E"};
	ulong discspace1 = 0;

	/*process body*/
	cls_printf("多文件互操作测试中...");
	cnt = 60;//为提高效率将cnt改小成60
	
	// step (1)
	for (countnum = 0; countnum<5; countnum++)
		NDK_SP_FsDel(filename[countnum]);
	
	// step (2) 以"w"方式连续打开5个文件:FT21A,FT21B,FT21C,FT21D,FT21E,分别写入5K字节的数据(五个文件所写的数据内容各不相同)
	for (countnum = 0; countnum<5; countnum++)
	{
		if ((fp[countnum] =  NDK_SP_FsOpen(filename[countnum], "w"))<0) // 创建文件
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:创建文件(%d)失败", __LINE__, countnum);
			for (i = countnum; i >= 0; i--)
			{
				NDK_SP_FsClose(fp[i]);
				NDK_SP_FsDel(filename[i]);
			}
			return;
		}

		memset(writebuf, countnum, sizeof(writebuf));
		for (j = 0; j < cnt; j++)
		{
			if ((writelen =  NDK_SP_FsWrite(fp[countnum],writebuf, BUFLEN))!=BUFLEN) // 写入数据长度出错
			{
				cls_show_msg_record(FILENAME,FUNCNAME, "line %d:写第%d个文件的第%dK数据出错(%d)", __LINE__, countnum+1, j+1, writelen);
				for (i = countnum; i >= 0; i--)
				{
					NDK_SP_FsClose(fp[i]);
					NDK_SP_FsDel(filename[i]);
				}
				return;
			}
		}
	}
	//针对高端产品出错的bug增加了检测文件是否存在20171113(全部平台通用)
	for (countnum = 0; countnum<5; countnum++)
	{
		if((ret=NDK_SP_FsExist(filename[countnum])) != NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:检测第%d个文件是否存在出错(%d)", __LINE__, countnum+1, ret);
			for (i = 0; i<5; i++)
			{
				NDK_SP_FsClose(fp[i]);
				NDK_SP_FsDel(filename[i]);
			}
			return;
		}
	}

	// step (3) 关闭这五个文件
	for (countnum = 0; countnum<5; countnum++)
		NDK_SP_FsClose(fp[countnum]);

	// step (4) 以"r"方式打开这五个文件并分别进行数据校验
	for (countnum = 0; countnum<5; countnum++)
	{
		if ((fp[countnum] = NDK_SP_FsOpen(filename[countnum], "r"))<0) // 打开文件失败
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:打开文件(%d)失败", __LINE__, countnum);
			for (i = countnum; i >= 0; i--)
			{
				NDK_SP_FsClose(fp[i]);
				NDK_SP_FsDel(filename[i]);
			}
			return;
		}

		for (j = 0; j < cnt; j++)
		{
			memset(readbuf, 0, sizeof(readbuf));
			if ((readlen =  NDK_SP_FsRead(fp[countnum],readbuf, BUFLEN))!=BUFLEN) // 读出数据长度出错 
			{
				cls_show_msg_record(FILENAME,FUNCNAME, "line %d:读第%d个文件的第%dK数据出错(%d)", __LINE__, countnum+1, j+1, readlen);
				for (i = countnum; i >= 0; i--)
				{
					NDK_SP_FsClose(fp[i]);
					NDK_SP_FsDel(filename[i]);
				}
				return;
			}

			memset(tempbuf, countnum, sizeof(tempbuf));
			if (memcmp(tempbuf, readbuf, readlen)) // 校验内容
			{
				cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d个文件校验出错", __LINE__, countnum+1);
				for (i = 0; i<5; i--)
				{
					NDK_SP_FsClose(fp[i]);
					NDK_SP_FsDel(filename[i]);
				}
				return;
			}
		}
		
		//删除某个文件后,读取剩余文件进行数据比较,数据应该不会被改变
		if(countnum%2==1)//将FTG8B和FTG8D文件删除后读取剩余文件进行比较
		{
			NDK_SP_FsClose(fp[countnum]);
			NDK_SP_FsDel(filename[countnum]);
		}
	}

	for (i = 0; i<5; i++)
	{
		NDK_SP_FsClose(fp[i]);
		NDK_SP_FsDel(filename[i]);
	}

	cls_show_msg_record(FILENAME,FUNCNAME, "多文件互操作测试通过"); // 测试通过
	return;
}

static void filesys_press(void)
{
	/*private & local definition*/
	int keyin = '0';

	/*process body*/
	while (1)
	{
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			 //对应下面的菜单数+1，菜单增加此处也要对应的增加
			if((++keyin == '4')||(cls_show_msg1(2, "即将进行%s连续压力测试,ESC退出",TESTITEM)==ESC))
				return ;
		}
		else
		{
			keyin = cls_show_msg("_文件系统压力_\n"
								"1.单个文件读写\n" 
								"2.单个超大文件\n"
								"3.多文件互操作\n");
		}
		
		switch (keyin)
		{
		case '1':
			filesys_press_wr();
			break;
		case '2':
			filesys_press_bigfile();
			break;			
		case '3':
			filesys_press_multifiles();
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

//设计说明:不关注功能、压力(比如在一定强度下是否成功)等,仅关注性能
//一般来说,也不用关注ability返回,性能值在函数运行过程中给出
static void filesys_ability(void)
{
	/*private & local definition*/
	char writebuf2[BUFLEN] = {0}, readbuf2[BUFLEN] = {0};
	int ret = -1,fp = 0, writelen = 0, readlen = 0, j = 0;
	char str[32] = {0};
	int times = 255;  
	
	/*process body*/
	
	//读写1M数据耗时测试
	NDK_SP_FsDel(FILETESTG8);//保护动作
	memset(writebuf2, rand()/256/*0x44*/, sizeof(writebuf2));
	fp =  NDK_SP_FsOpen(FILETESTG8, "w");

	cls_printf("写文件中...  ");
	lib_StartStopwatch();
	//for(j=0; j<1024; j++)//循环写1K数据1K*10240次
	for(j=0; j<times; j++)
	{
		if ((writelen= NDK_SP_FsWrite(fp,writebuf2, BUFLEN))!=BUFLEN)//上面的压力有所不同,所以蛮判断一下返回
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:写文件失败(%d,%d)", __LINE__,j, writelen);
			NDK_SP_FsClose(fp);
			NDK_SP_FsDel(FILETESTG8);
			return;
		}
	}
	memset(str,0,sizeof(str));
	ftos(times/lib_ReadStopwatch(),str);
	cls_show_msg_record(FILENAME,FUNCNAME, "写文件速度为%skb/s", str);
	if ((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:关闭文件失败(%d)", __LINE__, ret);
		NDK_SP_FsDel(FILETESTG8);
		return;
	}
	if ((fp=NDK_SP_FsOpen(FILETESTG8, "r"))<0)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:打开文件失败(%d)", __LINE__, fp);
		NDK_SP_FsDel(FILETESTG8);
		return;
	}

	cls_printf("读文件中...  ");
	memset(readbuf2, 0, sizeof(readbuf2));
	lib_StartStopwatch();
	for(j=0; j<times; j++)//读255K的数据
	{
		if ((readlen= NDK_SP_FsRead(fp,readbuf2, BUFLEN))!=BUFLEN)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:读文件失败(%d)", __LINE__, readlen);
			NDK_SP_FsClose(fp);
			NDK_SP_FsDel(FILETESTG8);
			return;
		}
	}
	memset(str,0,sizeof(str));
	ftos(times/lib_ReadStopwatch(),str);
	cls_show_msg_record(FILENAME,FUNCNAME, "读文件速度为%skb/s", str);
	if ((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:关闭文件失败(%d)", __LINE__, ret);
		NDK_SP_FsDel(FILETESTG8);
		return;
	}
	if ((ret=NDK_SP_FsDel(FILETESTG8))!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:删除文件失败(%d)", __LINE__, ret);
		NDK_SP_FsDel(FILETESTG8);
		return;
	}
	
	return;
}

void systest96(void)
{
	/*private & local definition*/
	int keyin = 0;
	ulong pulSpace = 0;
	
	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);

	if(auto_flag==1)//autotest_flag
	{
		//自动化测试内容
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s连续测试开始",TESTITEM);
		
		//执行压力测试
		filesys_press();

		//执行性能测试
		filesys_ability();
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s连续测试结束",TESTITEM);
		return;
	}
	else
	{
	while(1)
       {
              keyin = cls_show_msg("1.压力测试\n"
							"2.性能测试\n");
              					//"3.异常测试\n"高端产品不支持取剩余空间，不做异常测试
							//"4.清理环境\n");
		switch(keyin) 
		{
		case '1':
			cls_show_msg1(1, "文件系统压力测试中...\n");// 1.一定强度的压力测试;
			filesys_press();
			break;
		case '2':
			cls_show_msg1(1, "文件系统性能测试中...\n");// 2.性能测试
			filesys_ability();
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
	return;
}

