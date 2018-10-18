/*
* 新大陆公司 版权所有(c) 2006-2008
*
* POS API
* 文件配置相关	--- Config.c	*
* 作    者：    		产品开发部
* 日    期：    		2012-08-17
* 最后修改人：  	lidh
* 最后修改日期：
*/

#ifndef __GUI_H__
#define	__GUI_H__

int socket_unix_recv (int s, void *read_buf, int total_size);
int socket_unix_send (int s, void *write_buf, int total_size);

#define socket_recv(a, b, c)	if (socket_unix_recv(a, b, c) != c) { return -1; }
#define socket_send(a, b, c)	if (socket_unix_send(a, b, c) != c) { return -1; }

#ifndef __NDKAPI__H
typedef unsigned int color_t;/**<RGB色彩数值,0(黑色) - 0xFFFF(白色)*/
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
 * 图像数据结构
 * 根据解码库，如libpng，libjpeg，最低支持24位色深，最高32位色深
 * 单个通道8位色深，低于或高于这些颜色模式的会被转化
 * 因此，bytes_per_pixel总是字节对齐（2，3字节）
 * 显示模块可能会修改该数据结构，如：
 * 根据显示能力，将24，或32位色深装化为本地的色深保存，而
 * 如果是32位色深，显示模块可以将image_buf中的alpha通道分离到image_alpha中
 */
typedef struct {
	uint32 width;  			/**< 图像宽度（像素） */
	uint32 height; 			/**< 图像高度（像素）*/
	uint32 bytes_per_pixel; /**< 每个像素多少字节 */
	void * image_buf;		/**< 图像色彩数据 */
	void * image_alpha;		/**< 图像alpha通道 */
	int    user;			/**< 图像被共享的次数 */
}image_t;


/** 位图信息结构 */
typedef struct {
	uint32 width;     		/**< 位图宽 */
	uint32 height;			/**< 位图高 */
	uint32 graylevel;		/**< 位图每个点的灰度级，小于或等于bits_per_pixel所能表示的最大值 */
	uint32 bits_per_pixel;	/**< 位图每个点占的位数 */
	uint32 bytes_per_line;	/**< 位图每行占据的字节数 */
	void * bitmap_buf;		/**< 位图数据 */
}bitmap_t;

/**
 * 表面绘制模式，在复制的表面上，通常都是虚拟显存
 * 真实显存用于支持类似Linux Framebuffer的显示模块
 */
typedef enum {
	/** 图形将直接绘制在真实显存上  */
	SURFACE_REAL       = 0x1000,
	/** 图形将直接绘制在虚拟显存上，不及时显示  */
	SURFACE_VIRTUAL    = 0x2000,
	/** 图形将直接绘制在虚拟显存上，但及时显示  */
	SURFACE_NEEDEXPOSE = 0x4000,
	SURFACE_DRAW_MASK  = 0x0F00,
	/** 正常绘制 */
	SURFACE_DRAW_COPY  = 0x0000,
	/** alpha绘制 */
	SURFACE_ALPHABLEND = 0x0100,
	/** alpha掩码 */
	SURFACE_ALPHA_MASK = 0x00FF,
}surface_mode_t;

/**
 * 显示表面操作函数，块操作支持alpha
 * 点线，位图，以及缩放操作不支持直接alpha
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
	surface_t * sur;		/**< 主显示表面 */
	/** 显示能力 */
	uint32 bytes_per_pixel; /**< 每像素字节数 */
	uint32 bits_per_pixel;	/**< 每像素位数 */
	uint32 bytes_per_line;	/**< 每行字节数 */
	uint32 blue_length;		/**< 蓝色所占位数 */
	uint32 green_length;	/**< 绿色所占位数 */
	uint32 red_length;		/**< 红色所占位数 */
	uint32 blue_offset;		/**< 蓝色偏移 */
	uint32 green_offset;	/**< 绿色偏移 */
	uint32 red_offset;		/**< 红色偏移 */
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
	video_t *video;			/**< 显示驱动 */
    font_t * font;          /**< 系统字体 */
    int  themeshm;          /**< 主题共享内存 */
    theme_t * theme;
    theme_t * shmaddr;
	void * dlhandle;		/**< 显示插件句柄（动态链接库）*/
	int socket_fd;			/**< 与服务器通讯句柄 */
	rect_t view;
	color_t fg;
	color_t bg;
	color_t fb;
	char fonttype;			/*字体类型*/
	char wspace;			/*字间距*/
	char hspace;			/*行间距*/
	int secinfo_h;				/*实际用户可显示纵向的上限*/
	image_t *backimg;
	video_t *backvideo;
	guiinfo_t * guiinfo;//xmulk
	int activeid;//xmulk
}guicore_t;

/** 套接字请求类型 */
typedef enum {
	SOCKET_DATA_NOTHING = 0x00, /**< 空请求 */
	SOCKET_DATA_NEW     = 0x01, /**< 新建应用程序 */
	SOCKET_DATA_SHOW    = 0x02, /**< 显示应用程序 */
	SOCKET_DATA_CLOSE   = 0x03, /**< 关闭应用程序 */
	SOCKET_DATA_CONFIG  = 0x04, /**< 服务器发送配置 */
	SOCKET_DATA_MOUSE   = 0x05, /**< 服务器鼠标事件 */
	SOCKET_DATA_KEYPAD  = 0x06, /**< 服务器键盘事件 */
	SOCKET_DATA_PAUSE   = 0x07, /**< 暂停服务器所有事件 */
	SOCKET_DATA_PUSH    = 0x08, /**< 暂停当前窗口 */
	SOCKET_DATA_POP     = 0x09, /**< 弹出之前暂停示的窗口 */
	SOCKET_DATA_RELEASEFOCUS = 0x0A,
	SOCKET_DATA_CHANGEFOCUS = 0x0B,
	SOCKET_LCD_IOCSBLOCK = 0x20,/**< 刷新指定的显示区域 */
	SOCKET_LCD_IOCGINFO = 0x21,/**< 获取液晶屏信息 */
	SOCKET_LCD_IOCBL = 0x22,	/**< 背光控制 */
	SOCKET_LCD_IOCSPIXEL = 0x23,/**< 画点 */
	SOCKET_LCD_IOREADLCDBUF = 0x24,/**< 读液晶屏幕显存 */
	SOCKET_LCD_IOCSCONTRAST = 0x25/**< 设液晶屏幕对比度 */
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

/** 鼠标数据 */
typedef struct {
	int x;			/**< x坐标 */
	int y;			/**< y坐标 */
	mouse_button_t buttons;	/**< 按键状态 */
}video_input_data_mouse_t;

/** 键盘数据 */
typedef struct {
	keypad_state_t state;
	int ascii;
}video_input_data_keybd_t;


extern guicore_t sys;
#endif
