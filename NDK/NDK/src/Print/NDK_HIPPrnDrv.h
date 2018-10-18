#ifndef _NDK_HIPPRNDRV_H_
#define _NDK_HIPPRNDRV_H_

extern int hip_setprintfont(int font);
extern int hip_FeedPrinterStepByStep(int nStepCounts);
extern int hip_printimage(int xsize,int ysize,int xpos,char *ImgBuf);
extern int hip_getprinterstatus(void);
extern int hip_setprintmode(int mode,int row_space);
extern int hip_setprintpagelen(int pagelen);
extern int SetPageTopMargin(unsigned int n);
extern int hip_clrprintbuf(void);
extern int hip_print(char * pbuf);
extern int hip_SetUnderLine(int emStatus);

/*��ӡ��״̬*/
#define	PR_ERR_OK		0	//0-�ɹ�
#define	PR_ERR_INIT		1	//1-�ӵ�ʧ�� : ��ӡ���ӵ�ʧ�ܣ�Ӳ�����ϣ���Ҫά��
#define	PR_ERR_PAPER	2	//2-ȱֽ : ��Ҫ�û���Ԥ
#define	PR_ERR_HEAT		4	//4-����ͷ���� : ��ӡ����ͣ
#define	PR_ERR_BUSY		8	//8-��ӡ��æ : �ӵ�ʧ�ܡ�ȱֽ������ͷ���Ȼ��ߴ�ӡ���������ɵ��´�ӡ��æ
#define	PR_ERR_DONE		0x10	//0x10-��ӡ���
#define	PR_ERR_HPInvalid	0x20	/* HP status invalid during printing(CR moving) */

#define PERPRINTMAXLEN	1024
#define PR_MAXLINEWIDE	360

// ��ӡ�豸��
#define PRN_DEV_NAME	"/dev/prn"

#define PRN_IOCTL	1000

#define PRN_READ_STATE	( PRN_IOCTL + 1 )
#define PRN_DEAL_DATA	( PRN_READ_STATE + 1 )
#define PRN_CLEAR_BUF	( PRN_DEAL_DATA + 1 )
#define PRN_PRINT_DONE	( PRN_CLEAR_BUF + 1 )
#define PRN_GET_VERSION	( PRN_PRINT_DONE + 1 )

#endif

