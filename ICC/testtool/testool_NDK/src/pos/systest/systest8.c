/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 其他
* file name		: 8.c
* Author 			: chenfm
* version			: V1.0
* DATE			: 20130117
* directory 		: 
* description		: 文件系统综合测试
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
#define	TESTITEM	"fsys性能,压力"
#if !K21_ENABLE
#define	FILETESTG8 "FTG8"
#define	BUFFERSIZE	(1024*20)
#else
#define	FILETESTG8 "/appfs/FTG8"
#define	BUFFERSIZE	(1024)
#endif
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
*					chenfm				20130117	created
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
	NDK_FsDel(FILETESTG8);//保护动作

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
			
		if ((fp=NDK_FsOpen(FILETESTG8, "w"))<0)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:%s第%d次:创建文件失败(%d)", __LINE__,TESTITEM, bak-cnt, fp);
			continue;//return; 20171106修改:执行人员要求统计测试成功率
			
		}
		if ((writelen= NDK_FsWrite(fp,writebuf2, BUFLEN))!=BUFLEN)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:写文件失败(%d)", __LINE__, bak-cnt,writelen);
			NDK_FsClose(fp);
			NDK_FsDel(FILETESTG8);
			continue;//return;
		}
		if ((ret=NDK_FsClose(fp))!=NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:关闭文件失败(%d)", __LINE__, bak-cnt, ret);
			NDK_FsDel(FILETESTG8);
			continue;//return;
		}
		if ((fp=NDK_FsOpen(FILETESTG8, "r"))<0)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:打开文件失败(%d)", __LINE__, bak-cnt, fp);
			NDK_FsDel(FILETESTG8);
			continue;//return;
		}
		memset(readbuf2, 0, sizeof(readbuf2));
		if ((readlen=NDK_FsRead(fp,readbuf2, BUFLEN))!=BUFLEN)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:读文件失败(%d)", __LINE__, bak-cnt, readlen);
			NDK_FsClose(fp);
			NDK_FsDel(FILETESTG8);
			continue;//return;
		}
		if (memcmp(writebuf2, readbuf2, sizeof(writebuf2)))
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:校验错", __LINE__, bak-cnt);
			NDK_FsClose(fp);
			NDK_FsDel(FILETESTG8);
			continue;//return;
		}		
		if ((ret=NDK_FsClose(fp))!=NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:关闭文件失败(%d)", __LINE__, bak-cnt, ret);
			NDK_FsDel(FILETESTG8);
			continue;//return;
		}
		if ((ret=NDK_FsDel(FILETESTG8))!=NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d次:删除文件失败(%d)", __LINE__, bak-cnt, ret);
			NDK_FsDel(FILETESTG8);
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
#if defined SP610
#define BLOCK_SIZE 8	
#else
#define BLOCK_SIZE 4
#endif

	//定义每个产品的单个文件最大大小(中端产品没有限制单个文件大小,低端产品文件大小由单个文件最大扇区数64*单个扇区大小4或者8来决定,所以是256或者512,安卓平台未知)
#if defined ME31
#define MAX_FLIE_SIZE 360
#elif defined ME30||defined ME30THM||defined ME32THM||defined ME30MH||defined ME30GX||defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME62||defined ME50NX||defined ME68||defined ME11||defined ME15||defined ME15C||defined ME20
#define MAX_FLIE_SIZE BLOCK_SIZE*64
#elif ANDIROD_ENABLE //defined IM81||defined N900||
#define MAX_FLIE_SIZE 240
#elif defined ME15C
#define MAX_FLIE_SIZE 66
#elif defined ME30S
#define MAX_FLIE_SIZE 88
#else
#define MAX_FLIE_SIZE 65535	//单个文件大小没有限制的情况SP610和中端产品 
#endif

	/*process body*/
	// step	(1) 取电子盘剩余空间
#if !(ANDIROD_ENABLE||defined ME15C|| defined ME30S)//目前除ME15C,高端产品的K21部分外其它产品均支持NDK_FsGetDiskSpace函数defined IM81||defined N900||
#if !K21_ENABLE
	sync();sleep(3);
#endif
	NDK_FsGetDiskSpace(1,&discspace1);
#if !K21_ENABLE
	cnt2 = discspace1/sizeof(writebuf)+1;//多加1的目的是让下面一定会写满电子盘,以达到边界测试的效果	
#else
	cnt2 = (discspace1-discspace1/sizeof(writebuf)/BLOCK_SIZE*7)/sizeof(writebuf)+1;//低端产品获取到的剩余空间大小和文件读写都是以扇区为基础单位，扇区大小可以是4或者8，每个扇区要使用4+2+1字节来记录文件系统信息,所以有剩余扇区数*7个字节的空间无法被使用
#endif
	cnt = (cnt2<MAX_FLIE_SIZE)?cnt2:MAX_FLIE_SIZE;//将剩余空间和最大文件大小进行比较，将小的值作为超大文件的实际大小进行测试
#else
	cnt = MAX_FLIE_SIZE;
#endif
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
	NDK_FsDel(FILETESTG8);
	
	// step	(2) 以"w"方式打开一特定文件
	if ((fp= NDK_FsOpen(FILETESTG8, "w"))<0)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:创建超大文件失败", __LINE__);
		goto ERR;
	}

	// step	(3) 写入20KB数据(随机数)至该文件中
	// step	(4) 循环至步骤2,创建超大文件
	memset(writebuf, rand()/256/*0x24*/, sizeof(writebuf));
	oldtime = time(NULL);
	lib_StartStopwatch();
	for (loop=0; loop<cnt; loop++)
	{
		if ((writelen=NDK_FsWrite(fp,writebuf, sizeof(writebuf)))!=sizeof(writebuf))
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
	if (NDK_FsClose(fp)!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME, "line %d:关闭文件出错", __LINE__);
		goto ERR;
	}

	// step	(6) 以"r"方式打开该文件
	if ((fp= NDK_FsOpen(FILETESTG8, "r"))<0)
	{
		cls_show_msg_record(FILENAME,FUNCNAME, "line %d:打开超大文件失败", __LINE__);
		goto ERR;
	}

	// step	(7) 取得文件长度
	NDK_FsFileSize(FILETESTG8,&filelen);
	if (filelen<filelen1)
	{
		cls_show_msg_record(FILENAME,FUNCNAME, "line %d:文件长度异常(%d,%d)", __LINE__, filelen, filelen1);
		goto ERR;
	}

	// step	(8) 读出文件数据并进行校验
	for (loop=0; loop<cnt; loop++)
	{
		memset(readbuf, 0, sizeof(readbuf));
		if ((readlen=NDK_FsRead(fp,readbuf, sizeof(readbuf)))!=sizeof(readbuf))
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
		if ((readlen=NDK_FsRead(fp,readbuf, filelen-filelen1))!=(filelen-filelen1))
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
	NDK_FsClose(fp);
	NDK_FsDel(FILETESTG8);

	// step	(10)再次取电子盘剩余空间，看是否为步骤1取得空间及一致
	//目前中端产品的文件系统是基于Linux的,空间并不能立即释放,只有老85的或者低端产品这点要求才能满足20170809 凯哥解释jym修改
#if  K21_ENABLE && !defined ME30S//!(defined IM81||defined N900||ANDIROD_ENABLE||ME15C)//30S不支持NDK_FsGetDiskSpace 20180509
#if 0//!K21_ENABLE
	sync();sleep(3);
#endif 
	NDK_FsGetDiskSpace(1,&discspace2);
	if (discspace1!=discspace2)
	{
		cls_show_msg_record(FILENAME,FUNCNAME, "line %d:电子盘空间未完全释放(%ld,%ld)", __LINE__, discspace1, discspace2);
		return;
	}
#endif

#if !K21_ENABLE
	cls_show_msg_record(FILENAME,FUNCNAME, "超大文件测试通过(创建大小为%dKB文件耗时%fs)", (cnt*sizeof(writebuf)+writelen)/1024, fTimeElapsed);
#else
	ftos(fTimeElapsed,str);
	cls_show_msg_record(FILENAME,FUNCNAME, "超大文件测试通过(创建大小为%dKB文件耗时%ss)", (cnt*sizeof(writebuf)+writelen)/1024, str);
#endif
	return;
ERR:
	NDK_FsClose(fp);
	NDK_FsDel(FILETESTG8);
	return;
}

static void filesys_press_multifiles(void)
{
	/*private & local definition*/
	int fp[5] ={-1, -1, -1, -1, -1}, ret = -1;
	int countnum = 0, i = 0, j = 0, cnt = 0/*, filelen[5] = {0}*/;
	int  writelen = 0, readlen = 0;
	char writebuf[BUFLEN] = {0}, readbuf[BUFLEN] = {0}, tempbuf[BUFLEN] = {0};
#if !K21_ENABLE
	char filename[5][10] ={"FTG8A", "FTG8B", "FTG8C", "FTG8D", "FTG8E"};
#else
	char filename[5][20] ={"/appfs/FTG8A", "/appfs/FTG8B", "/appfs/FTG8C", "/appfs/FTG8D", "/appfs/FTG8E"};
#endif
	ulong discspace1 = 0;

	/*process body*/
	cls_printf("多文件互操作测试中...");

	//NDK_FsGetDiskSpace(1,&discspace1);
	//cnt = discspace1/1024/5/8*8-7;//每个文件都是8K的整数倍-7个字节,这样低端产品每个文件会占用8K空间,但实际文件大小是8K-7
#if defined ME20||defined ME30THM||defined ME32THM||defined ME32||defined ME65 //ME20文件系统大小是68K,每个文件都是4K的整数倍-7个字节，一个文件写11K就是占用12K的大小
	cnt = 11;
#elif defined ME32GX
	cnt = 7;
#else
	cnt = 60;//为提高效率将cnt改小成60
#endif
	
	// step (1)
	for (countnum = 0; countnum<5; countnum++)
		NDK_FsDel(filename[countnum]);
	
	// step (2) 以"w"方式连续打开5个文件:FT21A,FT21B,FT21C,FT21D,FT21E,分别写入5K字节的数据(五个文件所写的数据内容各不相同)
	for (countnum = 0; countnum<5; countnum++)
	{
		if ((fp[countnum] =  NDK_FsOpen(filename[countnum], "w"))<0) // 创建文件
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:创建文件(%d)失败", __LINE__, countnum);
			for (i = countnum; i >= 0; i--)
			{
				NDK_FsClose(fp[i]);
				NDK_FsDel(filename[i]);
			}
			return;
		}

		memset(writebuf, countnum, sizeof(writebuf));
		for (j = 0; j < cnt; j++)
		{
			if ((writelen =  NDK_FsWrite(fp[countnum],writebuf, BUFLEN))!=BUFLEN) // 写入数据长度出错
			{
				cls_show_msg_record(FILENAME,FUNCNAME, "line %d:写第%d个文件的第%dK数据出错(%d)", __LINE__, countnum+1, j+1, writelen);
				for (i = countnum; i >= 0; i--)
				{
					NDK_FsClose(fp[i]);
					NDK_FsDel(filename[i]);
				}
				return;
			}
		}
	}
	//ME 32 ME32GX需要关闭之后再进行检测
#if K21_ENABLE  //defined ME32|| defined ME32GX || defined ME32THM|| defined ME30S||defined ME65||CPU5810X_ENABLE//三个 产品 NDK_FsExist实现是通过fsopen来判断,由于句柄数限制,会导致失败,故需要先关闭句柄20180412
	for (i = 0; i<5; i++)
		NDK_FsClose(fp[i]);
#endif
	//针对高端产品出错的bug增加了检测文件是否存在20171113(全部平台通用)
	for (countnum = 0; countnum<5; countnum++)
	{
		if((ret=NDK_FsExist(filename[countnum])) != NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:检测第%d个文件是否存在出错(%d)", __LINE__, countnum+1, ret);
			for (i = 0; i<5; i++)
			{
				NDK_FsClose(fp[i]);
				NDK_FsDel(filename[i]);
			}
			return;
		}
	}

	// step (3) 关闭这五个文件
	for (countnum = 0; countnum<5; countnum++)
		NDK_FsClose(fp[countnum]);

	// step (4) 以"r"方式打开这五个文件并分别进行数据校验
	for (countnum = 0; countnum<5; countnum++)
	{
		if ((fp[countnum] = NDK_FsOpen(filename[countnum], "r"))<0) // 打开文件失败
		{
			cls_show_msg_record(FILENAME,FUNCNAME, "line %d:打开文件(%d)失败", __LINE__, countnum);
			for (i = countnum; i >= 0; i--)
			{
				NDK_FsClose(fp[i]);
				NDK_FsDel(filename[i]);
			}
			return;
		}

		for (j = 0; j < cnt; j++)
		{
			memset(readbuf, 0, sizeof(readbuf));
			if ((readlen =  NDK_FsRead(fp[countnum],readbuf, BUFLEN))!=BUFLEN) // 读出数据长度出错 
			{
				cls_show_msg_record(FILENAME,FUNCNAME, "line %d:读第%d个文件的第%dK数据出错(%d)", __LINE__, countnum+1, j+1, readlen);
				for (i = countnum; i >= 0; i--)
				{
					NDK_FsClose(fp[i]);
					NDK_FsDel(filename[i]);
				}
				return;
			}

			memset(tempbuf, countnum, sizeof(tempbuf));
			if (memcmp(tempbuf, readbuf, readlen)) // 校验内容
			{
				cls_show_msg_record(FILENAME,FUNCNAME, "line %d:第%d个文件校验出错", __LINE__, countnum+1);
				for (i = 0; i<5; i--)
				{
					NDK_FsClose(fp[i]);
					NDK_FsDel(filename[i]);
				}
				return;
			}
		}
		
		//删除某个文件后,读取剩余文件进行数据比较,数据应该不会被改变
		if(countnum%2==1)//将FTG8B和FTG8D文件删除后读取剩余文件进行比较
		{
			NDK_FsClose(fp[countnum]);
			NDK_FsDel(filename[countnum]);
		}
	}

	for (i = 0; i<5; i++)
	{
		NDK_FsClose(fp[i]);
		NDK_FsDel(filename[i]);
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
#if !K21_ENABLE
	ulong space = 0;
#else
	char str[32] = {0};
#endif

#if !K21_ENABLE
	sync();sleep(3);
	NDK_FsGetDiskSpace(1,&space);
	int times = (space/BUFLEN>10240)?10240:(space/BUFLEN);//写文件大小由原来的1M修改成10M 20140219 根据向高要求修改
#elif defined ME15C||defined ME20||defined ME32THM	//ME15C 最大只支持67KB 
	int times = 66; 
#elif defined ME11||defined ME15 //ME11 最大只支持128KB (121+597)
	int times = 118; 
#elif defined ME30||defined ME30THM||defined ME30MH||defined ME32THM||defined ME30GX||CPU5810X_ENABLE||defined ME50NX||defined ME68	//ME30 最大只支持512KB
	int times = 511; 
#elif defined ME30S
    int times =255; 
#elif defined ME32GX||defined ME32||defined ME65
	int times = 51;
#else
	int times = 255;  //ME31最大只能存256KB,,实际可能还不到256,同时建议测试之前进行格式化
#endif
	/*process body*/
	if(times==0)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:磁盘空间不足,请先清理磁盘!", __LINE__);
		return;
	}
	
	//读写1M数据耗时测试
	NDK_FsDel(FILETESTG8);//保护动作
	memset(writebuf2, rand()/256/*0x44*/, sizeof(writebuf2));
	fp =  NDK_FsOpen(FILETESTG8, "w");

	cls_printf("写文件中...  ");
	lib_StartStopwatch();
	//for(j=0; j<1024; j++)//循环写1K数据1K*10240次
	for(j=0; j<times; j++)
	{
		if ((writelen= NDK_FsWrite(fp,writebuf2, BUFLEN))!=BUFLEN)//上面的压力有所不同,所以蛮判断一下返回
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:写文件失败(%d,%d)", __LINE__,j, writelen);
			NDK_FsClose(fp);
			NDK_FsDel(FILETESTG8);
			return;
		}
	}
#if !K21_ENABLE
	cls_show_msg_record(FILENAME,FUNCNAME, "写文件速度为%fkb/s", times/lib_ReadStopwatch());
#else
	memset(str,0,sizeof(str));
	ftos(times/lib_ReadStopwatch(),str);
	cls_show_msg_record(FILENAME,FUNCNAME, "写文件速度为%skb/s", str);
#endif
	if ((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:关闭文件失败(%d)", __LINE__, ret);
		NDK_FsDel(FILETESTG8);
		return;
	}
	if ((fp=NDK_FsOpen(FILETESTG8, "r"))<0)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:打开文件失败(%d)", __LINE__, fp);
		NDK_FsDel(FILETESTG8);
		return;
	}

	cls_printf("读文件中...  ");
	memset(readbuf2, 0, sizeof(readbuf2));
	lib_StartStopwatch();
	for(j=0; j<times; j++)//读10240*1024=10M的数据
	{
		if ((readlen= NDK_FsRead(fp,readbuf2, BUFLEN))!=BUFLEN)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:读文件失败(%d)", __LINE__, readlen);
			NDK_FsClose(fp);
			NDK_FsDel(FILETESTG8);
			return;
		}
	}
#if !K21_ENABLE
	cls_show_msg_record(FILENAME,FUNCNAME, "读文件速度为%fkb/s", times/lib_ReadStopwatch());
#else
	memset(str,0,sizeof(str));
	ftos(times/lib_ReadStopwatch(),str);
	cls_show_msg_record(FILENAME,FUNCNAME, "读文件速度为%skb/s", str);
#endif
	if ((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:关闭文件失败(%d)", __LINE__, ret);
		NDK_FsDel(FILETESTG8);
		return;
	}
	if ((ret=NDK_FsDel(FILETESTG8))!=NDK_OK)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:删除文件失败(%d)", __LINE__, ret);
		NDK_FsDel(FILETESTG8);
		return;
	}
	
	return;
}

static void fwrite_shutdown(void)
{
	/*private & local definition*/
	char wbuf[BUFFERSIZE] = {0};
	int fp = 0, wlen = 0, i = 0;
	time_t oldtime = 0;
	int cnt = 10;
	/*process body*/
	if ((fp=NDK_FsOpen(FILETESTG8, "w"))<0)
	{
		cls_show_msg("line %d:创建文件失败", __LINE__);
		return;
	}
	memset(wbuf, rand()/256/*0x44*/, sizeof(wbuf));
	cls_printf("写文件数据中...");
#if defined ME15C||defined ME20 
	cnt = 8;
#else
	cnt = 120;//低端文件系统以扇区为读写单位，每个扇区4K或8K
#endif
	oldtime = time(NULL);
	for(; i<cnt; i++)//取120,以接近2M左右的大小
	{
		if ((wlen=NDK_FsWrite(fp,wbuf, BUFFERSIZE))!=BUFFERSIZE)
			break;
		show_stopwatch(ENABLE, time(NULL)-oldtime);	
	}
	show_stopwatch(DISABLE, 0);

	cls_printf("已写入%d bytes数据!请直接断电!", BUFFERSIZE*i);
	while(1);//死循环,以强制用户关机
}

//测试步骤:1(测试前置)->2(记录创建文件前的大小)->3(直接关电源测试)->2(记录创建文件后的大小,变化量体现文件大小)->1(清理现场)->2(空间大小应恢复到先前)
//说明:执行3时,尽量使用"直接关电源"而不是按电源键,若按电源键,则一般会写到磁盘上;"直接关电源"则不一定会写入(文件也检测不到,空间也不变)
//故之后看到空间变小或不变都有可能.
//若空间大小在断电前后不变化,则还要注意空间实际情况与返回的空间大小一致(做完本用例后可使用性能及超大文件测试来验证是否还有足够空间)
static void filesys_shutdown(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	ulong pulSpace = 0;
	
	/*process body*/
	while (1)
	{
		nKeyin = cls_show_msg("选择合适的动作:\n"
							"1.删除测试文件\n"
              					"2.取剩余空间\n"
							"3.fwrite后断电\n");
		
		switch (nKeyin)
		{
		case '1':
			if( NDK_FsExist(FILETESTG8)==NDK_OK)
			{
				cls_show_msg1(2, "检测到测试文件,删除中...");
				NDK_FsDel(FILETESTG8);
			}
			else
				cls_show_msg1(2, "未检测到测试文件");
			break;
		case '2':
#if !K21_ENABLE
			sync();sleep(3);
#endif
			NDK_FsGetDiskSpace(1,&pulSpace);
			cls_show_msg("当前电子盘剩余空间:%ld,请记录!", pulSpace);
			break;	
		case '3':
			fwrite_shutdown();
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

//测试意图:在fwrite过程中按关机键,此时不应有什么异常,如,无法关机(数据写全或不全都有可能)
static void fwrite_shutdown1(int slct)
{
	/*private & local definition*/
	char wbuf[BUFFERSIZE] = {0};
	int fp = 0, wlen = 0, i = 0;
	//time_t oldtime = 0;
	int cnt = 10;
	/*process body*/
	NDK_FsDel(FILETESTG8);
	
	if ((fp= NDK_FsOpen(FILETESTG8, "w"))<0)
	{
		cls_show_msg("line %d:创建文件失败", __LINE__);
		return;
	}
	memset(wbuf, rand()/256/*0x44*/, sizeof(wbuf));
	cls_show_msg("先按任意键,再2s后%s", slct==1?"按关机键进行关机":"直接关闭电源");
	//oldtime = time(NULL);
#if defined ME15C||defined ME20 
		cnt = 64;  //只有68K的文件系统  写入64能够有时间进行关机
#else
		cnt = 1024;
#endif	
	for(; i<cnt/*102*/; i++)//取102,以接近2M左右的大小   20140714 修改102成1024，针对关机比较慢的设备增加写操作时间避免写完了还没有完成关机操作
	{
		if ((wlen=NDK_FsWrite(fp,wbuf, BUFFERSIZE))!=BUFFERSIZE)
			break;
		//show_stopwatch(ENABLE, time(NULL)-oldtime);	
	}
	show_stopwatch(DISABLE, 0);

	cls_show_msg("已写入%d bytes数据!关机失败!", BUFFERSIZE*i);
	while(1);//你主动关机,就强制你关机!
}

//fwrite过程中下电或掉电,此时不应有什么异常,如,无法关机(数据写全或不全都有可能)
static void filesys_shutdown1(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	
	/*process body*/
	while (1)
	{
		nKeyin = cls_show_msg("1.fwrite中关机\n"
              					"2.fwrite中掉电\n");
		
		switch (nKeyin)
		{
		case '1':
		case '2':
			fwrite_shutdown1(nKeyin-'0');
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

#if 0
//测试意图:写满电子盘,但不应影响系统其它正常功能(如,开机无法启动等),若要清理环境可以使用"文件综合"主菜单中的"清理"功能
static void filesys_bigfile(void)
{
	/*private & local definition*/
	int fp = 0, writelen = 0, loop = 0/*MIN_SIGNINT*/;
	char writebuf[BUFFERSIZE] = {0};//统一使用40K就好了,避免栈溢出
	time_t oldtime = 0;

	/*process body*/
	NDK_FsDel(FILETESTG8);

	// step	(2) 以"w"方式打开一特定文件
	if ((fp=NDK_FsOpen(FILETESTG8, "w"))<0)
	{
		cls_show_msg("line %d:创建超大文件失败", __LINE__);
		goto ERR;
	}

	// step	(3) 写入20KB数据(随机数)至该文件中
	// step	(4) 循环至步骤2,创建超大文件
	cls_printf("写满电子盘中...");
	memset(writebuf, rand()/256/*0x24*/, sizeof(writebuf));
	oldtime = time(NULL);
	for (; loop<MAX_SIGNINT; loop++)
	{
		if ((writelen= NDK_FsWrite(fp,writebuf, sizeof(writebuf)))!=sizeof(writebuf))
		{
			cls_printf("line %d:电子盘已写满,请重启机器!(%d,%d)", __LINE__, loop, writelen);
			 NDK_FsClose(fp);
			while(1);//死循环,以强制用户关机
		}
		show_stopwatch(ENABLE, time(NULL)-oldtime);//zhangnw20110715add
	}
	show_stopwatch(DISABLE, 0);

	cls_show_msg("line %d:电子盘未能写满,测试失败!", __LINE__);
ERR:	
	NDK_FsClose(fp);
	NDK_FsDel(FILETESTG8);
	return;
}
#endif

#if !(ANDIROD_ENABLE||ME15C)//不支持取电子盘空间函数的产品不做异常测试defined IM81||defined N900||
//注意:文件异常测试后将可能会残留测试文件在系统中,就会影响文件数等测试项目了.建议到时再进行一次格式化
static void filesys_abnormity(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	
	/*process body*/
	while (1)
	{
		nKeyin = cls_show_msg("_文件异常测试_\n"
			              		"1.fwrite后断电\n"  //20140827 1.12标准中NA 掉 相应的代码屏蔽
			              		"2.fwrite中断电\n"
			              		//"3.写满电子盘"
			              		);
		
		switch (nKeyin)
		{
		case '1':
			filesys_shutdown();
			break;
		case '2':
			filesys_shutdown1();
			break;
		//case '3':
			//filesys_bigfile();
			//break;
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
#endif

void systest8(void)
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
							"2.性能测试\n"
              					"3.异常测试\n"
							"4.清理环境\n");
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
		case '3':
#if !(ANDIROD_ENABLE||ME15C)//不支持取电子盘空间函数的产品不做异常测试defined IM81||defined N900||
			cls_show_msg1(1, "文件系统异常测试中...\n");
			filesys_abnormity();
#else
			cls_show_msg1(1, "无获取电子盘空间的函数，故不做异常测试\n");
#endif
			break;
		case '4':
#if !(ANDIROD_ENABLE||ME15C)//不支持取电子盘空间函数的产品不做异常测试defined IM81||defined N900||		
			if(NDK_FsExist(FILETESTG8)==NDK_OK)
			{	
#if !K21_ENABLE			
				sync();sleep(3);
#endif
				NDK_FsGetDiskSpace(1,&pulSpace);
				cls_show_msg("当前电子盘剩余空间:%ld", pulSpace);	
				cls_show_msg1(2, "测试环境清理%s!",  NDK_FsDel(FILETESTG8)==NDK_OK?"成功":"失败");
			}
			else
				cls_show_msg1(2, "未检测到测试文件");
#if !K21_ENABLE			
			sync();sleep(3);
#endif
			NDK_FsGetDiskSpace(1,&pulSpace);
			cls_show_msg("当前电子盘剩余空间:%ld", pulSpace);
#elif ANDIROD_ENABLE//!defined ME15C	//不支持NDK_FsClearAppFile函数的低端产品需要打开本开关使用NDK_FsFormat函数清理环境
			cls_show_msg("按任意键开始格式化");	
			NDK_FsFormat();//高端产品的k21部分直接用格式化函数清理环境
#else //其他低端产品用这个进行格式化
			cls_show_msg1(2, "测试环境清理%s!",  NDK_FsClearAppFile()==NDK_OK?"成功":"失败");//带防切机功能的低端产品不支持NDK_FsFormat
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
	}
	return;
}

