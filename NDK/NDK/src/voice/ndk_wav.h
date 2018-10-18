#ifndef	_NDKWAV_H_
#define _NDKWAV_H_

/* wav文件结构 */
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

/* wav头信息结构 */
typedef struct HeaderType
{
	char ChunkID[5]; /*RIFF类资源文件头部*/
	unsigned long ChunkSize; /*文件长度*/
	char Format[5]; /*"WAVE"标志*/

	char SubChunk1ID[5]; /*"fmt"标志*/
	unsigned long SubChunk1Size; /*过渡字节*/
	unsigned short AudioFormat; /*格式类别(10H为PCM形式的声音数据)*/
	unsigned short NumChannels; /*Channels 1 = 单声道; 2 = 立体声*/
	unsigned long SampleRate; /*采样频率*/
	unsigned long ByteRate; /*音频数据传送速率*/
	unsigned short BlockAlign; /*过渡字节*/
	unsigned short BitsPerSample;/*样本的数据位数(8/16)*/

	char SubChunk2ID[5]; /*数据标记符"data"*/
	unsigned long SubChunk2Size; /*语音数据的长度*/
} VOICEHEAD_WAV;
/**
 * 通用链表节点结构
 */
typedef struct {
	void * next;    /**< 指向下一个节点 */
	void * element; /**< 指向节点存放的数据项 */
}list_node_t;
/**
 * 通用链表头节点结构
 */
typedef struct {
	int num_elt;		/**< 节点总数 */
	list_node_t * node; /**< 指向第一个结点 */
}list_t;

int list_removevoice(list_t * li, int voiceid);
int loadvoice_mem(int voiceid,PVOICEWAVDATA vw);
int loadvoice_wav(int voiceid,char *path);
VOICE_SECTION_HEADER * list_getvoice(list_t * li, int voiceid);
VOICE_SECTION_HEADER *getvoice_wav(int voiceid);
VOICEWAVDATA *LoadWav(char * name_wav);
void free_voice(PVOICEWAVDATA Wav_file);

#endif
