/*
 * FileName:       
 * Author:         zhangnw  Version: 1.6.12  Date: 2009-12-27
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 */
#ifndef MIFARE_1_H
#define MIFARE_1_H

/*-----------------------includes-------------------------------*/

/*---------------constants/macro definition---------------------*/
#if 0
/*1、非接触卡部分 */
#define	MI_OK							(0)	//命令调用成功
#define	MI_NOTAGERR					(-1)  //0xff    感应区内没有射频卡 
#define	MI_CRCERR						(-2)	//接收CRC错误
#define	MI_EMPTY						(-3)  //0xfd    值溢出 
#define	MI_AUTHERR						(-4)  //0xfc    认证错误(M1) 
#define	MI_PARITYERR					(-5)  //0xfb    奇偶校验错误 
#define	MI_CODEERR						(-6)	//和RF卡的通讯错误
#define	MI_SERNRERR					(-8)	//防冲突中卡序列号错误
#define	MI_KEYERR						(-9)  //0xf7    认证密钥错误   
#define	MI_NOTAUTHERR					(-10)  //0xf6    未认证(M1) 
#define	MI_BITCOUNTERR				(-11)	//接收比特数错误
#define	MI_BYTECOUNTERR				(-12)	//接收字节数错误
#define	MI_WriteFifo					(-13)  //0xf3    写RC531数据异常 
#define	MI_TRANSERR					(-14)	//命令Transfer执行错误
#define	MI_WRITEERR					(-15)	//命令Write执行错误
#define	MI_INCRERR						(-16)	//命令Increment执行错误
#define	MI_DECRERR						(-17)  //0xef    命令Decrement执行错误 
#define	MI_READERR						(-18)	//命令Read执行错误
#define	MI_OVFLERR						(-19)	//值溢出错误
#define	MI_FRAMINGERR					(-21)	//帧错误
#define	MI_ACCESSERR					(-22)  //0xea    RC531读写异常 
#define	MI_UNKNOWN_COMMAND			(-23)	//无效命令
#define	MI_ErrCOMMAND					(MI_UNKNOWN_COMMAND)  //0xe9    无效命令 
#define	MI_COLLERR						(-24)	//冲突错误
#define	MI_RESETERR					(-25)	//复位错误
#define	MI_INITERR						(-25)	//初始化错误
#define	MI_INTERFACEERR				(-26)	//接口错误
#define	MI_ACCESSTIMEOUT				(-27)	//访问超时
#define	MI_NOBITWISEANTICOLL			(-28)	//防冲突过程中的TagType码错误
#define	MI_PROTOCOLERR				(MI_NOBITWISEANTICOLL)  //0xe4    防冲突过程中的TagType码错误 
#define	MI_QUIT						(-30)	//命令被中断退出
#define	MI_RECBUF_OVERFLOW			(-50)	//接收缓冲区溢出
#define	MI_SENDBUF_OVERFLOW			(-53)	//发送缓冲区溢出
#define	MI_WRONG_PARAMETER_VALUE	(-60)	//参数错误
//#define	PARAERR						(-87)  //0xA9    C接口调用，参数错误(mi_iccrw) 
#define	Err_Command					(-88)  //0xA8    C接口调用，错误命令  
#define	Err_ioctl						(-89)  //0xA7    C接口调用，linux接口，错误命令
#define	MI_NY_IMPLEMENTED			(-100)	//没装密钥错误
#define	MI_CopyErr						(-124) //0x84    内核数据拷贝出错 

/*2、非接触卡和接触卡在 API移植中共同使用的错误代码： */
#define read_err      -1001   // 读器件83c26出错 
#define write_err      -1002  // 写器件83c26出错 
#define getbyte_err1    -1011  // 接收重发出错 
#define poweroncheck_err  -1014  // 上电检查(电压)出错 
#define ATR_RST_err     -1015  // ATR 期间 RST信号异常 
#define ATR_IO_err      -1016  // ATR 期间 IO信号异常 
#define ATR_ColdStart_err  -1017  // 冷复位无 IC卡回复 
#define ATR_TA1_err     -1018  // ATR 的 TA1出错 
#define ATR_TA2_err     -1019  // ATR 的 TA2出错 
#define ATR_TC2_err     -1020  // ATR 的 TC2出错 
#define ATR_TD2_err     -1021  // ATR 的 TA3出错 
#define ATR_TA3_err     -1022  // ATR 的 TA3出错 
#define ATR_TB3_err     -1023  // ATR 的 TB3出错 
#define ATR_TC3_err     -1024  // ATR 的 TC3出错 
#define ATR_WarmStart_err  -1025  // 热复位无 IC卡回复 
#define T1_IFSD_err     -1026  // T=1 时设置IFSD出错 
#define T1_Abort_err    -1027  // T=1 时 IC卡放弃 
#define CopyErr       -1028  // 内核数据拷贝出错 

#define PowerOn_err     -1100  // 上电出错 
#define PowerOff_err    -1101  // 下电出错  
#define Com_err       -1102  // 命令出错 
#define CardPull_err    -1103  // 卡拔出了 
#define CardNoReady_err   -1104  // 卡未准备好 
#define CaseProce_err    -1105  // CASE 处理过程出错 
#define CaseOverflow_err  -1106  // 数据溢出 

#define Open_err      -2000   // 打开设备出错(接触卡) 
#define para_err      -2001  // 参数出错 
#define timeout_err     -2002  // readcard 超时返回 
#define IcAlreadyUp_err   -2003  // IC卡已经上电了 
#define Open_err_rf     -2004   // 打开设备出错(非接触卡) 
#define RC531_InitErr    -2005   // RC531未配置或者硬件有故障 
#endif

//以下由zhangnw20091227 add for mifare-1
//mifare-1卡命令帧的一些定义
#define	SEQNR				(0)
//定义固定域在命令字节流中的偏移位置
#define	OFFSET_SEQNR		(0)
#define	OFFSET_CMD		(OFFSET_SEQNR+1)	//(1)
#define	OFFSET_STATUS		OFFSET_CMD
#define	OFFSET_DATALEN	(OFFSET_CMD+1)	//(2)
#define	OFFSET_DATA		(OFFSET_DATALEN+1)	//(3)
#define	LEN_FRAMEHEAD		OFFSET_DATA
#define	LEN_FRAMETAIL		(1)
#define	LEN_ATQA			(32)	//(2)
#define	LEN_SAK			(32)	//(1) //为防止出现UID这样的情况,也将其视为变长,给个大的值
#define	LEN_UID			(10)	//(4) //注意:UID是可变长的,可能是4,7,10等值,如,A卡为UID;B卡databuf[1]~[4]为UID,其它是应用及协议信息.故开大些
#define	LEN_KEY			(6)
#define	LEN_BLKDATA		(16)
#define	LEN_BLKINCDEC		(4)
#define	MAXLEN_DATA		(0xff)	//(256)
#define	MAXLEN_FRAME		(MAXLEN_DATA+8)

//以下Mifare-1命令定义根据设计文档规定
#define	CMD_INIT							(0xff)
#define	CMD_HALT							(0x45)
#define	CMD_VOID_DATALEN_SND			(0)	//无发送data
#define	CMD_VOID_DATALEN_RCV				(0)	//无接收data
#define	CMD_REQUEST						(0x41)
#define	CMD_REQUEST_DATALEN_SND			(1)
#define	CMD_REQUEST_DATALEN_RCV			(LEN_ATQA)	//(2)
#define	CMD_ANTICOLL						(0x42)
#define	CMD_ANTICOLL_DATALEN_SND		(1)
#define	CMD_ANTICOLL_DATALEN_RCV		(LEN_UID)	//(4)
#define	CMD_SELECT							(0x43)
#define	CMD_SELECT_DATALEN_SND			(LEN_UID)	//(4)
#define	CMD_SELECT_DATALEN_RCV			(LEN_SAK)	//(1)
#define	CMD_STOREKEYE2					(0x36)
#define	CMD_STOREKEYE2_DATALEN_SND		(8)
#define	CMD_STOREKEYE2_DATALEN_RCV		(0)
#define	CMD_LOADKEYE2						(0x37)
#define	CMD_LOADKEYE2_DATALEN_SND		(3)
#define	CMD_LOADKEYE2_DATALEN_RCV		(0)
#define	CMD_AUTH							(0x44)
#define	CMD_AUTH_DATALEN_SND			(7)
#define	CMD_AUTH_DATALEN_RCV			(0)
#define	CMD_AUTH_EX						(0x54)	//旧的命令码(0x4a)
#define	CMD_AUTH_EX_DATALEN_SND			(12)
#define	CMD_AUTH_EX_DATALEN_RCV			(0)
#define	CMD_BLKREAD						(0x46)
#define	CMD_BLKREAD_DATALEN_SND			(1)
#define	CMD_BLKREAD_DATALEN_RCV			(LEN_BLKDATA)		//(16)
#define	CMD_BLKWRITE						(0x47)
#define	CMD_BLKWRITE_DATALEN_SND		(LEN_BLKDATA+1)		//(17)
#define	CMD_BLKWRITE_DATALEN_RCV		(0)
#define	CMD_BLKINC							(0x48)
#define	CMD_BLKDEC							(0x49)
#define	CMD_BLKINCDEC_DATALEN_SND		(LEN_BLKINCDEC+1)	//(5)
#define	CMD_BLKINCDEC_DATALEN_RCV		(0)
#define	CMD_BLKTRANSFER					(0x38)
#define	CMD_BLKRESTORE					(0x39)
#define	CMD_BLKTRANSREST_DATALEN_SND	(1)
#define	CMD_BLKTRANSREST_DATALEN_RCV	(0)
#define	CMD_POWERDOWN					(0x4e)
#define	CMD_POWERDOWN_DATALEN_SND		(2)
#define	CMD_POWERDOWN_DATALEN_RCV		(0)
//以下CPU命令定义为了兼容海信的旧接口功能而新增for RFcard7
#define	HS_SETICTYPE						(0x80)
#define	HS_SETICTYPE_DATALEN_SND		(1)
#define	HS_SETICTYPE_DATALEN_RCV			(0)
#define	HS_REQUEST_TYPEB_DATALEN_RCV	(12)	
#define	HS_SELRESP_TYPEA					(0x68)
#define	HS_SELRESP_TYPEA_DATALEN_SND	(1)
#define	HS_SELRESP_TYPEA_DATALEN_RCV_MAX	(20)		//返回数据不定长且小于20
#define	HS_POWERUP_TYPEB					(0x82)
#define	HS_POWERUP_TYPEB_DATALEN_SND	(0)
#define	HS_POWERUP_TYPEB_DATALEN_RCV	(1)
#define	HS_APDURW							(0x69)
//#define	HS_APDURW_DATALEN_SND_MAX		(MAXLEN_DATA)
//#define	HS_APDURW_DATALEN_RCV_MAX		(MAXLEN_DATA)
#define	HS_POWERDOWN						(0x6a)
#define	HS_POWERDOWN_DATALEN_SND		(0)
#define	HS_POWERDOWN_DATALEN_RCV		(0)

//应用层扩展了返回状态码,-2000开始为应用层定义的错误码
#define	ERR_FRAME_BASE			(-5000)	//不与底层已定义的错误返回码冲突
#define	ERR_FRAME_SEQNR			(ERR_FRAME_BASE-1)
#define	ERR_FRAME_BCC				(ERR_FRAME_BASE-2)
#define	ERR_FRAME_LEN				(ERR_FRAME_BASE-3)

#define	ERR_CMD_BASE				(-6000)
#define	ERR_CMD_VOID				(ERR_CMD_BASE-1)	//无参命令(init,halt等)发生错误
#define	ERR_CMD_REQUEST			(ERR_CMD_BASE-2)
#define	ERR_CMD_ANTICOLL			(ERR_CMD_BASE-3)
#define	ERR_CMD_SELECT			(ERR_CMD_BASE-4)
#define	ERR_CMD_STOREKEYE2		(ERR_CMD_BASE-5)
#define	ERR_CMD_LOADKEYE2		(ERR_CMD_BASE-6)
#define	ERR_CMD_AUTH				(ERR_CMD_BASE-7)
#define	ERR_CMD_AUTH_EX			(ERR_CMD_BASE-8)
#define	ERR_CMD_BLKREAD			(ERR_CMD_BASE-9)
#define	ERR_CMD_BLKWRITE			(ERR_CMD_BASE-10)
#define	ERR_CMD_BLKINCDEC			(ERR_CMD_BASE-11)
#define	ERR_CMD_BLKTRANSREST		(ERR_CMD_BASE-12)
#define	ERR_CMD_POWERDOWN		(ERR_CMD_BASE-13)
#define	ERR_CMD_INIT_SETTYPE		(ERR_CMD_BASE-14)
#define	ERR_CMD_INIT_SETTYPE1	(ERR_CMD_BASE-15)
//以下命令定义为了兼容海信的旧接口功能而新增for RFcard7
#define	ERR_HS_SETICTYPE			(ERR_CMD_BASE-16)
#define	ERR_HS_SETICTYPE1			(ERR_CMD_BASE-17)
#define	ERR_HS_SELRESP_TYPEA		(ERR_CMD_BASE-18)
#define	ERR_HS_APDURW_LEN_SND	(ERR_CMD_BASE-19)
#define	ERR_HS_APDURW_LEN_RCV	(ERR_CMD_BASE-20)
#define	ERR_HS_POWERDOWN		(ERR_CMD_BASE-21)

#define	ERR_RW_BASE				(-10000)

#define	REQA				(0x00)
#define	WUPA				(0x26)	//推荐值,一般情况下需要使用WUPA
#define	AUTHKEY_TYPE_A	(0x60)
#define	AUTHKEY_TYPE_B	(0x61)
#define	AUTHKEY_TYPE_A1	(0x00)	
#define	AUTHKEY_TYPE_B1	(0x01)
#define	AUTHKEY			((uchar *)"\xff\xff\xff\xff\xff\xff")		//一般卡出厂都是全FF,若要修改要有精悍许可,测试中使用它即可//"\x30\x31\x32\x33\x34\x35"
#define	BLK02DATA_FF		((uchar *)"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff")
#define	BLK02DATA_ORI		((uchar *)"\x67\x45\x23\x01\x98\xba\xdc\xfe\x67\x45\x23\x01\x02\xfd\x02\xfd")
#define	BLK02DATA_INC1	((uchar *)"\x68\x45\x23\x01\x97\xba\xdc\xfe\x68\x45\x23\x01\x02\xfd\x02\xfd")
//新增M0卡的初始认证密钥(出厂默认密钥)
#define	AUTHKEY_M0		((uchar *)"\x49\x45\x4D\x4B\x41\x45\x52\x42\x21\x4E\x41\x43\x55\x4F\x59\x46")

/*----------global variables declaration("extern")--------------*/
extern int (*rf_rw)(int, uchar *, uchar *);

/*---------------functions declaration--------------------------*/
extern int  RF_pack_cmd(uchar cmd, uchar *data, int datalen, uchar *cmdstr);
extern int __CMD_Void(uchar type);
//封装出来的mifare-1的API接口
extern int CMD_Init(void);
extern int CMD_Halt(void);
extern int CMD_Request(uchar mode, uchar *out);
extern int CMD_Anticoll(uchar *out);
extern int CMD_Select(uchar *UID, uchar *out);
extern int CMD_StoreKeyE2(uchar mode, uchar sector, uchar *key);
extern int CMD_LoadKeyE2(uchar mode, uchar sector);
extern int CMD_Auth(uchar mode, uchar *UID, uchar sector, uchar block);
extern int CMD_Auth_Ex(uchar mode, uchar block, uchar *key, uchar *UID);
extern int CMD_BLKRead(uchar block, uchar *out);
extern int CMD_BLKWrite(uchar block, uchar *wdata);
extern int CMD_BLKInc(uchar block, /*uint*/uchar *wdata);
extern int CMD_BLKDec(uchar block, /*uint*/uchar *wdata);
extern int CMD_BLKTransfer(uchar block);
extern int CMD_BLKRestore(uchar block);
extern int CMD_Powerdown(ushort msec);
//以下命令定义为了兼容海信的旧接口功能而新增for RFcard7
extern int HS_Init(uchar type);
//extern int HS_SetICType(uchar type);
#define	HS_Request_TypeA 	CMD_Request
#define	HS_Anticoll_TypeA	CMD_Anticoll
#define	HS_Select_TypeA	CMD_Select
extern int HS_SelResp_TypeA(uchar *out, int *outlen);
extern int HS_Request_TypeB(uchar mode, uchar *out);
extern int HS_Powerup_TypeB(void);
extern int HS_APDUrw(uchar *apdu, int apdulen, uchar *out, int *outlen);
extern int HS_Powerdown(void);

//extern int setpiccquickseek(unsigned char seekmodel);//开发已在posapi.h中提供
extern int mi_iccrw1(int sendlen, uchar *sendbuf, uchar *recebuf);//应用层提供
#endif

