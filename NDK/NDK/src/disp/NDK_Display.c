/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：    产品开发部
* 日    期：    2012-08-17
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
*/
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../public/config.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "disp.h"
#include "gui.h"

extern color_t rgb_color(int r, int g, int b);
extern int ndk_config_init(void);
extern widget_t * window_new(int flags, char * title, char * iconpath);
extern void ndk_config_exit();
extern widget_t * panel_new (widget_t * window, char * bgicon);
extern void window_destroy(widget_t * win);
extern void window_show(widget_t * win);
extern image_t * image_decode(char * filepath);
extern void image_destroy(image_t * pimage);
extern int font_add(char *fontlib_type, char *cfont_path, char *afont_path);
extern void font_choose(char *fontlib_type,int fit);
extern char * get_current_exfont(void);
extern int detect_colorscreen(void);
int NDK_ScrGetLcdSize(uint *punWidth,uint *punHeight);
int NDK_ScrSetViewPort(uint unX,uint unY,uint unWidth, uint unHeight);
int NDK_ScrSetAttr(EM_TEXT_ATTRIBUTE  emNewattr, EM_TEXT_ATTRIBUTE  *pemOldattr);
int NDK_ScrDispString(uint unX,uint unY,const char *pszS,uint unMode);
int NDK_ScrGotoxy(uint unX, uint unY);
void ndk_draw_image(int x, int y, int w, int h, image_t * img, int xoff, int yoff);
extern void BDF_GuiTextP(char font,int x,int y,unsigned short *text,char reverse);
extern int setfont_bdf(int fontid);
extern int loadfont_bdf(int fontid,char *path,char *w,char *h);
extern void waitfor_lcdsync(void);
extern void statusbar_disp(int flag,int draw);

static int nOldTextAttr = TEXT_ATTRIBUTE_NORMAL;    /**<默认显示模式为正常显示*/
static int auto_update = 1;             /**<默认为自动刷新*/
static char ndk_dispver[16]="V1.00";    /**<模块版本号*/
static color_t nor_fg_color;            /**<当前正常显示字体RGB数值*/
static color_t rev_fg_color;            /**<当前反向显示字体RGB数值*/
static int ndk_dispinit;                /**<模块初始化标志*/
uint CursorPosX = 0, CursorPosY = 0;    /**<当前像素坐标*/
uint guserfonttype =DISPFONT_CUSTOM;                    /**<当前使用字体类型,值范围EM_DISPFONT*/

pthread_mutex_t statusbar_mutex=PTHREAD_MUTEX_INITIALIZER;
#define INIT_TIMEOUT 5000

/**< 系统字体正常显示颜色 by zhengk [2/16/2011] */
extern unsigned int g_fg_normal_R;
extern unsigned int g_fg_normal_G;
extern unsigned int g_fg_normal_B;

/**< 系统字体反显颜色 by zhengk [2/16/2011] */
extern unsigned int g_fg_reverse_R;
extern unsigned int g_fg_reverse_G;
extern unsigned int g_fg_reverse_B;

ndk_statusbar_t gndk_statusbar;



/**< 存屏数据结构*/
typedef struct {
    int attr;
    int posx;
    int posy;
    unsigned char * copy;
    int len;
    int viewporth;
} save_t;

static save_t _pushsave = {
    .attr = TEXT_ATTRIBUTE_NORMAL,
    .posx = 0,
    .posy = 0,
    .len = 0,
    .copy = NULL,
    .viewporth = 0
};

static save_t pushsave = {
    .attr = TEXT_ATTRIBUTE_NORMAL,
    .posx = 0,
    .posy = 0,
    .len = 0,
    .copy = NULL,
    .viewporth = 0
};

int ndk_rect_subtract(rect_t * r1, rect_t * r2)
{
    if (r1 == NULL || r2 == NULL) return 0;

    if(r2->y==0) {
        int tmpy = r1->y+r1->h;
        r1->y = MAX(r1->y,(r2->y+r2->h));
        if(r1->y > tmpy)
            return 0;
    } else {
        if(r1->y >= r2->y)
            return 0;
        r1->h = MIN(r1->y+r1->h,r2->y)-r1->y;
    }
    return 1;
}


static void _ndk_refresh(rect_t *rect)
{
    sys.video->sur->ops->surface_update(sys.video->sur, rect);
}

void ndk_getguifocus(void)
{
    socket_header_t header;

    if(ndk_dispinit==0)
        return;
wait:
    socket_unix_request(sys.socket_fd, SOCKET_DATA_SHOW, NULL, 0);
    if(socket_unix_response(sys.socket_fd, &header, sizeof(header), INIT_TIMEOUT)==0) {
        if (header != SOCKET_DATA_SHOW) {
            goto wait;
        }
    } else {
        goto wait;
    }
    if(gndk_statusbar.show) {
        statusbar_disp(gndk_statusbar.status,1);
    }
}

void ndk_wait_echo(void)
{
    socket_header_t header;

    if(ndk_dispinit==0)
        return;
wait:
    socket_unix_request(sys.socket_fd, SOCKET_DATA_ECHO, NULL, 0);
    if(socket_unix_response(sys.socket_fd, &header, sizeof(header), INIT_TIMEOUT)==0) {
        if (header != SOCKET_DATA_ECHO) {
            goto wait;
        }
    } else {
        goto wait;
    }
}

void ndk_close_client(void)
{
    if(ndk_dispinit==0)
        return;
    close(sys.socket_fd);
}

void  ndk_DispASCxy(uint unCursorX,uint unCursorY,char ch)
{
    uint x,y;
    x=unCursorX;
    y=unCursorY;
    char d[2];
    d[0] = ch;
    d[1] = '\0';
    NDK_ScrDispString(x,y,d,0);
    NDK_ScrGotoxy(x,y);
}

char * ndk_disp_save_data(int x,int y,int h)
{
    char * tmp;
    int len,off;

    len = sys.video->sur->width *h* sys.video->bytes_per_pixel;
    off = (sys.video->sur->width*y+x)*sys.video->bytes_per_pixel;
    if((tmp = calloc(1,len))==NULL)
        return NULL;
    memcpy(tmp, ((char *)(sys.video->sur->vbuf))+off ,len);
    return tmp;
}

void ndk_disp_restore_data(int x,int y,int h,char *data)
{
    int len,off;

    if(data==NULL)
        return;
    len = sys.video->sur->width *h* sys.video->bytes_per_pixel;
    off = (sys.video->sur->width*y+x)*sys.video->bytes_per_pixel;
    memcpy(((char *)(sys.video->sur->vbuf))+off,data ,len);
    _ndk_refresh(&(sys.view));
}

void ndk_disprevstr(const char * str)
{
    EM_TEXT_ATTRIBUTE attr;
    NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE, &attr);
    NDK_ScrDispString(CursorPosX, CursorPosY, str, 0);
    NDK_ScrSetAttr(attr, NULL);
}
int ndk_ScrPush(void)
{
    if (_pushsave.copy == NULL) {
        _pushsave.copy = malloc(sys.video->sur->width *sys.video->sur->height * sys.video->bytes_per_pixel);

    }
    _pushsave.len = sys.view.h * sys.video->sur->width * sys.video->bytes_per_pixel;
    _pushsave.viewporth  = sys.view.h;
    memcpy(_pushsave.copy, sys.video->sur->vbuf + (sys.view.y*sys.video->sur->width*sys.video->bytes_per_pixel), _pushsave.len);
    _pushsave.attr = nOldTextAttr;
    _pushsave.posx = CursorPosX;
    _pushsave.posy = CursorPosY;
    return NDK_OK;
}

int ndk_ScrPop(void)
{
    surface_t sur;
    rect_t dst;

    if (_pushsave.copy == NULL) {
        return NDK_ERR;
    }
    memcpy(&sur,sys.video->sur, sizeof (surface_t));
    sur.vbuf = _pushsave.copy;
    CursorPosX = _pushsave.posx;
    CursorPosY = _pushsave.posy;
    nOldTextAttr = _pushsave.attr;
    memcpy(&dst,&(sys.view),sizeof(rect_t));

    if(sys.view.h>_pushsave.viewporth) {
        dst.h = _pushsave.viewporth;
//      fprintf(stderr,"%s,%d %d %d\n",__FUNCTION__,__LINE__,sys.view.h,_pushsave.viewporth);
        NDK_ScrClrLine(dst.h,sys.view.h-1);
    }
//  fprintf(stderr,"%s,%d %d %d\n",__FUNCTION__,__LINE__,dst.y,dst.h);
    sys.video->sur->ops->blit(sys.video->sur, &dst, &sur, 0,0);
    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }
    return NDK_OK;
}


/**
 *@brief    显示格式为image_t的图片，支持偏移。
 *@param    x       显示区域起始坐标的横坐标（单位：像素）。
 *@param    y       显示区域起始坐标的横坐标（单位：像素）。
 *@param    w       图片宽度（单位：像素）。
 *@param    h       图片高度（单位：像素）。
 *@param    img     image_t格式的图片数据。
 *@param    xoff        显示起始位置在图片中的x偏移
 *@param    yoff        显示起始位置在图片中的y偏移
 *@return   无
*/
void ndk_draw_image(int x, int y, int w, int h, image_t * img, int xoff, int yoff)
{
    rect_t rect;
    int ret = 1;

    if (x >= sys.view.w) return;
    if (y >= sys.view.h) return;
    if (w <= 0 || h <= 0) return;
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    if(img==NULL) return;
    rect.w = sys.view.w - x;
    if (rect.w > w) rect.w = w;
    rect.h = sys.view.h - y;
    if (rect.h > h) rect.h = h;
    rect.x = x + sys.view.x;
    rect.y = y + sys.view.y;

    if (xoff >= img->width) return;
    if (yoff >= img->height) return;
    if (rect.w > img->width) rect.w = img->width;
    if (rect.h > img->height) rect.h = img->height;
    if (xoff < 0) xoff = 0;
    if (yoff < 0) yoff = 0;

    if(ret) {
        sys.video->sur->ops->image_draw(sys.video->sur, &rect, img, xoff, yoff);
        if (auto_update) {
            _ndk_refresh(&(sys.view));
        }
    }
}

/**
 *@brief    Bitmap格式图片转化为image_t格式。
 *@param    input       Bitmap格式图片。
 *@param    output      image_t格式图片。
 *@param    inlen       input数据长度。
 *@return   无
*/
static void bitmap2image(const char * input,char* output,int inlen)
{
    int i,j;
    char mask ;
    int k=0;

    for(i=0; i<inlen; i++) {
        mask = 0x80;
        for(j=0; j<8; j++) {
            if(mask&input[i]) {
                output[k++] = 0;
                output[k++] = 0;
            } else {
                output[k++] = 0xff;
                output[k++] = 0xff;
            }

            mask=mask>>1;
        }

    }
}

static int ndk_line(uint unStartX, uint unStartY, uint unEndX, uint unEndY, color_t unColor,uint flag)
{
    uint tmp, x1 , x2 , y1, y2 ;
    int dx,dy;

    if ((flag==0)&&((unStartX>=sys.video->sur->width )||(unEndX>=sys.video->sur->width )||(unStartY>=sys.video->sur->height )||(unEndY>=sys.video->sur->height)))
        return NDK_ERR_PARA;

    /**<更改为绝对座标 by zhengk [2/17/2011]*/
    unStartX+=sys.view.x;
    unStartY+=sys.view.y;
    unEndX+=sys.view.x;
    unEndY+=sys.view.y;

    x1 = unStartX;
    x2 = unEndX;
    y1 = unStartY;
    y2 = unEndY;
    dx = x2 - x1;
    dy = y2 - y1;

    if(auto_update)
        sys.video->sur->mode = SURFACE_NEEDEXPOSE;
    /**根据cColor值画线 by zhengk [2/23/2011]*/
    if (ABS (dx) < ABS (dy)) {
        if (y1 > y2) {
            tmp = x1;
            x1 = x2;
            x2 = tmp;
            tmp = y1;
            y1 = y2;
            y2 = tmp;
            dx = -dx;
            dy = -dy;
        }
        x1 <<= 16;

        /**dy is apriori >0*/
        dx = (dx << 16) / dy;
        while (y1 <= y2) {
            sys.video->sur->ops->set_pixel(sys.video->sur, x1 >> 16, y1, unColor);
            x1 += dx;
            y1++;
        }
    } else {
        if (x1 > x2) {
            tmp = x1;
            x1 = x2;
            x2 = tmp;
            tmp = y1;
            y1 = y2;
            y2 = tmp;
            dx = -dx;
            dy = -dy;
        }
        y1 <<= 16;
        dy = dx ? (dy << 16) / dx : 0;
        while (x1 <= x2) {
            sys.video->sur->ops->set_pixel(sys.video->sur, x1, (y1 >> 16), unColor);
            y1 += dy;
            x1++;
        }
    }
    if(auto_update)
        sys.video->sur->mode = SURFACE_VIRTUAL;
    return NDK_OK;
}

/**
 *@brief    用户程序界面初始化。
 *@details  在程序最初调用，该函数成功调用后，显示模块的API才能正常使用。
 *@return
 *@li       NDK_OK                              操作成功
 *@li           NDK_ERR_INIT_CONFIG                           初始化配置失败
 *@li           NDK_ERR_CREAT_WIDGET                       创建界面错误
*/
NEXPORT int NDK_ScrInitGui(void)
{
    NDK_LOG_INFO (NDK_LOG_MODULE_DISP,"call %s \n", __FUNCTION__);
    int ret;
    widget_t * win;
    widget_t *panel;
    uint lcd_w,lcd_h;

    if(ndk_dispinit)
        return NDK_OK;
    ret = ndk_config_init();
    if (-1 == ret) {
        NDK_LOG_CRIT (NDK_LOG_MODULE_DISP,"%s fail return-%s \n", __func__,"NDK_ERR_INIT_CONFIG");
        return NDK_ERR_INIT_CONFIG;
    }

    if ((win = window_new(0, NULL, NULL)) == NULL) {
        NDK_LOG_CRIT (NDK_LOG_MODULE_DISP,"%s fail return-%s \n", __func__,"NDK_ERR_CREAT_WIDGET");
        ndk_config_exit();
        return NDK_ERR_CREAT_WIDGET;
    }
    if ((panel = panel_new(win, NULL)) == NULL) {
        NDK_LOG_CRIT (NDK_LOG_MODULE_DISP,"%s fail return-%s \n", __func__,"NDK_ERR_CREAT_WIDGET");
        window_destroy(win);
        ndk_config_exit();
        return NDK_ERR_CREAT_WIDGET;
    }

    nor_fg_color = rgb_color(g_fg_normal_R, g_fg_normal_G, g_fg_normal_B);
    rev_fg_color = rgb_color(g_fg_reverse_R, g_fg_reverse_G, g_fg_reverse_B);
    NDK_ScrGetLcdSize(&lcd_w,&lcd_h);
    sys.view.x = 0;
    sys.view.y = 0;
    sys.view.w = lcd_w;
    sys.view.h = lcd_h;

    __window_show(win);
    ndk_dispinit = 1;
//NDK_ScrStatusbar(STATUSBAR_DISP_WIFI|STATUSBAR_DISP_TIME);
//  NDK_ScrStatusbar(0x10001);
    return NDK_OK;
}


/**
 *@brief    获取显示模块版本。
 *@retval   pszVer  返回模块版本,输入的pnVer应该不小于16字节。
 *@return
 *@li       NDK_OK                  操作成功
 *@li           NDK_ERR_PARA          参数非法
*/
NEXPORT int NDK_ScrGetVer(char* pszVer)
{
    if(pszVer==NULL)
        return NDK_ERR_PARA;
    strncpy(pszVer,ndk_dispver,sizeof(ndk_dispver));
    return NDK_OK;
}

/**
 *@brief    设置显示模式，并获取之前的显示模式。
 *@param    emNewattr   要设置的新显示模式。
 *@retval   pemOldattr  输出之前的显示模式，peOldattr为NULL时不返回。
 *@return
 *@li   NDK_OK          操作成功
 *@li           NDK_ERR_PARA          参数非法
*/
NEXPORT int NDK_ScrSetAttr(EM_TEXT_ATTRIBUTE  emNewattr, EM_TEXT_ATTRIBUTE  *pemOldattr)
{
    if((emNewattr!=(TEXT_ATTRIBUTE_NORMAL|TEXT_ATTRIBUTE_UNDERLINE))&&(emNewattr!=TEXT_ATTRIBUTE_NORMAL)
       &&(emNewattr!=(TEXT_ATTRIBUTE_REVERSE|TEXT_ATTRIBUTE_UNDERLINE))&&(emNewattr!=TEXT_ATTRIBUTE_REVERSE)
       &&(emNewattr!=TEXT_ATTRIBUTE_UNDERLINE)&&(emNewattr!=TEXT_ATTRIBUTE_NOBACKCOLOR)
       &&(emNewattr!=(TEXT_ATTRIBUTE_NOBACKCOLOR|TEXT_ATTRIBUTE_UNDERLINE)))
        return NDK_ERR_PARA;
    if (TEXT_ATTRIBUTE_UNDERLINE == emNewattr) {
        switch(nOldTextAttr) {
            case TEXT_ATTRIBUTE_NORMAL:
                emNewattr = TEXT_ATTRIBUTE_NORMAL|TEXT_ATTRIBUTE_UNDERLINE;
                break;
            case TEXT_ATTRIBUTE_REVERSE:
                emNewattr = TEXT_ATTRIBUTE_REVERSE|TEXT_ATTRIBUTE_UNDERLINE;
                break;
            case TEXT_ATTRIBUTE_NOBACKCOLOR:
                emNewattr = TEXT_ATTRIBUTE_NOBACKCOLOR|TEXT_ATTRIBUTE_UNDERLINE;
                break;
            default:
                break;
        }
    }
    //处理正显
    if (emNewattr & TEXT_ATTRIBUTE_NORMAL) {
        if (!(nOldTextAttr & TEXT_ATTRIBUTE_NORMAL)) {
            sys.fg = nor_fg_color;
        }
    }
    //处理反显
    else if(emNewattr & TEXT_ATTRIBUTE_REVERSE) {
        if(!(nOldTextAttr & TEXT_ATTRIBUTE_REVERSE)) {
            sys.fg = rev_fg_color;

        }
    } else if(emNewattr & TEXT_ATTRIBUTE_NOBACKCOLOR) {
        sys.fg = nor_fg_color;

    }
    if (pemOldattr != NULL)
        *pemOldattr = nOldTextAttr;
    nOldTextAttr = emNewattr;
    return NDK_OK;
}

/**
 *@brief    保存当前屏幕
 *@details  包括显示内容、光标位置及显示模式，该保存结果可调用NDK_ScrPop快速恢复显示。
            NDK_ScrPush与NDK_SrcPop成对使用，不能嵌套。
 *@return
 *@li   NDK_OK              操作成功
*/
NEXPORT int NDK_ScrPush(void)
{
    if (pushsave.copy == NULL) {
        pushsave.copy = malloc(sys.video->sur->width *sys.video->sur->height * sys.video->bytes_per_pixel);

    }
    pushsave.len = sys.video->sur->width *sys.view.h * sys.video->bytes_per_pixel;
    pushsave.viewporth  = sys.view.h;
//  fprintf(stderr,"%s,%d %d x %d\n",__FUNCTION__,__LINE__,sys.video->sur->width,sys.view.h);
    memcpy(pushsave.copy, sys.video->sur->vbuf+(sys.view.y*sys.video->sur->width*sys.video->bytes_per_pixel), pushsave.len);
    pushsave.attr = nOldTextAttr;
    pushsave.posx = CursorPosX;
    pushsave.posy = CursorPosY;
    return NDK_OK;
}

/**
 *@brief    快速恢复利用NDK_ScrPush保存的显示状态，包括显示内容、光标位置及文本显示属性。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR         操作失败（未保存显示状态）
*/
NEXPORT int NDK_ScrPop(void)
{
    surface_t sur;
    rect_t dst;

    if (pushsave.copy == NULL) {
        return NDK_ERR;
    }
    memcpy(&sur,sys.video->sur, sizeof (surface_t));
    sur.vbuf = pushsave.copy;
    CursorPosX = pushsave.posx;
    CursorPosY = pushsave.posy;
    nOldTextAttr = pushsave.attr;
    memcpy(&dst,&(sys.view),sizeof(rect_t));
    if(sys.view.h>pushsave.viewporth) {
        dst.h = pushsave.viewporth;
        NDK_ScrClrLine(dst.h,sys.view.h-1);
    }
    sys.video->sur->ops->blit(sys.video->sur, &dst, &sur, 0,0);
//  fprintf(stderr,"%s,%d %d x %d\n",__FUNCTION__,__LINE__,sys.view.w,sys.view.h);
    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }
    return NDK_OK;
}

static void clr_bgpic(void)
{
    if(sys.backimg!=NULL)
        image_destroy(sys.backimg);
    sys.backimg = NULL;
}

/**
 *@brief    设置背景图片。图片文件支持格式请查看各机型的图片格式限制。
 *@param    pszfilepath     图片文件路径+文件名
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PATH                文件路径非法
 *@li       NDK_ERR_DECODE_IMAGE           图像解码失败
*/
NEXPORT int NDK_ScrSetbgPic(char *pszfilepath)
{
    if ((pszfilepath!=NULL)&&(access(pszfilepath,F_OK)<0))
        return NDK_ERR_PATH;
    clr_bgpic();
    if((sys.backimg = image_decode(pszfilepath))==NULL)
        return NDK_ERR_DECODE_IMAGE;
    else
        return NDK_OK;
}

/**
 *@brief    取消背景图片。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrClrbgPic(void)
{
    clr_bgpic();
    return NDK_OK;
}

/**
 *@brief    清屏，把光标移到像素坐标(0,0)，同时将屏幕显示模式设置为TEXT_ATTRIBUTE_NORMAL。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrClrs(void)
{

    if(sys.backimg==NULL) {
        sys.video->sur->ops->fill_rect(sys.video->sur, &(sys.view), sys.bg);
    } else {
        sys.video->sur->ops->fill_rect(sys.video->sur, &(sys.view), sys.bg);
        ndk_draw_image(0,0,sys.view.w,sys.view.h,sys.backimg,0,0);
    }

    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }

    NDK_ScrSetAttr(TEXT_ATTRIBUTE_NORMAL, NULL);
    CursorPosX = CursorPosY = 0;
    return NDK_OK;
}

/**
 *@brief    清除若干行(单位：像素)，把光标移到(0,unStartY)处。
            输入参数超出视口边界时，以视口边界为准。
 *@param    unStartY        开始行号（纵坐标，单位：像素），从0开始计数
 *@param    unEndY          结束行号（纵坐标，单位：像素），从0开始计数
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrClrLine(uint unStartY,uint unEndY)
{
    rect_t rect;
    if (unEndY >=sys.view.h) {
        unEndY = sys.view.h-1;
    }
    if ((unStartY >=sys.view.h) || (unStartY > unEndY)) {
        return NDK_ERR_PARA;
    }

    rect.x = sys.view.x;
    rect.y = unStartY+sys.view.y;
    rect.w = sys.view.w;
    rect.h = unEndY - unStartY+1;
    if(sys.backimg==NULL) {
        sys.video->sur->ops->fill_rect(sys.video->sur, &rect, sys.bg);
    } else
	{
        sys.video->sur->ops->fill_rect(sys.video->sur, &rect, sys.bg);
        ndk_draw_image(0,unStartY,rect.w,rect.h,sys.backimg,0,unStartY);
	}
    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }
    CursorPosX = 0;
    CursorPosY = unStartY;
    return NDK_OK;
}

/**
 *@brief    液晶显示光标位置移到像素坐标(unX,unY)处。
            如果输入参数非法，则光标保留位置不变,返回错误信息。
 *@param    unX     横坐标（单位：像素）
 *@param    unY     纵坐标（单位：像素）
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrGotoxy(uint unX, uint unY)
{
    if ((unX >= sys.view.w) ||(unY >= sys.view.h) ) {
        return NDK_ERR_PARA;
    }

    CursorPosX=unX;
    CursorPosY=unY;
    return NDK_OK;
}

/**
 *@brief    获取当前像素光标位置的横坐标和纵坐标。
 *@retval   punX 返回光标位置的横坐标（单位：像素）。
 *@retval   punY 返回光标位置的纵坐标（单位：像素）。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrGetxy(uint *punX, uint *punY)
{
    if (punX != NULL)
        *punX = CursorPosX;
    if (punY != NULL)
        *punY = CursorPosY;
    return NDK_OK;
}

/**
 *@brief    设置显示区域尺寸。
 *@details  未设置情况下显示区域为实际屏幕尺寸，通过该接口设置显示区域后，其它API的显示操作只在该区域内有效。\n
            如设置(10,10,100,100)为应用显示区域，则应用程序中使用的像素坐标(0,0)实际上
            是屏幕像素坐标(10,10),清屏操作也只清除像素坐标(10,10,100,100)范围内的显示数据。
 *@param    unX     显示区域起始坐标的横坐标（单位：像素）。
 *@param    unY     显示区域起始坐标的横坐标（单位：像素）。
 *@param    unWidth 显示区域宽度（单位：像素）。
 *@param    unHeight    显示区域高度（单位：像素）。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrSetViewPort(uint unX,uint unY,uint unWidth, uint unHeight)
{
    if ((unX>sys.video->sur->width)||(unY>sys.video->sur->height)||(unWidth>sys.video->sur->width)||(unHeight>sys.video->sur->height)
        ||((unX+unWidth)>sys.video->sur->width) ||( (unY+unHeight)>sys.video->sur->height)) {
        return NDK_ERR_PARA;
    }

    if(gndk_statusbar.show) {
        if(unHeight>(sys.video->sur->height-gndk_statusbar.rect.h)) {
            return NDK_ERR_PARA;
        }
        if(gndk_statusbar.rect.y==0) {
            if((unY<gndk_statusbar.rect.h)||((unY+unHeight)>(sys.video->sur->height))) {
                return NDK_ERR_PARA;
            }
        } else {
            if((unY>sys.video->sur->height-gndk_statusbar.rect.h-1)||((unY+unHeight)>(sys.video->sur->height-gndk_statusbar.rect.h))) {
                return NDK_ERR_PARA;
            }
        }
        sys.view.x = unX;
        sys.view.y = unY;
        sys.view.w = unWidth;
        sys.view.h = unHeight;
    } else {
        sys.view.x = unX;
        sys.view.y = unY;
        sys.view.w = unWidth;
        sys.view.h = unHeight;
    }
    NDK_LOG_INFO(NDK_LOG_MODULE_DISP,"%s succ,current display area(%d,%d,%d,%d)\n",__func__,unX,unY,unWidth,unHeight);
    return NDK_OK;
}

/**
 *@brief    获取当前显示区域尺寸。
 *@retval   punX        显示区域起始坐标的横坐标（单位：像素）。
 *@retval   punY        显示区域起始坐标的纵坐标（单位：像素）。
 *@retval   punWidth    显示区域高度（单位：像素）。
 *@retval   punHeight   显示区域高度（单位：像素）。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrGetViewPort(uint *punX,uint *punY,uint *punWidth,uint *punHeight)
{
    if(punX!=NULL)
        *punX = sys.view.x;
    if(punY!=NULL)
        *punY = sys.view.y;
    if(punWidth!=NULL)
        *punWidth = sys.view.w;
    if(punHeight!=NULL)
        *punHeight = sys.view.h;
    return NDK_OK;
}

/**
 *@brief    在显示区域显示Bitmap图片。
 *@details  bitmap格式：1byte对应8个像素点,0表示白点，1表示黑点，显示数据横向排列，如下图所示:\n
-----------------D7~~D0--------------D7~~D0------------------\n
Byte 1: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte2 \n
Byte 3: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte4 \n
Byte 5: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte6 \n
Byte 7: ○ ○ ● ● ● ● ● ●  ●  ● ● ● ● ● ○ ○ Byte8 \n
Byte 9: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte10    \n
Byte11: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte12    \n
Byte13: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte14    \n
Byte15: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte16    \n
---------------------------------------------------------------\n
    如果显示图片范围超出屏幕范围或通过NDK_ScrSetViewPort设置的用户使用区域时，则该函数操作无效，返回调用失败。
 *@param    unX         图片在显示区域的左上角横坐标（单位：像素）
 *@param    unY         图片在显示区域的左上角纵坐标（单位：像素）
 *@param    unWidth     图片宽度（单位：像素）
 *@param    unHeight    图片高度（单位：像素）
 *@param    psBuf       Bitmap图片数据
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrDrawBitmap(uint unX,uint unY,uint unWidth, uint unHeight, const char *psBuf)
{
    image_t * image;
    int len;

    if ((unX> sys.view.w)||(unY> sys.view.h)||(unWidth> sys.video->sur->width)||(unHeight>  sys.video->sur->height)||((unX+unWidth) >  sys.video->sur->width)
        || ((unY+unHeight)> sys.video->sur->height) ||(psBuf==NULL))
        return NDK_ERR_PARA;

    image = calloc(1,sizeof(image_t));
    image->width = 8*((unWidth+7)/8);
    image->height = unHeight;
    image->bytes_per_pixel = 2;
    len = image->width*image->height*image->bytes_per_pixel;
    image->user=1;
    image->image_buf = calloc(1,len*sizeof(char));

    bitmap2image(psBuf,image->image_buf,image->height*image->width/8);

    ndk_draw_image(unX,unY,unWidth,unHeight,image,0,0);

    free(image->image_buf);
    free(image);
    return NDK_OK;
}

/**
 *@brief    从显示区域上像素坐标(unStartX,unStartY)到(unEndX,unEndY)画直线，unColor表示画线的RGB色彩值。
 *@param    unStartX    直线的起点横坐标（单位：像素）
 *@param    unStartY    直线的起点纵坐标（单位：像素）
 *@param    unEndX      直线的终点横坐标（单位：像素）
 *@param    unEndY      直线的终点纵坐标（单位：像素）
 *@param    unColor         颜色数值 <0-0xFFFF>
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrLine(uint unStartX, uint unStartY, uint unEndX, uint unEndY, color_t unColor)
{
    return ndk_line(unStartX,unStartY,unEndX,unEndY,unColor,0);
}

/**
 *@brief    在显示区域画一个矩形。
 *@details  如果矩形边界超出屏幕范围时，则该函数操作无效，返回调用失败。
 *@param    unX         矩形的起点横坐标（单位：像素）
 *@param    unY         矩形的起点纵坐标（单位：像素）
 *@param    unWidth         矩形的宽（单位：像素）
 *@param    unHeight        矩形的高（单位：像素）
 *@param    emFill_pattern  0为非填充模式，1为填充模式
 *@param    unColor 颜色数值 <0-0xFFFF>
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrRectangle(uint unX, uint unY, uint unWidth, uint unHeight, EM_RECT_PATTERNS emFill_pattern, color_t unColor)
{
    uint x1, y1, x2, y2;
    uint x_end,y_end;

    if ((unX> sys.view.w)||(unY> sys.view.h)||(unWidth> sys.video->sur->width)||(unHeight>sys.video->sur->height)
        ||((unX+unWidth) > sys.video->sur->width) || ((unY+unHeight) > sys.video->sur->height)) {
        return NDK_ERR_PARA;
    }

    if(emFill_pattern<RECT_PATTERNS_NO_FILL||emFill_pattern>RECT_PATTERNS_SOLID_FILL)
        return NDK_ERR_PARA;

    //更改为绝对座标 by zhengk [2/17/2011]
    unX+=sys.view.x;
    unY+=sys.view.y;

    //算出矩形结束点的相对坐标
    x_end = unX + unWidth-1;
    y_end = unY + unHeight-1;

    //超出部分无效 by zhengk [2/19/2011]
#if 0
    if(unX>=sys.view.x+sys.view.w)
        unX=sys.view.x+sys.view.w-1;
    if(x_end>=sys.view.x+sys.view.w)
        x_end=sys.view.x+sys.view.w-1;
    if(unY>=sys.view.y+sys.view.h)
        unY=sys.view.y+sys.view.h-1;
    if(y_end>=sys.view.y+sys.view.h)
        y_end=sys.view.y+sys.view.h-1;
#endif

    x1 = MIN(unX, x_end);
    y1 = MIN(unY, y_end);
    x2 = MAX(unX, x_end);
    y2 = MAX(unY, y_end);

    if (emFill_pattern == RECT_PATTERNS_SOLID_FILL) {
        rect_t rt;
        rt.x = x1;
        rt.y = y1;
        rt.w = x2 - x1 + 1;
        rt.h = y2 - y1 + 1;
        sys.video->sur->ops->fill_rect(sys.video->sur, &rt, unColor);
    } else {
        sys.video->sur->ops->draw_hline(sys.video->sur, x1, x2, y1, unColor);
        sys.video->sur->ops->draw_hline(sys.video->sur, x1, x2, y2, unColor);
        sys.video->sur->ops->draw_vline(sys.video->sur, x1, y1, y2, unColor);
        sys.video->sur->ops->draw_vline(sys.video->sur, x2, y1, y2, unColor);
    }

    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }
    return NDK_OK;
}

/**
 *@brief    在显示区域显示指定的图片，图片文件支持格式请查看各机型的图片格式限制。
 *@details  如果显示图片范围超出屏幕范围或通过NDK_ScrSetViewPort设置的用户使用区域时，则该函数操作无效，返回调用失败。
 *@param    unX         图片显示的左上角横坐标（单位：像素）
 *@param    unY         图片显示的左上角纵坐标（单位：像素）
 *@param    unWidth     图片的宽（单位：像素）
 *@param    unHeight    图片显示的高（单位：像素）
 *@param    pszPic  图片文件所在的路径。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_DECODE_IMAGE        图像解码失败
*/
NEXPORT int NDK_ScrDispPic(uint unX,uint unY,uint unWidth, uint unHeight, const char *pszPic)
{
    image_t *bg;
    rect_t rect;

    if ((pszPic!=NULL)&&(access(pszPic,F_OK)<0))
        return NDK_ERR_PATH;
    if ((unX> sys.view.w)||(unY> sys.view.h)||(unWidth> sys.video->sur->width)||(unHeight> sys.video->sur->height)
        ||((unX+unWidth)> sys.video->sur->width) || ((unY+unHeight)>sys.video->sur->height)) {
        return NDK_ERR_PARA;
    }
    if ((bg = image_decode((char *)pszPic)) == NULL)
        return NDK_ERR_DECODE_IMAGE;

    rect.x = unX + sys.view.x;
    rect.y = unY + sys.view.y;
    rect.w = MIN(unWidth,sys.view.w-unX);
    rect.h = MIN(unHeight,sys.view.h-unY);
    if(bg->width<rect.w)
        rect.w = bg->width;
    if(bg->height<rect.h)
        rect.h = bg->height;
    sys.video->sur->ops->image_draw(sys.video->sur, &rect, bg, 0, 0);
    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }
    image_destroy(bg);
    return NDK_OK;
}

/**
 *@brief    取显示区域上指定像素坐标点的颜色数值。
 *@param    unX         横坐标（单位：像素）
 *@param    unY         纵坐标（单位：像素）
 *@retval   punColor    返回的颜色值。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrGetPixel(uint unX, uint unY, color_t *punColor)
{
    unsigned short * addr = sys.video->sur->vbuf;

    if ((unX >= sys.view.w) || (unY >= sys.view.h)||(punColor==NULL))
        return NDK_ERR_PARA;

    //更改为绝对座标 by zhengk [2/22/2011]
    unX+=sys.view.x;
    unY+=sys.view.y;

    *punColor = (int)(*(addr +unX+unY*(sys.video->sur->width)));//返回RGB数值
    return NDK_OK;
}

/**
 *@brief    在显示区域上指定像素坐标画点。
 *@param    unX         横坐标（单位：像素）
 *@param    unY     纵坐标（单位：像素）
 *@param    unColor     颜色数值 <0-0xFFFF>
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrPutPixel(uint unX, uint unY, color_t unColor)
{
    color_t pixel;
    if ((unX >= sys.view.w) || (unY >= sys.view.h))
        return NDK_ERR_PARA;

    //更改为绝对座标 by zhengk [2/17/2011]
    unX+=sys.view.x;
    unY+=sys.view.y;

    pixel = unColor;
    if(auto_update)
        sys.video->sur->mode = SURFACE_NEEDEXPOSE;
    sys.video->sur->ops->set_pixel(sys.video->sur, unX, unY, pixel);
    if(auto_update)
        sys.video->sur->mode = SURFACE_VIRTUAL;
    return NDK_OK;
}

/**
 *@brief    将显存中用户设置的显示区域内的数据刷新到液晶屏上显示。
 *@details  系统缺省为自动刷新。为避免闪屏，可以通过NDK_ScrAutoUpdate设置为非自动刷新，在NDK_ScrRefresh
            调用后系统才将显存中的数据刷新到液晶屏上。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrRefresh(void)
{
    _ndk_refresh(&(sys.view));
    //waitfor_lcdsync();
    return NDK_OK;
}

/**
 *@brief    将显存中全部数据刷新到LCD上显示。
 *@details  该接口与NDK_ScrRefresh区别在于不受限于显示区域的大小，当通过NDK_ScrSetViewPort设置的显示区域为全屏时，
            则NDK_ScrRefresh与该接口效果一样
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrFullRefresh(void)
{
    rect_t rect;

    rect.x=0;
    rect.y=0;
    rect.w = sys.video->sur->width;
    rect.h = sys.video->sur->height;
    _ndk_refresh(&(rect));
    return NDK_OK;
}

/**
 *@brief    设置是否自动刷新。
 *@param    nNewauto
                    非0:自动刷新
                    0:不自动刷新，只有调用NDK_ScrRefresh才显示显存中的数据。
 *@retval   pnOldauto   返回设置之前的自动刷新状态。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrAutoUpdate(int nNewauto, int *pnOldauto)//这里变更为取旧值???因为按键输入需要用到
{
    if (pnOldauto != NULL) {
        *pnOldauto = auto_update;
    }
    if (0 == nNewauto)
        auto_update = 0;
    else
        auto_update = 1;
    return NDK_OK;
}

/**
 *@brief    获取液晶屏尺寸。
 *@retval       punWidth    返回LCD宽度（单位：像素）。
 *@retval       punHeight   返回LCD高度（单位：像素）。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrGetLcdSize(uint *punWidth,uint *punHeight)
{
    if((punWidth==NULL)||(punHeight==NULL))
        return NDK_ERR_PARA;
    *punWidth = sys.video->sur->width;
    *punHeight = sys.video->sur->height;
    //特殊处理,可视宽度为128 by zhengk [8/31/2012]
    if((*punWidth==132)&&(*punHeight==160))
        *punWidth=128;

    return NDK_OK;
}


/**
 *@brief    获取液晶屏色深。
 *@detail   可用于判断液晶屏是单色屏或彩屏
 *@retval       puncd   返回液晶屏色深：1----黑白两色,
                                        16----16位色，彩屏
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrGetColorDepth(uint *puncd)
{
    if(puncd==NULL)
        return NDK_ERR_PARA;
    *puncd = sys.video->bits_per_pixel;
    return NDK_OK;
}

/**
 *@brief    开关背光操作。
 *@param    emBL    0 – 关闭液晶背光
                    1 –打开液晶背光
                    2 –液晶背光常亮
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrBackLight(EM_BACKLIGHT emBL)
{
    int gpio_fd, ret;
    int contrl;

    switch(emBL) {
        case BACKLIGHT_OFF:
            contrl=0x20;
            break;
        case BACKLIGHT_ON:
            contrl = 0x21;
            break;
        case BACKLIGHT_LOCKON:
            contrl=0x11;
            break;
        default:
            return NDK_ERR_PARA;
            break;
    }

    gpio_fd = open( "/dev/gpio", O_WRONLY );
    if (gpio_fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }
    ret = ioctl(gpio_fd, GPIO_IOCS_BACKLIGHT, &contrl);
    if (ret < 0) {
        close(gpio_fd);
        return NDK_ERR_IOCTL;
    }
    close(gpio_fd);
    return NDK_OK;
}

/**
 *@brief        设置屏幕对比度。
 *@param        unContrast  对比度级别为0~63，0最亮，63最暗。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_OPEN_DEV        打开设备文件错误
 *@li   NDK_ERR_IOCTL       驱动调用错误
*/
NEXPORT int NDK_ScrSetContrast(uint Level)
{
    int disp_fd,ret;
    uint tmplevel;

    if(Level>63)
        return NDK_ERR_PARA;
    disp_fd = open( "/dev/disp", O_WRONLY );
    if (disp_fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }
    tmplevel=Level;
    Level= Level*0X02+0x50;
    ret = ioctl(disp_fd, DISP_IOC_SCONTRAST , &Level);
    if (ret) {
        close(disp_fd);
        return NDK_ERR_IOCTL;
    }
    close(disp_fd);
    ndk_setconfig("lcd", "lcdcontrast", CFG_INT,&tmplevel);
    return NDK_OK;
}

/**
 *@brief        设置用户自定义字体。
 *@details      设置成功后，通过 NDK_ScrSetFontType类型DISPFONT_USER来使用自定义的字体。\n
                注意:为了系统统一处理时显示排版，英文字库宽必须为汉字字库宽的一半，高度与汉字字库相同。
 *@param        pcCpath 汉字字库。
 *@param        pcApath 英文字库。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR     操作失败(设置用户自定义字体失败)
*/
NEXPORT int NDK_ScrFontAdd(const char *pcCpath,const char *pcApath)
{
    extern int font_add(char *fontlib_type, char *cfont_path, char *afont_path);

    if((pcCpath==NULL)&&(pcApath==NULL))
        return NDK_ERR_PARA;
    if(font_add("userfont", (char *)pcCpath, (char *)pcApath)<0)
        return NDK_ERR;
    return NDK_OK;
}

/**
 *@brief    设置使用的显示字体。
 *@details  系统默认支持2种尺寸点阵大小的字体可供切换,不同机型设定的系统默认字体尺寸不一定相同\n\n
            彩屏支持:汉字16x16 ASCII:8x16 (DISPFONT_CUSTOM)\n
                     汉字24x24 ASCII:12x24 (DISPFONT_EXTRA)\n
            黑白屏支持:汉字12x12 ASCII:6x12 (DISPFONT_CUSTOM)\n
                        汉字16x16 ASCII:8x16 (DISPFONT_EXTRA)\n\n
            设置显示字体对后续显示字体起作用，对之前已刷新显示的内容无效
 *@param    emType  选择字体
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrSetFontType(EM_DISPFONT emType)
{
    extern void change_font(int type);
    if(emType<DISPFONT_CUSTOM||emType>DISPFONT_USER)
        return NDK_ERR_PARA;
    guserfonttype = emType;
    if(emType==DISPFONT_USER) {
        font_choose("userfont",1);
        return NDK_OK;
    }
    change_font(emType);
    return NDK_OK;
}

/**
 *@brief    获取当前使用的显示字体类型。
 *@details  系统默认支持2种尺寸点阵大小的字体可供切换,不同机型设定的系统默认字体尺寸不一定相同\n\n
            彩屏支持:汉字16x16 ASCII:8x16 (DISPFONT_CUSTOM)\n
                     汉字24x24 ASCII:12x24 (DISPFONT_EXTRA)\n
            黑白屏支持:汉字12x12 ASCII:6x12 (DISPFONT_CUSTOM)\n
                        汉字16x16 ASCII:8x16 (DISPFONT_EXTRA)\n\n
            设置显示字体对后续显示字体起作用，对之前已刷新显示的内容无效
 *@param    pemType  返回当前字体类型
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrGetFontType(EM_DISPFONT *pemType)
{
    if(pemType==NULL)
        return NDK_ERR_PARA;
    *pemType = guserfonttype;
    return NDK_OK;
}
/**
 *@brief    获取当前系统使用的汉字显示字体宽和高。
 *@details  系统使用的ASCII字符字体点阵宽度固定为汉字的一半
 *@retval   punWidth    返回当前系统当前显示字体的汉字点阵宽。
 *@retval   punHeight   返回当前系统当前显示字体的汉字点阵高。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrGetFontSize(uint *punWidth,uint *punHeight)
{
    if(punWidth!=NULL) {
        *punWidth = sys.font->cfontw;
        if((sys.font->fonth%8)&&(sys.font->cfontw>sys.font->fonth))
            *punWidth =  sys.font->fonth;
    }
    if(punHeight!=NULL)
        *punHeight = sys.font->fonth;
    return NDK_OK;
}

/**
 *@brief    设置字体颜色，包含正显、反显、反显背景色。
 *@param    unColor 颜色数值
 *@param    emType  选择设置对像

 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrSetFontColor(color_t unColor, EM_FONTCOLOR emType)
{
    if (emType > FONTCOLOR_BG_REVERSE || emType < FONTCOLOR_NORMAL) {
        return NDK_ERR_PARA;
    }
    switch(emType) {
        case FONTCOLOR_NORMAL:
            if ((TEXT_ATTRIBUTE_NORMAL& nOldTextAttr) ||(TEXT_ATTRIBUTE_NOBACKCOLOR& nOldTextAttr)) {
                sys.fg = unColor;
            }
            nor_fg_color = unColor;
            break;
        case FONTCOLOR_REVERSE:
            if(TEXT_ATTRIBUTE_REVERSE & nOldTextAttr) {
                sys.fg = unColor;
            }
            rev_fg_color = unColor;
            break;
        case FONTCOLOR_BG_REVERSE:
            sys.fb = unColor;
            break;
        default:
            break;
    }
    return NDK_OK;
}

/**
 *@brief    设置行间距和字间距。
 *@details  wspace大于当前使用的英文字体像素宽度的2倍时，字间距设为英文字体像素宽度的2倍
            hspace大于当前使用的英文字体像素高度时，行间距设为英文字体像素高度
 *@param    unWspace    字间距（单位：像素）
 *@param    unHpace 行间距（单位：像素）
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_ScrSetSpace(uint wspace,uint hspace)
{
    sys.wspace=MIN(wspace,sys.font->cfontw);
    sys.hspace = MIN(hspace,sys.font->fonth);
    return NDK_OK;
}


static int DispStringEx(uint x, uint y, const char *s, uint space)
{
    int i, cr = 0;
    rect_t rt, spacert;
    char line[128];
    int ret =1,retbg =1,retbg1 = 0;


    if (x >= sys.view.w|| y >= sys.view.h ||( s==NULL)) {
        return NDK_ERR_PARA;
    }

    if (y + sys.font->fonth > sys.view.h) {
        return NDK_ERR_PARA;
    }
    i = 0;

    spacert.x = rt.x = x + sys.view.x;
    spacert.y = rt.y = y + sys.view.y;
    rt.w = 0;
    spacert.w =0;
    rt.h = sys.font->fonth;
    spacert.h = rt.h +sys.hspace;
    if(nOldTextAttr&TEXT_ATTRIBUTE_NOBACKCOLOR)
        retbg1=1;

    while (1) {
        switch (*s) {
            case 0:
                cr = 2; /* end */
                break;
            case '\n':  /* CL */
                cr = 1;
                break;
            case '\r':  /* =0 */
                break;
            case 0x03:
                cr = 3; /* show but continue */
                break;
            case 0x02:
                cr = 5;
                break;

            default :
                if (*s & 0x80) {
                    if(sys.font->fonth<sys.font->cfontw) { //小字体情况下不显示汉字
                        s+=2;
                        continue;
                    }
                    if (rt.x - sys.view.x + rt.w + sys.font->cfontw > sys.view.w) {
                        cr = 4; /* new line auto */
                    } else {
                        line[i ++] = *s ++;
                        line[i ++] = *s ++;
                        if((rt.x - sys.view.x + rt.w+sys.font->cfontw+sys.wspace)>sys.view.w) {
                            rt.w += sys.font->cfontw;
                            spacert.w += sys.font->cfontw;
                            cr = 4;
                            break;
                        } else {
                            rt.w += (sys.font->cfontw+sys.wspace);
                            spacert.w += (sys.font->cfontw+sys.wspace);
                        }
                        continue;
                    }
                } else {
                    if (rt.x - sys.view.x + rt.w + sys.font->afontw > sys.view.w) {
                        cr = 4;
                    } else {
                        line[i ++] = *s ++;

                        if((rt.x - sys.view.x + rt.w + sys.font->afontw+sys.wspace)>sys.view.w) {
                            rt.w += sys.font->afontw;
                            spacert.w += sys.font->afontw;
                            cr = 4;
                            break;
                        } else {
                            rt.w += (sys.font->afontw+sys.wspace);
                            spacert.w += (sys.font->afontw+sys.wspace);
                        }
                        continue;
                    }
                }
                break;
        }
        if (cr != 4) s ++;

        line[i] = 0;

        if (nOldTextAttr&TEXT_ATTRIBUTE_REVERSE) {
            sys.video->sur->ops->fill_rect(sys.video->sur, &rt, sys.fb);
        } else if (space) {
            if(retbg1!=1)
                sys.video->sur->ops->fill_rect(sys.video->sur, &spacert, sys.bg);
        } else
		{
            if(retbg1!=1)
                sys.video->sur->ops->fill_rect(sys.video->sur, &spacert, sys.bg);
            ndk_draw_image(spacert.x-sys.view.x,spacert.y-sys.view.y,spacert.w,spacert.h,sys.backimg,spacert.x-sys.view.x,spacert.y-sys.view.y);
		}
        if (nOldTextAttr&TEXT_ATTRIBUTE_UNDERLINE) {
            if(rt.w)
                ndk_line(rt.x-sys.view.x, rt.y-sys.view.y+rt.h-1, (rt.x+rt.w-sys.view.x-1), rt.y-sys.view.y+rt.h-1, sys.fg,1);
        }
        sys.font->draw_text(sys.font, sys.video->sur, &rt, line, 0, 0, sys.fonttype, sys.wspace,sys.fg, 0);
        if (cr == 2) break;
        if (cr == 3 || cr == 5) {
            rt.x += rt.w;
            spacert.x+=rt.w;
            rt.w = spacert.w = 0;
            i = 0;
            if (cr == 3) {
                nOldTextAttr = TEXT_ATTRIBUTE_REVERSE;
                sys.fg = rev_fg_color;
            } else {
                nOldTextAttr = TEXT_ATTRIBUTE_NORMAL;
                sys.fg = nor_fg_color;
            }
            cr = 0;
            continue;
        }
        if (cr) {
            rt.y += (sys.font->fonth+sys.hspace);
            spacert.y = rt.y;
            cr = 0;
            CursorPosX = 0;
            CursorPosY = rt.y - sys.view.y;
        }
        if ((rt.y -sys.view.y+ sys.font->fonth) > sys.view.h)  {
            break;
        }
        spacert.x = rt.x = sys.view.x;
        i = 0;
        rt.w =spacert.w= 0;
    }
    CursorPosX = rt.x - sys.view.x + rt.w;
    CursorPosY = rt.y - sys.view.y;
    if(CursorPosX>=sys.view.w) { //换行时坐标修正 by zhengk [2/22/2011]
        CursorPosX = 0;
        CursorPosY +=(sys.font->fonth+sys.hspace);
    }
    if(CursorPosY>=sys.view.h) {
        CursorPosX = 0;
        CursorPosY =sys.view.h-sys.font->fonth;// 超出底行时，坐标重置在最后一行by zhengk [2/22/2011]
    }
    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }

    return NDK_OK;
}

/**
 *@brief    显示字符串。
 *@param    unX 显示字符串位置的横坐标
 *@param    unY 显示字符串位置的纵坐标
 *@param    pszS    要显示的字符串指针
 *@param    unMode  设置显示ASCII字符时尺寸
                    0：系统当前使用的显示字体中ASCII点阵尺寸
                    1：使用小号英文字体，不适用于包含汉字的字符串
                        黑白屏：8x8尺寸英文字体
                        彩屏：8x16尺寸英文字体
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrDispString(uint unX,uint unY,const char *pszS,uint unMode)
{
    int flag=0;
    int ret = NDK_OK;

    if(pszS==NULL || unMode>1)
        return NDK_ERR_PARA;
    if(sys.backimg==NULL)
        flag = 1;

    if (1 == unMode) {
        char *exfont = NULL;
        char *bacup = NULL;

        bacup = get_current_exfont();
        if(bacup) {
            exfont = strdup(bacup);
        }
        if(detect_colorscreen()) {
            font_add("asc8x16",NULL,"/guiapp/share/fonts/mono8x16");
            font_choose("asc8x16",0);
        } else {
            font_add("asc8x8",NULL,"/guiapp/share/fonts/asc8x8_h.bin");
            font_choose("asc8x8",0);
        }
        ret = DispStringEx(unX, unY, pszS, flag);
        font_choose(exfont,0);
        NDK_ScrSetFontType(guserfonttype);
        if(exfont)
            free(exfont);
        return ret;
    }
    ret = DispStringEx(unX, unY, pszS, flag);
    return ret;
}

/**
 *@brief    屏幕显示格式化输出，使用方法同printf
 *@param    format  参数输出的格式
  *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrPrintf(const char *format, ...)
{
    va_list ap;
    char tmp_str[1024];

    if (format == NULL) {
        return NDK_ERR_PARA;
    }

    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);

    return NDK_ScrDispString(CursorPosX, CursorPosY, tmp_str, 0);
}

/**
 *@brief    控制屏幕底部显示状态栏
 *@details  未调用该接口时，系统默认是关闭状态栏显示，通过该接口关闭或打开状态栏时，视口会发生变化，如果用户通过NDK_ScrSetViewPort自定义过视口，在操作状态栏开关后，需再次自定义视口大小
 *@param    emFlag  控制状态栏显示与关闭控制,支持并存,如 :STATUSBAR_DISP_BATTERY|STATUSBAR_DISP_WLSIGNAL
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
*/
NEXPORT int NDK_ScrStatusbar(EM_STATUSBAR emFlag)
{
    extern int statusbar_get_position(rect_t * rect);
    int position = (emFlag>>16)&0xFF;
    int disp = emFlag&0xFFFF;

    if((ndk_dispinit==0)||(disp&0xFE00)||((disp&0xFFFE)&&(disp&0x01))||(position>1)
		||((disp&STATUSBAR_DISP_DATE)&&((disp&STATUSBAR_DISP_TIME)==0))) {
        return NDK_ERR_PARA;
    }

    if((gndk_statusbar.show ==0)&&(disp==0))
        return NDK_OK;
    ndk_ScrPush();
    if(disp) {
        if(position) {
            statusbar_position(0x10);
        } else {
            statusbar_position(0x08);
        }
    }
    if(disp==0) {
        gndk_statusbar.show = 0;
        sys.view.x = 0;
        sys.view.y = 0;
        sys.view.w = sys.video->sur->width;
        sys.view.h = sys.video->sur->height;
        ndk_wait_echo();
    } else {

        gndk_statusbar.show = 1;
        gndk_statusbar.status = disp;
//      pthread_mutex_lock(&statusbar_mutex);
        statusbar_get_position(&gndk_statusbar.rect);

        if(gndk_statusbar.rect.y==0) {
            sys.view.x = 0;
            sys.view.y = gndk_statusbar.rect.h;
            sys.view.w = sys.video->sur->width;
            sys.view.h = sys.video->sur->height - gndk_statusbar.rect.h;
        } else {
            sys.view.x = 0;
            sys.view.y = 0;
            sys.view.w = sys.video->sur->width;
            sys.view.h = sys.video->sur->height - gndk_statusbar.rect.h;
        }

        ndk_wait_echo();
        //ndk_getguifocus();//本操作只为等待nlgui响应，为避免后续的显示操作在nlgui未收到数据前将共享内存中的数据刷新
//      pthread_mutex_unlock(&statusbar_mutex);
    }
    ndk_wait_echo();
    statusbar_disp(disp,1);
    ndk_ScrPop();
    NDK_LOG_INFO(NDK_LOG_MODULE_DISP,"%s succ,current bar status(%d)\n",__func__,disp);
    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }
    return NDK_OK;

}

/**
 *@brief    加载BDF字体
 *@details  用户自定义BDF格式的字体，支持加载多个BDF字体文件
 *@param    unFontID 自定义字体ID,如0,1,2等,NDK_ScrDispBDFText使用。如果uFontID与之前设置的BDF字体相同，规替换之前BDF字体
 *@param    pszFile BDF文件路径+文件名
 *@param    punWidth 获取该BDF字体最大像素宽
 *@param    punHeight 获取该BDF字体最大像素高
 *@return
 *@li   NDK_OK  操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR     BDF字体文件格式无法识别
*/
NEXPORT int NDK_ScrLoadBDF(uint unFontID,char *pszFile,uint *punWidth,uint *punHeight)
{
    if((pszFile==NULL)||(access(pszFile,F_OK)<0))
        return NDK_ERR_PARA;
    if(loadfont_bdf(unFontID,pszFile,(char *)punWidth,(char *)punHeight)<0)
        return NDK_ERR;
    else {
        if(punWidth!=NULL&&punHeight!=NULL)
            NDK_LOG_INFO(NDK_LOG_MODULE_DISP,"%s succ,current BDF info(Id:%d,Name:%s,Width:%d,Height:%d)\n",__func__,unFontID,pszFile,*punWidth,*punHeight);
        return NDK_OK;
    }
}

/**
 *@brief    使用已加载的BDF字体显示数据
 *@details
 *@param    uFontID NDK_ScrLoadBDF加载字体的ID号
 *@param    unX 显示字符串位置的横坐标
 *@param    unY 显示字符串位置的纵坐标
 *@param    pszText 显示的字符串，其值是BDF文件中字符相对应的编码值，内码或UNICODE，具体视BDF文件而定
 *@return
 *@li   NDK_OK  操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR     BDF字体文件格式无法识别
*/
NEXPORT int NDK_ScrDispBDFText(uint unFontID,uint unX,uint unY,ushort *pszText)
{
    int reverse = 0;
    if ((unX >= sys.view.w) || (unY >= sys.view.h) || ( pszText==NULL)) {
        return NDK_ERR_PARA;
    }

    if(setfont_bdf(unFontID)<0)
        return NDK_ERR;

    if (nOldTextAttr&TEXT_ATTRIBUTE_REVERSE) {
        reverse =1;
    }
    BDF_GuiTextP(unFontID,unX,unY,pszText,reverse);
    return NDK_OK;
}

/**
 *@brief    运行触屏校准程序
 *@return
 *@li       NDK_OK    校准成功
 *@li       \ref NDK_ERR_NO_DEVICES     "NDK_ERR_NO_DEVICES"        POS无触屏设备
 *@li       \ref NDK_ERR_QUIT           "NDK_ERR_QUIT"              用户退出
 *@li       \ref NDK_ERR                "NDK_ERR"                   操作失败
*/
int  NDK_ScrTSCalibrate(void)
{
    pid_t pid;
    int status;
    uint lcdtype,len;
    char hardwareinfo[16]={0};
    
    if(ndk_dispinit==0)
        return NDK_ERR;
    if(NDK_ScrGetColorDepth(&lcdtype)!=NDK_OK)
        return NDK_ERR;
    if(lcdtype==1)
        return NDK_ERR_NO_DEVICES;
    if(NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO,&len,hardwareinfo)!=NDK_OK)
        return NDK_ERR;
    if(hardwareinfo[11]==0xff)
        return NDK_ERR_NO_DEVICES;

    pid = fork();
    if (pid == 0) {
        execl("/guiapp/bin/calibrate","calibrate","10",(char *)0);
        exit(64);
    } else {
        if (wait(&status)==pid) {
		    ndk_getguifocus();
			NDK_SysMsDelay(100);
			NDK_KbFlush(); 
			NDK_ScrRefresh();
            switch(WEXITSTATUS(status)) {
			case 64:
                return NDK_ERR_NO_DEVICES;
				break;
			case 2:
				return  NDK_ERR_QUIT;
				break;
			case 3://传入触屏程序的参数非法
			case 1://触屏程序执行失败
				return NDK_ERR;
				break;
			default:
				break;
            }
        }
    }
	return NDK_OK;
}

/**
 *@brief    图片解码数据保存到指定缓存
 *@param    pszFile 图片文件所在路径,图片文件支持格式:png、jpg、bmp
 *@retval   pOutput 解码后的图片数据缓存
 *@return
 *@li       NDK_OK    操作成功
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        参数错误
 *@li       \ref NDK_ERR_PATH	  "NDK_ERR_PATH"  图片文件不存在
 *@li		\ref NDK_ERR	"NDK_ERR" 解码失败
*/
int  NDK_ScrImgDecode(char *pszFile,char **pOutput)
{

    if (pszFile==NULL)
		return NDK_ERR_PARA;
    if (access(pszFile,F_OK)<0)
        return NDK_ERR_PATH;
	*pOutput = image_decode(pszFile);
	if(*pOutput == NULL)
		return NDK_ERR;
	else
	{
		image_t *img = *pOutput;
		if(img->user==2012)
		{
			image_destroy(*pOutput);
			*pOutput = NULL;
			return NDK_ERR;
		}
	}
	return NDK_OK;
}

/**
 *@brief    释放指定的图片缓存
 *@details  与\ref NDK_ScrImgDecode "NDK_ScrImgDecode"配对使用
 *@retval   pImg 待释放的图片数据缓存,由\ref NDK_ScrImgDecode "NDK_ScrImgDecode"输出
 *@return
 *@li       NDK_OK    操作成功
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        参数错误
*/
int NDK_ScrImgDestroy(char *pImg)
{
    if (pImg==NULL)
		return NDK_ERR_PARA;
	if(pImg)
		image_destroy(pImg);
	pImg = NULL;
	return NDK_OK;
}

/**
 *@brief    在指定位置开始显示图片缓存
 *@details  如果图片范围超出屏幕范围或通过NDK_ScrSetViewPort设置的用户使用区域时，则该函数操作无效，返回调用失败。
 *@param    unX 图片显示区域左上角横坐标（单位：像素）
 *@param    unY 图片显示区域左上角纵坐标（单位：像素）
 *@param    unW 图片显示区域宽（单位：像素）
 *@param    unH 图片显示区域高（单位：像素）
 *@param    pImg 图片数据缓存，由\ref NDK_ScrImgDecode "NDK_ScrImgDecode"输出
 *@return
 *@li       NDK_OK    操作成功
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        参数错误
*/
int NDK_ScrDispImg(uint unX,uint unY,uint unW,uint unH,char *pImg)
{
    if ((unX> sys.view.w)||(unY> sys.view.h)||(unW> sys.video->sur->width)||(unH> sys.video->sur->height)
        ||((unX+unW)> sys.video->sur->width) || ((unY+unH)>sys.video->sur->height)||(pImg==NULL)) {
        return NDK_ERR_PARA;
    }
	ndk_draw_image(unX,unY,unW,unH,pImg,0,0);
	return NDK_OK;
}

/**
 *@brief    根据输入字符串取出系统当前字体相应的点阵缓冲
 *@param    pszText 字符串
 *@retval   psOutbuf   返回点阵数据为rgb565格式，即1像素对应用16bits(2字节),如16x16汉字点阵缓冲大小至少为16x16*2＝512字节。
 *@param    unBuf_x 将字体点阵缓冲存入psOutbuf中的左上角起始点x坐标，
 *@param    unBuf_y 将字体点阵缓冲存入psOutbuf中的左上角起始点y坐标
 *@param    unBuf_w psOutbuf 缓存像素宽
 *@param    unBuf_h psOutbuf 缓存像素高
 *@param    usColor 字库显示色彩，rgb565格式，0表示黑，0xFFFF表示白

 *@return
 *@li       NDK_OK    操作成功
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        参数错误
 *@li       \ref NDK_ERR_OVERFLOW  "NDK_ERR_OVERFLOW"  psOutbuf大小不足
*/
int NDK_ScrGetFontBuf(const char *pszText,unsigned short* psOutbuf,unsigned int unBuf_x,unsigned int unBuf_y,unsigned int unBuf_w,unsigned int unBuf_h,unsigned short usColor)
{
	extern int get_string_buf(const char *pText,char* outbuf,unsigned int buf_x,unsigned int buf_y,unsigned int buf_w,unsigned int buf_h,color_t color);
    if ((unBuf_x>unBuf_w)||(unBuf_y>unBuf_h)||(unBuf_w==0)||(unBuf_h==0)||(pszText==NULL)||(psOutbuf==NULL)) {
        return NDK_ERR_PARA;
    }
	if(get_string_buf(pszText,(char *)psOutbuf,unBuf_x,unBuf_y,unBuf_w,unBuf_h,usColor)<0)
	{
		return NDK_ERR_OVERFLOW;
	}
	return NDK_OK;

}
/* end of this file */
