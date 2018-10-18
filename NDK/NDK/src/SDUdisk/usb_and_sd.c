#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/mount.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/vfs.h>
#include "usb_and_sd.h"
#include "../public/config.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "devmgr.h"

#define KILOBYTE   1024

#define DEBUG
#ifdef DEBUG
#define PDEBUG(fmt, args...) fprintf(stderr,  "UandSD disk : "fmt, ##args)
#else
#define PDEBUG(fmt, args...)
#endif

static pthread_mutex_t rtc_mutex = PTHREAD_MUTEX_INITIALIZER;
static int usbflag = DISKNOTOPEN,sdflag = DISKNOTOPEN;
int usbhosttype = 0;
static int fd = -1;
static int sdclosefalg = 0,uclosefalg = 0,sdopenflag = 0,uopenflag = 0;
extern int ndk_ifunloaddrv(char *driver);
extern int ndk_cpu_type(void);
struct tempdisk *td=NULL;

struct tempdisk {
    int type;
    int timeout;
};

static long kscale(long b, long bs)
{
    return ( b * (long long) bs + KILOBYTE/2 ) / KILOBYTE;
}
static int exeSdCmd(char *cmd)
{
    char tmpstr[256] = {0};
    if(cmd!=NULL)
        sprintf(tmpstr,"sudo /bin/mount -t vfat -o umask=0 %s /mnt/sdmmc &>/dev/null",cmd);
    return system(tmpstr);
}
#if 1
static int mount_sd(void)
{
    if(ndk_cpu_type() == 1) {
        if(exeSdCmd(SD_5892_STORAGE)) {
            if(exeSdCmd(SD_5892_STORAGE_1)) {
                return -1;
            }
        }
    } else {
        if(exeSdCmd(SD_STORAGE)) {
            if(exeSdCmd(SD_STORAGE_1)) {
                if(exeSdCmd(SD_STORAGE_2)) {
                    if(exeSdCmd(SD_STORAGE_3)) {
                        if(exeSdCmd(SD_STORAGE_4)) {
                            return -1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
#endif
#if 0
static int mount_sd(void)
{
    int ret = -1;
    if(ndk_cpu_type() == 1) {
        if (mount(SD_5892_STORAGE, SD_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
            if (mount(SD_5892_STORAGE_1, SD_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0)
                return -1;
        }
    } else {
        if ((ret=mount(SD_STORAGE, SD_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") )!= 0) {
            fprintf(stderr,"mount %s fail return [%d][%d]\n",SD_STORAGE,ret,errno);
            if ((ret=mount(SD_STORAGE_1, SD_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") )!= 0) {
                fprintf(stderr,"mount %s fail return [%d][%d]\n",SD_STORAGE_1,ret,errno);
                if (ret=(mount(SD_STORAGE_2, SD_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt")) != 0) {
                    fprintf(stderr,"mount %s fail return [%d][%d]\n",SD_STORAGE_2,ret,errno);
                    if ((ret=mount(SD_STORAGE_3, SD_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt")) != 0) {
                        fprintf(stderr,"mount %s fail return [%d][%d]\n",SD_STORAGE_3,ret,errno);
                        if ((ret=mount(SD_STORAGE_4, SD_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") )!= 0) {
                            fprintf(stderr,"mount %s fail return [%d][%d]\n",SD_STORAGE_4,ret,errno);
                            return -1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}
#endif
static int exeCmd(char *cmd)
{
    char tmpstr[256] = {0};
    if(cmd!=NULL)
        sprintf(tmpstr,"sudo /bin/mount -t vfat -o umask=0 %s /mnt/usbsd &>NULL",cmd);
    return system(tmpstr);
}
#if 1
static int mount_usb(void)
{
    if(exeCmd(USB_STORAGE)) {
        if(exeCmd(USB_STORAGE_1)) {
            if(exeCmd(USB_STORAGE_2)) {
                if(exeCmd(USB_STORAGE_3)) {
                    if(exeCmd(USB_STORAGE_4)) {
                        if(exeCmd(USB_STORAGE_5)) {
                            if(exeCmd(USB_STORAGE_6)) {
                                if(exeCmd(USB_STORAGE_7)) {
                                    if(exeCmd(USB_STORAGE_8)) {
                                        return -1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
#endif
#if 0
static int mount_usb(void)
{
    if (mount(USB_STORAGE, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
        if (mount(USB_STORAGE_1, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
            if (mount(USB_STORAGE_2, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
                if (mount(USB_STORAGE_3, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
                    if (mount(USB_STORAGE_4, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
                        if (mount(USB_STORAGE_5, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
                            if (mount(USB_STORAGE_6, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
                                if (mount(USB_STORAGE_7, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
                                    if (mount(USB_STORAGE_8, USB_DISK_DIR, "vfat", MS_SYNCHRONOUS, "shortname=winnt") != 0) {
                                        return -1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}
#endif
static void *create(void *arg)
{
    pthread_detach(pthread_self());
    time_t diff = 0, oldtime = time(NULL);
    struct tempdisk *tmph;
    tmph=(struct tempdisk *)arg;
    while(1) {
        if(tmph->timeout==0) {
            if(tmph->type==UDISK) {
                if(mount_usb()==0) {
                    pthread_mutex_lock(&rtc_mutex);
                    usbflag=DISKMOUNTSUCC;
                    pthread_mutex_unlock(&rtc_mutex);
                    return (void *)0;
                } else {
                    if(uclosefalg==1) {
                        return (void *)0;
                    }
                    pthread_mutex_lock(&rtc_mutex);
                    usbflag=DISKMOUNTING;
                    pthread_mutex_unlock(&rtc_mutex);
                }
            } else {
                if(mount_sd()==0) {
                    pthread_mutex_lock(&rtc_mutex);
                    sdflag=DISKMOUNTSUCC;
                    pthread_mutex_unlock(&rtc_mutex);
                    return (void *)0;
                } else {
                    pthread_mutex_lock(&rtc_mutex);
                    sdflag=DISKMOUNTING;
                    pthread_mutex_unlock(&rtc_mutex);
                    if(sdclosefalg==1) {
                        return (void *)0;
                    }
                }
            }
        } else {
            diff=time(NULL)-oldtime;
            if(tmph->type==UDISK) {
                if(mount_usb()==0) {
                    pthread_mutex_lock(&rtc_mutex);
                    usbflag=DISKMOUNTSUCC;
                    pthread_mutex_unlock(&rtc_mutex);
                    return (void *)0;
                } else {
                    if(diff>=(tmph->timeout)) {
                        pthread_mutex_lock(&rtc_mutex);
                        usbflag=DISKTIMEOUT;
                        pthread_mutex_unlock(&rtc_mutex);
                        return (void *)0;
                    } else if(uclosefalg==1) {
                        //usbflag=DISKNOTOPEN;
                        return (void *)0;
                    }
                    pthread_mutex_lock(&rtc_mutex);
                    usbflag=DISKMOUNTING;
                    pthread_mutex_unlock(&rtc_mutex);
                }
            } else {
                if(mount_sd()==0) {
                    pthread_mutex_lock(&rtc_mutex);
                    sdflag=DISKMOUNTSUCC;
                    pthread_mutex_unlock(&rtc_mutex);
                    return (void *)0;
                } else {
                    if(diff>(tmph->timeout)) {
                        pthread_mutex_lock(&rtc_mutex);
                        sdflag=DISKTIMEOUT;
                        pthread_mutex_unlock(&rtc_mutex);
                        return (void *)0;
                    } else if(sdclosefalg==1) {
                        //sdflag=DISKNOTOPEN;
                        return (void *)0;
                    } else {
                        pthread_mutex_lock(&rtc_mutex);
                        sdflag=DISKMOUNTING;
                        pthread_mutex_unlock(&rtc_mutex);
                    }
                }
            }
        }
        usleep(30*1000);
    }
}
static void My_pthread(struct tempdisk *diskhandle)
{
    pthread_t tidp;
    int error;
    error=pthread_create(&tidp,NULL,create,(void *)diskhandle);
    if(error!=0) {
        PDEBUG("%s pthread_create fail(%d)\n",__func__,__LINE__);
        return;
    }
}
/**
 *@brief    打开U盘或SD卡
 *@param    emType  类型（UDISK：表示U盘,SDDISK：表示SD卡.\ref EM_DISKTYPE "EM_DISKTYPE"）
 *@param    nTimeOut 设置超时时间(单位秒，0表示阻塞，非0表示在规定的超时时间内没有返回DISKMOUNTSUCC该状态，此时获取到的状态应为DISKTIMEOUT)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_DiskOpen(EM_DISKTYPE emType,int nTimeOut)
{
    int power_fd = -1;
    int ops = 1;

    if(nTimeOut<0)
        return NDK_ERR_USDDISK_PARAM;
    td=(struct tempdisk *)malloc(sizeof(struct tempdisk));
    td->type=emType;
    td->timeout=nTimeOut;
    if(emType==SDDISK) {
        sdclosefalg = 0;
        if(sdopenflag==1)
            return NDK_OK;
        /*   sp60 bcm5892 cpu 走不同分支       */
        if(ndk_cpu_type() == 1) {
            if(ndk_ifunloaddrv("fat")!=0) {
                if(load_driver_by_name("fat","")!=0) {
                    PDEBUG("%s load fat FAIL(%d)\n",__func__,__LINE__);
                    sdflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("vfat")!=0) {
                if(load_driver_by_name("vfat","")!=0) {
                    PDEBUG("%s load vfat FAIL(%d)\n",__func__,__LINE__);
                    sdflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("sd_card")!=0) {
                if(load_driver_by_class("sd_card")!=0) {
                    PDEBUG("%s load sd_card FAIL(%d)\n",__func__,__LINE__);
                    sdflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }

            if (system("cat /proc/partitions | grep mmcblk0") != 0) {
                sdflag=DISKNOEXIT;
                return NDK_OK;
            }
            sdopenflag=1;
        } else {
            if(ndk_ifunloaddrv("fat")!=0) {
                if(load_driver_by_name("fat","")!=0) {
                    PDEBUG("%s load fat FAIL(%d)\n",__func__,__LINE__);
                    sdflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("vfat")!=0) {
                if(load_driver_by_name("vfat","")!=0) {
                    PDEBUG("%s load vfat FAIL(%d)\n",__func__,__LINE__);
                    sdflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("sd_spi")!=0) {
                if(load_driver_by_name("sd_spi","")!=0) {
                    PDEBUG("%s load sd_spi FAIL(%d)\n",__func__,__LINE__);
                    sdflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            sdopenflag=1;
            fd = open("/dev/psd", O_RDONLY);
            if (fd < 0) {
                sdflag=DISKNOEXIT;
                return NDK_OK;
            }
        }
    } else if(emType==UDISK) {
        uclosefalg = 0;
        if(uopenflag==1)
            return NDK_OK;
        if(ndk_cpu_type()==1) {
            if(ndk_ifunloaddrv("fat")!=0) {
                if(load_driver_by_name("fat","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("vfat")!=0) {
                if(load_driver_by_name("vfat","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("bcm589x-usbcdc")==0) {
                if(unload_driver_by_name("bcm589x-usbcdc")!=0)
                    fprintf(stderr,"---------[%s][%d]\n",__func__,__LINE__);
            }
            if(ndk_ifunloaddrv("scsi_mod")!=0) {
                if(load_driver_by_name("scsi_mod","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("sd_mod")!=0) {
                if(load_driver_by_name("sd_mod","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }

            if(ndk_ifunloaddrv("usbcore")!=0) {
                if(load_driver_by_name("usbcore","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            ndk_getconfig("usb","port",CFG_INT,&usbhosttype);
            if(usbhosttype==0) {
                if(ndk_ifunloaddrv("dwc_common_port_lib")!=0) {
                    if(load_driver_by_name("dwc_common_port_lib","")!=0) {
                        usbflag=DISKDRIVERLOADFAIL;
                        return NDK_ERR_USDDISK_DRIVELOADFAIL;
                    }
                }
                if(ndk_ifunloaddrv("bcm589x-otg")!=0) {
                    if(load_driver_by_name("bcm589x-otg","")!=0) {
                        usbflag=DISKDRIVERLOADFAIL;
                        return NDK_ERR_USDDISK_DRIVELOADFAIL;
                    }
                }
            } else if(usbhosttype==1) {
                if(ndk_ifunloaddrv("bcm589x-usb-host")!=0) {
                    if(load_driver_by_name("bcm589x-usb-host","")!=0) {
                        usbflag=DISKDRIVERLOADFAIL;
                        return NDK_ERR_USDDISK_DRIVELOADFAIL;
                    }
                }
            } else {
                usbflag=DISKDRIVERLOADFAIL;
                return NDK_ERR_USDDISK_DRIVELOADFAIL;
            }
            if(ndk_ifunloaddrv("usb_storage")!=0) {
                if(load_driver_by_name("usb_storage","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
        } else {
            char p;
            fd = open("/dev/gpio",O_RDONLY);
            if(fd<0) {
                usbflag = DISKNOEXIT;
                return NDK_OK;
            }
            if(ioctl(fd,GPIO_IOCS_USB_HOST,&p)<0) {
                usbflag = DISKNOEXIT;
                return NDK_ERR_USDDISK_IOCFAIL;
            }
            unload_driver_by_name("g_serial");
            unload_driver_by_name("za9l1_udc");
            if(ndk_ifunloaddrv("fat")!=0) {
                if(load_driver_by_name("fat","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("vfat")!=0) {
                if(load_driver_by_name("vfat","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("scsi_mod")!=0) {
                if(load_driver_by_name("scsi_mod","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("sg")!=0) {
                if(load_driver_by_name("sg","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("sd_mod")!=0) {
                if(load_driver_by_name("sd_mod","")!=0) {
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("za9l1_hcd")!=0) {
                if(load_driver_by_name("za9l1_hcd","")!=0) {
                    PDEBUG("%s load za9l1_hcd FAIL(%d)\n",__func__,__LINE__);
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
            if(ndk_ifunloaddrv("usb_storage")!=0) {
                if(load_driver_by_name("usb_storage","")!=0) {
                    PDEBUG("%s load usb_storage FAIL(%d)\n",__func__,__LINE__);
                    usbflag=DISKDRIVERLOADFAIL;
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
                }
            }
        }
        power_fd = open("/dev/power", O_RDWR);
        if (power_fd < 0) {
            return NDK_ERR;
        }
        if (ioctl(power_fd, POWER_SET_USB_DISK_STATUS, &ops) != 0) {
            close(power_fd);
            return NDK_ERR_USDDISK_IOCFAIL;
        }
        close(power_fd);
        uopenflag=1;
    } else {
        PDEBUG("%s Please choose the U disk or SD card type(%d)\n",__func__,__LINE__);
        return NDK_ERR_USDDISK_NONSUPPORTTYPE;
    }
    My_pthread((struct tempdisk *)td);

    return NDK_OK;
}
/**
*@brief    获取U盘或SD卡信息
*@param    pszDiskdir  U盘或SD卡跟目录
*@param    pstInfo   磁盘信息结构（参考\ref ST_DISK_INFO "ST_DISK_INFO")
*@return
*@li   NDK_OK              操作成功
*@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_DiskGetInfo(char *pszDiskdir,ST_DISK_INFO  *pstInfo)
{
    struct statfs buf,*p;
    p=&buf;
    if(pstInfo==NULL||pszDiskdir==NULL)
        return NDK_ERR_USDDISK_PARAM;
    if(sdflag==DISKMOUNTSUCC||usbflag==DISKMOUNTSUCC) {
        if( statfs(pszDiskdir, p)<0)
            return NDK_ERR;
        pstInfo->unFreeSpace= kscale(p->f_bavail, p->f_bsize);
        pstInfo->unTotalSpace= kscale(p->f_blocks, p->f_bsize);
    } else {
        fprintf(stderr,"当前U盘或SD卡不可用\n");
        return NDK_ERR;
    }

    return NDK_OK;
}
/**
*@brief    获取U盘或SD卡状态
*@param    emType  类型（UDISK：表示U盘,SDDISK：表示SD卡.\ref EM_DISKTYPE "EM_DISKTYPE"）
*@retval   pnDiskstate   状态（参考\ref EM_DISKSTATE "EM_DISKSTATE"）
*@return
*@li   NDK_OK              操作成功
*@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_DiskGetState(EM_DISKTYPE emType,int *pnDiskstate)
{
    if(pnDiskstate==NULL)
        return NDK_ERR_USDDISK_PARAM;
    if(emType==UDISK)
        *pnDiskstate=usbflag;
    else if(emType==SDDISK)
        *pnDiskstate=sdflag;
    else {
        PDEBUG("%s Please choose the U disk or SD card type(%d)\n",__func__,__LINE__);
        return NDK_ERR_USDDISK_NONSUPPORTTYPE;
    }

    return NDK_OK;
}
/**
*@brief    关闭U盘或SD卡
*@param    emType  类型（UDISK：表示U盘,SDDISK：表示SD卡.\ref EM_DISKTYPE "EM_DISKTYPE"）
*@return
*@li   NDK_OK              操作成功
*@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_DiskClose(EM_DISKTYPE emType)
{
    int ret = 0;
    int ops = 0;
    int power_fd = -1;

    if(emType==SDDISK) {
        if(sdclosefalg==1)
            goto DONE1;
        system("sudo /bin/umount /mnt/sdmmc");
#if 0
        if (umount2(SD_DISK_DIR, MNT_FORCE) != 0) {
            PDEBUG("%s umount sd FAIL(%d)\n",__func__,__LINE__);
            //ret = -2;
        }
#endif
        if (fd > 0) {
            if (close(fd)!= 0) {
                PDEBUG("%s close psd FAIL(%d)\n",__func__,__LINE__);
                //ret = -1;
            }
        }

    DONE1:
        if(ndk_cpu_type() != 1) {
            if (ndk_ifunloaddrv("sd_spi") == 0) {
                if(unload_driver_by_name("sd_spi")!=0) {
                    PDEBUG("%s unload sd_spi FAIL(%d)\n",__func__,__LINE__);
                    //ret = -1;
                }
            }

        } else {
            if (ndk_ifunloaddrv("sd_card") == 0) {
                if(unload_driver_by_name("sd_card")!=0) {
                    PDEBUG("%s unload sd_card FAIL(%d)\n",__func__,__LINE__);
                    //ret = -1;
                }
            }
        }
        pthread_mutex_lock(&rtc_mutex);
        sdflag = DISKNOTOPEN;
        pthread_mutex_unlock(&rtc_mutex);
        sdclosefalg = 1;
        sdopenflag = 0;
    } else if(emType==UDISK) {
        if(uclosefalg==1)
            goto DONE2;
        system("sudo /bin/umount /mnt/usbsd");
    DONE2:
        if(ndk_cpu_type() == 1) {
            unload_driver_by_name("usb_storage");
            if(usbhosttype==0) {
                unload_driver_by_name("bcm589x_otg");
                unload_driver_by_name("dwc_common_port_lib");
            } else
                unload_driver_by_name("bcm589x-usb-host");
            unload_driver_by_name("usbcore");
            unload_driver_by_name("sd_mod");
            unload_driver_by_name("scsi_mod");
        } else {
            if (fd > 0) {
                if (close(fd)!= 0) {
                    PDEBUG("%s close FAIL(%d)\n",__func__,__LINE__);
                    //ret = -1;
                }
            }

            unload_driver_by_name("za9l1_hcd");
            unload_driver_by_name("usb_storage");
        }
        power_fd = open("/dev/power", O_RDWR);
        ioctl(power_fd, POWER_SET_USB_DISK_STATUS, &ops);
        close(power_fd);
        pthread_mutex_lock(&rtc_mutex);
        usbflag = DISKNOTOPEN;
        pthread_mutex_unlock(&rtc_mutex);
        uclosefalg=1;
        uopenflag=0;
    } else {
        PDEBUG("%s Please choose the U disk or SD card type(%d)\n",__func__,__LINE__);
        return NDK_ERR_USDDISK_NONSUPPORTTYPE;
    }
    if(td!=NULL) {
        free(td);
        td=NULL;
    }
    if(ret==0)
        return NDK_OK;
    else if(ret==-2)
        return NDK_ERR_USDDISK_UNMOUNTFAIL;
    else
        return NDK_ERR_USDDISK_UNLOADDRIFAIL;
}
/**
*@brief    获取U盘或SD卡根目录
*@param    emType  类型（UDISK：表示U盘,SDDISK：表示SD卡.\ref EM_DISKTYPE "EM_DISKTYPE")
*@retval   pszRdir  根目录
*@return
*@li   NDK_OK              操作成功
*@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_DiskGetRootDirName(EM_DISKTYPE emType,char **pszRdir)
{
    if(pszRdir==NULL)
        return NDK_ERR_USDDISK_PARAM;
    if(emType==UDISK) {
        if(usbflag==DISKMOUNTSUCC)
            memcpy(pszRdir, USB_DISK_DIR, strlen(USB_DISK_DIR));
        else {
            pszRdir=NULL;
            PDEBUG("%s U disk disable(%d)\n",__func__,__LINE__);
            return NDK_ERR;
        }
    } else if(emType==SDDISK) {
        if(sdflag==DISKMOUNTSUCC)
            memcpy(pszRdir, SD_DISK_DIR, strlen(SD_DISK_DIR));
        else {
            pszRdir=NULL;
            PDEBUG("%s SD disk disable(%d)\n",__func__,__LINE__);
            return NDK_ERR;
        }
    } else {
        pszRdir=NULL;
        PDEBUG("%s Please choose the U disk or SD card type(%d)\n",__func__,__LINE__);
        return NDK_ERR_USDDISK_NONSUPPORTTYPE;
    }
    return NDK_OK;
}
