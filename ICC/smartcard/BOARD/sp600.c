/*
 * sp600.c
 *
 *  Created on: 2015年5月19日
 *      Author: Gavin
 */
#include "board.h"
#include "bcm5892_define.h"
#include "icc_define.h"
#include <linux/module.h>
#include <linux/delay.h>
#include "sam.h"
#include "iccard.h" /*seg g_atr*/
int g_sam_card_num = 3; /*default 2*/
module_param(g_sam_card_num, int, S_IRUSR|S_IWUSR);

int g_vcc_keep_on = 0; /*default 0*/
module_param(g_vcc_keep_on, int, S_IRUSR|S_IWUSR);

int g_7816_open = 0;
module_param(g_7816_open, int, S_IRUSR|S_IWUSR);

sam_gpio_t sam_gpio;
int g_ccm3310_num = SC_SAM3;

#define IC_TDA8035_1V8          (3)      //GPA3
#define IC_TDA8035_5V3V         (1)      //GPA1
/*2张sam卡时使用tda8035的io、rst和auxio1、auxio2
 * 3张sam卡时使用74hc4025进行选择，然后只使用io rst*/

#define SAM_ALL_GPIO_VCC      (HW_GPIO2_PIN_MAX + 2)    //GPD2
#define SAM1_GPIO_IO          (HW_GPIO3_PIN_MAX + 2)    //GPE2
#define SAM1_GPIO_RST         (HW_GPIO3_PIN_MAX + 3)    //GPE3
#define SAM2_GPIO_IO          (HW_GPIO3_PIN_MAX + 4)    //GPE4
#define SAM2_GPIO_RST         (HW_GPIO3_PIN_MAX + 9)    //GPE9

#define SAM3_GPIO_IO          (HW_GPIO2_PIN_MAX + 15)    //GPD15
#define SAM3_GPIO_RST         (HW_GPIO2_PIN_MAX + 14)    //GPD14


void ic_sam_gpio_init(void)
{
	printk(" BCM5892 SP600  \n");
    if((g_7816_open == 0) && (g_vcc_keep_on == 0)){
        g_ccm3310_num = 0;
        g_sam_card_num = 2;
    }
	/*ic card 、memory card gpio request */
	gpio_request(IC_TDA8035_5V3V,NULL);
	gpio_request(IC_TDA8035_1V8,NULL);
	gpio_request(IC_TDA8035_IO,NULL);
	gpio_request(IC_TDA8035_AUXIO2,NULL);
	gpio_request(IC_TDA8035_AUXIO1,NULL);
	gpio_request(IC_TDA8035_VCC,NULL);
	gpio_request(IC_TDA8035_RST,NULL);
	gpio_request(IC_TDA8035_DET,NULL);
	gpio_request(IC_TDA8035_CLK,NULL);
	/*sam card gpio request*/
	gpio_request(SAM_ALL_GPIO_VCC,NULL);
	gpio_request(SAM1_GPIO_IO,NULL);
	gpio_request(SAM1_GPIO_RST,NULL);
	gpio_request(SAM2_GPIO_IO,NULL);
	gpio_request(SAM2_GPIO_RST,NULL);
	if (g_sam_card_num == 3){
    	gpio_request(SAM3_GPIO_IO,NULL);
    	gpio_request(SAM3_GPIO_RST,NULL);
    }
	/*上拉*/
	ic_vcc_set(VCC5V);
	rst_ic_module();
	ic_power_down();

	reg_write32(REG_GPIO4_PAD_RES,reg_read32(REG_GPIO4_PAD_RES)|(1 << 2)|(1 << 4));

//	reg_gpio_set_pull_up_down_enable(SAM3_GPIO_IO);
//	reg_gpio_set_pull_up_down(SAM3_GPIO_IO, 1);

//	gpio_direction_output(SAM_ALL_GPIO_VCC,1);
	if(g_vcc_keep_on || g_7816_open) gpio_direction_output(SAM_ALL_GPIO_VCC,0); /*TODO modify to pow on */
	else gpio_direction_output(SAM_ALL_GPIO_VCC,1);
	gpio_direction_output(SAM1_GPIO_IO,0);
	gpio_direction_output(SAM1_GPIO_RST,0);
	gpio_direction_output(SAM2_GPIO_IO,0);
	gpio_direction_output(SAM2_GPIO_RST,0);
    if (g_sam_card_num == 3){
        gpio_direction_output(SAM3_GPIO_IO,0);
        gpio_direction_output(SAM3_GPIO_RST,0);
    }
    
}
void ic_sam_gpio_exit(void)
{
	/*ic card 、memory card gpio free */
	gpio_free(IC_TDA8035_5V3V);
	gpio_free(IC_TDA8035_1V8);
	gpio_free(IC_TDA8035_IO);
	gpio_free(IC_TDA8035_AUXIO2);
	gpio_free(IC_TDA8035_AUXIO1);
	gpio_free(IC_TDA8035_VCC);
	gpio_free(IC_TDA8035_RST);
	gpio_free(IC_TDA8035_DET);
	gpio_free(IC_TDA8035_CLK);
	/*sam card gpio free*/
	gpio_free(SAM_ALL_GPIO_VCC);
	gpio_free(SAM1_GPIO_IO);
	gpio_free(SAM1_GPIO_RST);
	gpio_free(SAM2_GPIO_IO);
	gpio_free(SAM2_GPIO_RST);
    if (g_sam_card_num == 3){
    	gpio_free(SAM3_GPIO_IO);
    	gpio_free(SAM3_GPIO_RST);
	}
}
void ic_vcc_set(tda8035_vcc_type vcc)
{
	switch(vcc){
		case VCC5V:
			gpio_direction_output(IC_TDA8035_5V3V,1);
			gpio_direction_output(IC_TDA8035_1V8,1);
		break;
		case VCC3V:
			gpio_direction_output(IC_TDA8035_5V3V,0);
			gpio_direction_output(IC_TDA8035_1V8,1);
		break;
		case VCC1P8V:
			gpio_direction_output(IC_TDA8035_5V3V,1);
			gpio_direction_output(IC_TDA8035_1V8,0);
		break;
		case DEEP_SLEEP:
			gpio_direction_output(IC_TDA8035_5V3V,0);
			gpio_direction_output(IC_TDA8035_1V8,0);
		break;
		default:
		break;
	}
}
void ic_cold_reset(tda8035_vcc_type vcc,uint8_t d)
{
	rst_ic_module();
	ic_vcc_set(vcc);
	//mdelay(20);
	ctrl_ic_clk(d);/*default d = 1*/
	ctrl_ic_io(1);
	ctrl_ic_rst(0);
	ctrl_ic_vcc(0);//vcc cmd  = 0
	wait_ic_clk_cnt(55000);
	ctrl_ic_rst(1);
}
void ic_warm_reset(void)
{
	rst_ic_module();
	ctrl_ic_rst(0);
	ctrl_ic_io(1);
	wait_ic_clk_cnt(42500);
	ctrl_ic_rst(1);
}
void ic_power_down(void)
{
	if(!check_ic_module_work())/*not work in ic mode */
		rst_ic_module();
	ctrl_ic_vcc(1);
	ctrl_ic_rst(0);
	close_ic_clk();
	udelay(50);
	ctrl_ic_io(0);
//	ctrl_ic_vcc(1);
	//ic_vcc_set(DEEP_SLEEP);
	/*set input for low power consumption*/
//	bcm5892_gpio_set_pin_type(IC_TDA8035_IO,input);
//	bcm5892_gpio_set_pin_type(IC_TDA8035_AUXIO1,input);
//	bcm5892_gpio_set_pin_type(IC_TDA8035_AUXIO2,input);
	g_atr.ATROK = 0;
}
/*sam card*/
void sam_select_num(uint8_t samnum)
{
	sam_gpio.vcc = SAM_ALL_GPIO_VCC;
	switch(samnum){
		case SC_SAM1:
			sam_gpio.io = SAM1_GPIO_IO;   //GPD2
			sam_gpio.rst = SAM1_GPIO_RST;   //GPD8
		break;
		case SC_SAM2:
			sam_gpio.io = SAM2_GPIO_IO;   //GPE4
			sam_gpio.rst = SAM2_GPIO_RST;   //GPE9
		break;
	case SC_SAM3:
			sam_gpio.io = SAM3_GPIO_IO;   //GPE4
			sam_gpio.rst = SAM3_GPIO_RST;   //GPE9
		break;
		default:
		break;
	}
}
void sam_cold_rst(uint32_t samnum)
{
	gpio_direction_output(sam_gpio.rst,0);
	if(samnum != g_ccm3310_num ) {
//		if(!g_vcc_keep_on || !g_7816_open) gpio_direction_output(sam_gpio.vcc,0);
		if(!g_vcc_keep_on && !g_7816_open) gpio_direction_output(sam_gpio.vcc,0);
	}
	sam_ctrl_clk(1);
	bcm5892_gpio_set_pin_type(sam_gpio.io,input);
	sam_wait_clk_cnt(40000);
	gpio_direction_output(sam_gpio.rst,1);
}
void sam_warm_rst(uint8_t samnum)
{
	gpio_direction_output(sam_gpio.rst,0);
	bcm5892_gpio_set_pin_type(sam_gpio.io,input);
	sam_wait_clk_cnt(40000);
	gpio_direction_output(sam_gpio.rst,1);
}
void sam_power_down(uint8_t samnum)
{
	//if(sam_gpio.vcc != SAM_ALL_GPIO_VCC)
	sam_select_num(samnum);
	if(samnum != g_ccm3310_num) {
		gpio_direction_output(sam_gpio.rst,0);
	//	sam_ctrl_clk(0);
		gpio_direction_output(sam_gpio.io,0);
//		if(!g_vcc_keep_on||!g_7816_open) gpio_direction_output(sam_gpio.vcc,1);
		if(!g_vcc_keep_on && !g_7816_open) {
			sam_ctrl_clk(0);
			gpio_direction_output(sam_gpio.vcc,1);
		}
//		if(samnum != g_ccm3310_num ) gpio_direction_output(sam_gpio.vcc,1);
	}
	else {
		gpio_direction_output(sam_gpio.rst,0);
		sam_ctrl_clk(0);
		gpio_direction_output(sam_gpio.io,0);
	}
	sam_stop_timer();
	#if 1
	sam_stat[SC_SAM1].power_stat = 0;
	sam_stat[SC_SAM1].atr_stat = 0;

	sam_stat[SC_SAM2].power_stat = 0;
	sam_stat[SC_SAM2].atr_stat = 0;
	#endif
}
void sam_send_io(uint8_t val)
{
	gpio_direction_output(sam_gpio.io,val);
}
int sam_get_io(void)
{
	return __gpio_get_value(sam_gpio.io);
}
void sam_set_io_type(io_type_t type)
{
	bcm5892_gpio_set_pin_type(sam_gpio.io,type);
}
void ic_sam_suspend_ctrl(int val)
{
	if(val != 0) {
		ic_vcc_set(DEEP_SLEEP);
	}
	else {
		ic_vcc_set(VCC3V);
		mdelay(20);
	}
}

