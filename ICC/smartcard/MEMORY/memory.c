#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <mach/reg_gpio.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include "board.h"
#include "bcm5892sc_define.h"
#include "memory.h"
#include "ic_memory.h"

int tda8035_get_io(void)
{
	return __gpio_get_value(IC_TDA8035_IO);
}
int tda8035_get_det(void)
{
	return __gpio_get_value(IC_TDA8035_DET);
}
void tda8035_set_io_type(io_type_t type)
{
	type ? reg_gpio_iotr_set_pin_type(IC_TDA8035_IO,GPIO_PIN_TYPE_OUTPUT)\
		 : reg_gpio_iotr_set_pin_type(IC_TDA8035_IO,GPIO_PIN_TYPE_INPUT); 
}
void tda8035_set_io(uint8_t val)
{
	tda8035_set_io_type(output);
	gpio_direction_output(IC_TDA8035_IO,val);
}
void tda8035_set_rst(uint8_t val)
{
	gpio_direction_output(IC_TDA8035_RST,val);
}
void tda8035_set_auxio1(uint8_t val)
{
	gpio_direction_output(IC_TDA8035_AUXIO1,val);
}
void tda8035_set_auxio2(uint8_t val)
{
	gpio_direction_output(IC_TDA8035_AUXIO2,val);
}
void tda8035_set_clk(uint8_t val)
{
	gpio_direction_output(IC_TDA8035_CLK,val);
}
void tda8035_set_vcc_onoff(uint8_t val)
{
	gpio_direction_output(IC_TDA8035_VCC,val);
}
void set_sc0_gpio(void)
{
	reg_write8(SMARTCARD_BASE + SC_P_MODE_REGISTER,0);//disable sc0
	reg_gpio_iotr_set_pin_type(IC_TDA8035_VCC,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_RST,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_CLK,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_IO,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_AUXIO1,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_AUXIO2,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_DET,GPIO_PIN_TYPE_INPUT);
	tda8035_set_io(0);
	tda8035_set_clk(0);
	tda8035_set_rst(0);
	tda8035_set_vcc_onoff(1);
	ic_vcc_set(VCC5V);
}

void set_sc0_gpio_tda8035vcc5v(void)
{
	reg_write8(SMARTCARD_BASE + SC_P_MODE_REGISTER,0);//disable sc0
	reg_gpio_iotr_set_pin_type(IC_TDA8035_VCC,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_RST,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_CLK,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_IO,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_AUXIO1,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_AUXIO2,GPIO_PIN_TYPE_OUTPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_DET,GPIO_PIN_TYPE_INPUT);
	tda8035_set_io(0);
	tda8035_set_clk(0);
	tda8035_set_rst(0);
	tda8035_set_vcc_onoff(1);
#ifdef CONFIG_SP60
	ic_vcc_set_tda8035vcc5v();
#else
	ic_vcc_set(VCC5V);
#endif
}

void mem_timer_init(void)
{
/*clear timer int flag */
	unsigned int value = 0;
	reg_write32(TIMER45_REG_BASE + REG_TIMER1_INTCLR,0x00);
	value = (1 << 0) |(1 << 1);/*one-shot,32bit 6M disable*/
	reg_write32(TIMER45_REG_BASE + REG_TIMER1_CONTROL,value);
	reg_write32(TIMER45_REG_BASE + REG_TIMER1_LOAD,0xffffff);
}
void delay_us(uint32_t us)
{
	udelay(us);
}
void delay_ms(uint32_t ms)
{
	mdelay(ms);
	//delay_us(ms*1000);
}

void tda8035_poweron_clk(void)
{
	unsigned char i = 0;
	for(i = 0;i < 15;i++)
	{
		delay_us(10);
		tda8035_set_clk(1);
		delay_us(10);
		tda8035_set_clk(0);
	}
	delay_us(10);
}
uint8_t check_mem_vcc_on(void)
{
	int i = reg_gpio_get_pin_output(IC_TDA8035_VCC);
	return (i ? 0 : 1); //vcc =0 vcc _on 
}
uint8_t check_mem_insert(void)
{
	uint8_t det = 0;
	reg_gpio_iotr_set_pin_type(IC_TDA8035_DET,GPIO_PIN_TYPE_INPUT);
	delay_us(100);
	det = __gpio_get_value(IC_TDA8035_DET);// =1 card insert;
	if(det == 0) reg_gpio_iotr_set_pin_type( GPIO_AUX_SCI0, GPIO_PIN_TYPE_ALTERNATIVE_FUNC0 );
	return det;
}
void tda8035_mem_powdown(void)
{
	tda8035_set_rst(0);
	tda8035_set_clk(0);
	tda8035_set_io(0);
	tda8035_set_auxio1(0);
	tda8035_set_auxio2(0);
	tda8035_set_vcc_onoff(1);
	rst_ic_module();
	delay_ms(1);
	/*set input for low power consumption*/
	reg_gpio_iotr_set_pin_type(IC_TDA8035_IO,GPIO_PIN_TYPE_INPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_AUXIO1,GPIO_PIN_TYPE_INPUT);
	reg_gpio_iotr_set_pin_type(IC_TDA8035_AUXIO2,GPIO_PIN_TYPE_INPUT);
//	ic_vcc_set(DEEP_SLEEP);

	mem_powon_flag = 0;
}
void tda8035_mem_gpio_init(void)
{
	tda8035_set_rst(0);
	tda8035_set_clk(0);
	tda8035_set_io_type(output);
	tda8035_set_io(0);
	tda8035_set_auxio1(0);
	tda8035_set_auxio2(0);
	tda8035_set_io_type(input);
}

