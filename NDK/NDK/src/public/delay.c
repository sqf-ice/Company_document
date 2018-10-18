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
 *@brief        ��ʱ(�ɸ��ݵڶ���������ʱ�ĵ�λҲ��һ��)
 *@details
 *@param       unDelayTime ��ʱʱ��(��λΪ΢��)����ΧunDelayTime > 0
 *@param       units    ΢��ı���(���Ҫ����ʱ�ĵ�λΪ������units=1000��0.1��--units=100*1000��΢��--units=1)
 *@return
 *@li    0        �ɹ�����
 *@li    -1       ʧ��
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

        /*�����źš���Դ״̬ˢ����Ҫʹ��SIGALRM�ź�(widget/notifier.c)���˴����ܱ����ź��жϣ������Ҫ��ʱ����*/
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

// ��ʱģ�飬��λ0.1s
void ndk_delay(int nDelayTime)
{
    if (nDelayTime < 0) {
        return;
    }

    sysdelay(nDelayTime,100*1000);
}

// ��ʱģ�飬��λ1ms
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

