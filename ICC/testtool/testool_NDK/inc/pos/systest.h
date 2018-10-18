/*
 * FileName:       
 * Author:         zhangnw  Version: 2.0.1  Date: 2012-11-16
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 */
#ifndef SYSTEST_H
#define SYSTEST_H

/*-----------------------includes-------------------------------*/
#include "mode.h"	//"typedef.h"
#include "devices.h"

/*---------------constants/macro definition---------------------*/
#define	RESET_NO			(0x00)
#define	RESET_PPPOPEN		(0x01)
#define	RESET_TCPOPEN		(0x02)
#define	RESET_TCPCLOSE	(0x03)	//CLOSE标志是指先打开再关闭
#define	RESET_PPPCLOSE	(0x04)	//CLOSE标志是指先打开再关闭

#define	DATA16		"0123456789ABCDEF"

#if !K21_ENABLE //K21平台只支持绝对路径
	#define	LOGFILE  "testtool_result.txt"  //综合测试结果的记录文件
#else
	#define	LOGFILE  "/appfs/result.txt" //ME30 最大文件名长度限制为11,所以不能用上面的文件名
#endif

/*----------global variables declaration("extern")--------------*/
extern int g_keeptime;
extern uchar g_RF_bak[/*MAXLEN_DATA*/LEN_BLKDATA+8];
extern LinkSetting links[10];
extern NetworkSetting nets[10];

/*---------------functions declaration--------------------------*/
extern int g_CycleTime;
int GetCycleValue(void);
int GetCommPackLen(void);

#if (ETH_ENABLE||WLAN_ENABLE||NEW_WIFI_ENABLE)
int conf_conn_LAN(void);
void create_recv_only_packet(Packet *packet, char *buf);
void send_recv_press(uint h_tcp);
void recv_only_press(uint h_tcp);
void send_only_press(uint h_tcp);
#endif

#if PRN_ENABLE
int GetPrinterStatus(unsigned int timeout);
void print_triangle(int nPageWidth);
void print_triangle_384(void);
void print_testpage(void);
void print_rand(void);
void print_guo(void);
void print_fill(void);
void print_bill(void);
void print_verticalbill(void);
void print_blank(void);
void print_font(void);
void print_image(int MageWide, int MageHigh, int MagePon, const char *p);
void print_image_CCB128(void);
void print_enter(void);
void print_compress(void);
void print_landi(void);
void print_xinguodu(void);
void print_signlfont(void);
void prn_press(const char *title, PF func);
#endif

#if MAG_ENABLE
int MagcardReadTest(int select_TK, int IsDisplayed, int waittime);
int systest_MagCard(void);
#endif

void set_snd_packet(Packet *packet);
int  update_snd_packet(Packet *packet);
void init_snd_packet(Packet *packet, char *buf);

#if (SDLC_ENABLE || ASYN_ENABLE)
int mdm_send(char *buf, int len);
int mdm_rev(char *buf, int len, int timeout);
void mdm_clrportbuf_all(void);
void mdm_reset(void);
int mdm_init(void);
int mdm_dial(char* phonenum);
int mdm_hangup(void);
int mdm_dial_comm(char *dialstr, Packet *sendpacket, int rst, float *call_time_once, float *hangdowntime_once);
int MDM_NetDown(int nMdmType);
#endif

#if (NDKSOCK_ENABLE || NDKSSL_ENABLE)
int  sock_recv(uint s, char  *buf, int len, int  timeout);
int sock_send(uint s, char  *buf, int len, int  timeout);
#endif

#if (WLM_ENABLE && (NDKSOCK_ENABLE || NDKSSL_ENABLE))
int wireless_dial_comm(Packet *sendpacket, int flag);
#endif

int RFID_init(SMART_t type);
int RFID_detect(SMART_t type, uchar *UID, int *pUIDlen);
int RFID_detect_poll(SMART_t type, uchar *UID, int *pUIDlen,int nTimeOut);
int RFID_active(SMART_t type, uchar *UID, int *pUIDlen);
int RFID_APDUrw(SMART_t type, uchar *UID, int *cpurcvlen);
int RFID_deactive(SMART_t type, ushort msec);
//注:本函数一般只在能出测试前使用一次即可,在测试中一般不调用
//#define RFID_finish	NDK_RfidCloseRf
int RFID_finish(SMART_t type);

int select_ICSAM(void);
int ICSAM_detect(EM_ICTYPE cardno);
int ICSAM_APDUrw(EM_ICTYPE cardno);
#define ICSAM_powerup		NDK_IccPowerUp
#define ICSAM_powerdown	NDK_IccPowerDown

SMART_t select_smarttype(void);
int smart_detect(SMART_t type, uchar *UID, int *pUIDlen);
int smart_active(SMART_t type, uchar *UID, int *pUIDlen);
int smart_APDUrw(SMART_t type, EM_ICTYPE cardno, uchar *UID);
int smart_deactive(SMART_t type, EM_ICTYPE cardno, ushort msec);

void Local2Global(LinkType lnktype);
void Global2Local(LinkType lnktype);
#endif

int systest_SDCard(int disktype);
int systest_touch(void);

#if (NDKSOCK_ENABLE || NDKSSL_ENABLE)
void tcp_abnormal(void);
#endif

