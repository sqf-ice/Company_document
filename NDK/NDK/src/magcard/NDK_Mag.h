/*
* �´�½��˾ ��Ȩ����(c) 2006-2008
*
* POS API
* �ļ��������	--- NDK_Mag.c	*
* ��    �ߣ�    		��Ʒ������
* ��    �ڣ�    		2012-09-21
* ����޸��ˣ�  	huds
* ����޸����ڣ�
*/
#ifndef _NDK_MCR_H_
#define _NDK_MCR_H_
//#include <stdint.h>

/* �豸���� */
#define MAG_DEV_STRING			"mag"

/* ���豸�� */
#define MAG_MAJOR				10		//240
#define	MAG_MINOR				246		//0
#define MAG_DEVS				1		/*number of mag devices*/

/*IOCTL definitions */
#define MAG_IOC_MAGIC 					'M'
#define	MAG_IOCS_RESET					_IO(MAG_IOC_MAGIC, 0)
#define MAG_IOCG_INFO					_IO(MAG_IOC_MAGIC, 1)
#define MAG_IOCG_STATUS 				_IO(MAG_IOC_MAGIC, 2)
#define MAG_IOCG_SWIPED					_IO(MAG_IOC_MAGIC, 3)
#define MAG_IOCG_TOTAL_SWIPED			_IO(MAG_IOC_MAGIC, 4)
#define MAG_IOCS_START					_IO(MAG_IOC_MAGIC, 5)
#define MAG_IOCS_END					_IO(MAG_IOC_MAGIC, 6)

#define MAG_IOCG_TRACK 					_IOR(MAG_IOC_MAGIC, 0, int)
#define MAG_IOCS_TRACK 					_IOW(MAG_IOC_MAGIC, 1, int)
#define MAG_IOCS_STRICT 				_IOW(MAG_IOC_MAGIC, 2, int)
#define MAG_IOCG_VER 					_IOR(MAG_IOC_MAGIC, 3, int)
#define MAG_IOCG_CRMODE 				_IOR(MAG_IOC_MAGIC, 4, int)

/* ������״̬��Ϣ */
#define MAG_IDLE						(1<<0)
#define MAG_READY						(1<<1)
#define MAG_BUSY						(1<<2)
#define MAG_ERROR						(1<<3)

/*
 * Track 1 Notes:
 * 210 bpi, 7 bit data (includes 1 ODD parity bit), max of 79 alphanumeric chars, RO
 * To convert raw data to ASCII add 0x20 to each 6-bit data byte.
 * Start sentinel:	0x25
 * End sentinel:	0x3F
 */

/*
 * Track 2 Notes:
 * 75 bpi, 5 bit data (includes 1 ODD parity bit), max of 40 numeric chars, RO
 * To convert raw data to ASCII add 0x30 to each 4-bit data byte.
 * Start sentinel:	0x3B
 * End sentinel:	0x3F
 */

/*
 * Track 3 Notes:
 * 210 bpi, 7 bit data (includes 1 ODD parity bit), max of 107 numeric chars, RW
 * To convert raw data to ASCII add 0x20 to each 6-bit data byte.
 * Start sentinel:	0x25
 * End sentinel:	0x3F
 */ 
typedef struct BSP_MagCardInfo {
    char	type[16];	// �ͺ� "za9l mag"
    int		version;	// �汾�� 100
    char	drv_ver[8];	// �����汾
    int		function;	// �������� bit1:read; bit2:write
    int		format;		// �ŵ����� bit1:IBM; bit2:ISO
}BSP_MagCardInfo;

#define  TK2    				0x02								/*ֻ������*/
#define  TK3    				0x04								/*ֻ������*/
#define  TK2_3    				(TK2 | TK3)   						/* 0x06  ͬʱ��������*/
#define	 TK1					0x01								//ֻ��һ�ŵ�
#define	 TK1_2_3				( TK1 | TK2 | TK3 )
#define	 TK1_2					( TK1 | TK2 )
#define  TK1_3					( TK1 | TK3 )

#endif
/*********************************************************************************************************
*                                       END
*********************************************************************************************************/
