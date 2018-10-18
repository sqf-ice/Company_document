#ifndef _NDK_TPPRINT_H_
#define _NDK_TPPRINT_H_

#include <linux/ioctl.h>
#include "NDK.h"
#include "NDK_debug.h"

#define PRN_DEV_NAME	"/dev/prn"  /*    ��ӡ�豸��  */
#define prnMajor		10          /*��ӡ���豸��*/
#define PRN_IOC_MAGIC	prnMajor

#define MAXHIGHT 		48
#define MAXLINEDOT		384
#define MaxFontNum 		8
#define OpenLine  	0     /*���»���*/
#define CloseLine		1	/*���»���*/
#define UnableLine 	2	/*�ر��»��߹���*/

#define LEFTALIGN	 0	/*�����*/
#define MIDALIGN	 1	/*���ж���*/
#define RIGHTALIGN	 2	/*�Ҷ���*/
#define UNABLEALIGN	 3	/*�رն���ģʽ*/

#define	PRN_ERR_OK 		 0		/* 0 �������,����Ϊ��ӡ������*/
#define	PRN_ERR_INIT		(1<<7)  // 1 �������,����Ϊ��ӡ���ϵ縴λʧ��
#define	PRN_ERR_PAPER	    (1<<1)  // 2 �������,����Ϊ��ӡ��ȱֽ
#define	PRN_ERR_HEAT		(1<<2)  // 4 �������,����Ϊ��ӡͷ�¶ȹ���
#define	PRN_ERR_BUSY		(1<<3)  // 8 ������,����Ϊ��ӡ��æ(���ڴ�ӡ����).
#define	PRN_ERR_DONE		(1<<8)  // ������,����Ϊ�ϴδ�ӡ����ɹ����
#define	PRN_ERR_HPInvalid	(1<<9)  // 0x20 �������,����Ϊ��ӡ������HPλ���쳣.��ֽ������������ӡ��ͷ���������¸ô���
#define TP_ERR_VOLT_LOW	(1<<4)	//�͵�ѹ����
#define TP_ERR_VOLT_WARN	(1<<5)	//�͵�ѹ��ʾ
#define TP_ERR_VOLT_TOP	(1<<6)	//�ߵ�ѹ����
#define TP_ERR_VOLT			(TP_ERR_VOLT_LOW|TP_ERR_VOLT_WARN|TP_ERR_VOLT_TOP)


/*****************************************************************************
 * �����Ǵ�ӡģ����ص���Ϣ *******************************************************
 *****************************************************************************/
/********************
 * ��ӡ������Ϣ        *
 ********************/

#define HZFONTNUM  		22    /**! !Attention�����������ʱ����Ҫ�޸ĸò��� */
#define ASCFONTNUM  	27   /**! !Attention: ���������ʱ����Ҫ�޸ĸò��� */

#define PRN_IOCT_RESET				_IO(PRN_IOC_MAGIC, 1)		// param is 	null
#define PRN_IOCG_INFO				_IO(PRN_IOC_MAGIC, 2)		// prninfo,
#define PRN_IOCG_STATUS			_IO(PRN_IOC_MAGIC, 3)		// int

#define PRN_IOCG_MODE				_IO(PRN_IOC_MAGIC, 4)       	// int
#define PRN_IOCS_MODE				_IO(PRN_IOC_MAGIC, 5)		// int

#define PRN_IOCT_FEED_PAPER		_IO(PRN_IOC_MAGIC, 6)		// int
#define PRN_IOCS_REPORT			_IO(PRN_IOC_MAGIC, 7)		// prnreport,
#define PRN_IOCT_IMAGE				_IO(PRN_IOC_MAGIC, 8)		// prnimage

#define PRN_IOCG_PARAM				_IO(PRN_IOC_MAGIC, 9)		// get prnparam
#define PRN_IOCS_PARAM				_IO(PRN_IOC_MAGIC, 10)		// set prnparam

#define PRN_IOCG_ERRNO				_IO(PRN_IOC_MAGIC, 11)		// int

#define PRN_IOCS_B_ADDON			_IO(PRN_IOC_MAGIC, 13)		// int
#define PRN_IOCG_B_ADDON			_IO(PRN_IOC_MAGIC, 14)		// int

#define PRN_IOCS_MISSSTEPDETECT 	_IO(PRN_IOC_MAGIC, 15)      	// int
#define PRN_IOCG_MISSSTEPDETECT 	_IO(PRN_IOC_MAGIC, 16)      	// int
#define PRN_IOCS_SPEED				_IO(PRN_IOC_MAGIC, 17)
#define PRN_IOCT_CLRBUF         		_IO(PRN_IOC_MAGIC, 20)      	// param is 	null

#define PRN_IOCG_PRINTER_TYPE   	_IO(PRN_IOC_MAGIC, 30)      	// int

#define PRN_IOCT_TEST				_IO(PRN_IOC_MAGIC, 101)	    	// null
#define PRN_IOCT_INTEGRATE			_IO(PRN_IOC_MAGIC, 102)
#define PRN_IOCS_GREYSCALE			_IO(PRN_IOC_MAGIC, 103)
#define PRN_IOCG_ISPRINTERBUSY		_IO(PRN_IOC_MAGIC, 104)
#define PRN_IOCS_HEATMODE			_IO(PRN_IOC_MAGIC, 105)		//���ü���ģʽ:�������Ȼ��ٶ�����
#define PRN_IOCS_MOTORSTEPMODE	_IO(PRN_IOC_MAGIC, 106)		//������ﲽ��ģʽ: 8��������4������
#define PRN_IOCG_MOTORSTEPCOUNT	_IO(PRN_IOC_MAGIC, 107)		//��ȡ�����ܹ��������������ڼ�����ֽ���ܳ���

#define PRN_IOCG_VER				_IO(PRN_IOC_MAGIC, 108)
#define PRN_IOCS_STOP				_IO(PRN_IOC_MAGIC, 109)
#define PRN_IOCS_PRNRANGE			_IO(PRN_IOC_MAGIC, 110)	  /*��߽�,�ּ��,�м���趨*/
#define PRN_IOCS_ALIGNTYPE			_IO(PRN_IOC_MAGIC, 111)	 /*ͼ�ζ��뷽ʽ*/
#define PRN_IOCS_FEEDTP			_IO(PRN_IOC_MAGIC, 112)
#define PRN_IOCT_DATA				_IO(PRN_IOC_MAGIC, 113)		// prndata
#define PRN_IOCT_DATA_NDK  		_IO(PRN_IOC_MAGIC, 117)		/*ndk�����ݽӿ�*/
#define PRN_IOCS_FEEDENDPRINT		_IO(PRN_IOC_MAGIC, 121)		/*��ӡ����ֽ��˺ֽ��*/
#define PRN_IOCS_FEEDBACK		      _IO(PRN_IOC_MAGIC, 122)		/*��ֽ*/


/***********************
 * ����ͼ�����Ű�ģʽ*
 ***********************/
typedef enum PRN_TYPESETTING {
    TPSET_AUTO = 0,		//<�Զ���Ӧ�����ֻ���ͼ�񣬱�֤���غϴ�ӡ
    TPSET_TEXTUP,		//<�������ϣ��������غϣ��ļ���ֱ�Ӹ�д��ͼ����
    TPSET_PICUP,		//<ͼ�����ϣ��������غϣ�ͼ��ֱ�Ӹ�д���ļ���
    TPSET_MIX			//<����ͼ��Ƕ�ף����غϣ����ֺ�ͼ��Ƕ�״�ӡ
} PRN_TYPESETTING;


/**
 * @brief Ҫ��ӡ��ͼ����Ϣ
 */
typedef struct {
	int  leftmargin;			/* ͼ�ε����Ͻǵ���λ�ã��Ե�Ϊ��λ��*/
	int  width;				/* ͼ�εĿ�ȣ��Ե�Ϊ��λ��*/
	int  height;				/* ͼ�εĸ߶ȣ��Ե�Ϊ��λ��*/
	void *buffer;				/* ͼ���������,Ϊ��������*/
} prnimage;

/**
 * @brief Ҫ��ӡ�����ּ�ͼ�����ϵ���Ϣ�ṹ
 */
typedef struct PrinterIntegrate {
	char *pstrbuf;					/*�����ַ���ָ��*/
	int nstrlen;						/*���ֳ���*/
	int ImageYPos;					/*ͼ��ʼ��ӡ��������*/
	prnimage sPrnImage;				/*������һͬ��ӡ��λͼ��Ϣ*/
	PRN_TYPESETTING TypeSetting;	/*�Ű�ģʽ*/
} PrinterIntegrate;

/**
 * @brief ��ӡ������
 */
typedef struct {
	int charset;			/* �ַ���*/
	int line_attr;			/*������*/
	int ASCfont;			/* ASC����*/
	int tabwidth;			/* tabֵ*/
	int pagelenth;		/* ҳ��*/
	int linespace;		/* �м��*/
	int wordspace;		/* �ּ��  ��TODO:δ���ԣ�*/
	int BMcounter;		/* �ڱ������TODO:δʵ�֣�*/
	int heat_time;		/*����ʱ��*/
} prnparam;


/**
 * @brief  ��ӡ�л��������ݽṹ���洢1���еĴ�ӡ����
*/
typedef	struct {
	unsigned char ucData[MAXHIGHT][MAXLINEDOT/8];	//��ӡ���ݶ���Ԫ�ص�����
	unsigned int  usPrnSteps;	//��ǰ�д�ӡ����֮�����Ĳ�����������ʵ���м��
	unsigned int  usPrnLineHigh;
	unsigned int  usPrnLineOffset;
	unsigned int  usmode;
	unsigned int  usPrnGray;

}PrnDotlineData;

/**
 * @brief  ��ӡͼƬ��Ϣ�����ݽṹ
*/

typedef struct {
	unsigned int width;  			/**< ͼ���ȣ����أ� */
	unsigned int height; 			/**< ͼ��߶ȣ����أ�*/
	unsigned int bytes_per_pixel; /**< ÿ�����ض����ֽ� */
	void * image_buf;		/**< ͼ��ɫ������ */
	void * image_alpha;		/**< ͼ��alphaͨ�� */
	int    user;			/**< ͼ�񱻹���Ĵ��� */
}image_t;
/**
 * @brief  ��ӡͼƬ��Ϣ�����ݽṹ
*/
typedef struct {
	unsigned int width;  			/**< ͼ���ȣ����أ� */
	unsigned int height; 			/**< ͼ��߶ȣ����أ�*/
	void * image_buf;		/**< ͼ��ɫ������ */
}print_buf;

/**
 * @brief �����������ݽṹ
*/
typedef	struct stFontContrl{
	int fonttype;			/*��Ӧ���������ͣ����� HZ24x24A */
	char *fontname;		/*���������ļ�ϵͳ�е��ֿ��ļ�����·��*/
	int fthandle;			/*�ֿ��ļ���� */
	int (* ft_getdotoffset)(const char *);	/*ȡ�������ƫ�Ƶĺ���*/
}stFontContrl;

/**
 * @brief �߾����ýṹ��
*/

typedef struct BSP_printerRange{
	int nBoard;		/*��߽�*/
	int nColumn;		/*�ּ��*/
	int nRaw;		/*�м��*/
}BSP_printerRange;

#endif
