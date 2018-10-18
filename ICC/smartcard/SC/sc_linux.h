#ifndef _SC_LINUX_H_
#define _SC_LINUX_H_



#define MaxBufLen_CAPDU    (261)
#define MaxBufLen_RAPDU    (512)

typedef struct  
{
	unsigned char	devnum;		//…Ë±∏∫≈¬Î
	unsigned char	buf_capdu[MaxBufLen_CAPDU];
	unsigned int	capdu_len;
	unsigned char	buf_rapdu[MaxBufLen_RAPDU];
	unsigned int	rapdu_len;
	unsigned char	cmdflag;
	int				retflag;
}ioctl_CMDarg;
typedef struct 
{
	volatile int ic1;
	volatile int ic2;
}IC_TYPE;
typedef struct 
{
	int flag_pollselect;
}IC_PARAM;
typedef struct
{
	spinlock_t lock;
	unsigned long flag;
}spin_lock;
#define sc_dev_name "sc0"
#define sc_dev_minor 3
#define ModuleVersion "2.2.5.1"

#define __SUCC 0
#define __FAIL -1

#define Magic	'S'
#define N_POWERDOWN					_IOWR(Magic, 1, int)
#define N_POWERON					_IOWR(Magic, 2, int)
#define N_GetState					_IOWR(Magic, 3, int)
#define N_GetState_c 				_IOWR(Magic, 4, int)
#define N_SC_start					_IOWR(Magic, 66, int)
#define N_GetPowerSta				_IOWR(Magic, 67, int)
#define ioctl_SC_APDU				_IOWR(Magic, 68, int)
#define ioctl_SC_Read				_IOWR(Magic, 69, int)
#define ioctl_SetICType				_IOWR(Magic, 70, int)
#define ioctl_GetICType				_IOWR(Magic, 71, int)
#define ioctl_SC_PowerOn			_IOWR(Magic, 72, int)
#define ioctl_ClearICInit			_IOWR(Magic, 73, int)
#define ioctl_ResetSC				_IOWR(Magic, 74, int)

#define ioctl_mem_init_card			_IOWR(Magic, 80, int)
#define ioctl_mem_card_powerup		_IOWR(Magic, 81, int)
#define ioctl_mem_card_powerdown	_IOWR(Magic, 82, int)
#define ioctl_mem_card_getsta		_IOWR(Magic, 83, int)
#define ioctl_mem_card_rw			_IOWR(Magic, 84, int)


#define ioctl_gpiotest				_IOWR(Magic, 100, int)
#define ioctl_gpiotest_end			_IOWR(Magic, 101, int)

#define ioctl_ifsderr				_IOWR(Magic, 103, int)
#define ioctl_ppsset				_IOWR(Magic, 104, int)
#define ioctl_ppsget				_IOWR(Magic, 105, int)

#define ioctl_setatrin4t			_IOWR(Magic, 106, int)
#define ioctl_getatrin4t			_IOWR(Magic, 107, int)

#define ioctl_getvision				_IOWR(Magic, 108, int)

#define ioctl_getjiffies			_IOWR(Magic, 109, int)
#define ioctl_icsamactivate			_IOWR(Magic, 200, int)
#define ioctl_icsamcoldreset		_IOWR(Magic, 201, int)
#define ioctl_icsamwarmreset		_IOWR(Magic, 202, int)

#define IC_IOCG_WORKSTATUS		    _IO(Magic, 205)
#define ioctl_ic_insert_times       _IOWR(Magic, 206, int)
#define ioctl_sc_suspend            _IOWR(Magic, 207, int)

#endif

