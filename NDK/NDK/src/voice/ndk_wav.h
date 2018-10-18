#ifndef	_NDKWAV_H_
#define _NDKWAV_H_

/* wav�ļ��ṹ */
typedef struct WaveData
{
   unsigned long sample_lenth;
   unsigned short rate;
   unsigned short channels;
   unsigned char  time_constant;
   char           bit_res;
   char       *name_wav;
 
   unsigned char       *sample;
} VOICEWAVDATA,*PVOICEWAVDATA;
typedef struct voicecustomized
{
    int voiceid;
    struct voicecustomized *next;           // pNext
        PVOICEWAVDATA  vw;
} VOICE_SECTION_HEADER;

/* wavͷ��Ϣ�ṹ */
typedef struct HeaderType
{
	char ChunkID[5]; /*RIFF����Դ�ļ�ͷ��*/
	unsigned long ChunkSize; /*�ļ�����*/
	char Format[5]; /*"WAVE"��־*/

	char SubChunk1ID[5]; /*"fmt"��־*/
	unsigned long SubChunk1Size; /*�����ֽ�*/
	unsigned short AudioFormat; /*��ʽ���(10HΪPCM��ʽ����������)*/
	unsigned short NumChannels; /*Channels 1 = ������; 2 = ������*/
	unsigned long SampleRate; /*����Ƶ��*/
	unsigned long ByteRate; /*��Ƶ���ݴ�������*/
	unsigned short BlockAlign; /*�����ֽ�*/
	unsigned short BitsPerSample;/*����������λ��(8/16)*/

	char SubChunk2ID[5]; /*���ݱ�Ƿ�"data"*/
	unsigned long SubChunk2Size; /*�������ݵĳ���*/
} VOICEHEAD_WAV;
/**
 * ͨ������ڵ�ṹ
 */
typedef struct {
	void * next;    /**< ָ����һ���ڵ� */
	void * element; /**< ָ��ڵ��ŵ������� */
}list_node_t;
/**
 * ͨ������ͷ�ڵ�ṹ
 */
typedef struct {
	int num_elt;		/**< �ڵ����� */
	list_node_t * node; /**< ָ���һ����� */
}list_t;

int list_removevoice(list_t * li, int voiceid);
int loadvoice_mem(int voiceid,PVOICEWAVDATA vw);
int loadvoice_wav(int voiceid,char *path);
VOICE_SECTION_HEADER * list_getvoice(list_t * li, int voiceid);
VOICE_SECTION_HEADER *getvoice_wav(int voiceid);
VOICEWAVDATA *LoadWav(char * name_wav);
void free_voice(PVOICEWAVDATA Wav_file);

#endif
