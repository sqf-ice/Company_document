/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: IC卡性能,压力测试
* file name		: 5.c
* Author 			: chenfm
* version			: V1.0
* DATE			: 20130115
* directory 		: 
* description		: IC卡性能,压力测试
* related document	: NL8200程序员参考手册.doc
*				  
************************************************************************
* log			: 
* Revision 1.0  20130115 10:10 chenfm
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"IC卡性能,压力"

#define	DEFAULT_CNT_STR	"15000"	//"3000"//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(1000)

#define	DEFAULT_CNT_STR2	"3000"	//"3000"//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE2	atoi(DEFAULT_CNT_STR2)	//(1000)

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
*			  		  chenfm 		   	20130115		created
*****************************************************************/
//设计说明:不关注功能、压力(比如在一定强度下是否成功)等,仅关注性能
//也不关注ability返回,性能值在函数运行过程中给出
static void IC_SAM_ability(void)
{
	/*private & local definition*/
#include "icsam.h"
#define	nTestTime	(10)	//(5)

	//int nTestTime = 5;
	int ret = -1,i = 0, rlen = 0, count = 0,pnAtrlen = 0;
	uchar buf[128] = {0};
#if K21_ENABLE
	char str[32] = {0};
#else
	time_t oldtime = 0, diff = 0;
#endif
	float fTimeElapsed = 0.0;
#if defined ME15CHX||defined ME15B
	uint time1 = 0, time2 = 0;
#endif

	uchar sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令

	/*process body*/
	do
	{
		//上电
		if ((ret=NDK_IccPowerUp(CardNo[i], buf,&pnAtrlen))!=NDK_OK)//不关注成功否,由压力测试去关注
		{	
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:%s上电失败", __LINE__, CardNoStr[i]);
			continue;
		}
		//测试iccrw性能
		cls_printf("正在测试NDK_Iccrw对%s读写速度...\n", CardNoStr[i]);
		count = 0;
		fTimeElapsed = 0;
		memset(buf,0,sizeof(buf));
#if !K21_ENABLE
		oldtime = time(NULL);//从第一次收到忙状态开始计时
		while(1)
		{
			lib_StartStopwatch();
			if(NDK_Iccrw(CardNo[i],sizeof(sendbuf),sendbuf,&rlen,buf)==NDK_OK)
			{
				count++;//累加成功次数
				fTimeElapsed+=lib_ReadStopwatch();//累加成功时间
			}
			else
			{
				NDK_IccPowerDown(CardNo[i]);
				NDK_IccPowerUp(CardNo[i], buf,&pnAtrlen);
			}

			if(fTimeElapsed>nTestTime)//读写时间超过10秒则退出
				break;
			if((diff=time(NULL)-oldtime)>5*nTestTime)//累计读写时间超过50秒则退出
				break;
		}
#elif defined ME15CHX||defined ME15B
		NDK_SysReadWatch(&time1);
		while((ret=NDK_Iccrw(CardNo[i],sizeof(sendbuf),sendbuf,&rlen,buf))==NDK_OK)
		{
			count++;
			NDK_SysReadWatch(&time2);
			if((fTimeElapsed=(time2-time1)/1000)>nTestTime)	
				break;
		}
#else	
		lib_StartStopwatch();
		while((ret=NDK_Iccrw(CardNo[i],sizeof(sendbuf),sendbuf,&rlen,buf))==NDK_OK)
		{
			count++;
			if((fTimeElapsed=lib_ReadStopwatch())>nTestTime)	
				break;
		}
#endif
		if(fTimeElapsed>nTestTime)
#if !K21_ENABLE
			cls_show_msg_record(FILENAME,FUNCNAME,"NDK_Iccrw每秒读%s卡:%.0f字节", CardNoStr[i], count*rlen/fTimeElapsed);//zhangnw20110308修正:原计算方法为count*sizeof(sendbuf)/fTimeElapsed, sendbuf[4]改成rlen 20161116 linwl
#else
		{
			memset(str,0,sizeof(str));
			ftos(count*rlen/fTimeElapsed,str);
			cls_show_msg_record(FILENAME,FUNCNAME,"NDK_Iccrw每秒读%s卡:%s字节", CardNoStr[i], str);
		}
#endif
		else 
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:累计成功读写时间不足10秒(%s卡, fTimeElapsed=%f)\n", __LINE__, CardNoStr[i], fTimeElapsed);

		//下电
		if (NDK_IccPowerDown(CardNo[i])!=NDK_OK)//不关注成功否,由压力测试去关注
		{	
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:%s下电失败", __LINE__, CardNoStr[i]);
			continue;
		}
	}while(++i!=(ICSAMNUM/*SZ_ARRAY(CardNo)-1*/));
	
	return;
}

//IC/SAM 卡压力测试
//设计说明:关注在一定强度下是否仍运行正常
//对于接触/非接的压力测试,除了要判断iccrw类的接口返回成功外,还要对命令状态字(SW)或状态域进行进一步判断!!!
static void IC_SAM_press(void)
{
	/*private & local definition*/
#include "icsam.h"
	
	int i = 0, ret = 0, rlen = 0, succ = 0,pnAtrlen = 0;
	uint unLen = 0;
	uchar buf[128] = {0};
	int cnt = DEFAULT_CNT_VLE2, bak = 0;
	char CntStr[8] = DEFAULT_CNT_STR2;
	ushort sw=0;
	uchar sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
	//根据所发的卡的不同,有的可能支持下面的命令,与机具无关,与使用的测试卡有关
	//char sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 0};

	/*process body*/
	cls_printf("默认压力测试次数:\n");
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE2;
	else
		bak = cnt = atoi(CntStr);
	//NDK_IccSetType(ICTYPE_ISO7816);//ISO7816的卡片进行压力测试时需要增加该设置
	do
	{		
		cnt = bak;
		succ = 0;
		while(cnt)
		{
			if (ESC==cls_show_msg1(1, "%s压力测试中...\n还剩%d次(已成功%d次),ESC中断测试", CardNoStr[i], cnt, succ))
				break;
			cnt--;
					
			//上电
			//memset(buf,0,sizeof(buf));
			if ((ret=NDK_IccPowerUp(CardNo[i], buf,&pnAtrlen))!=NDK_OK)
			{	
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次:%s上电失败(%d)", __LINE__, bak-cnt, CardNoStr[i], ret);
				continue;//break;
			}
			//使用iccrw发取随机数命令
			memset(buf,0,sizeof(buf));
			if((ret=NDK_Iccrw(CardNo[i], sizeof(sendbuf), sendbuf, &rlen, buf))!=NDK_OK||((sw=SMART_GETSW(buf+rlen-2))!=0x9000&&(sw=SMART_GETSW(buf+rlen-2))!=0x6d00))
			{	
				NDK_IccPowerDown(CardNo[i]);						
				cls_show_msg1_record(FILENAME,FUNCNAME, g_keeptime, "line %d:第%d次:%s读卡失败(%d,0x%04x)", __LINE__, bak-cnt, CardNoStr[i], ret,sw);
				continue;//break;					
			}		
			//下电
			if ((ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK)
			{	
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次:%s下电失败(%d)", __LINE__, bak-cnt, CardNoStr[i], ret);
				continue;//break;
			}
			succ++;
		}
		cls_show_msg_record(FILENAME,FUNCNAME,"%s压力测试完成,已执行次数为%d,成功%d次", CardNoStr[i], bak-cnt, succ);
	}while(++i!=(ICSAMNUM/*SZ_ARRAY(CardNo)-1*/));//不测试0xff
	
	return;
}

static void IC_SAM_rw_press(void)
{
	/*private & local definition*/
#include "icsam.h"
	
	int i = 0, ret = 0, rlen = 0, succ = 0,pnAtrlen = 0;
	uint unLen = 0;
	uchar buf[128] = {0};
	int cnt = DEFAULT_CNT_VLE, bak = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	ulong sw = 0;
	uchar sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
	//根据所发的卡的不同,有的可能支持下面的命令,与机具无关,与使用的测试卡有关
	//char sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 0};

	/*process body*/
	if(g_SequencePressFlag)
	{
		i = 0;  //如果是连续压力测试从IC1开始逐个测试
	}
	else
	{
		if((i=select_ICSAM())==NDK_ERR_QUIT)
			return;	
		else
			cls_show_msg1(2, "已选择%s",  CardNoStr[i]);		
	}
	
	//测试使用次数做限制,不用时间(因为读写SAM有关时钟中断,计时有较大偏差)
	cls_printf("默认压力测试次数:\n");
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	while(1)
	{
		if(g_SequencePressFlag)  
		{
			if(cnt == 0) /*一种卡循环结束, 则自加准备进行别种卡的循环*/
			{
				if(++i==(ICSAMNUM))
					break;
				else
				{
					if ((ret=NDK_IccPowerDown(CardNo[i-1]))!=NDK_OK) //下电上一次未下电的卡
					{	
						cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:%s下电失败(%d)", __LINE__, CardNoStr[i-1], ret);
					}
					cls_show_msg_record(FILENAME,FUNCNAME,"%s读写压力完成,已执行次数为%d,成功%d次", CardNoStr[i-1],bak, succ);
					cnt = bak;   /*每种卡片测试完初值重新设置*/
					succ = 0;    /*每种卡片测试完清空*/
				}
			}
		}
		else
		{
			if(cnt == 0) 
				break;
		}
		//上电
		//memset(buf,0,sizeof(buf));
		if ((ret=NDK_IccPowerUp(CardNo[i], buf,&pnAtrlen))!=NDK_OK)
		{
			cnt--;
			if(ESC==cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次:%s上电失败(%d)", __LINE__, bak-cnt, CardNoStr[i], ret))
				break;
			else
				continue;
		}
			
		while(cnt)
		{
			cls_printf("%s读写压力中...\n还剩%d次(已成功%d次),请耐心等待", CardNoStr[i], cnt, succ);
			cnt--;
			
			//使用iccrw发取随机数命令
			//memset(buf,0,sizeof(buf));
			if((ret=NDK_Iccrw(CardNo[i], sizeof(sendbuf), sendbuf, &rlen, buf))!=NDK_OK||((sw=SMART_GETSW(buf+rlen-2))!=0x9000&&(sw=SMART_GETSW(buf+rlen-2))!=0x6d00))
			{	
				NDK_IccPowerDown(CardNo[i]);						
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次:%s读卡失败(%d,0x%04x)", __LINE__, bak-cnt, CardNoStr[i], ret,sw);
				break;//continue,此处不能使用continue否则后续读写由于已经下电了会连续失败，并且无法退出程序	
			}
						
			succ++;
		}
	}
	
	//下电
	if(g_SequencePressFlag)  
		--i;  /* 上面连续压力测试在跳出循环时候被自增过，所以如果是连续压力测试这里需要自减 */
	
	if ((ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK)
	{	
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:%s下电失败(%d)", __LINE__, CardNoStr[i], ret);
		//此时下电失败,仍允许报告压力测试成功率return;
	}
	
	cls_show_msg_record(FILENAME,FUNCNAME,"%s读写压力完成,已执行次数为%d,成功%d次", CardNoStr[i],bak-cnt, succ);
	return;
}

void IC_abnormal1(void)
{
	int i = 0, ret = 0, buflen=0, nSta = 0, pnAtrlen = 0;
	uchar buf[128] = {0};
	uchar sendbuf[] = {'\x00', '\x84', '\x00', '\x00', 8};
	ulong sw = 0;
	
	for(i=0;i<2;i++)
	{
		//正确插卡上电
		cls_printf("请正向(正确方式)插入IC卡...");	

		while(1)
		{
			nSta = 0;
			if((ret=NDK_IccDetect(&nSta))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次:探测失败(%d)", __LINE__, i+1, ret);
				return;
			}
			if(nSta==0x01)
				break;
			sleep(1);
		}
		
		if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&buflen))!=NDK_OK)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC上电失败(%d)", __LINE__, i+1, ret);
			return;
		}
		//使用iccrw发取随机数命令
		memset(buf,0,sizeof(buf));
		buflen=0;
		if((ret=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf), sendbuf, &buflen, buf))!=NDK_OK||((sw=SMART_GETSW(buf+buflen-2))!=0x9000&&(sw=SMART_GETSW(buf+buflen-2))!=0x6d00))
		{	
			NDK_IccPowerDown(ICTYPE_IC);						
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC1读卡失败(%d,0x%04x)", __LINE__, i+1,  ret,sw);
			return;//break;					
		}
		//下电
		if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC卡下电失败(%d)", __LINE__, i+1, ret);
			return;
		}

		cls_show_msg("请将卡拔出,按任意键继续..");
		if((ret=NDK_IccDetect(&nSta))!=NDK_OK||(nSta&0xff)!=0x00)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC卡未拔出(%d)", __LINE__, i+1, ret);
			return;
		}
		if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC卡下电失败(%d)", __LINE__, i+1, ret);
			return;
		}
		if((ret=NDK_IccDetect(&nSta))!=NDK_OK||(nSta&0xff)!=0x00)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC卡未拔出(%d)", __LINE__, i+1, ret);
			return;
		}
		
		//错误插卡上电
		cls_printf("请反向(错误方式)插入IC卡...");
		while(1)
		{
			nSta = 0;
			if((ret=NDK_IccDetect(&nSta))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次:探测失败(%d)", __LINE__, i+1, ret);
				return;
			}
			if(nSta==0x01)
				break;
			sleep(1);
		}
		
		memset(buf,0,sizeof(buf));
		buflen = 0;
		if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&buflen))==NDK_OK)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC上电应失败(%d)", __LINE__, i+1, ret);
			return;
		}

		//错误插卡下电，不关心下电是否成功
		NDK_IccPowerDown(ICTYPE_IC);	
		cls_show_msg("请将卡拔出,按任意键继续..");
		if((ret=NDK_IccDetect(&nSta))!=NDK_OK||(nSta&0xff)!=0x00)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC卡未拔出(%d)", __LINE__, i+1, ret);
			return;
		}
		NDK_IccPowerDown(ICTYPE_IC);	
		if((ret=NDK_IccDetect(&nSta))!=NDK_OK||(nSta&0xff)!=0x00)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:IC卡未拔出(%d)", __LINE__, i+1, ret);
			return;
		}
	}
	
	//新增异常测试用例测试MEMORY卡上电后不下电 操作IC卡应该要正常 20140901 linwl	
	memset(buf, 0, sizeof(buf));
	if((ret=NDK_IccPowerUp(ICTYPE_M_1, buf, &pnAtrlen))==NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:MEMORY卡上电应该失败(%d)", __LINE__, ret);
		return;
	}
	cls_show_msg("请插入IC卡,按任意键继续..");
	if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&buflen))!=NDK_OK)
	{	
		cls_show_msg1(g_keeptime, "line %d:IC上电应失败(%d)", __LINE__, ret);
		return;
	}
	NDK_IccPowerDown(ICTYPE_IC);
	NDK_IccPowerDown(ICTYPE_M_1);

	cls_show_msg("异常插卡测试通过");
	return;
}

void IC_abnormal2(void)
{
#if !K21_ENABLE||defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME30GX||defined ME32GX||defined ME32THM||ANDIROD_ENABLE||defined ME15B//||defined N900||defined IM81
	int i = 0, rlen = 0, pnAtrlen = 0, ret = 0;
	uchar buf[256] = {0};
	uchar recvbuf[256] = {0};
	EM_ICTYPE MEMTYPE[] = {ICTYPE_M_1,ICTYPE_M_1_1,ICTYPE_M_1_64, ICTYPE_M_2, ICTYPE_M_3, ICTYPE_M_3, ICTYPE_M_4, ICTYPE_M_5, ICTYPE_M_6,ICTYPE_M_7};
	char MEM_TYPE[][16] = {"at24c32", "at24c01", "at24c64", "sle44x2", "sle44x8", "sle5528", "at88sc102", "at88sc1604", "at88sc1608","at88sc153"};
	ushort sw = 0;
#endif

	/*process body*/
#if !K21_ENABLE||defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME30GX||defined ME32GX||defined ME32THM||ANDIROD_ENABLE||defined ME15B//||defined N900||defined IM81//K21平台不支持MEMORY卡 IM81已经支持Memory卡
	//case1:测试插入的卡为MEMORY卡和上电操作IC卡类型不一致，应该返回失败
	for(i=0;i<SZ_ARRAY(MEMTYPE);i++)
	{
		cls_show_msg("请确保%s已插入,任意键继续...", MEM_TYPE[i]);//实际插入的是MEM卡，对IC卡操作应该失败
		memset(buf, 0, sizeof(buf));
		if((ret=NDK_IccPowerUp(ICTYPE_IC, buf, &pnAtrlen))!=NDK_ERR_IOCTL/*NDK_ERR_OPEN_DEV*/)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(%s:%d)", __LINE__, MEM_TYPE[i], ret);
			NDK_IccPowerDown(ICTYPE_IC);
			return;
		}
		if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK/*NDK_ERR_OPEN_DEV*/)//无论什么卡类型，下电都成功
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(%s:%d)", __LINE__, MEM_TYPE[i], ret);
			NDK_IccPowerDown(ICTYPE_IC);
			return;
		}
	}

	//case2:插入IC卡进行MEMORY卡上电操作:开发无法判断插入的卡类型和上电类型不一致的情况，上电可以成功也可以失败，但具体读写操作时会异常
	cls_show_msg("请确保已插入IC卡,任意键继续...");
	for(i=0;i<SZ_ARRAY(MEMTYPE);i++)
	{
		memset(buf, 0, sizeof(buf));
		if((ret=NDK_IccPowerUp(MEMTYPE[i], buf, &pnAtrlen))!=NDK_OK/*NDK_ERR_OPEN_DEV*/)
			;
		else//上电成功情况下，读写应该失败
		{
			if((ret=NDK_Iccrw(ICTYPE_M_1, 9,(uchar *)"\x00\xD0\x00\x01\x04\x31\x32\x33\x34",&rlen,recvbuf))==NDK_OK||(sw=SMART_GETSW(recvbuf+rlen-2))!=0x6581)//跟开发确认中端是返回NDK_ERR_PARA 低端返回NDK_ERR_IOCTL 为了统一改成如果返回NDK_OK出错20150413 modify		
			{
			
				cls_show_msg1(g_keeptime, "line %d:测试失败(%d,0x%04x)", __LINE__, ret, sw);
				NDK_IccPowerDown(MEMTYPE[i]);
				return;
			}
		}
		
		if((ret=NDK_IccPowerDown(MEMTYPE[i]))!=NDK_OK/*NDK_ERR_OPEN_DEV*/)//无论什么卡类型，下电都成功，下电返回出错的情况是先对应管脚拉低 然后再读取是否拉低了 如果没拉低 判断下电失败
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(%s:%d)", __LINE__, MEM_TYPE[i], ret);
			NDK_IccPowerDown(MEMTYPE[i]);
			return;
		}
	}

	//case3:异常操作后正常操作IC卡上下电测试应该成功
	if((ret=NDK_IccPowerUp(ICTYPE_IC, buf, &pnAtrlen))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(IC卡:%d)", __LINE__, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		return;
	}
	if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:测试失败(IC卡:%d)", __LINE__, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		return;
	}
	//case4:异常操作后正常MEMORY卡上下电测试应该成功
	cls_show_msg1(2, "将进行MEMORY卡测试,任意键继续...");
	for(i=0;i<SZ_ARRAY(MEMTYPE);i++)
	{
		cls_show_msg("请确保%s已插入,任意键继续...", MEM_TYPE[i]);
		memset(buf, 0, sizeof(buf));
		if((ret=NDK_IccPowerUp(MEMTYPE[i], buf, &pnAtrlen))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(%s:%d)", __LINE__, MEM_TYPE[i], ret);
			NDK_IccPowerDown(MEMTYPE[i]);
			return;
		}
		if((ret=NDK_IccPowerDown(MEMTYPE[i]))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:测试失败(%s:%d)", __LINE__, MEM_TYPE[i], ret);
			NDK_IccPowerDown(MEMTYPE[i]);
			return;
		}
	}
#else
	cls_show_msg("请确认本产品是否支持MEMORY卡,如果不支持则无需测试本用例");
	return;
#endif
	
	cls_show_msg("插错卡片测试通过");
	return;
}

void IC_abnormal(void)
{
	int keyin = '0';
	while(1)
    {
		keyin = cls_show_msg("1.异常插卡测试\n"
							 "2.插错卡片测试\n");
		switch(keyin)
		{
		case '1':
			IC_abnormal1();
			break;
		case '2':
			IC_abnormal2();
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

void systest5(void)
{
	/*private & local definition*/
	int keyin = '0';
	
	/*process body*/
	//cls_show_msg("%s测试中...请插上所有IC/SAM卡,按任意键开始\n", TESTITEM);
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试中...请插上所有IC/SAM卡,按任意键开始", TESTITEM);
	
	while(1)
    {
       	if(auto_flag==1)  //判断统一模块内是否连续进行压力性能测试
		{
			if( ++keyin == '4' ) /*此处只做综合压力读写压力和性能测试，异常测试要手工进行*/
			{
				cls_show_msg_record(FILENAME ,FUNCNAME ,"%s连续压力测试和性能测试结束\n",TESTITEM);
				return ;
			}
			if(cls_show_msg1(5, "将连续进行综合压力和读写压力测试和性能测试,如要退出请按ESC")==ESC)
				return;
		}
		else
		{
			keyin = cls_show_msg("1.IC/SAM综合压力\n"
								"2.IC/SAM读写压力\n"
								"3.IC/SAM性能测试\n"
								" 4.异常");  //0.配置
		}
		switch(keyin) 
		{
		case '1':// 1.一定强度的压力测试;
			cls_show_msg1(2, "IC/SAM综合压力测试...");
			IC_SAM_press();
			break;
		case '2':
			if(!g_SequencePressFlag)  
			{	
				if(ENTER!=cls_show_msg("IC/SAM读写专项压力测试(本项测试不接受ESC中断测试),ENTER键继续..."))
					break;
			}
			IC_SAM_rw_press();
			break;
		case '3':// 2.性能测试
			cls_show_msg1(2, "IC/SAM性能测试...");
			IC_SAM_ability();
			break;
		case '4':
			IC_abnormal();
			break;
		case '0':
			//conf_icsam_pps();
			break;
		case ESC:
			cls_show_msg("请关机换其它IC/SAM并设置不同PPS重新测试!任意键继续...");
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	//test over
	return;
}

