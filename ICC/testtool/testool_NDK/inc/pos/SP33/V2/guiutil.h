#ifndef GUIUTIL_H_
#define GUIUTIL_H_

#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/mman.h>

#include <linux/input.h>
/**
 * @ingroup list
 * @{
 */
/**
 * 通用链表节点结构
 */
typedef struct {
	void * next;    /**< 指向下一个节点 */
	void * element; /**< 指向节点存放的数据项 */
}list_node_t;


typedef enum {
	POS_LEFT      = 0x00000001,
	POS_HCENTER   = 0x00000002,
	POS_RIGHT     = 0x00000004,
	POS_UP        = 0x00000008,
	POS_DOWN      = 0x00000010,
	POS_VCENTER   = 0x00000020,
	POS_CENTER    = 0x00000022,
	POS_VERT	  = 0x00000005,
	POS_HORIZ     = 0x00000018,
}position_t;

/**
 * 通用链表头节点结构
 */
typedef struct {
	int num_elt;		/**< 节点总数 */
	list_node_t * node; /**< 指向第一个结点 */
}list_t;

/**
 * 控件在链表中的信息
 */
typedef struct {
	void * current_widget;
	int current_position;
}widget_list;


int list_init(list_t ** li);
int list_uninit(list_t * li);
int list_eol(list_t * li, int i);
list_node_t * list_get_node(list_t * li, int pos);
void * list_get(list_t * li, int pos);
void * list_remove(list_t * li, int pos);
int list_add(list_t * li, void * element, int pos);
void * list_find(list_t * li, int (*cmp_func) (void * data, void * node), void * data);
int list_get_pos(list_t * li, void * node);
int list_for_each(list_t * li, void (*call)(void * data, void * element), void * data);
int list_for_each_breakoff(list_t * li, int (*call)(void * data, void * element), void * data);
widget_list list_find_down(list_t * li, int pos,int (*call)(void * data, void * element), void * data);
widget_list list_find_up(list_t * li, int pos,int (*call)(void * data, void * element), void * data);
int list_cleanup(list_t * li, void (*callback)(void * data, void * element), void * data, int freehead);

typedef struct {
	int x;
	int y;
	int w;
	int h;
}rect_t;

typedef struct {
	char *name;		/**< 变量名 */
	char *value;	/**< 变量值 */
}config_var_t;

typedef struct {
	char *name;			/**< 类别名 */
	list_t *variable;	/**< 变量列表 */
}config_cat_t;

typedef struct {
	list_t *category;	/**< 类别链表 */
}config_t;

int config_initialize (config_t **cfg);
int config_parse (config_t *cfg, char *name);
int config_uninit (config_t *cfg);
int config_category_uninit (config_cat_t *cat);
int config_variable_uninit (config_var_t *var);
int config_category_init (config_cat_t **cat, char *name);

typedef unsigned int   uint32;
typedef unsigned char  uint8;
typedef unsigned short uint16;

typedef uint32 color_t; /**< 颜色表示，最终要根据显示模块的颜色模式合成 */
void color_rgb(color_t c, int *r, int *g, int *b);
color_t rgb_color(int r, int g, int b);

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

image_t * image_decode(char * filepath);
int image_encode(image_t * pimage, char * filepath, char * type);
void image_destroy(image_t * pimage);
image_t * image_share(image_t * pimage);

int init_gui(void);
void uninit_gui(void);
int init_guiforapp(unsigned char*appname);

#include "widget.h"

int pt_in_rect(rect_t * r, int x, int y);
void rect_inflate(rect_t * r, int dx, int dy);
void rect_deflate(rect_t * r, int dx, int dy);
int rect_intersect(rect_t * r, rect_t * r1, rect_t * r2);
void rect_set(rect_t * rect, int x, int y, int w, int h);
void rect_offset(rect_t * r, int x, int y);
void rect_position(rect_t * contain, int w, int h, rect_t * pos, position_t where);
int rect_is_empty(rect_t * rect);
int rect_equal(rect_t * r1, rect_t * r2);
void rect_range(rect_t * con, image_t * image, int strw, int strh, rect_t * img, rect_t * str, position_t pos,int inner);


int get_lcdtype(char * type);
int get_screensize(unsigned int * width,unsigned int * height);
int detect_colorscreen(void);

#define EKEY_ESC          0x1B
#define EKEY_ALPHA        0x1C
#define EKEY_ENTER        0x0D
#define EKEY_DOT          0x2E
#define EKEY_BACKSPACE    0x0A

#define LCDBUF_TOP		  0
#define LCDBUF_BOTTOM     1

#define EKEY_F1           0x01
#define EKEY_F4           0x04
#define EKEY_F2           0x02
#define EKEY_F3           0x03
#define EKEY_MENU		EKEY_F3	
#if defined(CONFIG_GP730_MONO)||defined(CONFIG_GP730_COLOR)
#define EKEY_UP			EKEY_F1
#define EKEY_DOWN		EKEY_F2
#define EKEY_LEFT       EKEY_F1
#define EKEY_RIGHT		EKEY_F2
#endif

#ifdef CONFIG_NLGP720
#define EKEY_UP			0x11
#define EKEY_DOWN		0x12
#define EKEY_LEFT       0x13
#define EKEY_RIGHT		0x14
#endif

#ifdef CONFIG_NLGP750
#define EKEY_UP			EKEY_F2
#define EKEY_DOWN		EKEY_F3
#define EKEY_LEFT       EKEY_F2
#define EKEY_RIGHT		EKEY_F3
#endif


#define EKEY_DOT_EXRA   (EKEY_DOT|0X80)
#define EKEY_ALPHA_EXRA   (EKEY_ALPHA|0X80)

#endif
