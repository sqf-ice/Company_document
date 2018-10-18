#ifndef _WLAN_H_
#define _WLAN_H_

#define IPV4_ADDR_LEN       15
#define AP_NUM_MAX          100

#define FAIL (-1)
#define SUCC 0
#define FALSE 0
#define TRUE (!FALSE)

#define KEYFILE "/bin/busybox"

//#define DEBUG 1
#ifdef DEBUG
#define PDEBUG(fmt, args...) fprintf(stderr,  "WIFI-NDK: "fmt, ##args)
#else
#define PDEBUG(fmt, args...)
#endif


typedef enum
{
	GetWiFiOpenStatus_FUNC,
	GetWiFiScanList_FUNC,
	GetWiFiMac_FUNC,
	GetWiFiServStatus_FUNC,
}DBUS_WIFI_METHOD_NAME;

typedef struct tWiFiIP
{
	char wifi_ip[IPV4_ADDR_LEN + 1];
	char wifi_netmask[IPV4_ADDR_LEN + 1];
	char wifi_gateway[IPV4_ADDR_LEN + 1];
	char wifi_dns_primary[IPV4_ADDR_LEN + 1];
	char wifi_dns_secondary[IPV4_ADDR_LEN + 1];
}T_WiFiIP;


typedef struct tWiFiConnectParam
{
	T_WiFiIP wifi_connect_ip;
	int key_type;
	char asEssid[128 + 1];
	char asKey[128 + 1];
	int encrypt_type;
	int if_DHCP;
}T_WiFiConnectParam;


#endif

