#ifndef _AT24XX_H_
#define _AT24XX_H_
extern int at24cxx_poweron(unsigned char *atrbuf);
extern int at24cxx_pro_main(unsigned char type_card,const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf);
extern void iic_start(void);
extern void iic_stop(void);
extern int iic_writebyte(unsigned char data);
extern unsigned char iic_readbyte(void);
extern void iic_send_ack(void);
extern void iic_send_nack(void);
#endif

