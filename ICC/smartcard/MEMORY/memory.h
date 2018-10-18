#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "bcm5892_define.h"
#include <linux/kernel.h>

#define MEM_RW_MAX_LEN (248)
//#define MEM_DEBUG
#undef MEM_DEBUG
#ifdef MEM_DEBUG
#define mem_printk(fmt, arg...)  printk(fmt, ##arg)
#else
#define mem_printk(fmt, arg...)    
#endif


extern int tda8035_get_io(void);
extern int tda8035_get_det(void);
extern void tda8035_set_io_type(io_type_t type);
extern void tda8035_set_io(uint8_t val);
extern void tda8035_set_rst(uint8_t val);
extern void tda8035_set_auxio1(uint8_t val);
extern void tda8035_set_auxio2(uint8_t val);
extern void tda8035_set_clk(uint8_t val);
extern void tda8035_set_vcc_onoff(uint8_t val);
extern void set_sc0_gpio(void);
extern void delay_us(uint32_t us);
extern void delay_ms(uint32_t ms);
extern void tda8035_poweron_clk(void);
extern uint8_t check_mem_vcc_on(void);
extern uint8_t check_mem_insert(void);
extern void tda8035_mem_powdown(void);
extern void mem_timer_init(void);
extern void tda8035_mem_gpio_init(void);
#ifdef CONFIG_SP60
extern void ic_vcc_set_tda8035vcc5v(void);
#endif
extern void set_sc0_gpio_tda8035vcc5v(void);
#endif

