#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_Voice.h"
#include "ndk_wav.h"

#define ARR_SIZE(a)             (sizeof((a))/sizeof((a[0])))
#define DEBUG 1
#ifdef DEBUG
#define PDEBUG(fmt, args...) fprintf(stderr,  "VOICE: "fmt, ##args)
#else
#define PDEBUG(fmt, args...)
#endif

int voice_fd = -1;
int currentvoicefd = -111;
struct WAV_PARA wav_para;
/**
 *@brief    ���ſ��ơ�������ǰ�������NDK_VoiceLoadFile��uint unVoiceID,char *pszFile������Ӧ����Ƶ�ļ����룬��Ƶ���뵽�����У�ͨ��id���ſ��Ʋ�ͬ����Ƶ�ļ�����
 *@param        unVoiceId   ��ƵID
 *@param        emCtrlId    ����ѡ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"    �豸�ļ���ʧ��
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(unVoiceId�Ƿ�)
 *@li   \ref NDK_ERR "NDK_ERR"              ����ʧ��(������Ƶ�ļ�ʧ�ܣ����ܸ�ID��Ӧ����Ƶ�ļ�δ����NDK_VoiceLoadFile���м��ء�)
 *@li   \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL"  �����ӿڵ��ô���(��Ƶ�����ӿڵ���ʧ�ܷ���)
*/
NEXPORT int NDK_VoiceCtrl(uint unVoiceId,EM_VOICE_CTRL emCtrlId,uint unValue)
{
    VOICE_SECTION_HEADER *voicesection;
    int ret = 0,voicestatus = 0;
    if(emCtrlId<VOICE_CTRL_START||emCtrlId>VOICE_CTRL_VOLUME)
        return NDK_ERR_PARA;
    if(unValue<0||unValue>4) {
        fprintf(stderr,"----[%s][%d][%d]\n",__func__,__LINE__,unValue);
        return NDK_ERR_PARA;
    }
    if(emCtrlId==VOICE_CTRL_VOLUME) {
        ioctl(voice_fd, PLAY_IOCS_VOLUME, &unValue);
        return NDK_OK;
    }
    if(currentvoicefd!=unVoiceId) {
        if(voice_fd>0)
            close(voice_fd);
        voice_fd=open("/dev/dac", O_RDWR);
        if(voice_fd<0) {
            PDEBUG("%s open voice fail\n", __func__);
            return NDK_ERR_OPEN_DEV;
        }
        if((voicesection=getvoice_wav(unVoiceId))==NULL) {
            fprintf(stderr,"-------[%s][%d]get voice wav fail\n",__func__,__LINE__);
            return NDK_ERR;
        }
        ret=ioctl(voice_fd, PLAY_IOCG_STATUS, &voicestatus);
        if(ret<0) {
            PDEBUG("%s ��Ƶ״̬��ȡʧ��\n", __func__);
            return NDK_ERR_IOCTL;
        }
        if(voicestatus != VOICE_PLAYSTATUS_IDLE) {
            PDEBUG("%s ����״̬���ǿ���\n", __func__);
            goto EXIT;
        }
        wav_para.datalen=voicesection->vw->sample_lenth;
        wav_para.rate=voicesection->vw->rate;
        ret=ioctl(voice_fd, PLAY_IOCS_PARA, &wav_para);
        if(ret<0) {
            PDEBUG("%s ��Ƶ����ʧ��\n", __func__);
            return NDK_ERR_IOCTL;
        }
        write(voice_fd,voicesection->vw->sample,voicesection->vw->sample_lenth);
    }
    ret=ioctl(voice_fd, PLAY_IOCS_VOLUME, &unValue);
    if(ret<0) {
        PDEBUG("%s ��������ʧ��\n", __func__);
        return NDK_ERR_IOCTL;
    }
    ret=ioctl(voice_fd, PLAY_IOCS_CTRL, &emCtrlId);//��������
    if(ret<0) {
        PDEBUG("%s ���ſ���ʧ��\n", __func__);
        return NDK_ERR_IOCTL;
    }
    currentvoicefd=unVoiceId;
    if(emCtrlId==VOICE_CTRL_STOP) {
        close(voice_fd);
        currentvoicefd = -111;
        return NDK_OK;
    }

    return NDK_OK;
EXIT:
    return NDK_ERR;
}
NEXPORT int NDK_VoiceLoadFile(uint unVoiceID,char *pszFile)
{
    if((pszFile==NULL)||(access(pszFile,F_OK)<0))
        return NDK_ERR_PARA;
    if(loadvoice_wav(unVoiceID,pszFile)!=0)
        return NDK_ERR;
    else
        return NDK_OK;
}


