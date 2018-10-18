#ifndef	_NDKSCANNER_H_
#define _NDKSCANNER_H_
#include <linux/ioctl.h>

typedef enum{
        SCAN_TYPE_EM1300 = 0,	/**< 扫描类型--EM1300*/
        SCAN_TYPE_EM3000 = 1,	/**< 扫描类型--EM3000*/
        SCAN_TYPE_SE955 = 2,	/**< 扫描类型--SE955*/
        SCAN_TYPE_UE966 = 3,	/**< 扫描类型--UE966*/
        SCAN_TYPE_EM3095=4,    /**< 扫描类型--EM3095*/
        SCAN_TYPE_EM1365=5,    /**< 扫描类型--EM1365*/
}EM_SCAN_TYEP;

#define SCANNER_DEV_NAME "/dev/scanner"
/* 设备名字 */
#define SCANNER_DEV_STRING	"scanner"   /* mknod /dev/scanner c 10 40 */
#define SCANNER_DEV_MINOR   45
#define SCANNER_DEV_MAGIC    'S' 


#define SCAN_IOCS_INIT       _IO(SCANNER_DEV_MAGIC, 0)
#define SCAN_IOCS_TRIG      _IO(SCANNER_DEV_MAGIC, 1)
#define SCAN_IOCS_EXIT       _IO(SCANNER_DEV_MAGIC, 2)

#endif
