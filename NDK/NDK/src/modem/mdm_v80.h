/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-v80协议
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */

#ifndef MDM_V80_INCLUDE_H
#define MDM_V80_INCLUDE_H

#define V80_EM_CHAR_NUM     4       //转义字符数

//#define MODEM_DEBUG_V80
#ifdef MODEM_DEBUG_V80
#define V80DEBUG(format, args ...)  fprintf(stderr, format, ## args)
#else
#define V80DEBUG(format, args ...)
#endif

//SDLC帧类型
typedef enum {
    SDLC_FRAME_TYPE_ERR = 0,        //错误帧初始化使用
    SDLC_FRAME_TYPE_SNRM,           //SNRM帧
    SDLC_FRAME_TYPE_RR,             //RR帧
	SDLC_FRAME_TYPE_REJ,             //REJ帧
    SDLC_FRAME_TYPE_UA,             //UA帧
    SDLC_FRAME_TYPE_I,              //I帧
    SDLC_FRAME_TYPE_INVALILD,       //19 B2错误I帧
} EM_SDLC_FRAME_TYPE;

typedef enum {
    V80_ESCAPE_EM19 = 0x19, //19转义字符控制
    V80_ESCAPE_EM99 = 0x99, //99转义字符控制
    V80_ESCAPE_DC1 = 0x11,  //11转义字符控制
    V80_ESCAPE_DC3 = 0x13,  //13转义字符控制
} EM_V80_ESCAPE;

//1Synchronous Access Mode In-Band Commands
typedef enum {
    V80_CMD_MARK = 0xb0,    //103:begin transparent sub-mode,104:HDLC Abort detected in Framed sub-Mode
    V80_CMD_FLAG = 0xb1,    //103:transmit a flag; enter Framed sub-Mode if currently in Transparent sub-Mode. If enabled, precede with FCS if this follows a non-flag octet sequence
    //104:Non-flag to flag transition detected. Preceding data was valid frame; FCS valid if CRC checking was enabled
    V80_CMD_ERR = 0xb2,     //103:transmit Abort,104:Non-flag to flag transition detected. Preceding data was not a valid frame
    V80_CMD_HUNT = 0xb3,    //103:put receiver in hunt condition,104:– not applicable –
    V80_CMD_UNDER = 0xb4,   //103:– not applicable –,104:transmit data underrun
    V80_CMD_TOVER = 0xb5,   //103:– not applicable –,104:transmit data overrun
    V80_CMD_ROVER = 0xb6,   //103:– not applicable –,104:receive data overrun
    V80_CMD_RESUME = 0xb7,  //103:resume after transmit underrun or overrun,104:– not applicable –
    V80_CMD_BNUM = 0xb8,    //103:– not applicable –,104:the following octets,<octnum0><octnum1 >,specify the number of octets in the transmit data buffer
    V80_CMD_UNUM = 0xb9,    //103:– not applicable –,104:the following octets,<octnum0><octnum1 >,specify the number of discarded octets

    //103:duplex carrier control,104:duplex carrier status
    V80_CMD_EOT = 0xba,     //103:terminate carrier, return to command state,104:loss of carrier detected, return to command state
    V80_CMD_ECS0 = 0xbb,    //103:go to on-line command state,104:confirmation of <EM><esc>command
    V80_CMD_RRN = 0xbc,     //103:Request rate reneg. (duplex),104:indicate rate reneg. (duplex)
    V80_CMD_RTN = 0xbd,     //103:Request rate retrain (duplex),104:indicate rate retrain (duplex)
    V80_CMD_RATE = 0xbe,    //103:following octets, <tx><rx>,set max. tx and rx rates,
    //104:retrain/reneg. completed;following octets, <tx><rx>,indicate tx and rx rates

    //103:V.34 HD carrier control，104:V.34 HD duplex carrier status
    V80_CMD_PRI = 0xbc,     //103:go to primary ch. operation,104:pri. ch. operation commenced;following octet, <prate>,indicates bit rate
    V80_CMD_CTL = 0xbf,     //103:go to control ch. operation,104:ctl. ch. operation commenced;following octets,<prate><crate>, indicates bit rates
    V80_CMD_RTNH = 0xbd,    //103:initiate pri. channel retrain,104:indicate pri. channel retrain
    V80_CMD_RTNC = 0xc0,    //103:initiate ctl. channel retrain,104:indicate ctl. channel retrain
    V80_CMD_RATEH = 0xbe,   //103:following octets, <maxp> <prefc>, set max. pri, rate and preferred ctl. ch. rate,104:
    V80_CMD_EOTH = 0xba,    //103:terminate carrier,104:carrier termination detected
    V80_CMD_ECS1 = 0xbb,    //103:go to command state,104:
} EM_V80_CTRL;

//SDLC控制字符
typedef enum {
    SDLC_CTRL_START = 0x30,
    SDLC_CTRL_UA = 0x73,
    SDLC_CTRL_SNRM = 0x93,
} EM_SDLC_CTRL;

typedef struct {
    EM_V80_CTRL CtrlByte;
    int     offset;
} ST_CTRLBYTE_OFFSET;

typedef struct {
    EM_SDLC_FRAME_TYPE  type;           //同步帧类型
    char            recno;          //希望接收序号
    char            sendno;         //发送帧序号
    char *          inbuf;          //输入buf缓冲
    int         inlen;          //输入数据长度
    char *          outbuf;         //输出缓冲区
    int         outmaxlen;      //允许输出的最大长度
    int         outlen;         //输出数据长度
} ST_SDLC_V80;


int mdm_sdlc_pack(ST_SDLC_V80 *pstSdlc);
int mdm_sdlc_unpack(ST_SDLC_V80 *pstSdlc);

#endif
