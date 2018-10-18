#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>

/**
 *@brief        延时(可根据第二个参数延时的单位也不一样)
 *@details
 *@param       unDelayTime 延时时间(单位为微妙)，范围unDelayTime > 0
 *@param       units    微秒的倍数(如果要想延时的单位为毫秒则units=1000、0.1秒--units=100*1000、微妙--units=1)
 *@return
 *@li    0        成功返回
 *@li    -1       失败
*/
int sysdelay(int unDelayTime,int units)
{
    long delaycount = 0,totalcount = 0,tmptime = 0,offset = 0;
    sigset_t newmask,oldmask;

    if(unDelayTime<0||units<0)
        return -1;

    totalcount=unDelayTime*units;
    if(totalcount>1000*1000)
        offset=10*1000;
    while(totalcount>0) {
        tmptime=totalcount%(1000*1000);

        /*无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽*/
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGALRM);
        sigprocmask(SIG_BLOCK, &newmask, &oldmask);
        delaycount=tmptime?tmptime:1000000;
        usleep(delaycount-offset);
        totalcount-=delaycount;
        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    }

    return 0;
}

// 延时模块，单位0.1s
void ndk_delay(int nDelayTime)
{
    if (nDelayTime < 0) {
        return;
    }

    sysdelay(nDelayTime,100*1000);
}

// 延时模块，单位1ms
void ndk_msdelay(int nDelayTime)
{
    if (nDelayTime < 0) {
        return;
    }

    sysdelay(nDelayTime,1000);
}

void ndk_udelay(int nDelayTime)
{
    if (nDelayTime < 0) {
        return;
    }
    sysdelay(nDelayTime,1);
}

