#ifndef WIDGET_H_
#define WIDGET_H_

#include <stdlib.h>

typedef void widget_t;
#undef color_t
typedef unsigned int color_t; /**< 颜色表示，最终要根据显示模块的颜色模式合成 */

#define WIN_PROP_MASK			0x0000FFFF

#define WIN_PROP_TITLE_BAR_MASK	0x000000FF
#define WIN_PROP_TITLE_ICON		0x00000001
#define WIN_PROP_TITLE_STRING	0x00000002
#define WIN_PROP_TITLE_CLOSE	0x00000004

#define WIN_PROP_NORMAL         (WIN_PROP_TITLE_CLOSE|WIN_PROP_TITLE_STRING|WIN_PROP_TITLE_ICON)
#define WIN_PROP_NONE			0x00000000
#define WIN_PROP_DESKTOP		WIN_PROP_NONE

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
 * 图像数据结构
 * 根据解码库，如libpng，libjpeg，最低支持24位色深，最高32位色深
 * 单个通道8位色深，低于或高于这些颜色模式的会被转化
 * 因此，bytes_per_pixel总是字节对齐（2，3字节）
 * 显示模块可能会修改该数据结构，如：
 * 根据显示能力，将24，或32位色深装化为本地的色深保存，而
 * 如果是32位色深，显示模块可以将image_buf中的alpha通道分离到image_alpha中
 */
typedef struct {
	unsigned int width;  			/**< 图像宽度（像素） */
	unsigned int height; 			/**< 图像高度（像素）*/
	unsigned int bytes_per_pixel; /**< 每个像素多少字节 */
	void * image_buf;		/**< 图像色彩数据 */
	void * image_alpha;		/**< 图像alpha通道 */
	int    user;			/**< 图像被共享的次数 */
}image_t;

void widget_textout(widget_t * widget, int x, int y, char * text, color_t cr, int draw);
void widget_draw(widget_t * widget, int x, int y, image_t * img, int xoff, int yoff, int draw);
void widget_line(widget_t * widget, int x1, int y1, int x2, int y2, color_t cr, int draw);
void widget_fill(widget_t * widget, color_t cr, int draw);
void widget_show(widget_t * widget, int draw);
void widget_hide(widget_t * widget, int draw);
void widget_disable(widget_t * widget, int draw);
void widget_enable(widget_t * widget, int draw);
void widget_restore(widget_t * widget, int draw);
void widget_refresh(widget_t * widget, int x, int y, int w, int h);
void widget_rect(widget_t * wid, int *x, int *y, int *w, int *h);
image_t * widget_to_image(widget_t * wid);
int widget_image_zoom(char *imgout,int width,int height,char * imgin);
image_t * image_zoom(int width,int height,image_t * img);
void widget_clean_input(void);
void widget_set_focus_from(widget_t *widget);
void widget_set_focus(widget_t *widget);
void widget_release_focus(widget_t * widget);


void window_set_title(widget_t * win, char * title, int draw);
void window_set_icon(widget_t * win, char * iconpath, int draw);
void window_destroy(widget_t * win);
widget_t * window_new(int flags, char * title, char * iconpath);
void window_show(widget_t * win);
void window_pause(widget_t * win, int clearevent);
int window_loop(widget_t * win);
void window_exit(widget_t * win);
void window_close_enable(widget_t * win, int draw);
void window_close_disable(widget_t * win, int draw);
int window_direction_key_disable(widget_t * win);
int window_direction_key_enable(widget_t * win);
int window_attach_loop(widget_t * win,int (*on_loop)(widget_t * win));

widget_t * panel_new (widget_t * window, char * bgicon);
widget_t* panel_select(widget_t * panel);
void panel_attach_key_press(widget_t * panel, void (*on_click)(int ascii));
void panel_attach_key_with_return(widget_t * panel, int (*on_click)(int ascii));

widget_t * notify_new(widget_t * parent, int x, int y, int w, int h, int req_inter);
void notify_destroy(widget_t * noti);
int notify_set_inter(widget_t * noti, int newinter);
void notify_attach_update(widget_t * noti, void (*update)(widget_t *), int (*need_update)(widget_t *));

int notifier_new(int timeoutsec);
void notifier_free(void);
void notifier_stop(void);
void notifier_start(void);
void notifier_add(widget_t * notify);
void notifier_remove(widget_t * notify);

widget_t * picture_new(widget_t * parent, char * bgicon, int x, int y, int w, int h);
void picture_set_position(widget_t * picture, position_t imagepos, int draw);
void picture_destroy(widget_t * pic);
void picture_attach_mouse_up(widget_t* picture, void (*on_mouse_up)(int x, int y));
void picture_attach_mouse_down(widget_t* picture, void (*on_mouse_down)(int x, int y));
int picture_change_bg(widget_t * picture,char* bgicon,int type,int draw);

widget_t * table_new(widget_t * parent, int x, int y, int w, int h,
		 int col_width, int row_width, position_t strpos, int autoalign);
void table_destroy(widget_t * table);
int table_insert(widget_t * table, int col, int row, char * string, char * image, void * data);
void table_attach_select(widget_t * table, void (*select)(int, int, void *));
void table_attach_destroy(widget_t * table, void (*destroy)(widget_t *));
void table_attach_keypress(widget_t * _table, void (*on_click)(widget_t *tb,int ascii));
void table_set_target(widget_t * table, int col, int row);
void table_remove(widget_t * table, int col, int row);
void * table_get_data(widget_t * table, int col, int row);
int table_set_data(widget_t * _table, int col, int row,char * data);
void table_set_rim(widget_t * table,int rim);
void table_set_scrollbar(widget_t * _table,int scrollbar);
int table_set_focus(widget_t * table, int col, int row);
int table_get_focus(widget_t * table,int *col,int *row);
int table_focus_select(widget_t * table,int col,int row);
void table_attach_mouse_release(widget_t * table, void (*_on_release)(widget_t *tb));
int table_set_color(widget_t * table,int col,int row,color_t bg,color_t fg);
int table_remove_color(widget_t * table,int col,int row);
int table_set_string(widget_t * table,int col,int row,char * str);
int table_get_string(widget_t * table,int col,int row,char * str);


widget_t * button_new(widget_t * parent, int x, int y, int w, char * string, char * icon);
void button_attach_onclick(widget_t * widget, void (*on_click)(void));
void button_attach_key(widget_t * widget, int ascii);
int button_set_height(int height);

widget_t * checkbox_new(widget_t * parent, int x, int y, int w, char * string, int check);
void checkbox_destroy(widget_t * widget);
int checkbox_get_check(widget_t * widget);
int checkbox_set_check(widget_t * widget, int check, int draw);
void checkbox_attach_oncheck(widget_t * widget, void (*on_check)(int check));
void checkbox_attach_key(widget_t * widget, int ascii);

widget_t * radiobox_new(widget_t * parent, int x, int y, int w, char * string);
int radiobox_set_check(widget_t * widget, int draw);
int radiobox_get_check(widget_t * widget);
int radiobox_group(widget_t * wida, widget_t * widb);
void radiobox_attach_oncheck(widget_t * widget, void (*on_check)(void));
void radiobox_attach_key(widget_t * widget, int ascii);
void radiobox_destroy(widget_t * widget);

widget_t * text_new(widget_t * parent, int x, int y, int w, int h);
void text_set_bg_color(widget_t * text, color_t bg, int draw);
void text_set_fg_color(widget_t * text, color_t fg, int draw);
void text_set_content(widget_t * text, char * string, int draw);
void text_set_file(widget_t * text, char * file, int draw);
void text_set_linewidth(widget_t * text, int linewidth, int draw);
void text_attach_exit(widget_t * text, void (*text_exit)(void* data,int yoffset));
void text_set_bookmarks(widget_t * text,int yoffset);
void text_attach_key(widget_t * text,int pageup_key,int pagedown_key,int exit_key);
void text_set_bookmarks(widget_t * text,int yoffset);
int text_get_line(widget_t *text);
int text_set_line(widget_t *text,int line,int draw);
void text_attach_onclick(widget_t * text,void (*text_onclick)(void));

widget_t * dock_new(widget_t * parent, int x, int y, int w, int h,
		char * defimg, char * preimg, position_t dockpos);
int dock_add(widget_t * dock, char * img, void * data);
void dock_attach_select(widget_t * dock, void (*dock_on_select)(int sel, void * data));

widget_t *progress_bar_new(widget_t *parent,int x,int y,int w,int h,unsigned int max);
void progress_bar_step(widget_t* pg);
void progress_bar_reset(widget_t* widget);

widget_t * combobox_new(widget_t * parent, int x, int y, int w,char * title);
int combobox_insert(widget_t * widget,int row, char * string, int dfvalue);
int combobox_getvalue(widget_t * widget,char * value);
int combobox_destroy(widget_t * widget);
int combobox_clean(widget_t * widget);
int combobox_attach_onclick(widget_t * widget,void (*on_click)(void));

widget_t * editbox_new(widget_t * parent, int x, int y, int w,int h,int input,char * title,unsigned int maxlen);
int editbox_setvalue(widget_t * widget,char * value);
int editbox_getvalue(widget_t * widget,char * value);
int editbox_getmaxlen(widget_t * widget);
void editbox_attach_keypressd(widget_t * widget, void (*select)(widget_t *,int));
void editbox_attach_onenter(widget_t * widget, void (*onenter)(void));
int editbox_select_mode(widget_t * widget,int mode);
int editbox_change_inputmode(widget_t * widget,int mode);
int editbox_attach_input_rule(widget_t * widget,int (*ValidateFuncInterface)(char *pszOldInput, 
							  int OldinputLen,const char *pszNewInput, int NewinputLen,int OutLen,void *pAdditionParam));
int editbox_alpha_enable(widget_t * widget,int flag);

widget_t * string_new(widget_t * parent, int x, int y,int w,int h,char * string);
int string_set_value(widget_t * widget,char *value);
int string_get_value(widget_t * widget,char *value);

widget_t * messagebox_new (widget_t * window, int w,int h);
widget_t* messagebox_select(widget_t * widget);
void messagebox_destroy(widget_t * message);

void statusbar_attach_key(int ascii);
void statusbar_disp(int flag,int draw);
void statusbar_position(char position);

void sp_fill_rect(int x,int y,int w,int h,color_t color,int lcdbuf);
void sp_set_pixel(int x,int y,color_t color,int lcdbuf);
void sp_draw_hline(int x1,int x2,int y,color_t color,int lcdbuf);
void sp_draw_vline(int x,int y1,int y2,color_t color,int lcdbuf);
void sp_image_draw(int x,int y,int w,int h,image_t * img,int lcdbuf);
void sp_disp_update(int x,int y,int w,int h,int lcdbuf);

int loadfont_bdf(int fontid,char *path);
void SDK_GuiSetFont (char s, char e);
void SDK_GuiTextP(char font,int x,int y,char *text);

void color_rgb(color_t c, int *r, int *g, int *b);
color_t rgb_color(int r, int g, int b);

image_t * image_decode(char * filepath);
int image_encode(image_t * pimage, char * filepath, char * type);
void image_destroy(image_t * pimage);
image_t * image_share(image_t * pimage);

int init_gui(void);
void uninit_gui(void);
int init_guiforapp(unsigned char*appname);

int get_lcdtype(char * type);
int get_screensize(unsigned int * width,unsigned int * height);
int detect_colorscreen(void);

#endif
