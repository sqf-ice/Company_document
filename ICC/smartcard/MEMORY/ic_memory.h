#ifndef _IC_MEMORY_H_
#define _IC_MEMORY_H_
#define rw_memorycard_maxlength  (238+10)   // 读写Memory数据的最大长度

//-------------------------------------
//Memory卡类型定义
#define   ic0cpucard            0x00
#define   ic0at24c01            0x01
#define   ic0at24c02            0x02
#define   ic0at24c04            0x03
#define   ic0at24c08            0x04
#define   ic0at24c16            0x05
#define   ic0at24c32            0x06
#define   ic0at24c64            0x07
#define   ic0at45d041_3v        0x08
#define   ic0sle44x2            0x09
#define   ic0at88sc102          0x0a
#define   ic0at88sc1604         0x0b
#define   ic0at88sc1608         0x0c 
#define   ic0sle44x8            0x0d
#define   ic0at88sc153          0x0e 
//--------------------------------------
extern int mem_powon_flag ;

extern int memory_poweron(unsigned char mem_type,unsigned char *atrbuf);
extern int memory_rw(unsigned char mem_type,const unsigned char *sendbuf,
				  unsigned char *rcvlen,unsigned char *recebuf);


#endif
