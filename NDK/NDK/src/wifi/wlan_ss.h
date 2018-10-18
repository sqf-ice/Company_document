#ifndef _WLAN_SS_H_
#define _WLAN_SS_H_

#define WLAN_DEV_NAME "/dev/wifi_ss"

#define WIFI_SS_IOC_MAGIC 'W'

#define WIFI_SS_IOCS_DAIL_CMD       _IO(WIFI_SS_IOC_MAGIC, 0)
#define WIFI_SS_IOCS_HANGUP_CMD     _IO(WIFI_SS_IOC_MAGIC, 1)
#define WIFI_SS_IOCS_SUCC_CMD       _IO(WIFI_SS_IOC_MAGIC, 2)

extern  int SendWifiConnectCmd(void);
extern	int SendWifiShutdownCmd(void);
extern	int SendWifiSuccCmd(void);

#endif
