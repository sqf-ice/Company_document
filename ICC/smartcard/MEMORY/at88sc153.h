/*
 * at88sc153.h
 *
 *  Created on: 2015Äê1ÔÂ26ÈÕ
 *      Author: Gavin
 */

#ifndef AT88SC153_H_
#define AT88SC153_H_
#include <linux/types.h>
typedef struct {
	uint8_t last_calc;
	uint8_t ri0;
	uint8_t si0;
	uint8_t ti0;
	uint8_t ri0_addr[8];
	uint8_t si0_addr[8];
	uint8_t ti0_addr[8];
	uint8_t ri_addr[8];
	uint8_t ci[8];
	uint8_t q1[8];
	uint8_t q2[8];
	uint8_t gc[8];
	uint8_t q0[8];
}sc153_auth_data_t;
int at88sc153_poweron(unsigned char *atrbuf);
int at88sc153_pro_main(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf);
void authenticaiton(sc153_auth_data_t *auth_data);
int compare_q2_ci(sc153_auth_data_t *auth_data);
#endif /* AT88SC153_H_ */
