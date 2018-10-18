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
 *@brief    播放控制。（播放前必须调用NDK_VoiceLoadFile（uint unVoiceID,char *pszFile）将相应的音频文件解码，音频解码到链表中，通过id播放控制不同的音频文件。）
 *@param        unVoiceId   音频ID
 *@param        emCtrlId    控制选项
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_OPEN_DEV "NDK_ERR_OPEN_DEV"    设备文件打开失败
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(unVoiceId非法)
 *@li   \ref NDK_ERR "NDK_ERR"              操作失败(加载音频文件失败，可能该ID对应的音频文件未调用NDK_VoiceLoadFile进行加载。)
 *@li   \ref NDK_ERR_IOCTL "NDK_ERR_IOCTL"  驱动接口调用错误(音频驱动接口调用失败返回)
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
            PDEBUG("%s 音频状态获取失败\n", __func__);
            return NDK_ERR_IOCTL;
        }
        if(voicestatus != VOICE_PLAYSTATUS_IDLE) {
            PDEBUG("%s 播放状态不是空闲\n", __func__);
            goto EXIT;
        }
        wav_para.datalen=voicesection->vw->sample_lenth;
        wav_para.rate=voicesection->vw->rate;
        ret=ioctl(voice_fd, PLAY_IOCS_PARA, &wav_para);
        if(ret<0) {
            PDEBUG("%s 音频设置失败\n", __func__);
            return NDK_ERR_IOCTL;
        }
        write(voice_fd,voicesection->vw->sample,voicesection->vw->sample_lenth);
    }
    ret=ioctl(voice_fd, PLAY_IOCS_VOLUME, &unValue);
    if(ret<0) {
        PDEBUG("%s 音量控制失败\n", __func__);
        return NDK_ERR_IOCTL;
    }
    ret=ioctl(voice_fd, PLAY_IOCS_CTRL, &emCtrlId);//启动播放
    if(ret<0) {
        PDEBUG("%s 播放控制失败\n", __func__);
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


