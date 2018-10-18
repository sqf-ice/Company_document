#ifndef	_NDKVOICE_H_
#define _NDKVOICE_H_
#include <linux/ioctl.h>

struct WAV_PARA
{
	int rate;
	int datalen;
};

typedef enum {
	VOICE_PLAYSTATUS_IDLE=0,
	VOICE_PLAYSTATUS_BUSY,
	VOICE_PLAYSTATUS_PARA_NO_SET
}EM_VOICE_PLAYSTATUS;

#define DAC_IOC_MAGIC 'D'
#define PLAY_IOCG_VER		_IO(DAC_IOC_MAGIC, 1)
#define PLAY_IOCS_PARA		_IO(DAC_IOC_MAGIC, 2)
#define PLAY_IOCG_STATUS	_IO(DAC_IOC_MAGIC, 3)
#define PLAY_IOCS_CTRL		_IO(DAC_IOC_MAGIC, 4)
#define PLAY_IOCS_VOLUME           _IO(DAC_IOC_MAGIC, 5) 


int NDK_VoiceCtrl(uint unVoiceid,EM_VOICE_CTRL emCtrlId,uint unValue);
int NDK_VoiceLoadFile(uint unVoiceID,char *pszFile);
#endif
