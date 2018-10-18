#ifndef _ICCARD_H_
#define _ICCARD_H_
#include "icc_define.h"
#include <linux/interrupt.h>
#define	SC_State_CardPull	1
#define	SC_State_CardInsert	2
#define	SC_State_Ready		3
#define	SC_State_Error		4


#define ATR_ColdStart_err   1017
#define poweroncheck_err	-1014	//- 上电检查出错
#define	Com_err				-1102
#define CardPull_err		-1103
#define CardNoReady_err		-1104
#define CaseProce_err		-1105
#define CaseOverflow_err	-1106

typedef struct {
	unsigned long WWT;
	unsigned long CWT;
	unsigned long BWT;
	unsigned int F;
	unsigned char D;
	unsigned char IFSC;
	unsigned char NAD;
	unsigned char GTR;
	unsigned char wtx;
	unsigned char T;
	unsigned char cwi;
	
	unsigned char IFD_Seq;
	unsigned char ICC_Seq;
	unsigned char IFSD_Negotiated;
	unsigned char ATROK;
	unsigned char Vcc;
}atr_t;


extern atr_t g_atr;
extern unsigned long g_iccard_insert_cnt ;

extern int icc_send_apdu(uint8_t *buffer, uint32_t length, uint8_t *rbuffer);
extern int icc_powerup(uint8_t *atrstr,uint8_t len,icc_standard_t standard);
extern int get_sc_state(void);
extern int get_sc_state_c(void);
extern irqreturn_t icc_isr(int irq, void *dev_id);
#endif
