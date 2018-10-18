#ifndef _BCM5892_DEFINE_H_
#define _BCM5892_DEFINE_H_
#include <linux/types.h>
#include <mach/reg_gpio.h>
#define TIMER_FACTOR  2 //timer clk =6M sam card clk = 3M

#define REG_GPIO0_PAD_RES      IO_ADDRESS(0x000cc834)
#define REG_GPIO3_PAD_RES      IO_ADDRESS(0x000d7834)
#define REG_GPIO4_PAD_RES      IO_ADDRESS(0x000e6834)
#define REG_GPIO1_PAD_RES      IO_ADDRESS(0x000cd834)	

#define TIMER_GROUP_2_BASE     (0x000C2000) /*timer 4,5*/
#define TIMER45_REG_BASE       IO_ADDRESS(TIMER_GROUP_2_BASE)
#define REG_TIMER1_LOAD        (0x000)
#define REG_TIMER1_VALUE       (0x004)
#define REG_TIMER1_CONTROL     (0x008)
#define REG_TIMER1_INTCLR      (0x00C)

#define IC_TDA8035_IO           (12)
#define IC_TDA8035_AUXIO2       (13)
#define IC_TDA8035_AUXIO1       (14)
#define IC_TDA8035_VCC          (15)
#define IC_TDA8035_RST          (16)
#define IC_TDA8035_DET          (17)
#define IC_TDA8035_CLK          (18)

typedef struct{
	uint8_t clkdiv;
	uint8_t prescale;
	uint8_t bauddiv;
	uint8_t etudiv;
}ic_clk_type;

typedef enum{
	etu = 0,
	clk,
}timer_src;
typedef enum{
	start_immediate = 0,
	start_next_byte,
}timer_mode;
typedef enum{
	disable = 0,
	enable,
}timer_onff;

typedef struct {
	timer_src src;
	timer_mode mode;
	timer_onff onoff;
	uint16_t cnt;
}timer_ctrl_type;
typedef enum{
	VCC5V = 0,
	VCC3V,
	VCC1P8V,
	DEEP_SLEEP,
}tda8035_vcc_type;
typedef enum{
	input = 0,
	output,
}io_type_t;
extern uint8_t reg_read8(uint32_t reg);
extern void reg_write8(uint32_t reg, uint8_t data);
extern uint32_t reg_read32(uint32_t reg);
extern void reg_write32(uint32_t reg,uint32_t data);
extern void ctrl_ic_vcc(uint8_t vcc);
extern void ctrl_ic_io(uint8_t io);
extern void ctrl_ic_rst(uint8_t rst);
extern void ctrl_ic_pres_pol(uint8_t pres_pol);
extern int check_iccard_pres(void);
extern int check_iccard_vccon(void);
extern int ctrl_ic_clk(uint8_t d);
extern void close_ic_clk(void);
extern void rst_ic_module(void);
extern void powdown_ic_module(void);
extern int check_ic_module_work(void);
extern void ctrl_ic_timer(timer_ctrl_type *timer_ctrl);
extern int check_ic_timer_over(void);
extern void wait_ic_clk_cnt(uint16_t cnt);
extern void wait_ic_etu_cnt(uint16_t cnt);
extern void sam_ctrl_clk(uint8_t onoff);
extern void sam_start_timer_cnt(uint32_t cnt);
extern uint32_t sam_read_timer_value(void);
extern int sam_check_timer_over(void);
extern void sam_stop_timer(void);
extern void sam_wait_clk_cnt(uint32_t clk);
extern void sam_wait_etu_cnt(uint32_t etu);
extern void bcm5892_gpio_set_pin_type(uint32_t pin,io_type_t type);
#endif
