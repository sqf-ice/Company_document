#ifndef __LIBDEVMGR_H__
#define __LIBDEVMGR_H__

/* 设备类型名称 */
#define DEV_CLASS_CPLD 			"cpld"
#define DEV_CLASS_DISPLAY 		"disp"
#define DEV_CLASS_TOUCHSCREEN 		"ts"
#define DEV_CLASS_WIRELESS_MODEM 	"wlsmdm"
#define DEV_CLASS_WIFI 			"wifi"
#define DEV_CLASS_MODEM 		"mdm"
#define DEV_CLASS_ETHERNET 		"eth"
#define DEV_CLASS_MAG_CARD 		"mag"
#define DEV_CLASS_PRINTER 		"prn"
#define DEV_CLASS_RFID 			"rfid"
#define DEV_CLASS_SMART_CARD 	"sc"
#define DEV_CLASS_AUX 			"aux"
#define DEV_CLASS_SCANNER 		"scanner"
#define DEV_CLASS_MACHINE_TYPE 	"machine"
#define DEV_CLASS_PINPAD        "pinpad"
#define DEV_CLASS_USB           "usb"
#define DEV_CLASS_RFIDLED        "led"
#define DEV_CLASS_BT        "bt"
#define DEV_CLASS_GM        "gm"
#define DEV_CLASS_KDB			"keypad"

#define DEV_CLASS_SERIAL            "g_serial"
#define DEV_CLASS_STORAGE            "usb-storage"
#define DEV_CLASS_HCD            "za9l1_hcd"
#define DEV_CLASS_UDC            "za9l1_udc"
/*配置项名称*/
#define CFG_NAME_EXIST 	"exist"
#define CFG_NAME_AUTO 	"auto"
#define CFG_NAME_TYPE 	"type"
#define CFG_NAME_DRIVER "driver"
#define CFG_NAME_PARAM 	"param"

/**查询设备信息
 * 参数：
 * 	dev_class : 设备类别 （输入参数）
 * 	exist : 设备是否存在，0 - 不存在，1 - 存在 （输出参数）
 * 	type ：类型 （输出参数）, 如"thermal"代表"热敏"
 */
//extern void get_dev_info(void *dev_class, int *ret, int *exist, void *type);

/**加载设备驱动
 * 参数：
 * 	dev_class : 设备类别 （输入参数）
 * 	ret : 0 - 成功，1 - 失败 （输出参数）
 */
extern int load_driver_by_class(const char *dev_class);

/**卸载设备驱动
 * 参数：
 * 	dev_class : 设备类别 （输入参数）
 * 	ret : 0 - 成功，1 - 失败 （输出参数）
 */
extern int unload_driver_by_class(const char *dev_class);

/**加载设备驱动
 * 参数：
 * 	driver_name : 驱动名称 （输入参数）
 * 	ret : 0 - 成功，1 - 失败 （输出参数）
 */
extern int load_driver_by_name(const char *driver_name, const char *param);

/**卸载设备驱动
 * 参数：
 * 	driver_name : 驱动名称 （输入参数）
 * 	ret : 0 - 成功，1 - 失败 （输出参数）
 */
extern int unload_driver_by_name(const char *driver_name);

/**获取配置文件参数
 * 参数：
 * 	dev_class : 设备类型 
 *  	cfgname：配置项名称
 *  	cfgvalue：配置项值 (输出参数)
 *返回值: 0 - 成功，< 0 - 失败 
 */
extern int get_dev_info(const char *dev_class, const char *cfgname, char *cfgvalue);

/**设置配置文件参数
 * 参数：
 * 	driver_name : 驱动名称 （输入参数）
 * 	cfgname:	配置项名称 （输入参数）
 *  cfgvalue:	配置项值   （输入参数）
 */
extern int set_dev_info(const char *dev_class, const char *cfgname, const char *cfgvalue);
#endif
