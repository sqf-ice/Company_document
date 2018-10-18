/*
 * at88sc1608.c
 *
 *  Created on: 2015Äê3ÔÂ11ÈÕ
 *      Author: Gavin
 */
#include "memory.h"
#include "at88sc1608.h"
#include "at88sc153.h"
#include "at24xx.h"
#include <linux/string.h>
int at88sc1608_set_user_addr(uint8_t addr)
{
	int ret = 0;
	int i = 0;
	addr &= 0x07;
	for(i = 0; i < 80; i++) {
		iic_start();
		ret = iic_writebyte(SET_USER_ADDR_CMD);
		if(ret == 0) break;
		iic_stop();
		delay_ms(5);
	}
	if(ret < 0 ) goto end;
	ret = iic_writebyte(addr);
	end: iic_stop();
	return ret;
}
int at88sc1608_read_data(uint8_t cmd, uint8_t addr, uint8_t len, uint8_t *rece_buf)
{
	int ret = 0;
	int i = 0;
	cmd &= READ_CONF_ZONE_CMD;
	for(i = 0; i < 80; i++) {
		iic_start();
		ret = iic_writebyte(cmd);
		if(ret == 0) break;
		iic_stop();
		delay_ms(5);
	}
	if(ret < 0){
		goto end;
	}
	ret = iic_writebyte(addr);
	if(ret < 0) {
		goto end;
	}
	for(i = 0; i < len; i++) {
		rece_buf[i] = iic_readbyte();
		if(i != (len -1)) iic_send_ack();
	}
	iic_send_nack();
	end: iic_stop();
	return ret;
}
int at88sc1608_write_data(uint8_t cmd, uint8_t addr, uint8_t len, const uint8_t *send_buf)
{
	int ret = 0, i = 0;
	cmd &= WRITE_CONF_ZONE_CMD;
	/*after verify passwd ,we should polling this cmd ack ok,so add 2015-09-23*/
	for(i = 0; i < 80; i++) {
		iic_start();
		ret = iic_writebyte(cmd);
		if(ret == 0) break;
		iic_stop();
		delay_ms(5);
	}
	if(ret < 0) goto end;
	ret = iic_writebyte(addr);
	if(ret < 0){
		goto end;
	}
	for(i = 0; i < len; i++) {
		ret = iic_writebyte(send_buf[i]);
		if(ret < 0){
			break;
		}
		addr++;
		if((addr & 0x0f) == 0x00) {
			iic_stop();
			delay_ms(10);
			/*rewrite command and address*/
			iic_start();
			ret = iic_writebyte(cmd);
			if(ret < 0) goto end;
			ret = iic_writebyte(addr);
			if(ret < 0) goto end;
		}
	}
	end: iic_stop();
	delay_ms(10);
	return ret;
}
int at88sc1608_read_fuse(uint8_t *fuse)
{
	int ret = 0;
	iic_start();
	ret = iic_writebyte(READ_CONF_ZONE_CMD);
	if(ret < 0) goto end;
	ret = iic_writebyte(0x80);
	if(ret < 0) goto end;
	*fuse = iic_readbyte();
	iic_send_nack();
	end:iic_stop();
	return ret;
}
int at88sc1608_verify_passwd(uint8_t index, uint8_t *pac, const uint8_t *passwd)
{
	int ret = 0, i = 0;
	index &= 0x0f;
	for(i = 0; i < 80; i++) {
		iic_start();
		ret = iic_writebyte(VERIFY_PASSWD_CMD);
		if(ret == 0) break;
		iic_stop();
		delay_ms(5);
	}
	if(ret < 0) goto end;
	ret = iic_writebyte(index);
	for(i = 0; i < 3; i++) {
		ret = iic_writebyte(passwd[i]);
		if(ret < 0) break;
	}
	end:iic_stop();
	if(ret == 0) {
		/*read pac */
		delay_ms(5);
		ret = at88sc1608_read_data(READ_CONF_ZONE_CMD, 0x40 + ((index & 0x07) << 3) + ((index & 0x08) >> 1) , 1, pac);
	}
	return ret;
}
int at88sc1608_init_auth(uint8_t *random)
{
	int ret = 0, i = 0;
	for(i = 0; i < 80; i++) {
		iic_start();
		ret = iic_writebyte(INIT_AUTH_CMD);
		if(ret == 0) break;
		iic_stop();
		delay_ms(5);
	}
	if(ret < 0) goto end;
	for(i = 0; i < 8; i++) {
		ret = iic_writebyte(random[i]);
		if(ret < 0) break;
	}
	end: iic_stop();
	return ret;
}
int at88sc1608_verify_auth(uint8_t *host_q1)
{
	int ret = 0, i = 0;
	for(i = 0; i < 80; i++) {
		iic_start();
		ret = iic_writebyte(VERIFY_AUTH_CMD);
		if(ret == 0) break;
		iic_stop();
		delay_ms(5);
	}
	if(ret < 0) goto end;
	for(i = 0; i < 8; i++) {
		ret = iic_writebyte(host_q1[i]);
		if(ret < 0) break;
	}
	end: iic_stop();
	return ret;
}
int at88sc1608_authentication(const uint8_t *send_buf, uint8_t len, uint8_t *aac)
{
	sc153_auth_data_t auth_data;
	uint8_t rece_buf[20] = {0,};
	int i = 0, ret = 0;
	memset(&auth_data, 0, sizeof(auth_data));
	*aac = 0xff;
	/*read cryptogram (ci)*/
	ret = at88sc1608_read_data(READ_CONF_ZONE_CMD, 0x28, 8, rece_buf);
	if(ret < 0) return -1001;
	for(i = 0; i < 8; i++) {
		auth_data.q0[i] = send_buf[i];
		auth_data.gc[i] = send_buf[i + 8];
		auth_data.ci[i] = rece_buf[i];
	}
	authenticaiton(&auth_data);
	ret = at88sc1608_init_auth(auth_data.q0);
	if(ret < 0) return -1001;
	delay_ms(10);
	ret = at88sc1608_verify_auth(auth_data.q1);
	if(ret < 0) return -1001;
	delay_ms(10);
	/*read cryptogram (ci)*/
	ret = at88sc1608_read_data(READ_CONF_ZONE_CMD, 0x28, 8, rece_buf);
	if(ret < 0) return -1001;
	for(i = 0; i < 8; i++) {
		auth_data.ci[i] = rece_buf[i];
	}
	if(compare_q2_ci(&auth_data) == 0) {
		ret = at88sc1608_read_data(READ_CONF_ZONE_CMD, 0x20, 1, rece_buf);
		if(ret < 0) return -1001;
		*aac = rece_buf[0];
	}
	return 0;
}
int at88sc1608_poweron(unsigned char *atrbuf)
{
	int temp = 0,i = 0, j = 0;
	delay_ms(1);
	if(!check_mem_insert()) {
		mem_printk("1608 card not insert! \n");
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
		mem_printk("at88sc1608 pow on get io err \n");
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
void at88sc1608_pro_auth(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{
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
	ret = at88sc1608_authentication(&sendbuf[5], 16, &aac);
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
	return;
}
void at88sc1608_pro_verify_passwd(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{
	int ret = 0;
	uint8_t pac = 0;
	if(sendbuf[4] != 0x03) {
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return;
	}
	if((sendbuf[2] != 3) || (sendbuf[3] > 15)) {
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return;
	}
	ret = at88sc1608_verify_passwd(sendbuf[3], &pac, &sendbuf[5]);
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
void at88sc1608_pro_read(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{
	uint8_t cmd = 0;
	int ret = 0;
	if(sendbuf[2] == 1) {
		cmd = READ_USER_ZONE_CMD;
		if((sendbuf[4] == 0) || (sendbuf[4] + sendbuf[3] > 256) || (sendbuf[4] > 248)) {
			recebuf[0] = 0x67;
			recebuf[1] = 0x00;
			*rcvlen = 2;
			return;
		}
	}
	else if (sendbuf[2] == 5) {
		cmd = READ_CONF_ZONE_CMD;
		if((sendbuf[4] == 0) || (sendbuf[4] + sendbuf[3] > 129)) {
			recebuf[0] = 0x67;
			recebuf[1] = 0x00;
			*rcvlen = 2;
			return;
		}
	}
	else {
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return;
	}
	ret = at88sc1608_read_data(cmd, sendbuf[3], sendbuf[4], recebuf);
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
void at88sc1608_pro_write(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
{
	uint8_t cmd = 0;
	int ret = 0;
	if(sendbuf[2] == 0) {
		cmd = WRITE_USER_ZONE_CMD;
		if((sendbuf[4] == 0) || (sendbuf[4] + sendbuf[3] > 256) || (sendbuf[4] > 248)) {
			recebuf[0] = 0x67;
			recebuf[1] = 0x00;
			*rcvlen = 2;
			return;
		}
	}
	else if (sendbuf[2] == 4) {
		cmd = WRITE_CONF_ZONE_CMD;
		if((sendbuf[4] == 0) || (sendbuf[4] + sendbuf[3] > 129)) {
			recebuf[0] = 0x67;
			recebuf[1] = 0x00;
			*rcvlen = 2;
			return;
		}
		if(((sendbuf[3] == 0x80) && (sendbuf[4] != 0x00)) ||
				((sendbuf[3] != 0x80) && (sendbuf[4] == 0x00))) {
			recebuf[0] = 0x67;
			recebuf[1] = 0x00;
			*rcvlen = 2;
			return;
		}
	}
	else if(sendbuf[2] == 2)
	{
		if(sendbuf[3] > 7) {
			recebuf[0] = 0x6a;
			recebuf[1] = 0x86;
			*rcvlen = 2;
			return;
		}
		if(sendbuf[4] != 0) {
			recebuf[0] = 0x67;
			recebuf[1] = 0x00;
			*rcvlen = 2;
			return;
		}
		ret = at88sc1608_set_user_addr(sendbuf[3]);
		if(ret < 0) {
			recebuf[0] = 0x65;
			recebuf[1] = 0x81;
			*rcvlen = 2;
			return;
		}
		recebuf[0] = 0x90;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return;
	}
	else {
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return;
	}
	ret = at88sc1608_write_data(cmd, sendbuf[3], sendbuf[4], &sendbuf[5]);
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
int at88sc1608_pro_main(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf)
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
			 at88sc1608_pro_auth(sendbuf, rcvlen, recebuf);
		break;
		case 0x20:
			at88sc1608_pro_verify_passwd(sendbuf, rcvlen, recebuf);
		break;
		case 0xB0:
			at88sc1608_pro_read(sendbuf, rcvlen, recebuf);
		break;
		case 0xD0:
			at88sc1608_pro_write(sendbuf, rcvlen, recebuf);
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
