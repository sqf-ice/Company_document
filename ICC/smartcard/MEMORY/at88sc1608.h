/*
 * at88sc1608.h
 *
 *  Created on: 2015Äê3ÔÂ11ÈÕ
 *      Author: Gavin
 */

#ifndef AT88SC1608_H_
#define AT88SC1608_H_

#define WRITE_USER_ZONE_CMD       (0xB0)
#define READ_USER_ZONE_CMD        (0xB1)
#define WRITE_CONF_ZONE_CMD       (0xB4)
#define READ_CONF_ZONE_CMD        (0xB5)
#define SET_USER_ADDR_CMD         (0xB2)
#define VERIFY_PASSWD_CMD         (0xB3)
#define INIT_AUTH_CMD             (0xB6)
#define VERIFY_AUTH_CMD           (0xB7)
extern int at88sc1608_poweron(unsigned char *atrbuf);
int at88sc1608_pro_main(const uint8_t *sendbuf, uint8_t *rcvlen, uint8_t *recebuf);
#endif /* AT88SC1608_H_ */
