#ifndef _ICC_DEFINE_H_
#define _ICC_DEFINE_H_
/*set ic card define to sam1*/
#ifdef CONFIG_SPU90
	#define	SC_IC1            (2)
	#define	SC_IC2            (1)
	#define	SC_SAM1           (2)
	#define	SC_SAM2           (3)
	#define	SC_SAM3           (4)
#else
	#define	SC_IC1            (0)
	#define	SC_IC2            (1)
	#define	SC_SAM1           (2)
	#define	SC_SAM2           (3)
	#define	SC_SAM3           (4)
#endif

#define  SPECIFIC_MODE_USE_DEFAULT_F_D   (1)      /* Use default iso7816  F and D value */
#define  SPECIFIC_MODE_USE_TA1_F_D       (2)      /* Use Card ATR TA1 byte F and D value */
#define  NEGOTIABLE_MODE                 (3)      /* Card is in Negotiable mode */
#define RPAR_RETRY_CNT                   (4)
#define TPAR_RETRY_CNT                   (4)

#define RECEIVE_DATA_MAX                  (300)
typedef enum {
	emv,
	iso7816,
}icc_standard_t;

#define ERR_ATR_START_TIMEOUT   -1001
#define ERR_ATR_BYTE_TIMEOUT    -1002
#define ERR_ATR_TIMEOUT         -1003
#define ERR_ATR_TS              -1004
#define ERR_ATR_PARITY          -1005
#define ERR_ATR_TCK_T0          -1006
#define ERR_ATR_TCK_T1          -1007
#define ERR_INVALID_SW          -1008
#define ERR_SEND_PARITY         -1009
#define ERR_RECV_TIMEOUT        -1010
#define ERR_RECV_PARITY         -1011
#define ERR_RECV_LRC            -1012
#define ERR_INVALID_NAD         -1013
#define ERR_INVALID_LEN         -1014
#define ERR_ABORT_T1            -1015
#define ERR_SETIFSD_FAILURE     -1016
#define ERR_INVALID_INF         -1017
#define ERR_MAX_RETRIED         -1018
#define ERR_IFSD_ERROR          -1019

#define ERR_BGT_ERROR           -1020
#define ERR_CWT_ERROR           -1021
#define ERR_RLEN_ERROR          -1022
#define ERR_BWT_ERROR           -1023
#define ERR_ROVERFLOW_ERROR     -1024
#define ERR_PPS_ERROR           -1025

#define ERR_ABORT_T1_TRANSACTION                -1200
#define ERR_INVALID_STATUS_BYTE                 -1201
#define ERR_POWERUP_ATR_CRC_FAILURE             -1203
#define ERR_RECEIVE_DATA_OVER                   -1204
#endif
