
#ifndef HW_CFG_INFO_INCLUDE_H
#define HW_CFG_INFO_INCLUDE_H


/*
 * BIT Definitions
 */
#define BIT31   ((unsigned int)1<<31)
#define BIT30   ((unsigned int)1<<30)
#define BIT29   ((unsigned int)1<<29)
#define BIT28   ((unsigned int)1<<28)
#define BIT27   ((unsigned int)1<<27)
#define BIT26   ((unsigned int)1<<26)
#define BIT25   ((unsigned int)1<<25)
#define BIT24   ((unsigned int)1<<24)
#define BIT23   ((unsigned int)1<<23)
#define BIT22   ((unsigned int)1<<22)
#define BIT21   ((unsigned int)1<<21)
#define BIT20   ((unsigned int)1<<20)
#define BIT19   ((unsigned int)1<<19)
#define BIT18   ((unsigned int)1<<18)
#define BIT17   ((unsigned int)1<<17)
#define BIT16   ((unsigned int)1<<16)
#define BIT15   ((unsigned int)1<<15)
#define BIT14   ((unsigned int)1<<14)
#define BIT13   ((unsigned int)1<<13)
#define BIT12   ((unsigned int)1<<12)
#define BIT11   ((unsigned int)1<<11)
#define BIT10   ((unsigned int)1<<10)
#define BIT9    ((unsigned int)1<<9)
#define BIT8    ((unsigned int)1<<8)
#define BIT7    ((unsigned int)1<<7)
#define BIT6    ((unsigned int)1<<6)
#define BIT5    ((unsigned int)1<<5)
#define BIT4    ((unsigned int)1<<4)
#define BIT3    ((unsigned int)1<<3)
#define BIT2    ((unsigned int)1<<2)
#define BIT1    ((unsigned int)1<<1)
#define BIT0    ((unsigned int)1<<0)

#define PMU_BASE        0xFFFFE000
#define PMU_ID1_REG		(PMU_BASE | 0x14)

#define XTAL_FREQ	24000000

/*
 * PMU_ID1_REG bit definitions
 */
#define PMU_PART_NUMBER_MASK				(BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10)
#define PMU_PKG_TYPE_MASK					(BIT9 | BIT8)
#define PMU_VERSION_MASK					0xFF

#define NOR_PART_NUM 	3
#define NAND_PART_NUM 	5

#define HW_CFG_NAME_MAINBOARD		"mainboard"
#define HW_CFG_NAME_WLSMDM_TYPE		"wlsmdm"
#define HW_CFG_NAME_WIFI_TYPE		"wifi"
#define HW_CFG_NAME_ETH0_IP			"eth0ip"
#define HW_CFG_NAME_ETH0_MASK		"eth0mask"
#define HW_CFG_NAME_ETH0_GW			"eth0gw"
#define HW_CFG_NAME_ETH0_MAC		"eth0mac"

typedef enum
{
	HW_CFG_MAINBOARD_VER,
	HW_CFG_WLSMDM_TYPE,
	HW_CFG_WIFI_TYPE,
	HW_CFG_ETH0_IP,
	HW_CFG_ETH0_MASK,
	HW_CFG_ETH0_GW,
	HW_CFG_ETH0_MAC
}HW_INFO_TYPE;

struct pos_info {
    char cpu_ver[16];
    unsigned int fclk, hclk, sdclk;			//单位：MHZ
    int  sdram_size, nor_size, nand_size;	//单位：sdram_size(KB) nor_size, nand_size(MB)
//	unsigned char hwconfig;
};


int get_hw_cfg_info(int type, void *param);
int set_hw_cfg_info(int type,const void *param);

#endif
