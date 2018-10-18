#ifndef _SAM_H_#define _SAM_H_#include "icc_define.h"typedef struct{	uint8_t power_stat;	uint8_t atr_stat; }sam_stat_t;
typedef struct {
        uint8_t bit_reverse;    /* bit logic convention */
        uint8_t t1;             /* T1 or T0 */
        uint8_t rec_byte;       /* received byte */
        uint8_t parity_count;   /* event parity counter */
        uint8_t IFSC;
        uint8_t NAD;
        uint8_t wtx;
        uint8_t IFD_Seq;
        uint8_t ICC_Seq;
        uint8_t IFSD_Negotiated;		uint8_t timer_factor; 		uint8_t D;             /* D factor */	//	sam_stat SAM[5]; /*SC_SAM1 = 2,SC_SAM3 = 4*/
        uint32_t etu_count;     /* ETU */
        uint32_t F;             /* F factor */
        uint32_t WWT;           /* work waiting time 960 * D * WI */
        uint32_t CWT;           /* character waiting time 2^CWI + 11 */
        uint32_t BWT;           /* block waiting time 2^BWI * 960 + 11 */
        uint32_t BGT;
        uint32_t GTR;		uint32_t start_cnt;		uint32_t wwt_cnt;		
}icc_ctrl_t;extern icc_ctrl_t *ctrl;
extern sam_stat_t sam_stat[5];
extern int sam_powerup(uint8_t *atrstr,uint8_t len,uint8_t samnum,icc_standard_t standard);
extern int sam_send_apdu(uint8_t *buffer, uint32_t length, uint8_t *rbuffer,uint8_t samnum);
#endif

