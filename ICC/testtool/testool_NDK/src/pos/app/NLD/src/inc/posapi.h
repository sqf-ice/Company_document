/*
    FileName: posapi.h
	Ver 1.0
	Create By: Nizj 2008-7-15 9:15
	Mody By:
*/
#ifndef _POSAPI_H_
#define _POSAPI_H_

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* for struct socketaddr_in */
#include <time.h>
#include <dirent.h>

#include "parsecfg.h"
#include "define.h"

#ifndef NULL
#define NULL 0                /* see <stddef.h> */
#endif

#define EOF      (-1)

#define SEEK_SET 0 /* start of stream (see fseek) */
#define SEEK_CUR 1 /* current position in stream (see fseek) */
#define SEEK_END 2 /* end of stream (see fseek) */

//
//标准输入输出API
//
extern int pos_printf(const char * /*format*/, ...);
#define printf	pos_printf
//extern int sprintf(char * /*s*/, const char * /*format*/, ...);
extern int pos_scanf(const char * /*format*/, ...);
#define scanf	pos_scanf
//extern int sscanf(const char * /*s*/, const char * /*format*/, ...);
extern int pos_getchar(void);
#define getchar	pos_getchar
extern int pos_putchar(int);
#define putchar	pos_putchar

//用户程序入口、退出API
extern int	_nlmain(void);
extern void nlexit(int a);
#define shutdown()	nlshutdown()

//蜂鸣器API
extern void beep(void);
extern void _beep(unsigned frequency, unsigned miliseconds);
extern void Beepnum(int num);
extern void playmusic(const char * music);
// 蜂鸣器新增api接口
extern int setbeepvolumn(int volumn);    
extern int getbeepvolumn(void);
extern int getbeepversion(char ver[]);
extern int setbeepparame(int width, int time);/*设置按键音操作*/

//其他
extern int	GetErrorCode(void);
extern void getbiosver(char *ver);
extern int gethwver(unsigned char *ver);
extern void getguiver(char *ver);

//
// 串口通讯相关
//
extern int initaux(int port,int bps,int flags);
extern int portread(int port,int len,char *OutBuf,int seconds);
extern int portwrite(int port,int len,const char *InBuf);
extern int clrportbuf(int port,int rxtx);
extern int getreadlen(int usart_id);
extern int portPreReadChar(int usart_id);
extern void enableIrdaport(void);
#define USE_SDLC_MODEM
#ifdef USE_SDLC_MODEM
//wiremodem api 同步MODEM通讯(SDLC)   对于异步的情况可使用(ASYN)
extern int SelectModemCountry( int COUNTRYID);//可用结构查询方式
extern int	SDLC_getmodemstatus(void);
extern int SDLC_initmodem(char * patstr);
extern int SDLC_predial(char * patstr);
extern int SDLC_hangup(void);
extern int SDLC_write(char * pData , int datalen);
extern int SDLC_read(char * pData , int * datalen , int nSeconds);
extern int SDLC_clrportbuf(int rxtx);//不推荐使用
#endif

#define USE_MODEM
#ifdef USE_MODEM
//MODEM的基本通讯使用 可供异步时使用
extern int getmodemreadlen(void);
extern int modemportread(char * Outbuf,int len,int nSeconds );
extern int modemportwrite(char * data , int len);
extern int modemclrportbuf(int rxtx);
extern int relayforpredial(void);	//拨号前的继电器操作
extern int relayforhangup(void);	//挂机后的继电器操作
extern void relaysetvalid(int user_relayvalid);//决定是否进行并机检测，零不检测非零进行检测
extern int modemportPreReadChar(void);
extern void modemreset(void);
#endif

//
//打印相关API
//
extern int getprintertype(void);
extern const char * getprinterversion();
extern int printtest(void);
extern int printimage(int xsize,int ysize,int xpos,const char *ImgBuf);
extern int print(char *buf);	//以\0为结尾的串
extern int setprintfont(int);
extern int clrprintbuf(void);
extern int getprinterstatus(void);
extern int setprintmode(int mode,int row_space);
extern int setprintpagelen(int pagelen);
extern void ip_setautoresetpagesum(unsigned int pagesum);
extern int SetPrinterTabWidth(int nTabWidth);           // 设定制表符宽度
extern int FeedPrinterStepByStep(int nStepCounts);      // 实现正反向逐步走纸,line>0:进纸,<0:退纸,=0:不动。1点=2步 入口参数792取模
extern int SetPageTopMargin(unsigned int n);
extern int SetHPCheckMode(unsigned char m);
extern int SetDotTime(unsigned int n);
extern int SetMissStepDetectType(unsigned int n);
extern int GetMissStepDetectType(int * p_missStepDetectType);
extern int resetprint(void);
extern int clearprintbuf(void);
extern int printintegrate(int xsize,int ysize,int xpos,int ypos,char *ImgBuf,char *TextBuf, int mode);
extern int setprintgreyscale(int greyscale);
extern int IsPrinterBusy(void);
extern int setprintheatmode(int mode);
extern int setprintmotormode(int mode);

//
//按键相关API
//
extern int pos_getline(char *buf,int maxlen,int mode,int WaitTime);
#define getline	pos_getline
extern int getreturnline(char *buf,int maxlen,int mode,int WaitTime);
extern int getkey(int waittime);
extern int getkeycode(int time);
extern int kbhit(void);
extern int getnumstr(char *buf,int maxlen,int mode,int WaitTime);

extern int ComboKeyDetected(unsigned char *keyarray,int keycount,int tm_sec, int tm_usec);
/*按键新增API*/
extern unsigned int BackLightSwitch(int state);    		/*背光灯开关函数。   0：关闭背光；	非零为每次背光灯的持续时间*/
extern unsigned int KeyBeepSwitch(int state); 			/*按键音开关函数。   0：关闭按键音；1：打开按键音*/
extern int getkeypadversion(char ver[]);				/*获取键盘驱动版本号*/
extern unsigned int SetAutoSendKey(int time);   		/*设置自动重发时长*/

//
//电子盘相关API
//
extern void	format(void);
extern int IsVdiskFormat(void);
extern long diskspace(void);
//extern long disktotalspace(void);

//文件、目录操作API
extern int fdel(const char *name);
extern int dir(char *buffer);
extern void DispFileErrorCode(char *s);
extern void sendfilepwd(char *buffer);
extern int changefilepwd(char *filename, char *oldpwd, char * newpwd);
extern int setfilename(const char *name,int len);
extern int setfilelen(const char *filename, long newlen);
extern long filelength(int hFile);

extern int pos_fopen(const char *filename,const char *mode);
extern int pos_fclose ( int handle );
extern int pos_fseek ( int handle, long distance, int position );
extern int pos_fread ( char * buffer, int length, int handle );
extern int pos_fwrite ( const char * buffer, int length, int handle );
extern unsigned long pos_ftell(int hnd);
extern int pos_rename(const char *srcname,const char *dstname);
extern int pos_access(const char *filename);

#ifndef	__LINUX_NATIVE_FILE_
#define	fopen(name,mode)			pos_fopen(name,mode)
#define fclose(hnd)					pos_fclose (hnd)
#define fseek(hnd,offset,origin)	pos_fseek(hnd,offset,origin)
#define fread(s,len,hnd)			pos_fread(s,len,hnd)
#define fwrite(s,len,hnd)			pos_fwrite(s,len,hnd)
#define ftell(hnd)					pos_ftell(hnd)
#define rename(srcname, dstname)	pos_rename(srcname, dstname)
#define access(filename)			pos_access(filename)
#endif

//
//USB相关功API
//
extern int UsbDiskOpen(void);
extern int UsbDiskClose(void);
extern int UsbDiskGetRootdirName(char *rdn);
extern int UsbDiskGetRootdirStruc(DIR **dp);

extern void close_usb_aux(void);
extern int open_usb_aux(unsigned short sec);
extern int read_usb_aux(char *byRecBuf, int nCount, int nSeconds);
extern int write_usb_aux(const char *bySendBuf, int nCount);

//
//LCD相关API
//
extern void gotoxy(int,int);
extern int  wherex(void);
extern int  wherey(void);
extern void pushscreen(void);
extern void popscreen(void);
extern void clrscr(void);
extern void getimage(int x0,int y0,int width, int height, char *buf);
extern void putimage(int x0,int y0,int width, int height, const char *buf);
extern int  getpixel(unsigned int x, unsigned int y);
extern void putpixel(unsigned int x, unsigned int y, int color);
extern void line(long lX1, long lY1, long lX2, long lY2, int cColor);
extern void rectangle(int nX1, int nY1, int nX2, int nY2, int fill_pattern, int cColor);
extern int settextattr(int textattr);
extern void DispASC8x8(int x,int y,char c);
extern void DispString(int x,int y,const char *s);
extern void DispString12(int x,int y,const char *s);
extern void DispASC8x8Str(int x,int y,const char *s);
#define Disp8x8Str	DispASC8x8Str		//兼容税控
extern void disprevstr(const char * str);
extern int disp_reset(void);
extern int GetCharacterDotMatrix(int outtype, char *codebuf,int font, int rotatetype,TYPEDOTMATRIX *outbuf);
extern void SetBackLightState(int state);
extern int disp_lock(int flag);
extern void tsc_shift(char flag);
extern void change_font(int type);
extern int font_add(char *fontlib_type, char *cfont_path, char *afont_path);
extern void font_choose(char *fontlib_type,int fit);
extern char * get_current_exfont(void);
extern int change_theme(const char *themepath);
extern int get_font_size(void);
//LED灯控制
extern void TurnOnLedAndDelay(int nseconds);
extern void TurnOnLed2AndDelay(int nseconds);
extern int SetLEDstatus(unsigned char red, unsigned char green, unsigned char yellow, unsigned char blue);
//新增LED接口
extern void TurnOnAnyLedAndDelay(int lednum, int nseconds);
extern int GetLEDVersion(char *p);		//获取LED驱动版本号
extern int SetAnyLedStatus(unsigned char lednum, unsigned char state); //操作指定灯进入亮\暗\闪烁三种中任一状态

//以下函数仅作特效时使用，不推荐使用
extern void TurnOnLCD(void);
extern void TurnOffLCD(void);
extern void SetLCDstartRow(int nStartRow);

//
// 掉电保护数据相关API
//为了提高掉电保护数据区的寿命每次保存时数据尽量集中一次写入，减少写入次数
//
extern int getvar(char *obuf,int off,int len);
extern int savevar(const char *ibuf,int off,int len);
extern int getframtype(void);	// 获取FRAM的类型 8k 2k 0k	对应为getframtype

//
//  磁卡、IC卡API
//
extern int  readcard(int type, int tracks, int keylen, 	int waittime,
                     char *buf2, char *buf3);//读23磁道
//读123磁道 //第3个Buf为一磁道 其格式同二三磁道 tracks的值参照define.h
extern int  readcard3(int type, int tracks, int keylen, int waittime,
                      char *track2, char *track3 , char *track1 ) ;
extern int magstripe_read(char flag, int timeout, int pipefd, char * tk1, char * tk2, char * tk3);
extern int SetTrackDecodeType(int iTrack, unsigned int uiDecodeType);
extern int GetTrackDecodeType(unsigned int* uiDecodeType, int iTrack);
extern int SetICType(int c1Type,int c2Type);
extern int GetICType(char *ICType );
extern int icpowerup(int cardno,char *buf);
extern int icpowerdown(int cardno);
extern int iccrw ( int cardno, int sendlen, char *sendbuf, char *recebuf);
extern int iccrw_new(int cardno, int sendlen, char *sendbuf, int *rcvlen, char *recebuf);
extern int getDS5002state(void);
extern int resetDS5002(void);
extern int getDS5002ver(char *VerBuf);

extern int mi_iccrw(int sendlen, unsigned char *sendbuf, unsigned char *recebuf);

//memory card
extern int init_mem_card(void);
extern int mem_card_powerup(unsigned char memerycardno, unsigned char *atrbuf);
extern int mem_card_powerdown(unsigned char memerycardno);
extern int mem_card_check(void);
extern int mem_card_pwr_chk(void);
extern int rw_mem_card_check(void);
extern int mem_card_rw(unsigned char memerycardno, int sendlen, unsigned char *sendbuf, int *rcvlen, unsigned char *recebuf);

//cicc
extern int cicc_removal(void);
extern int cicc_pcdinit(void);
extern int cicc_picchalt(unsigned char piccmode);
extern int setpiccquickseek(unsigned char seekmodel);

extern int rfid_ProtocolSet(int type, int *reg);
extern int rfid_ProtocolGet(int type, int *reg);	


//扫描头相关
extern void ScannerInit(void);
extern void ScannerWakeup(void);
extern void ScannerTrig(void);
extern int  ScannerSetIO(int pin, int value);		//pin 取值:EM3K_TRIG或EM3K_WKUP

//
// 安全模块
//
extern int initkey(void);
extern int saveplainkey(int groupno, int serialno, char *key, char typeflag);
extern int savekey(int groupno, int serialno, char *key,int ugroupno,int userialno);
extern int saveplainkey2(int groupno, int serialno, char *key, char typeflag);
extern int savekey2(int groupno, int serialno, char *key, int ugroupno,int userialno);
extern int des(int direction, int groupno, int serialno, char *ibuf, char *obuf);
extern int des3(int direction, int groupno,	int serialno, char *ibuf, char *obuf);
extern int mdes(int direction, unsigned char *ibuf, unsigned char *obuf , unsigned char *ikey);	//软DES用于高速加密
extern int mdes3(int direction, unsigned char *ibuf, unsigned char *obuf , unsigned char *ikey);

//
// 时间相关类函数
//
extern void delay(int);		//以100ms为单位
extern void msdelay(int nMiliSeconds);		//以1ms为单位
//extern time_t time(time_t *timer);
extern void  StartStopwatch(void);
extern float ReadStopwatch(void);
extern void EnableRTC(int x,int y);
extern void DisableRTC(void);
extern int	getpostime(struct postime *pt);
extern int	setpostime(struct postime pt);
extern int SetRTCstyle(int style);

//序列号读写
extern int ReadSerialNo(char *cReadBuf);

#if 1
#define u_char		unsigned char
#define	u_int		unsigned int
#define u_short 	unsigned short
#define u_long		unsigned long

//8200 085新增API
extern int OneHZInput(char *s,int method);	//输入法增加

#ifdef USE_ETHERNET
extern int EthOpen(void);			//使得以太网芯片开始运行
extern int EthClose(void);			//使得以太网芯片停止运行
extern u_char SetupETHAddress(char * pstrIP,char * pstrMask,char * pstrGateway);//设置本地Ip地址子网掩码网关
//extern int setNETdevice(int dev_id);	//选择网络设备 对协议栈进行初始化
extern int SelectNetDev(int devtype);
#define setNETdevice	SelectNetDev	//for compatible
#define setPPPdevice	SelectNetDev	//for compatible
extern int getethmacaddr(unsigned char * macaddr);	//获取以太网MAC地址
extern int setethmacaddr(const unsigned char * macaddr);	//设置以太网MAC地址

//************************************************
//*	DNS接口(VB202P)
//************************************************

extern int Add_DNS_Server(char* name,int where);
extern int Del_DNS_Server(char* name);
extern struct hostent *gethostbyname(const char *name);
#endif

extern u_long hostAddr(register const char *cp);	//ip地址字符串到整数转换

//GPRS使用的api
extern int tcpOpen(void);
extern int tcpClose(u_int td);
extern int tcpBind(u_int td, struct sockaddr_in *myAddr);
extern int tcpConnectJiffy(u_int td, const struct sockaddr_in *remoteAddr, u_char tos, u_int timeout);
extern int tcpWait(u_int td);
extern int tcpWriteJiffy(u_int td, const void *s1, u_int len, u_int timeout);
extern int tcpReadJiffy(u_int td, void *s1, u_int len, u_int timeout);

#if 0
extern int pppOpen(int fd);
extern int pppClose(int pd);
extern u_long getLocalAddr(void);
extern void netSetLogin(const char *luser, const char *lpassword);
extern int getPPPphase(int ph);
extern int getPPPerrorcode(int ph);
extern void ShowSignalQuality(int x, int y);
extern int getcopswirelessmodem(char * respnetname );
extern int getsqwirelessmodem(void);
extern int icmp_echoreq(void);	//可用于心跳处理
#endif
#endif

//摩擦针式打印
extern int FeedPrinterStepByStep(int nStepCounts);		//实现正反向逐步走纸,line>0:进纸,<0:退纸,=0:不动。1点=2步 入口参数792取模
#define printerfeedpaper FeedPrinterStepByStep	// 与NL660兼容
extern int SetBMStepCounts(int n);	//设置黑标检测步数计数器
#define setfeedoffset SetBMStepCounts			// 与NL660兼容
extern int GetBMStepCounts(void);	//读取黑标检测步数计数器
#define getfeedoffset GetBMStepCounts			// 与NL660兼容
// int SetPrinterTabWidth(int nTabWidth);// 设定制表符宽度
#define printersettabvalue	SetPrinterTabWidth	// 与NL660兼容

//LOG API
extern void ClearLog(void);//将所有的log记录清除
extern int AddLog(char *Log,char cLogflag,unsigned char ucLogLength);
extern int ReadLog(unsigned char cLogFlag,char *cBuf,int iDate);

/***********************************************************************
 多应用
***********************************************************************/
#ifdef USE_MULTIAPP_V1
extern int	GetPrgCreatedTime(int nUsrPrgType, char *pcReadBuf2);
extern int UpdateUserProgram(char *szFilename,char cType);
extern void FeedRewindKeyCtrl(unsigned char cFlag);
extern int GetUserPrgInfo(const char *pDespBuf, USRPRGINFO *pUsrPrgInfo, int mode);
extern void RunProgram(int nPrgEntry);
extern void * SYS_CallAPI(int nPrgEntry, int FuncNo);
extern int EraseUserPrgInfo(char * pDespBuf);
#endif

/************************************************************************/
/* V2版多应用相关接口                                                   */
/************************************************************************/
extern int RunProgramByName(const char*sAppName,int nFuncNo);
extern int GetNextProgramName(char*sAppName, int len, char* cAppType, int iPos);
extern int GetProgramInfo(const char* sAppName, V2USRPRGINFO* pPrgInfo);
extern int AddUserProgram(V2USRPRGINFO* pPrgInfo);
extern int DelUserProgram(const char *AppName);
extern int UpdateUserProgram(V2USRPRGINFO* pPrgInfo);
extern int GetUserPrgNum(void);
extern int GetProductionName(char *psName);
extern int GetOneAppInfo(V2USRPRGINFO *pAppInfo, int index);
extern int ExtractAppFile(char *pPackHeader, char *pstrFilePath);
#ifdef	CONFIG_NLGP730_L1
extern int ExtractTarFile(char *pPackHeader, char *pstrFilePath);
#endif
//extern int UpdateV2UserProgram (UPDATECMD* plocalCmd, unsigned int uiCmdCount);
//extern unsigned long getmodelblockerrorcode(int modelblockID);
//extern int InitLibMemory(char*sLibName);
extern int send_to_child(void *data, int len);
extern int send_to_master(void *data, int len);
extern int get_from_child(void *data, int len);
extern int get_from_master(void *data, int len);

/***********************************************************************
 配置信息相关
***********************************************************************/
extern int config_init(void);
extern void config_exit(void);
extern int getconfig(const char * confname, cfgValueType type, void * confvalue);
extern int setconfig(const char * confname, cfgValueType type, const void * confvalue);
extern int gethardwareinfo( int flag , int reqlen , unsigned char *usroutput );//获取POS当前状态信息

#if 0
extern unsigned int g_language; /* 1 english, 0 chinese */
extern unsigned int g_beepvolumn;
extern unsigned int g_startclock;
extern unsigned int g_autorun;
extern char * g_userpasswd;		/* encrypted user password */
extern char * g_superpasswd;		/* encrypted super password */
extern unsigned int g_lcd_contrast;		/* lcd contrast */
extern unsigned int g_lcd_coloffset;		/* lcd col offset */
extern unsigned int g_lcd_coldir;			/* lcd col direction */
extern unsigned int g_lcd_rowdir;			/* lcd row direction */
extern unsigned int g_commun_type;			/* comunication type */
extern unsigned int g_wireless_type;			/* wiless modem type: gprs or cdma */
extern unsigned int g_hip_compensation;         /* hip compensation value */
extern unsigned int g_hip_printer_quality;      /* hip printer quality */
extern int g_loststep_mode_value;      /* lost step mode value */
extern char * g_ipaddr;		/* ip address */
extern char * g_netmask;	/* netmask */
extern char * g_gateway;	/* gateway */
extern int g_enable_selftest;		/* enable selftest */
extern int g_enable_magtest;		/* enable magcard test */
extern int g_enable_filetest;		/* enable file test */
extern int g_enable_timeset;		/* enalbe time set */
#endif

//定时器API
extern int	SetTimerCallBack(int nSeconds, void(*func)(void));
extern int GetConfigforTimerCallBack(int ConfigID,void *pData,int datalen);
extern int SetConfigforTimerCallBack(int ConfigID,void *pData,int datalen);

extern void get_hw_info(struct pos_info *info);
extern int GotoSleep(void);
extern int set_sleep_switch(int enable);

extern unsigned int Get_MachineLanguage();
#endif

/* end of posapi.h */
