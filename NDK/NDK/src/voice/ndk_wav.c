#include "ndk_wav.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int list_init(list_t ** li);
extern void* list_remove(list_t * li, int pos);
extern int list_add(list_t * li, void * element, int pos);

static list_t* voicecustomized_list = NULL;

VOICEWAVDATA *LoadWav(char * name_wav)
{
    PVOICEWAVDATA Wav_file;
    unsigned char *xxx;
    unsigned char tmpstr[1024] = {0},tmplen[256] = {0};
    char *pStart,*pEnd;
    char *endptr;

    int flag = 0,count = 0,j = 0,totalcount = 0,flag1 = 0,flag2 = 0;

    time_t oldtime,changetime;

    FILE *wavfp;
    wavfp=fopen(name_wav,"rb");
    if(wavfp==NULL) {
        printf("WAV文件未正确读入\n");
        return NULL;
    }
    /* 开辟空间 */
    if ((Wav_file = (VOICEWAVDATA *)malloc(sizeof(VOICEWAVDATA))) == NULL) {
        printf("have't mem!");
        fclose(wavfp);
        return NULL;
    }
    Wav_file->name_wav=name_wav;
    oldtime=time(NULL);
    fprintf(stderr,"-----------[%s][%d][%s]time[%d]\n",__func__,__LINE__,Wav_file->name_wav,oldtime);
    while(fgets(tmpstr,sizeof(tmpstr),wavfp)) {
        //fprintf(stderr,"get line[%d],str = [%s][%02x][%02x]\n",h++,tmpstr,' ','\t');
        if(strstr(tmpstr,"#define SOUND_SamplesPerSec")!=NULL) {
            memset(tmplen,0,sizeof(tmplen));
            pStart=strstr(tmpstr,"#define SOUND_SamplesPerSec  ");
            pEnd=strchr(tmpstr,0x0d);
            if(pStart!=NULL&&pEnd!=NULL)
                strncpy(tmplen,pStart+strlen("#define SOUND_SamplesPerSec  "),pEnd-pStart-strlen("#define SOUND_SamplesPerSec  "));
            Wav_file->rate=strtol(tmplen,&endptr,0);
            flag1=1;
        }
        if(strstr(tmpstr,"#define SOUND_LENGTH")!=NULL) {
            memset(tmplen,0,sizeof(tmplen));
            pStart=strstr(tmpstr,"#define SOUND_LENGTH  ");
            pEnd=strchr(tmpstr,0x0d);
            if(pStart!=NULL&&pEnd!=NULL)
                strncpy(tmplen,pStart+strlen("#define SOUND_LENGTH  "),pEnd-pStart-strlen("#define SOUND_LENGTH  "));
            Wav_file->sample_lenth=strtol(tmplen,&endptr,0);
            Wav_file->sample=(unsigned char *)malloc(sizeof(unsigned char)*Wav_file->sample_lenth);
            xxx=(unsigned char *)malloc(sizeof(unsigned char)*Wav_file->sample_lenth);
            flag2=1;
        }
        if(strstr(tmpstr,"const uchar DATA[ SOUND_LENGTH ]= {")!=NULL) {

            flag=1;
            continue;
        }
        if(flag==1) {
            count = 0;
            unsigned char pp[32] = {0};
            for(j=0; j<strlen(tmpstr); j++) {

                if(tmpstr[j]==0x30&&tmpstr[j+1]=='x') {
                    count++;
                    j=j+1;
                    continue;
                } else if(tmpstr[j]==','||tmpstr[j]=='\t'||tmpstr[j]==0x20||tmpstr[j]=='\d'||tmpstr[j]=='\a') {
                    continue;
                } else {
                    if(tmpstr[j+1]==',')
                        sprintf(pp,"%s%c%c",pp,'0',tmpstr[j]);
                    else if(tmpstr[j+2]==',') {
                        sprintf(pp,"%s%c%c",pp,tmpstr[j],tmpstr[j+1]);
                        j++;
                    }

                }
            }
            switch(count) {
                case 1:
                    sscanf(pp,"%02x",Wav_file->sample+(totalcount++));
                    break;
                case 2:
                    sscanf(pp,"%02x%02x",Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++));
                    break;
                case 3:
                    sscanf(pp,"%02x%02x%02x",Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++));
                    break;
                case 4:
                    sscanf(pp,"%02x%02x%02x%02x",Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++));
                    break;
                case 5:
                    sscanf(pp,"%02x%02x%02x%02x%02x",Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++));
                    break;
                case 6:
                    sscanf(pp,"%02x%02x%02x%02x%02x%02x",Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++));
                    break;
                case 7:
                    sscanf(pp,"%02x%02x%02x%02x%02x%02x%02x",Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++),Wav_file->sample+(totalcount++),
                           Wav_file->sample+(totalcount++));
                    break;
                case 8:
                    sscanf(pp,"%02x%02x%02x%02x%02x%02x%02x%02x",
                           (Wav_file->sample)+(totalcount++),
                           (Wav_file->sample)+(totalcount++),
                           (Wav_file->sample)+(totalcount++),
                           (Wav_file->sample)+(totalcount++),
                           (Wav_file->sample)+(totalcount++),
                           (Wav_file->sample)+(totalcount++),
                           (Wav_file->sample)+(totalcount++),
                           (Wav_file->sample)+(totalcount++));
                    break;
                default:
                    printf("----[%s][%d][%d]\n",__func__,__LINE__,count);
                    break;
            }

        }

        memset(tmpstr,0,sizeof(tmpstr));
    }
    changetime=time(NULL)-oldtime;
    fprintf(stderr,"----[%s][%d]time=[%d]\n",__func__,__LINE__,changetime);
    fclose(wavfp);
    wavfp = NULL;
    if(flag==0||flag1==0||flag2==0) {
        free(Wav_file);
        return NULL;
    }
    return Wav_file;
}
VOICE_SECTION_HEADER * list_getvoice(list_t * li, int voiceid)
{
    list_node_t * ntmp;
    VOICE_SECTION_HEADER * voice_custm;


    if (li == NULL) {
        return NULL;
    }
    ntmp = li->node;
    while (ntmp) {
        voice_custm = (VOICE_SECTION_HEADER * )(ntmp->element);
        if (voice_custm->voiceid==voiceid) {
            break;
        }
        ntmp = ntmp->next;
    }
    return (ntmp == NULL ? NULL : ntmp->element);
}
void free_voice(PVOICEWAVDATA Wav_file)
{
    if (Wav_file) {
        free(Wav_file->sample);
        Wav_file->sample = NULL;
        free(Wav_file);
        Wav_file = NULL;
    }
}
int list_removevoice(list_t * li, int voiceid)
{
    list_node_t * ntmp;
    VOICE_SECTION_HEADER * voice_custm;
    int i=0;
    int *element;

    if (li == NULL) {
        return -1;
    }
    ntmp = li->node;
    while (ntmp) {
        voice_custm = (VOICE_SECTION_HEADER * )(ntmp->element);
        if (voice_custm->voiceid==voiceid) {
            element = list_remove(li,i);

            free_voice(((VOICE_SECTION_HEADER*)element)->vw);
            return 0;
        }
        i++;
        ntmp = ntmp->next;
    }
    return -1;
}


int loadvoice_wav(int voiceid,char *path)
{
    VOICE_SECTION_HEADER *voice_custm,*last_voice;
    PVOICEWAVDATA vw;

    if(voicecustomized_list==NULL) {
        if (list_init(&voicecustomized_list) < 0) {
            return -1;
        }
    }

    if((voice_custm = (VOICE_SECTION_HEADER *)calloc(1,sizeof(VOICE_SECTION_HEADER)))==NULL) {
        return -1;
    }

    if((vw = LoadWav(path))==NULL)
        return -1;
    voice_custm->vw= vw;
    voice_custm->voiceid= voiceid;
    voice_custm->next=NULL;

    if((last_voice=list_getvoice(voicecustomized_list,voiceid))!=NULL) {
        list_removevoice(voicecustomized_list,voiceid);

    }
    list_add(voicecustomized_list, voice_custm, -1);

    return 0;

}

int loadvoice_mem(int voiceid,PVOICEWAVDATA vw)
{
    VOICE_SECTION_HEADER *voice_custm,*last_voice;

    if(voicecustomized_list==NULL) {
        if (list_init(&voicecustomized_list) < 0) {
            return -1;
        }
    }

    if((voice_custm = calloc(1,sizeof(VOICE_SECTION_HEADER)))==NULL) {
        return -1;
    }

    voice_custm->vw= vw;
    voice_custm->voiceid= voiceid;
    voice_custm->next=NULL;


    if((last_voice=list_getvoice(voicecustomized_list,voiceid))!=NULL) {
        list_removevoice(voicecustomized_list,voiceid);
    }
    list_add(voicecustomized_list, voice_custm, -1);
    return 0;

}
VOICE_SECTION_HEADER *getvoice_wav(int voiceid)
{
    VOICE_SECTION_HEADER *voice_default;
    if(voicecustomized_list==NULL)
        return NULL;

    if((voice_default=list_getvoice(voicecustomized_list,voiceid))==NULL)
        return NULL;

    return voice_default;
}
