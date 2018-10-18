#ifndef _USBANDSD_H_
#define _USBANDSD_H_

typedef unsigned int uint;

/* IOCTL definitions */
#define GPIO_IOC_MAGIC 'G'
#define GPIO_IOCS_USB_HOST		_IO(GPIO_IOC_MAGIC, 0)
#define GPIO_IOCS_USB_SLAVE		_IO(GPIO_IOC_MAGIC, 1)
#define POWER_IOC_MAGIC 'P'
#define POWER_SET_USB_DISK_STATUS    _IOW(POWER_IOC_MAGIC,19,int)     //add by shily 2014-6-17

#define USB_STORAGE     "/dev/sda"
#define USB_STORAGE_1   "/dev/sda1"
#define USB_STORAGE_2   "/dev/sda2"
#define USB_STORAGE_3   "/dev/sda3"
#define USB_STORAGE_4   "/dev/sda4"
#define USB_STORAGE_5   "/dev/sda5"
#define USB_STORAGE_6   "/dev/sda6"
#define USB_STORAGE_7   "/dev/sda7"
#define USB_STORAGE_8   "/dev/sda8"
#define USB_DISK_DIR    "/mnt/usbsd"


#define SD_STORAGE     "/dev/sd_spi"
#define SD_STORAGE_1   "/dev/sd_spi1"
#define SD_STORAGE_2   "/dev/sd_spi2"
#define SD_STORAGE_3   "/dev/sd_spi3"
#define SD_STORAGE_4   "/dev/sd_spi4"

#define SD_5892_STORAGE         "/dev/mmcblk0"
#define SD_5892_STORAGE_1     "/dev/mmcblk0p1"

#define SD_DISK_DIR    "/mnt/sdmmc"


#endif
