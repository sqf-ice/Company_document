#ifndef _BOARD_H_
#define _BOARD_H_
#include "bcm5892_define.h"
extern int g_sam_card_num ;
extern int g_ccm3310_num;
typedef struct{
	uint32_t vcc;
	uint32_t rst;
	uint32_t io;
}sam_gpio_t;
extern void ic_sam_gpio_init(void);
extern void ic_sam_gpio_exit(void);
extern void ic_vcc_set(tda8035_vcc_type vcc);
extern void ic_cold_reset(tda8035_vcc_type vcc,uint8_t d);
extern void ic_warm_reset(void);
extern void ic_power_down(void);
extern void sam_select_num(uint8_t samnum);
extern void sam_vcc_set(tda8035_vcc_type vcc);
extern void sam_cold_rst(uint32_t samnum);
extern void sam_warm_rst(uint8_t samnum);
extern void sam_power_down(uint8_t samnum);
extern void sam_send_io(uint8_t val);
extern int sam_get_io(void);
extern void sam_set_io_type(io_type_t type);
extern void ic_sam_suspend_ctrl(int val);
#endif
