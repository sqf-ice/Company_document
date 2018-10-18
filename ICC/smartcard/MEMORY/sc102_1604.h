#ifndef _SC102_1604_H_
#define _SC102_1604_H_
typedef struct
{
	unsigned int addr;
	unsigned char len;
	unsigned char try_times;
}CHECK_SC_DATA;
extern int sc102_1604_powon(unsigned char *atrbuf);
extern int at88sc102_pro_main(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf);
extern int at88sc1604_pro_main(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf);

#endif
