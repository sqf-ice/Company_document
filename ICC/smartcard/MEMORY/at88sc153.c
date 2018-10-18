/*
 * at88sc153.c
 *
 *  Created on: 2015Äê1ÔÂ26ÈÕ
 *      Author: Gavin
 */

#include "memory.h"
#include "at88sc153.h"
#include "at24xx.h"
#include <linux/string.h>
void iic_send_ack(void)
{
	tda8035_set_io(0);
	delay_us(2);
	tda8035_set_clk(1);
	delay_us(10);
	tda8035_set_clk(0);
	delay_us(2);
	tda8035_set_io(1);
}
void iic_send_nack(void)
{
	tda8035_set_io(1);
	delay_us(2);
	tda8035_set_clk(1);
	delay_us(10);
	tda8035_set_clk(0);
	delay_us(2);
	tda8035_set_io(0);
}
int at88sc153_read_eeprom(uint8_t comm,uint8_t addr,uint8_t length,uint8_t *rece_buf)
{
	int ret = 0, i = 0;
	iic_start();
	ret = iic_writebyte((comm & 0x0f) | 0xB0);
	if(ret < 0 ) goto end;
	ret = iic_writebyte(addr);
	if(ret < 0 ) goto end;
	for(i = 0; i < length; i++) {
		rece_buf[i] = iic_readbyte();
		if(i != (length -1)) iic_send_ack();
	}
	iic_send_nack();
	end: iic_stop();
	return ret;
}
int at88sc153_write_eeprom(uint8_t comm, uint8_t addr, uint8_t length, const uint8_t *send_buf)
{
	int ret = 0, i = 0;
	for(i = 0; i < 80; i++) {
		delay_ms(5);
		iic_start();
		ret = iic_writebyte((comm & 0x0f) | 0xB0);
		if(ret == 0) break;
	}
	if(ret < 0){
		goto end;
	}
	ret = iic_writebyte(addr);
	if(ret < 0){
		goto end;
	}
	for(i = 0; i < length; i++) {
		ret = iic_writebyte(send_buf[i]);
		if(ret < 0){
			break;
		}
		addr++;
		if(((addr & 0x0f) == 0x08) || ((addr & 0x0f) == 0x00)) {
			iic_stop();
			delay_ms(10);
			/*rewrite command and address*/
			iic_start();
			ret = iic_writebyte((comm & 0x0f) | 0xB0);
			if(ret < 0) goto end;
			ret = iic_writebyte(addr);
			if(ret < 0) goto end;
		}
	}
	end: iic_stop();
	delay_ms(10);
	return ret;
}
int at88sc153_read_fuse(uint8_t *fuse)
{
	int ret = 0;
	iic_start();
	ret = iic_writebyte(0xBE);
	if(ret < 0) goto end;
	*fuse = iic_readbyte();
	iic_send_nack();
	end: iic_stop();
	return ret;
}
int at88sc153_write_fuse(uint8_t fuse)
{
	int ret = 0;
	iic_start();
	ret = iic_writebyte(0xBA);
	if(ret < 0) goto end;
	ret = iic_writebyte(fuse & 0x07);
	end: iic_stop();
	return ret;
}
int at88sc153_verify_passwd(uint8_t cmd,uint8_t pac_addr,uint8_t *pac, const uint8_t *passwd)
{
	int ret = 0, i = 0, j = 0;
	/*send cmmand twice*/
	for(i = 0; i < 2; i++) {
		for(j = 0;j < 80; j++){
			iic_start();
			ret = iic_writebyte((cmd & 0x0f) | 0xB0);
			if(ret == 0 ) break;
		}
		if(ret < 0) goto end;
		for(j = 0; j < 3; j++) {
			ret = iic_writebyte(passwd[j]);
			if(ret < 0) goto end;
		}
	}
	end: iic_stop();
	if(ret == 0) {
		delay_ms(5);
		ret = at88sc153_read_eeprom(0xBD,pac_addr,1,pac);
	}
	return ret;
}
int at88sc153_init_auth(uint8_t *random)
{
	int ret = 0, i = 0;
	iic_start();
	ret = iic_writebyte(0xB2);
	if(ret < 0) goto end;
	for(i = 0; i < 8; i++) {
		ret = iic_writebyte(random[i]);
		if(ret < 0) break;
	}
	end:iic_stop();
	return ret;
}
int at88sc153_verify_auth(uint8_t *host_q1)
{
	int ret = 0, i = 0;
	iic_start();
	ret = iic_writebyte(0xB6);
	if(ret < 0) goto end;
	for(i = 0; i < 8; i++) {
		ret = iic_writebyte(host_q1[i]);
		if(ret < 0) break;
	}
	end: iic_stop();
	return ret;
}
int at88sc153_authentication(const uint8_t *send_buf,uint8_t len,uint8_t *aac)
{
	sc153_auth_data_t auth_data;
	uint8_t rece_buf[20] = {0,};
	int i = 0, ret = 0;
	memset(&auth_data, 0, sizeof(auth_data));
	/*read cryptogram (ci)*/
	ret = at88sc153_read_eeprom(0xbd,0x20,8,rece_buf);
	if(ret < 0) return -1001;
	for(i = 0; i < 8; i++) {
		auth_data.q0[i] = send_buf[i];
		auth_data.gc[i] = send_buf[i + 8];
		auth_data.ci[i] = rece_buf[i];
	}
	authenticaiton(&auth_data);
	ret = at88sc153_init_auth(auth_data.q0);
	if(ret < 0) return -1001;
	delay_ms(10);
	ret = at88sc153_verify_auth(auth_data.q1);
	if(ret < 0) return -1001;
	delay_ms(10);
	/*read cryptogram (ci)*/
	ret = at88sc153_read_eeprom(0xbd,0x20,8,rece_buf);
	if(ret < 0) return -1001;
	for(i = 0; i < 8; i++) {
		auth_data.ci[i] = rece_buf[i];
	}
	if(compare_q2_ci(&auth_data) == 0) {
		ret = at88sc153_read_eeprom(0xbd,0x20,1,rece_buf);
		if(ret < 0) return -1001;
		*aac = rece_buf[0];
	}
	return 0;
}

void calcul_state(sc153_auth_data_t *auth_data)
{
	uint8_t i = 0, temp = 0;
	uint8_t bit0 = 0;
	for(i = 0; i < 8; i++) {
		auth_data->ri_addr[i] = auth_data->ri0_addr[i];
	}
	bit0 = ((auth_data->ri_addr[6] >> 4) & 0x01);
	if((auth_data->ri_addr[6] & 0x80) == 0x80) i = ((auth_data->ri_addr[6] << 1) | 0x01);
	else i = (auth_data->ri_addr[6] << 1);
	temp = ((bit0 | i) & 0x1f);

	auth_data->ri_addr[6] = auth_data->ri_addr[5];
	auth_data->ri_addr[5] = auth_data->ri_addr[4];
	auth_data->ri_addr[4] = auth_data->ri_addr[3];
	auth_data->ri_addr[3] = auth_data->ri_addr[2] ^ auth_data->ri0;
	auth_data->ri_addr[2] = auth_data->ri_addr[1];
	auth_data->ri_addr[1] = auth_data->ri_addr[0];
	auth_data->ri_addr[0] = auth_data->ri_addr[4] + temp;
	if(auth_data->ri_addr[0] > 31) {
		auth_data->ri_addr[0] %= 31;
		if(auth_data->ri_addr[0] == 0x00) auth_data->ri_addr[0] = 31;
	}
	for(i = 0; i < 8; i++) {
		auth_data->ri0_addr[i] = auth_data->ri_addr[i];
		auth_data->ri_addr[i] = auth_data->si0_addr[i];
	}

	bit0 = ((auth_data->ri_addr[6] >> 6) & 0x01);
	if((auth_data->ri_addr[6] & 0x80) == 0x80) i = ((auth_data->ri_addr[6] << 1) | 0x01);
	else i = (auth_data->ri_addr[6] << 1);
	temp = ((bit0 | i) & 0x7f);

	auth_data->ri_addr[6] = auth_data->ri_addr[5];
	auth_data->ri_addr[5] = auth_data->ri_addr[4] ^ auth_data->si0;
	auth_data->ri_addr[4] = auth_data->ri_addr[3];
	auth_data->ri_addr[3] = auth_data->ri_addr[2];
	auth_data->ri_addr[2] = auth_data->ri_addr[1];
	auth_data->ri_addr[1] = auth_data->ri_addr[0];
	auth_data->ri_addr[0] = auth_data->ri_addr[6] + temp;
	if(auth_data->ri_addr[0] > 127) {
		auth_data->ri_addr[0] %= 127;
		if(auth_data->ri_addr[0] == 0x00) auth_data->ri_addr[0] = 127;
	}
	for(i = 0; i < 8; i++) {
		auth_data->si0_addr[i] = auth_data->ri_addr[i];
	}
	for(i = 0; i < 5; i++) {
		auth_data->ri_addr[i] = auth_data->ti0_addr[i];
	}
	temp = auth_data->ri_addr[4];

	auth_data->ri_addr[4] = auth_data->ri_addr[3];
	auth_data->ri_addr[3] = auth_data->ri_addr[2];
	auth_data->ri_addr[2] = auth_data->ri_addr[1] ^ auth_data->ti0;
	auth_data->ri_addr[1] = auth_data->ri_addr[0];
	auth_data->ri_addr[0] = auth_data->ri_addr[3] + temp;
	if(auth_data->ri_addr[0] > 31) {
		auth_data->ri_addr[0] %= 31;
		if(auth_data->ri_addr[0] == 0x00) auth_data->ri_addr[0] = 31;
	}
	for(i = 0; i < 5; i++) {
		auth_data->ti0_addr[i] = auth_data->ri_addr[i];
	}
}
void calcul_v(sc153_auth_data_t *auth_data)
{
	uint8_t temp = 0;
	uint8_t si_temp = 0;
	auth_data->last_calc = 0;
	auth_data->si0 = 0;
	auth_data->ri0 = 0;
	auth_data->ti0 = 0;
	calcul_state(auth_data);
	auth_data->ri_addr[0] = auth_data->ri0_addr[0];
	auth_data->ri_addr[4] = auth_data->ri0_addr[4];
	auth_data->ri0 = ((auth_data->ri_addr[0] ^ auth_data->ri_addr[4]) & 0x1f);

	auth_data->ri_addr[0] = auth_data->ti0_addr[0];
	auth_data->ri_addr[3] = auth_data->ti0_addr[3];
	auth_data->ti0 = ((auth_data->ri_addr[0] ^ auth_data->ri_addr[3]) & 0x1f);
	auth_data->si0 = auth_data->si0_addr[0];

	si_temp = 0xff - auth_data->si0;
	temp = auth_data->ri0 & si_temp;
	auth_data->last_calc = ((temp | (auth_data->ti0 & auth_data->si0)) & 0x0f);
}
void calcul_key(uint8_t key, sc153_auth_data_t *auth_data)
{
	uint8_t temp = 0;
	auth_data->ri0 = key & 0x1f;
	temp = (key << 3) & 0x78;
	auth_data->si0 = temp + ((key >> 5) & 0x07);
	auth_data->ti0 = (key >> 3) & 0x1f;
	calcul_state(auth_data);
}
void authenticaiton(sc153_auth_data_t *auth_data)
{
	uint8_t i = 0, q1_temp = 0, q2_temp = 0;
	for(i = 0; i < 4; i++) {
		calcul_key(auth_data->ci[i * 2], auth_data);
		calcul_key(auth_data->ci[i * 2 + 1], auth_data);
		calcul_key(auth_data->q0[i], auth_data);
	}
	for(i = 0; i < 4; i++) {
		calcul_key(auth_data->gc[i * 2], auth_data);
		calcul_key(auth_data->gc[i * 2 + 1], auth_data);
		calcul_key(auth_data->q0[i + 4], auth_data);
	}
	for(i = 0; i < 8; i++) {
		calcul_v(auth_data);
		calcul_v(auth_data);
		q2_temp = ((auth_data->last_calc << 4) | (auth_data->last_calc >> 4));
		calcul_v(auth_data);
		calcul_v(auth_data);
		q2_temp |= auth_data->last_calc;

		calcul_v(auth_data);
		calcul_v(auth_data);
		q1_temp = ((auth_data->last_calc << 4) | (auth_data->last_calc >> 4));
		calcul_v(auth_data);
		calcul_v(auth_data);
		q1_temp |= auth_data->last_calc;

		auth_data->q1[i] = q1_temp;
		auth_data->q2[i] = q2_temp;
	}
}
int compare_q2_ci(sc153_auth_data_t *auth_data)
{
	int i = 0;
	for(i = 0; i < 8; i++) {
		if(auth_data->q2[i] != auth_data->ci[i])
			return 0;
	}
	return 1;
}
void at88sc153_pro_auth(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{
#if 0
	uint8_t aac = 0;
	int ret = 0;
	if(sendbuf[4] != 0x10) {
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return;
	}
	if((sendbuf[2] != 0) || (sendbuf[3] != 0)) {
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return;
	}

	ret = at88sc153_authentication(&sendbuf[5], 16, &aac);
	if(ret < 0) {
		recebuf[0] = 0x65;
		recebuf[1] = 0x81;
		*rcvlen = 2;
		return;
	}
	if(aac == 0x00) {
		recebuf[0] = 0x69;
		recebuf[1] = 0x83;
		*rcvlen = 2;
		return;
	}
	if(aac != 0xff) {
		recebuf[0] = 0x63;
		recebuf[1] = aac;
		*rcvlen = 2;
		return;
	}

	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
#else
	if(sendbuf[4] != 0x10) {
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return;
	}
	if((sendbuf[2] != 0) || (sendbuf[3] != 0)) {
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return;
	}
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
#endif
	return;
}
void at88sc153_pro_verify_passwd(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{
	int ret = 0;
	uint8_t pac = 0;
	if(sendbuf[4] != 0x03) {
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return;
	}
	ret = at88sc153_verify_passwd(sendbuf[2], sendbuf[3], &pac, &sendbuf[5]);
	if(ret < 0) {
		recebuf[0] = 0x65;
		recebuf[1] = 0x81;
		*rcvlen = 2;
		return;
	}
	if(pac == 0x00) {
		recebuf[0] = 0x69;
		recebuf[1] = 0x83;
		*rcvlen = 2;
		return;
	}
	if(pac != 0xff) {
		recebuf[0] = 0x63;
		recebuf[1] = pac;
		*rcvlen = 2;
		return;
	}
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
}
void at88sc153_pro_read(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{

	int ret = 0;
	if((sendbuf[4] == 0) || (sendbuf[4] + sendbuf[3] > 64)) {
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return;
	}
	ret = at88sc153_read_eeprom(sendbuf[2], sendbuf[3], sendbuf[4], recebuf);
	if(ret < 0) {
		recebuf[0] = 0x65;
		recebuf[1] = 0x81;
		*rcvlen = 2;
		return;
	}
	*rcvlen = sendbuf[4] + 2;
	recebuf[sendbuf[4]] = 0x90;
	recebuf[sendbuf[4] + 1] = 0x00;
}
void at88sc153_pro_write(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{
	int ret = 0;
	if((sendbuf[4] == 0) || (sendbuf[4] + sendbuf[3] > 64)) {
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return;
	}
	ret = at88sc153_write_eeprom(sendbuf[2], sendbuf[3], sendbuf[4], &sendbuf[5]);
	if(ret < 0) {
		recebuf[0] = 0x65;
		recebuf[1] = 0x81;
		*rcvlen = 2;
		return;
	}
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
}
int at88sc153_poweron(unsigned char *atrbuf)
{
	unsigned char i = 0, j = 0,temp = 0;
	delay_ms(1);
	if(!check_mem_insert()) {
		mem_printk("153 card not insert! \n");
		return -1;
	}
	tda8035_set_io(1);
	tda8035_set_clk(0);
	tda8035_set_vcc_onoff(0);
	tda8035_set_io_type(input);
	delay_ms(4);
	tda8035_poweron_clk();
	tda8035_set_rst(1);
	delay_us(10);
	tda8035_set_clk(1);
	delay_us(10);
	if(!tda8035_get_io()) {
		mem_printk("at88sc153 pow on get io err \n");
		return -1;
	}
	tda8035_set_clk(0);
	delay_us(10);
	tda8035_set_rst(0);
	delay_us(10);
	/*read 4 byte atr*/
	for(i = 1; i < 5; i++) {
		temp = 0;
		for(j = 0; j < 8; j++) {
			tda8035_set_clk(1);
			delay_us(10);
			temp >>= 1;
			if(tda8035_get_io()) temp |= 0x80;
			tda8035_set_clk(0);
			delay_us(10);
		}
		atrbuf[i] = temp;
	}
	atrbuf[0] = 6;
	atrbuf[5] = 0x90;
	atrbuf[6] = 0x00;
	return 0;
}
int at88sc153_pro_main(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{
	int ret = 0;
	#if 1
		if(!check_mem_insert())
		{
			mem_printk("no card\n");
			return -1;
		}
	#endif
	tda8035_set_io(0);
	switch(sendbuf[1])
	{
		case 0x82:
			 at88sc153_pro_auth(sendbuf, rcvlen, recebuf);
		break;
		case 0x20:
			at88sc153_pro_verify_passwd(sendbuf, rcvlen, recebuf);
		break;
		case 0xB0:
			at88sc153_pro_read(sendbuf, rcvlen, recebuf);
		break;
		case 0xD0:
			at88sc153_pro_write(sendbuf, rcvlen, recebuf);
		break;
		default:
		recebuf[0] = 0x6d;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		ret = -1;
		break;
	}
	return ret;
}
