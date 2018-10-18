#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/mount.h>
#include <assert.h>

#include "../public/parsecfg.h"
#include "hwcfginfo.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "config.h"


extern int ndk_config_init(void);

//配置文件结构体
//注意:此处的HWINFO结构如果有修改的话相应的console目录下的inpector目录下的load.c的HWINFO也要修改
const char * data_conf_filename = "/mnt/hwinfo/hw_setupinfo.conf";

char  *g_mainboard="NULL";
char  *g_wlsmdm="M72";
char  *g_wifi="NULL";
char  *g_eth0mac="08:00:46:45:43:42";

//初始化硬件信息
static int init_HWINFO(void)
{
    if(ndk_config_init()!=0)
        return NDK_ERR;

    return NDK_OK;
}

//功能:读取配置文件
int read_HWINFO(void)
{
    if (access(data_conf_filename, F_OK) < 0)
    {
        //配置文件不存在重新初始化配置文件。
        init_HWINFO();
    }

    return NDK_OK;
}

unsigned int 	*ptr = MAP_FAILED - 1;


static void* do_map(off_t phy_addr, size_t size)
{
    int mem_fd;
    void *addr;

    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        return((void *)MAP_FAILED);
    }

    addr = mmap(
               NULL, size,
               PROT_READ /*| PROT_WRITE*/,	MAP_SHARED,
               mem_fd,	phy_addr);
    if ((long)addr < 0) {
      //  ErrorCode = errno;
    }

    close(mem_fd);
    return addr;
}


void dword2pnm(unsigned int pnm, unsigned int* pp, unsigned int* pn, unsigned int* pm)
{
    pnm >>= 16;
    *pm = pnm & 0xff;

    pnm >>= 8;
    *pn = pnm & 0x0f;

    pnm >>= 4;
    *pp = pnm;
}


unsigned int get_clock(int gethclk, unsigned int scale)
{
    unsigned int	temp;
    unsigned int	pll_pnm, p, n, m;
    unsigned int	sourceclk, fclk, hclk;

    /*
     * determine current fclk frequency
     */
    if (ptr == MAP_FAILED - 1)
        ptr = (unsigned int *)do_map(PMU_BASE, 0x34);
    temp = *(ptr + 0x04 / 4);

    if (temp & 0x40000000) {
        pll_pnm = *ptr;
        dword2pnm(pll_pnm, &p, &n, &m);
        sourceclk = (XTAL_FREQ / (n+1)) * (m+1) / (p+1);
    } else {
        pll_pnm = 0;
        sourceclk = XTAL_FREQ;
    }
    fclk = sourceclk / ((temp & 0x1f) + 1);
    temp >>= 8;
    hclk = fclk / ((temp & 0xff) + 1);

    if (gethclk) {
        temp = hclk;
    } else {
        temp = fclk;
    }
    temp = temp / scale;
    return(temp);
}

void get_hw_info(struct pos_info *info)
{
    unsigned int reg_val;
    FILE *fp;
    char buf[128];
    char s[9];
    char *p;
    int i, j;
    int part_size = 0;

    memset(info->cpu_ver,0,sizeof(info->cpu_ver));

    if (ptr == MAP_FAILED - 1)
        ptr = (unsigned int *)do_map(PMU_BASE, 0x34);
    reg_val = *(ptr + 0x14 / 4);

    i = (reg_val & PMU_PART_NUMBER_MASK) >> 10;
    if (i == 1) {
        strcat(info->cpu_ver, "ZA9L0 ");
        switch (reg_val & PMU_VERSION_MASK) {
        case 0x01:
            strcat(info->cpu_ver, "AA");
            break;
        case 0x03:
            strcat(info->cpu_ver, "AB");
            break;
        case 0x08:
            strcat(info->cpu_ver, "BA");
            break;
        case 0x09:
            strcat(info->cpu_ver, "BB");
            break;
        case 0x0b:
            strcat(info->cpu_ver, "BB+");
            break;
        default:
            break;
        }
    } else if (i == 2) {
        strcat(info->cpu_ver, "ZA9L1 ");
        switch (reg_val & PMU_VERSION_MASK) {
        case 0x01:
            strcat(info->cpu_ver, "AA");
            break;
        case 0x02:
            strcat(info->cpu_ver, "AB");
            break;
        default:
            break;
        }
    }

    info->fclk = get_clock(0, 1) / (1000 * 1000);
    info->hclk = get_clock(1, 1) / (1000 * 1000);
    info->sdclk = info->hclk;

    fp = fopen("/proc/meminfo", "r");
    fgets(buf, sizeof(buf), fp);
    fclose(fp);

    s[0] = '\0';
    p = buf;

    while (!isdigit(*p)) p++;

    for (i = 0; isdigit(*p); i++,p++)
        s[i] = *p;
    s[i]= '\0';


    sscanf(s, "%d", &info->sdram_size);

    fp = fopen("/proc/mtd", "r");

    fgets(buf, sizeof(buf), fp);

    i = 0;
    info->nor_size = 0;
    info->nand_size = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        p = strchr(buf, ':');
        p++;
        while (!isxdigit(*p)) p++;
        for (j = 0; isxdigit(*p); j++,p++)
            s[j] = *p;
        s[j]= '\0';
//		sscanf(s, "%08x", &part_size);
        part_size = strtol(s, NULL, 16);
        i++;
        if (i <= NOR_PART_NUM)
            info->nor_size += part_size;
        else
            info->nand_size += part_size;
    }

    info->nor_size /= (1024 * 1024);
    info->nand_size /= (1024 * 1024);

    fclose(fp);
}

