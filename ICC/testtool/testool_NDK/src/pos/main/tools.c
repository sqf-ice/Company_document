
/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 测试程序工具函数
* file name		: layer.c
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20101217
* directory 		: .\SRC\main\
* description		: 
* related document	: 
* 
************************************************************************
* log			:
* Revision 1.0  20101217 22:34 zhangnw
* 从原有测试程序公共库(lib.c)中分离提取
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------functions definition---------------------------*/
#if defined ME15CHX||defined ME15B||defined ME66
int putchar(int c)
{
	NDK_PortWrite(PORT_NUM_USB, 1, (char*)&c);
	
	if (c == '\n') 
	{
		putchar('\r');
	}
	return c;
}
int NDK_ByteToBcd(int nNum, uchar *psCh)
{
	if (psCh==NULL) {
		return NDK_ERR_PARA;
	}

	if ((nNum < 0) || (nNum > 99)) {
		return NDK_ERR_PARA;
	}
	*psCh = ((nNum / 10) << 4) | (nNum % 10);
	return NDK_OK;
}
int NDK_CalcLRC(const uchar *psBuf, int nLen, uchar *ucLRC)
{
	int	i;

	if ((psBuf == NULL) || (ucLRC==NULL) ||
         (nLen <= 0) ) {
		return NDK_ERR_PARA;
	}

	*ucLRC = 0x00;
	for (i = 0; i < nLen; i++) {
		*ucLRC ^= psBuf[i];
	}
	return NDK_OK;
}
#endif

#if K21_ENABLE
#if defined ME20||defined ME15CHX||defined ME15B||defined ME66//defined ME11||defined ME15||defined ME15C  //不支持NDK_SysGetPosTime函数的产品需打开该开关
time_t g_times = 0;
time_t time(time_t *calptr)
{
	sleep(1);
	g_times++;
	
	if(calptr!=NULL)
		*calptr = g_times;
	
	return g_times;
}
#else
time_t time(time_t *calptr)
{
	struct tm stOldPosTime;
	time_t tmp;
	
	if(NDK_SysGetPosTime(&stOldPosTime) != NDK_OK)
		return 0;

	tmp = mktime(&stOldPosTime);
	
	if(calptr!=NULL)
		*calptr = tmp;
	
	return tmp;
}
#endif
#endif

void Beep(int sec)
{
	/*private & local definition*/
	time_t oldtime = time(NULL);

	/*process body*/
	while((time(NULL)-oldtime)<=abs(sec))
		NDK_SysBeep();
}

#if KB_DISABLE
int lib_kbflush(void){return NDK_PortClrBuf(g_com);}

int lib_kbhit(void)
{
	/*private & local definition*/
	int nKeyin = 0, len = 0;

	/*process body*/
#if 0//ANDIROD_ENABLE
	NDK_AZPortRead(g_com, 1, (char *)&nKeyin, 0, &len);
#else
	NDK_PortRead(g_com, 1, (char *)&nKeyin, 0, &len);
#endif
	return nKeyin;
}

int lib_getkeycode(uint sec)
{
	/*private & local definition*/
	int nKeyin = 0, len = 0;

	/*process body*/
#if  0//ANDIROD_ENABLE
	NDK_AZPortRead(g_com, 1, (char *)&nKeyin, (sec<=0)?MAX_SIGNINT:(sec*1000), &len);
#else
	NDK_PortRead(g_com, 1, (char *)&nKeyin, (sec<=0)?MAX_SIGNINT:(sec*1000), &len);//NL829STD没有KB硬件,不允许死等
#endif
	return nKeyin;
}
#else
//增加lib前缀,以示区别于旧的API
#if !K21_ENABLE
int lib_kbhit(void)
{
	/*private & local definition*/
	int nKeyin = 0;

	/*process body*/	
	NDK_KbHit(&nKeyin);
	return nKeyin;
}
#else
int lib_kbhit(void){return lib_getkeycode(1);}
#endif
#if !ANDIROD_ENABLE
int lib_getkeycode(uint sec)
{
	/*private & local definition*/
	int nKeyin = 0;

	/*process body*/
	NDK_KbGetCode(sec, &nKeyin);
	return nKeyin;
}
#endif
#endif

/****************************************************************
* function name 	 	: wait_anykey
* functional description 	: 
* input parameter		: 参数为0时会一直等待按键不退出
* output parameter	 	: 
* return value			: 超时返回0;其它返回相应的键码
* history 		 		: author		date			remarks
*			 		 zhangnw	 20060621     	created 
*****************************************************************/
//本函数与getkeycode的区别是本函数会5s beep报警
int wait_anykey(int sec)
{
	/*private & local definition*/
	int nKeyin = 0, beepflg = FALSE;
	time_t oldtime = 0, diff = 0 ,diffold = 1;

	/*process body*/
	if (sec<=0)
	{
		NDK_SysBeep();
		lib_kbflush();
		nKeyin = lib_getch();
	}
	else
	{
		lib_kbflush();
		oldtime = time(NULL);
		while((diff=time(NULL)-oldtime)<sec && (nKeyin=lib_kbhit())==0)//注意,键盘扫描会在1s内执行多次的
		{	
			if(diffold!=diff)//时钟发生变化才刷新一次倒计时,避免一直调用show_stopwatch给系统占用资源
			{
				show_stopwatch(ENABLE, sec-diff);//倒计时
				diffold = diff;
			}
			if(diff%5==0)//5s一报警
			{
				if(beepflg==FALSE)
				{
					NDK_SysBeep();
					beepflg = TRUE;
				}
			}
			else
				beepflg = FALSE;
		}
		show_stopwatch(DISABLE, 0);//退出前清除计时
	}
	
	return nKeyin;
}

/****************************************************************
* function name 	 	: 
* functional description 	: 判断字符串是否完全由数字构成
* input parameter	 	: 
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20060704 	 	created
*****************************************************************/
int is_digit_str(char *str)
{
#if K21_ENABLE
	/*private & local definition*/
	int len = 0, i = 0;

	/*process body*/
	//zhangnw20060704 debug isdigit
	//测试发现原先因为没有初始化标准C库,使得isdigit均返回0
	if ((len = strlen(str))==0)
		return FALSE;
	for (; i<len; i++)
	{
		if (!ISDIGIT(str[i]))
			return FALSE;
	}
	return TRUE;
#else	
	//NDK有提供类似功能函数时,使用NDK函数
	return (NDK_IsDigitStr((uchar *)str)==NDK_OK)?TRUE:FALSE;
#endif
}

//本函数一般只推荐给与通讯相关的模块(如,MDM,COM等)使用,文件系统等模块不要使用,直接使用memcmp
//本函数依赖于linktype
//相等返回SUCC,不等返回FAIL(注意:返回值的语义与标准memcmp不完全相同)
int MemCmp(char *sbuf, char *rbuf, int len)
{
	/*private & local definition*/
	
	/*process body*/
	//MDM不比较5btyes的TPDU头
	if(getlinktype()==ASYN)
		return (memcmp(sbuf, rbuf, 3)||memcmp(sbuf+8, rbuf+8, len-8))?FAIL:SUCC;
	else if(getlinktype()==SYNC)
		return memcmp(sbuf+SDLCPCKTHEADERLEN, rbuf+SDLCPCKTHEADERLEN, len-SDLCPCKTHEADERLEN)?FAIL:SUCC;
	else
		return memcmp(sbuf, rbuf, len)?FAIL:SUCC;
}
#if !(defined ME15CHX||defined ME66)
void debug_buf(char *bufname, char *bufaddr, int buflen)
{
	/*private & local definition*/
	int left = buflen;
	char *start = bufaddr;//backup bufaddr,因为bufaddr是输入arg,不要去改它的值

	/*process body*/
	if(cls_show_msg("是否从串口1上送%s(长度:%dB)?\n[ENTER]上送,[其它]不上送", (bufname==NULL)?"":bufname, buflen)==ENTER)
	{
		NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1");
#if 0
		while(left>BUFSIZE_SERIAL)
		{
			NDK_PortWrite(PORT_NUM_COM1, BUFSIZE_SERIAL, start);//portwrite(AUX1, BUFSIZE_SERIAL, start);
			//更新发送缓冲首址与剩余发送长度
			start += BUFSIZE_SERIAL;
			left -= BUFSIZE_SERIAL;
			sleep(1);//msdelay(500);//如果没有停顿地不断发送(msdelay(100)不够的,500ms或干脆sleep(1)这样),实测发现会丢失数据
		}
#endif
		NDK_PortWrite(PORT_NUM_COM1, left, start);
		sleep(1);
	}
}

uchar MakeLRC(uchar *buf, int len)
{
#if K21_ENABLE
	/*private & local definition*/
	int i = 0;
	uchar c = 0x00;

	/*process body*/
	for (; i<len; i++)
		c^=buf[i];
	
	return c;
#else	
	//NDK有提供类似功能函数时,使用NDK函数
	/*private & local definition*/
	uchar c = 0x00;

	/*process body*/
	NDK_CalcLRC(buf, len, &c);
	return c;
#endif
}

uchar Data2BCD(int data)
{
#if K21_ENABLE
	/*private & local definition*/

	/*process body*/
	ASSERT(data>=0);// && data<100);
	data%=100;//仅保留个位与十位
	return (data/10*0x10+data%10);
#else	
	//NDK有提供类似功能函数时,使用NDK函数
	/*private & local definition*/
	uchar c = 0x00;

	/*process body*/
	data%=100;//仅保留个位与十位
	NDK_ByteToBcd(data, &c);
	return c;
#endif
}

/****************************************************************
* function name 	 	: 
* functional description 	: 该函数用于判断数据通讯压力是否连续出错
* input parameter	 	: 
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		linwl 		   20141119 	 	       由systest4.c移到本处
*****************************************************************/
int IsContinuous(int *cur, int *last)
{
	/*private & local definition*/
	int ret = FALSE;

	/*process body*/
	if(*cur==(*last+1))
		ret = TRUE;

	*last = *cur;
	return ret;
}

/******************************************************************************
*	function name 	 	:FGetC
*	functional description :由文件中读取一个字符	
*	input parameter	 	:int nFp 打开的文件句柄
*	output parameter	 	: 
*	return value		 	:
*						会返回读取到的字符，
						若返回-1则表示到了文件尾。
*     history 		 		: author			date			remarks
						jiangym 		    20150415      created
******************************************************************************/
int FGetC(int nFp)
{
	char ch = 0;
	if(NDK_FsRead(nFp, (char *)&ch, 1) != 1)
		return -1;
	else
		return ch;
}
/******************************************************************************
*	function name 	 	:FGets
*	functional description :由文件中读取一行字符串
*	input parameter	 	:int nMaxLen 读取的最大长度
*						 int nFp 打开的文件句柄
*	output parameter	 	: char *pszBuf 读到的字符串
*	return value		 	:
*						若成功则返回pszBuf指针，
*						返回NULL则表示有错误发生。
*     history 		 		: author			date			remarks
						jiangym 		    20150415      created
******************************************************************************/
char * FGets(char *pszBuf, int nMaxLen, int nFp)
{
	char *p;
	int nC = 0;

	if(pszBuf ==  NULL || nMaxLen <= 0)
		return NULL;
	p = pszBuf;
	while(--nMaxLen)
	{
		if((nC = FGetC(nFp)) == (-1))
			break;
		if ((*p++ = nC) == '\n')
			break;
	}
	if (p > pszBuf)
	{
		*p = 0;
		return pszBuf;
	}
	return NULL;
}
/******************************************************************************
*	function name 	 	:GetParaInfo
*	functional description :从文件中获取需要的信息
*	input parameter	 	:char *ConfName 是要查找的字符串
*	output parameter	 	:char *pInfo 查找到字符串对应的那一行信息
*	return value		 	:
*						若成功则返回SUCC
*						若失败返回FAIL
*     history 		 		: author			date			remarks
						jiangym 		    20150415      created
******************************************************************************/
int GetParaInfo(char *ConfName, char *pInfo)
{
	char szBuf[256] = {0};
	int fp = -1;

	if ((fp = NDK_FsOpen(PARACONF, "r"))<0)
	{	
		cls_show_msg("file:%s,line:%d打开参数配置文件失败(%d))",__FILE__ ,__LINE__,fp);
		return FAIL;
	}
	while(1)
	{
		memset(szBuf, 0, sizeof(szBuf));
		if(FGets(szBuf,sizeof(szBuf)-1,fp) == NULL)	//按行取信息
			break;
		
		szBuf[strlen(szBuf)-1] = '\0';
		if(strstr(szBuf, ConfName) != NULL)	//查找所需的信息
		{		
			strcpy(pInfo, szBuf );		
			NDK_FsClose(fp);		
			return SUCC;
		}	
	}
	NDK_FsClose(fp);
	cls_show_msg1_record(FILENAME,FUNCNAME,10,"获取%s参数失败", ConfName);//为WiFi兼容性测试改成获取参数失败直接保存文件后继续执行 20170927
	return FAIL;
}
/******************************************************************************
*	function name 	 	:trim_string2int
*	functional description :读取"="右边的字符串并转化为整形数
*	input parameter	 	:char *inbuf 是输入的字符串
*	output parameter	 	:char *numbuf 输出"="右边的并转为整形数
*	return value		 	:
*
*     history 		 		: author			date			remarks
						jiangym 		    20150415      created
******************************************************************************/
int trim_string2int(char *inbuf, int *numbuf)
{
	char *tempnum;
	if((tempnum = strchr(inbuf, '=')) != NULL)	
	{
		tempnum++;
		if(strlen(tempnum) == 0)
		{
			cls_show_msg("file:%s,line:%d参数配置项%s内容为空,请在配置文件中赋值",__FILE__ ,__LINE__,inbuf);
			return FAIL;
		}
		else
			*numbuf = atoi(tempnum);
	}
	return SUCC;
}
/******************************************************************************
*	function name 	 	:trim_string
*	functional description :读取"="右边的字符串
*	input parameter	 	:char *inbuf 是输入的字符串
*	output parameter	 	:char *numbuf 输出"="右边的字符串
*	return value		 	:
*
*     history 		 		: author			date			remarks
						jiangym 		    20150415      created
******************************************************************************/
int trim_string(char *inbuf, char *outbuf)
{
	char *tempout;
	if((tempout = strchr(inbuf, '='))!= NULL)	 
	{
		tempout++;		
		if(strlen(tempout) == 0)
		{
			cls_show_msg1(10,"file:%s,line:%d参数配置项%s内容为空,请在配置文件中赋值",__FILE__ ,__LINE__,inbuf);
			return FAIL;
		}
		else
			strcpy(outbuf, tempout);
	}
	return SUCC;
}
/******************************************************************************
*	function name 	 	:trim_left_string
*	functional description :读取"="左边的字符串
*	input parameter	 	:char *inbuf 是输入的字符串
*	output parameter	 	:char *outbuf 输出"="左边的字符串
*	return value		 	:
*
*     history 		 		: author			date			remarks
						jiangym 		    20171101      created
******************************************************************************/
void trim_left_string(char *inbuf, char *outbuf)
{
	char *tempout;
	char tempbuf[80]= {0},  tempin[80] = {0};
	
	strcpy(tempin, inbuf);
	if((tempout = strchr(inbuf, '='))!= NULL)	 
	{	
		strcpy(tempbuf, tempout);
	}
	strncpy(outbuf,tempin, strlen(tempin)-strlen(tempbuf));
	return ;
}

/******************************************************************************
*	function name 	 	:trim_char2uchar
*	functional description :读取"="右边的字符串并转化成mac数组
*	input parameter	 	:char *inbuf 是输入的字符串
*	output parameter	 	:uchar *outbuf 输出mac数组
*	return value		 	:
*
*     history 		 		: author			date			remarks
						jiangym 		    20160308      created
******************************************************************************/
int trim_char2uchar(char *inbuf, uchar *outbuf)
{
	char tempout[32];
	int i = 0, j = 0, k =0;
	if(strlen(inbuf)<=0)
	{
		cls_show_msg("file:%s,line:%d字符串为空",__FILE__ ,__LINE__);
		return FAIL;
	}
	else 
	{
		for(i=0,j=0;i<strlen(inbuf);i++)
		{
			if(inbuf[i]!=':')
			{
				if((inbuf[i]>='0')&&(inbuf[i]<='9'))
					tempout[j] = inbuf[i]-0x30;
				else if((inbuf[i]>='A')&&(inbuf[i]<='F'))
					tempout[j] = inbuf[i]-'A'+10;
				else if((inbuf[i]>='a')&&(inbuf[i]<='f'))
					tempout[j] = inbuf[i]-'a'+10;
				else
					return FAIL;
				j++;
			}
		}
	}
	for(i=0;i<12/*strlen(tempout)*/;i++)//strlen(tempout)遇到0会结束,如果mac带00会影响Mac的输出故写死成循环12次
	{
		k=i/2;j=i%2;
		if(j==0)
			outbuf[k]=tempout[i]<<4;
		else
			outbuf[k]+=tempout[i];
	}
	return SUCC;
}
#endif

