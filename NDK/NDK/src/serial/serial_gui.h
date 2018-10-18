/*
* �´�½��˾ ��Ȩ����(c) 2006-2008
*
* POS API
* �ļ��������	--- Config.c	*
* ��    �ߣ�    		��Ʒ������
* ��    �ڣ�    		2012-08-17
* ����޸��ˣ�  	lidh
* ����޸����ڣ�
*/

#ifndef __GUI_H__
#define	__GUI_H__

int socket_unix_recv (int s, void *read_buf, int total_size);
int socket_unix_send (int s, void *write_buf, int total_size);

#define socket_recv(a, b, c)	if (socket_unix_recv(a, b, c) != c) { return -1; }
#define socket_send(a, b, c)	if (socket_unix_send(a, b, c) != c) { return -1; }

#ifndef __NDKAPI__H
typedef unsigned int color_t;/**<RGBɫ����ֵ,0(��ɫ) - 0xFFFF(��ɫ)*/
#endif
typedef struct _surface_t surface_t;
typedef struct _font_t	font_t;
typedef unsigned int   uint32;
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef void widget_t;

typedef struct {
	int x;
	int y;
	int w;
	int h;
}rect_t;

/**
 * ͼ�����ݽṹ
 * ���ݽ���⣬��libpng��libjpeg�����֧��24λɫ����32λɫ��
 * ����ͨ��8λɫ����ڻ������Щ��ɫģʽ�Ļᱻת��
 * ��ˣ�bytes_per_pixel�����ֽڶ��루2��3�ֽڣ�
 * ��ʾģ����ܻ��޸ĸ����ݽṹ���磺
 * ������ʾ��������24����32λɫ��װ��Ϊ���ص�ɫ��棬��
 * �����32λɫ���ʾģ����Խ�image_buf�е�alphaͨ�����뵽image_alpha��
 */
typedef struct {
	uint32 width;  			/**< ͼ���ȣ����أ� */
	uint32 height; 			/**< ͼ��߶ȣ����أ�*/
	uint32 bytes_per_pixel; /**< ÿ�����ض����ֽ� */
	void * image_buf;		/**< ͼ��ɫ������ */
	void * image_alpha;		/**< ͼ��alphaͨ�� */
	int    user;			/**< ͼ�񱻹���Ĵ��� */
}image_t;


/** λͼ��Ϣ�ṹ */
typedef struct {
	uint32 width;     		/**< λͼ�� */
	uint32 height;			/**< λͼ�� */
	uint32 graylevel;		/**< λͼÿ����ĻҶȼ���С�ڻ����bits_per_pixel���ܱ�ʾ�����ֵ */
	uint32 bits_per_pixel;	/**< λͼÿ����ռ��λ�� */
	uint32 bytes_per_line;	/**< λͼÿ��ռ�ݵ��ֽ��� */
	void * bitmap_buf;		/**< λͼ���� */
}bitmap_t;

/**
 * �������ģʽ���ڸ��Ƶı����ϣ�ͨ�����������Դ�
 * ��ʵ�Դ�����֧������Linux Framebuffer����ʾģ��
 */
typedef enum {
	/** ͼ�ν�ֱ�ӻ�������ʵ�Դ���  */
	SURFACE_REAL       = 0x1000,
	/** ͼ�ν�ֱ�ӻ����������Դ��ϣ�����ʱ��ʾ  */
	SURFACE_VIRTUAL    = 0x2000,
	/** ͼ�ν�ֱ�ӻ����������Դ��ϣ�����ʱ��ʾ  */
	SURFACE_NEEDEXPOSE = 0x4000,
	SURFACE_DRAW_MASK  = 0x0F00,
	/** �������� */
	SURFACE_DRAW_COPY  = 0x0000,
	/** alpha���� */
	SURFACE_ALPHABLEND = 0x0100,
	/** alpha���� */
	SURFACE_ALPHA_MASK = 0x00FF,
}surface_mode_t;

/**
 * ��ʾ������������������֧��alpha
 * ���ߣ�λͼ���Լ����Ų�����֧��ֱ��alpha
 */
typedef struct {
	void (*set_pixel)(surface_t *, int , int, color_t);
	void (*draw_hline)(surface_t *, int, int, int, color_t);
	void (*draw_vline)(surface_t *, int, int, int, color_t);
	void (*fill_rect)(surface_t *, rect_t *, color_t);
	void (*blit)(surface_t *, rect_t *, surface_t *, int, int);
	void (*image_draw)(surface_t *, rect_t *, image_t *, int, int);
	void (*stretch_blit)(surface_t *, rect_t *, surface_t *, rect_t *);
	void (*bitmap_draw)(surface_t * sur, int dstx, int dsty, bitmap_t * pbits,
			int xoff, int yoff, int w, int h, color_t cr);
	image_t * (*surface_to_image)(surface_t * sur, rect_t *);
	void (*surface_update)(surface_t *, rect_t *);
	void (*surface_destroy)(surface_t * sur);
}surface_ops_t;

typedef struct {
	surface_t * sur;		/**< ����ʾ���� */
	/** ��ʾ���� */
	uint32 bytes_per_pixel; /**< ÿ�����ֽ��� */
	uint32 bits_per_pixel;	/**< ÿ����λ�� */
	uint32 bytes_per_line;	/**< ÿ���ֽ��� */
	uint32 blue_length;		/**< ��ɫ��ռλ�� */
	uint32 green_length;	/**< ��ɫ��ռλ�� */
	uint32 red_length;		/**< ��ɫ��ռλ�� */
	uint32 blue_offset;		/**< ��ɫƫ�� */
	uint32 green_offset;	/**< ��ɫƫ�� */
	uint32 red_offset;		/**< ��ɫƫ�� */
	char * (*video_verion)(void);
}video_t;

typedef enum {
    DEFAULT = 0,
    DISABLE = 1,
    PRESSED = 2,
    NUM_STA = 3
}ctrl_status_t;

typedef struct {
    char version[20];
    color_t default_bg;
    color_t default_fg;
    color_t disable_fg;
    image_t btn[NUM_STA];
    image_t chk_off[NUM_STA];
    image_t chk_on[NUM_STA];
    image_t rdi_off[NUM_STA];
    image_t rdi_on[NUM_STA];
    image_t lst[NUM_STA];
    image_t tit_cls[NUM_STA];
	image_t progress_bar[2];
	image_t combobox;
	image_t editbox;
	image_t messagebox;
	image_t scrollbar_bg_h;
	image_t scrollbar_bg_v;
	image_t scrollbar_h;
	image_t scrollbar_v;
	image_t battery_empty;
	image_t battery_adapter;
	image_t battery_red;
	image_t battery_green;
	image_t wlsignal[6];
    image_t tit_bar;
	image_t text_def;

}theme_t;

struct _font_t{
	void (*destroy_font)(font_t *);
	void (*draw_text)(font_t *, surface_t * sur, rect_t * rect,
            char * text, int srcx, int srcy, int fonttype, int wspace, color_t cr, int trunc);
	int (*get_textsize)(font_t *, char * text,int wspace);
//	int cfonth;
//	int afonth;
	int fonth;
	int cfontw;
	int afontw;
	int user;
    void (*draw_extra)(font_t * font_used,surface_t * sur, rect_t *rect,char *fontbitmap, int offset,bitmap_t * bits,color_t cr);
};

#define APPINFOLISTNUM 16
#define APPINFONAMELEN 255

typedef struct _appinfo_item{
	int appid;
	unsigned char appname[APPINFONAMELEN];
	int valid;
}appinfo_item;



typedef struct _guiinfo_t{
	int active_fd;
	int appinfonum;
	appinfo_item appinfo_list[APPINFOLISTNUM];
}guiinfo_t;

typedef struct _guicore_t {
	video_t *video;			/**< ��ʾ���� */
    font_t * font;          /**< ϵͳ���� */
    int  themeshm;          /**< ���⹲���ڴ� */
    theme_t * theme;
    theme_t * shmaddr;
	void * dlhandle;		/**< ��ʾ����������̬���ӿ⣩*/
	int socket_fd;			/**< �������ͨѶ��� */
	rect_t view;
	color_t fg;
	color_t bg;
	color_t fb;
	char fonttype;			/*��������*/
	char wspace;			/*�ּ��*/
	char hspace;			/*�м��*/
	int secinfo_h;				/*ʵ���û�����ʾ���������*/
	image_t *backimg;
	video_t *backvideo;
	guiinfo_t * guiinfo;//xmulk
	int activeid;//xmulk
}guicore_t;

/** �׽����������� */
typedef enum {
	SOCKET_DATA_NOTHING = 0x00, /**< ������ */
	SOCKET_DATA_NEW     = 0x01, /**< �½�Ӧ�ó��� */
	SOCKET_DATA_SHOW    = 0x02, /**< ��ʾӦ�ó��� */
	SOCKET_DATA_CLOSE   = 0x03, /**< �ر�Ӧ�ó��� */
	SOCKET_DATA_CONFIG  = 0x04, /**< �������������� */
	SOCKET_DATA_MOUSE   = 0x05, /**< ����������¼� */
	SOCKET_DATA_KEYPAD  = 0x06, /**< �����������¼� */
	SOCKET_DATA_PAUSE   = 0x07, /**< ��ͣ�����������¼� */
	SOCKET_DATA_PUSH    = 0x08, /**< ��ͣ��ǰ���� */
	SOCKET_DATA_POP     = 0x09, /**< ����֮ǰ��ͣʾ�Ĵ��� */
	SOCKET_DATA_RELEASEFOCUS = 0x0A,
	SOCKET_DATA_CHANGEFOCUS = 0x0B,
	SOCKET_LCD_IOCSBLOCK = 0x20,/**< ˢ��ָ������ʾ���� */
	SOCKET_LCD_IOCGINFO = 0x21,/**< ��ȡҺ������Ϣ */
	SOCKET_LCD_IOCBL = 0x22,	/**< ������� */
	SOCKET_LCD_IOCSPIXEL = 0x23,/**< ���� */
	SOCKET_LCD_IOREADLCDBUF = 0x24,/**< ��Һ����Ļ�Դ� */
	SOCKET_LCD_IOCSCONTRAST = 0x25/**< ��Һ����Ļ�Աȶ� */
}socket_header_t;

typedef enum {
	KEYPAD_STATE_DOWN  = 0,
	KEYPAD_STATE_RELEASE = 1,
}keypad_state_t;

typedef enum {
	MOUSE_BUTTON_RELEASE     = 0x00000000,
	MOUSE_BUTTON_RIGHT       = 0x00000001,
	MOUSE_BUTTON_MIDDLE      = 0x00000002,
	MOUSE_BUTTON_LEFT        = 0x00000004,
}mouse_button_t;

/** ������� */
typedef struct {
	int x;			/**< x���� */
	int y;			/**< y���� */
	mouse_button_t buttons;	/**< ����״̬ */
}video_input_data_mouse_t;

/** �������� */
typedef struct {
	keypad_state_t state;
	int ascii;
}video_input_data_keybd_t;


extern guicore_t sys;
#endif
