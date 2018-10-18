#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_
#include "NDK.h"

extern int BT_init(void);
extern int BT_GetStatus(void);
extern int BT_SetPin(const char *pincode);
extern int BT_GetPin(char * pincode);
extern int BT_Shutdown(void);
extern int BT_SetName(const char *Name);
extern int BT_GetName(char *Name);
extern int BT_SetMac(const char *Mac) ;
extern int BT_GetMac(char *Mac);
extern int BT_GetPairingStatus(char * pszKey, int *pnStatus);
extern int BT_PairingConfirm(const char * pszKey, uint unConfirm);
extern int BT_Disconnect(void);
extern int bt_read_server( uint unLen, char *pszOutbuf,int nTimeoutMs, int *pnReadlen);
extern int bt_write(uint unLen,const char *pszInbuf);
extern void bt_init_circ_buf(void);
extern int  bt_master_init_circ_buf(const char *mac);
extern void bt_read_circ_buf_length(int *pnReadlen);
extern int  bt_master_read_circ_buf_length(const char * mac,int *pnReadlen);
extern int bsa_set_config(unsigned char enable);
extern int bsa_set_security(unsigned char pairmode);
extern int BT_Scan(int device_type);
extern int BT_GetScanResults(const char *bt_name,ST_BT_DEV *bt_scan_results,unsigned int max_num,int *return_num);
extern int BT_StopScan(void);
extern int BT_Bond(const char * mac);
extern int BT_Connect(const char *bd_addr,int mode);
extern int bt_read_master(const char * mac, uint unLen, char * pszOutbuf, int nTimeoutMs, int * pnReadlen);
extern int bt_write_master(const char * mac,uint unLen, const char * pszInbuf);
extern int BT_Disconnect_master(const char * mac);
extern int BT_GetBondStatus(int *pnMode,char * pszKey,int *pnStatus);
extern int BT_BondConfirm(const char * pszKey,unsigned int unConfirm);
extern int BT_ConStatus(const char *bd_addr ,int *status);

#endif
