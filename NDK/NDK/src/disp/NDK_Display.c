/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�    ��Ʒ������
* ��    �ڣ�    2012-08-17
* ��    ����    V1.00
* ����޸��ˣ�
* ����޸����ڣ�
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

static int nOldTextAttr = TEXT_ATTRIBUTE_NORMAL;    /**<Ĭ����ʾģʽΪ������ʾ*/
static int auto_update = 1;             /**<Ĭ��Ϊ�Զ�ˢ��*/
static char ndk_dispver[16]="V1.00";    /**<ģ��汾��*/
static color_t nor_fg_color;            /**<��ǰ������ʾ����RGB��ֵ*/
static color_t rev_fg_color;            /**<��ǰ������ʾ����RGB��ֵ*/
static int ndk_dispinit;                /**<ģ���ʼ����־*/
uint CursorPosX = 0, CursorPosY = 0;    /**<��ǰ��������*/
uint guserfonttype =DISPFONT_CUSTOM;                    /**<��ǰʹ����������,ֵ��ΧEM_DISPFONT*/

pthread_mutex_t statusbar_mutex=PTHREAD_MUTEX_INITIALIZER;
#define INIT_TIMEOUT 5000

/**< ϵͳ����������ʾ��ɫ by zhengk [2/16/2011] */
extern unsigned int g_fg_normal_R;
extern unsigned int g_fg_normal_G;
extern unsigned int g_fg_normal_B;

/**< ϵͳ���巴����ɫ by zhengk [2/16/2011] */
extern unsigned int g_fg_reverse_R;
extern unsigned int g_fg_reverse_G;
extern unsigned int g_fg_reverse_B;

ndk_statusbar_t gndk_statusbar;



/**< �������ݽṹ*/
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
 *@brief    ��ʾ��ʽΪimage_t��ͼƬ��֧��ƫ�ơ�
 *@param    x       ��ʾ������ʼ����ĺ����꣨��λ�����أ���
 *@param    y       ��ʾ������ʼ����ĺ����꣨��λ�����أ���
 *@param    w       ͼƬ��ȣ���λ�����أ���
 *@param    h       ͼƬ�߶ȣ���λ�����أ���
 *@param    img     image_t��ʽ��ͼƬ���ݡ�
 *@param    xoff        ��ʾ��ʼλ����ͼƬ�е�xƫ��
 *@param    yoff        ��ʾ��ʼλ����ͼƬ�е�yƫ��
 *@return   ��
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
 *@brief    Bitmap��ʽͼƬת��Ϊimage_t��ʽ��
 *@param    input       Bitmap��ʽͼƬ��
 *@param    output      image_t��ʽͼƬ��
 *@param    inlen       input���ݳ��ȡ�
 *@return   ��
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

    /**<����Ϊ�������� by zhengk [2/17/2011]*/
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
    /**����cColorֵ���� by zhengk [2/23/2011]*/
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
 *@brief    �û���������ʼ����
 *@details  �ڳ���������ã��ú����ɹ����ú���ʾģ���API��������ʹ�á�
 *@return
 *@li       NDK_OK                              �����ɹ�
 *@li           NDK_ERR_INIT_CONFIG                           ��ʼ������ʧ��
 *@li           NDK_ERR_CREAT_WIDGET                       �����������
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
 *@brief    ��ȡ��ʾģ��汾��
 *@retval   pszVer  ����ģ��汾,�����pnVerӦ�ò�С��16�ֽڡ�
 *@return
 *@li       NDK_OK                  �����ɹ�
 *@li           NDK_ERR_PARA          �����Ƿ�
*/
NEXPORT int NDK_ScrGetVer(char* pszVer)
{
    if(pszVer==NULL)
        return NDK_ERR_PARA;
    strncpy(pszVer,ndk_dispver,sizeof(ndk_dispver));
    return NDK_OK;
}

/**
 *@brief    ������ʾģʽ������ȡ֮ǰ����ʾģʽ��
 *@param    emNewattr   Ҫ���õ�����ʾģʽ��
 *@retval   pemOldattr  ���֮ǰ����ʾģʽ��peOldattrΪNULLʱ�����ء�
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li           NDK_ERR_PARA          �����Ƿ�
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
    //��������
    if (emNewattr & TEXT_ATTRIBUTE_NORMAL) {
        if (!(nOldTextAttr & TEXT_ATTRIBUTE_NORMAL)) {
            sys.fg = nor_fg_color;
        }
    }
    //������
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
 *@brief    ���浱ǰ��Ļ
 *@details  ������ʾ���ݡ����λ�ü���ʾģʽ���ñ������ɵ���NDK_ScrPop���ٻָ���ʾ��
            NDK_ScrPush��NDK_SrcPop�ɶ�ʹ�ã�����Ƕ�ס�
 *@return
 *@li   NDK_OK              �����ɹ�
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
 *@brief    ���ٻָ�����NDK_ScrPush�������ʾ״̬��������ʾ���ݡ����λ�ü��ı���ʾ���ԡ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR         ����ʧ�ܣ�δ������ʾ״̬��
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
 *@brief    ���ñ���ͼƬ��ͼƬ�ļ�֧�ָ�ʽ��鿴�����͵�ͼƬ��ʽ���ơ�
 *@param    pszfilepath     ͼƬ�ļ�·��+�ļ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PATH                �ļ�·���Ƿ�
 *@li       NDK_ERR_DECODE_IMAGE           ͼ�����ʧ��
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
 *@brief    ȡ������ͼƬ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_ScrClrbgPic(void)
{
    clr_bgpic();
    return NDK_OK;
}

/**
 *@brief    �������ѹ���Ƶ���������(0,0)��ͬʱ����Ļ��ʾģʽ����ΪTEXT_ATTRIBUTE_NORMAL��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
 *@brief    ���������(��λ������)���ѹ���Ƶ�(0,unStartY)����
            ������������ӿڱ߽�ʱ�����ӿڱ߽�Ϊ׼��
 *@param    unStartY        ��ʼ�кţ������꣬��λ�����أ�����0��ʼ����
 *@param    unEndY          �����кţ������꣬��λ�����أ�����0��ʼ����
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief    Һ����ʾ���λ���Ƶ���������(unX,unY)����
            �����������Ƿ������걣��λ�ò���,���ش�����Ϣ��
 *@param    unX     �����꣨��λ�����أ�
 *@param    unY     �����꣨��λ�����أ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief    ��ȡ��ǰ���ع��λ�õĺ�����������ꡣ
 *@retval   punX ���ع��λ�õĺ����꣨��λ�����أ���
 *@retval   punY ���ع��λ�õ������꣨��λ�����أ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
 *@brief    ������ʾ����ߴ硣
 *@details  δ�����������ʾ����Ϊʵ����Ļ�ߴ磬ͨ���ýӿ�������ʾ���������API����ʾ����ֻ�ڸ���������Ч��\n
            ������(10,10,100,100)ΪӦ����ʾ������Ӧ�ó�����ʹ�õ���������(0,0)ʵ����
            ����Ļ��������(10,10),��������Ҳֻ�����������(10,10,100,100)��Χ�ڵ���ʾ���ݡ�
 *@param    unX     ��ʾ������ʼ����ĺ����꣨��λ�����أ���
 *@param    unY     ��ʾ������ʼ����ĺ����꣨��λ�����أ���
 *@param    unWidth ��ʾ�����ȣ���λ�����أ���
 *@param    unHeight    ��ʾ����߶ȣ���λ�����أ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief    ��ȡ��ǰ��ʾ����ߴ硣
 *@retval   punX        ��ʾ������ʼ����ĺ����꣨��λ�����أ���
 *@retval   punY        ��ʾ������ʼ����������꣨��λ�����أ���
 *@retval   punWidth    ��ʾ����߶ȣ���λ�����أ���
 *@retval   punHeight   ��ʾ����߶ȣ���λ�����أ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
 *@brief    ����ʾ������ʾBitmapͼƬ��
 *@details  bitmap��ʽ��1byte��Ӧ8�����ص�,0��ʾ�׵㣬1��ʾ�ڵ㣬��ʾ���ݺ������У�����ͼ��ʾ:\n
-----------------D7~~D0--------------D7~~D0------------------\n
Byte 1: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte2 \n
Byte 3: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte4 \n
Byte 5: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte6 \n
Byte 7: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte8 \n
Byte 9: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte10    \n
Byte11: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte12    \n
Byte13: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte14    \n
Byte15: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte16    \n
---------------------------------------------------------------\n
    �����ʾͼƬ��Χ������Ļ��Χ��ͨ��NDK_ScrSetViewPort���õ��û�ʹ������ʱ����ú���������Ч�����ص���ʧ�ܡ�
 *@param    unX         ͼƬ����ʾ��������ϽǺ����꣨��λ�����أ�
 *@param    unY         ͼƬ����ʾ��������Ͻ������꣨��λ�����أ�
 *@param    unWidth     ͼƬ��ȣ���λ�����أ�
 *@param    unHeight    ͼƬ�߶ȣ���λ�����أ�
 *@param    psBuf       BitmapͼƬ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief    ����ʾ��������������(unStartX,unStartY)��(unEndX,unEndY)��ֱ�ߣ�unColor��ʾ���ߵ�RGBɫ��ֵ��
 *@param    unStartX    ֱ�ߵ��������꣨��λ�����أ�
 *@param    unStartY    ֱ�ߵ���������꣨��λ�����أ�
 *@param    unEndX      ֱ�ߵ��յ�����꣨��λ�����أ�
 *@param    unEndY      ֱ�ߵ��յ������꣨��λ�����أ�
 *@param    unColor         ��ɫ��ֵ <0-0xFFFF>
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_ScrLine(uint unStartX, uint unStartY, uint unEndX, uint unEndY, color_t unColor)
{
    return ndk_line(unStartX,unStartY,unEndX,unEndY,unColor,0);
}

/**
 *@brief    ����ʾ����һ�����Ρ�
 *@details  ������α߽糬����Ļ��Χʱ����ú���������Ч�����ص���ʧ�ܡ�
 *@param    unX         ���ε��������꣨��λ�����أ�
 *@param    unY         ���ε���������꣨��λ�����أ�
 *@param    unWidth         ���εĿ���λ�����أ�
 *@param    unHeight        ���εĸߣ���λ�����أ�
 *@param    emFill_pattern  0Ϊ�����ģʽ��1Ϊ���ģʽ
 *@param    unColor ��ɫ��ֵ <0-0xFFFF>
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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

    //����Ϊ�������� by zhengk [2/17/2011]
    unX+=sys.view.x;
    unY+=sys.view.y;

    //������ν�������������
    x_end = unX + unWidth-1;
    y_end = unY + unHeight-1;

    //����������Ч by zhengk [2/19/2011]
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
 *@brief    ����ʾ������ʾָ����ͼƬ��ͼƬ�ļ�֧�ָ�ʽ��鿴�����͵�ͼƬ��ʽ���ơ�
 *@details  �����ʾͼƬ��Χ������Ļ��Χ��ͨ��NDK_ScrSetViewPort���õ��û�ʹ������ʱ����ú���������Ч�����ص���ʧ�ܡ�
 *@param    unX         ͼƬ��ʾ�����ϽǺ����꣨��λ�����أ�
 *@param    unY         ͼƬ��ʾ�����Ͻ������꣨��λ�����أ�
 *@param    unWidth     ͼƬ�Ŀ���λ�����أ�
 *@param    unHeight    ͼƬ��ʾ�ĸߣ���λ�����أ�
 *@param    pszPic  ͼƬ�ļ����ڵ�·����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR_DECODE_IMAGE        ͼ�����ʧ��
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
 *@brief    ȡ��ʾ������ָ��������������ɫ��ֵ��
 *@param    unX         �����꣨��λ�����أ�
 *@param    unY         �����꣨��λ�����أ�
 *@retval   punColor    ���ص���ɫֵ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
*/
NEXPORT int NDK_ScrGetPixel(uint unX, uint unY, color_t *punColor)
{
    unsigned short * addr = sys.video->sur->vbuf;

    if ((unX >= sys.view.w) || (unY >= sys.view.h)||(punColor==NULL))
        return NDK_ERR_PARA;

    //����Ϊ�������� by zhengk [2/22/2011]
    unX+=sys.view.x;
    unY+=sys.view.y;

    *punColor = (int)(*(addr +unX+unY*(sys.video->sur->width)));//����RGB��ֵ
    return NDK_OK;
}

/**
 *@brief    ����ʾ������ָ���������껭�㡣
 *@param    unX         �����꣨��λ�����أ�
 *@param    unY     �����꣨��λ�����أ�
 *@param    unColor     ��ɫ��ֵ <0-0xFFFF>
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
*/
NEXPORT int NDK_ScrPutPixel(uint unX, uint unY, color_t unColor)
{
    color_t pixel;
    if ((unX >= sys.view.w) || (unY >= sys.view.h))
        return NDK_ERR_PARA;

    //����Ϊ�������� by zhengk [2/17/2011]
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
 *@brief    ���Դ����û����õ���ʾ�����ڵ�����ˢ�µ�Һ��������ʾ��
 *@details  ϵͳȱʡΪ�Զ�ˢ�¡�Ϊ��������������ͨ��NDK_ScrAutoUpdate����Ϊ���Զ�ˢ�£���NDK_ScrRefresh
            ���ú�ϵͳ�Ž��Դ��е�����ˢ�µ�Һ�����ϡ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_ScrRefresh(void)
{
    _ndk_refresh(&(sys.view));
    //waitfor_lcdsync();
    return NDK_OK;
}

/**
 *@brief    ���Դ���ȫ������ˢ�µ�LCD����ʾ��
 *@details  �ýӿ���NDK_ScrRefresh�������ڲ���������ʾ����Ĵ�С����ͨ��NDK_ScrSetViewPort���õ���ʾ����Ϊȫ��ʱ��
            ��NDK_ScrRefresh��ýӿ�Ч��һ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
 *@brief    �����Ƿ��Զ�ˢ�¡�
 *@param    nNewauto
                    ��0:�Զ�ˢ��
                    0:���Զ�ˢ�£�ֻ�е���NDK_ScrRefresh����ʾ�Դ��е����ݡ�
 *@retval   pnOldauto   ��������֮ǰ���Զ�ˢ��״̬��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_ScrAutoUpdate(int nNewauto, int *pnOldauto)//������Ϊȡ��ֵ???��Ϊ����������Ҫ�õ�
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
 *@brief    ��ȡҺ�����ߴ硣
 *@retval       punWidth    ����LCD��ȣ���λ�����أ���
 *@retval       punHeight   ����LCD�߶ȣ���λ�����أ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
*/
NEXPORT int NDK_ScrGetLcdSize(uint *punWidth,uint *punHeight)
{
    if((punWidth==NULL)||(punHeight==NULL))
        return NDK_ERR_PARA;
    *punWidth = sys.video->sur->width;
    *punHeight = sys.video->sur->height;
    //���⴦��,���ӿ��Ϊ128 by zhengk [8/31/2012]
    if((*punWidth==132)&&(*punHeight==160))
        *punWidth=128;

    return NDK_OK;
}


/**
 *@brief    ��ȡҺ����ɫ�
 *@detail   �������ж�Һ�����ǵ�ɫ�������
 *@retval       puncd   ����Һ����ɫ�1----�ڰ���ɫ,
                                        16----16λɫ������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
*/
NEXPORT int NDK_ScrGetColorDepth(uint *puncd)
{
    if(puncd==NULL)
        return NDK_ERR_PARA;
    *puncd = sys.video->bits_per_pixel;
    return NDK_OK;
}

/**
 *@brief    ���ر��������
 *@param    emBL    0 �C �ر�Һ������
                    1 �C��Һ������
                    2 �CҺ�����ⳣ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief        ������Ļ�Աȶȡ�
 *@param        unContrast  �Աȶȼ���Ϊ0~63��0������63���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR_OPEN_DEV        ���豸�ļ�����
 *@li   NDK_ERR_IOCTL       �������ô���
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
 *@brief        �����û��Զ������塣
 *@details      ���óɹ���ͨ�� NDK_ScrSetFontType����DISPFONT_USER��ʹ���Զ�������塣\n
                ע��:Ϊ��ϵͳͳһ����ʱ��ʾ�Ű棬Ӣ���ֿ�����Ϊ�����ֿ���һ�룬�߶��뺺���ֿ���ͬ��
 *@param        pcCpath �����ֿ⡣
 *@param        pcApath Ӣ���ֿ⡣
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR     ����ʧ��(�����û��Զ�������ʧ��)
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
 *@brief    ����ʹ�õ���ʾ���塣
 *@details  ϵͳĬ��֧��2�ֳߴ�����С������ɹ��л�,��ͬ�����趨��ϵͳĬ������ߴ粻һ����ͬ\n\n
            ����֧��:����16x16 ASCII:8x16 (DISPFONT_CUSTOM)\n
                     ����24x24 ASCII:12x24 (DISPFONT_EXTRA)\n
            �ڰ���֧��:����12x12 ASCII:6x12 (DISPFONT_CUSTOM)\n
                        ����16x16 ASCII:8x16 (DISPFONT_EXTRA)\n\n
            ������ʾ����Ժ�����ʾ���������ã���֮ǰ��ˢ����ʾ��������Ч
 *@param    emType  ѡ������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief    ��ȡ��ǰʹ�õ���ʾ�������͡�
 *@details  ϵͳĬ��֧��2�ֳߴ�����С������ɹ��л�,��ͬ�����趨��ϵͳĬ������ߴ粻һ����ͬ\n\n
            ����֧��:����16x16 ASCII:8x16 (DISPFONT_CUSTOM)\n
                     ����24x24 ASCII:12x24 (DISPFONT_EXTRA)\n
            �ڰ���֧��:����12x12 ASCII:6x12 (DISPFONT_CUSTOM)\n
                        ����16x16 ASCII:8x16 (DISPFONT_EXTRA)\n\n
            ������ʾ����Ժ�����ʾ���������ã���֮ǰ��ˢ����ʾ��������Ч
 *@param    pemType  ���ص�ǰ��������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
*/
NEXPORT int NDK_ScrGetFontType(EM_DISPFONT *pemType)
{
    if(pemType==NULL)
        return NDK_ERR_PARA;
    *pemType = guserfonttype;
    return NDK_OK;
}
/**
 *@brief    ��ȡ��ǰϵͳʹ�õĺ�����ʾ�����͸ߡ�
 *@details  ϵͳʹ�õ�ASCII�ַ���������ȹ̶�Ϊ���ֵ�һ��
 *@retval   punWidth    ���ص�ǰϵͳ��ǰ��ʾ����ĺ��ֵ����
 *@retval   punHeight   ���ص�ǰϵͳ��ǰ��ʾ����ĺ��ֵ���ߡ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
 *@brief    ����������ɫ���������ԡ����ԡ����Ա���ɫ��
 *@param    unColor ��ɫ��ֵ
 *@param    emType  ѡ�����ö���

 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief    �����м����ּ�ࡣ
 *@details  wspace���ڵ�ǰʹ�õ�Ӣ���������ؿ�ȵ�2��ʱ���ּ����ΪӢ���������ؿ�ȵ�2��
            hspace���ڵ�ǰʹ�õ�Ӣ���������ظ߶�ʱ���м����ΪӢ���������ظ߶�
 *@param    unWspace    �ּ�ࣨ��λ�����أ�
 *@param    unHpace �м�ࣨ��λ�����أ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
                    if(sys.font->fonth<sys.font->cfontw) { //С��������²���ʾ����
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
    if(CursorPosX>=sys.view.w) { //����ʱ�������� by zhengk [2/22/2011]
        CursorPosX = 0;
        CursorPosY +=(sys.font->fonth+sys.hspace);
    }
    if(CursorPosY>=sys.view.h) {
        CursorPosX = 0;
        CursorPosY =sys.view.h-sys.font->fonth;// ��������ʱ���������������һ��by zhengk [2/22/2011]
    }
    if (auto_update) {
        _ndk_refresh(&(sys.view));
    }

    return NDK_OK;
}

/**
 *@brief    ��ʾ�ַ�����
 *@param    unX ��ʾ�ַ���λ�õĺ�����
 *@param    unY ��ʾ�ַ���λ�õ�������
 *@param    pszS    Ҫ��ʾ���ַ���ָ��
 *@param    unMode  ������ʾASCII�ַ�ʱ�ߴ�
                    0��ϵͳ��ǰʹ�õ���ʾ������ASCII����ߴ�
                    1��ʹ��С��Ӣ�����壬�������ڰ������ֵ��ַ���
                        �ڰ�����8x8�ߴ�Ӣ������
                        ������8x16�ߴ�Ӣ������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief    ��Ļ��ʾ��ʽ�������ʹ�÷���ͬprintf
 *@param    format  ��������ĸ�ʽ
  *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
 *@brief    ������Ļ�ײ���ʾ״̬��
 *@details  δ���øýӿ�ʱ��ϵͳĬ���ǹر�״̬����ʾ��ͨ���ýӿڹرջ��״̬��ʱ���ӿڻᷢ���仯������û�ͨ��NDK_ScrSetViewPort�Զ�����ӿڣ��ڲ���״̬�����غ����ٴ��Զ����ӿڴ�С
 *@param    emFlag  ����״̬����ʾ��رտ���,֧�ֲ���,�� :STATUSBAR_DISP_BATTERY|STATUSBAR_DISP_WLSIGNAL
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
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
        //ndk_getguifocus();//������ֻΪ�ȴ�nlgui��Ӧ��Ϊ�����������ʾ������nlguiδ�յ�����ǰ�������ڴ��е�����ˢ��
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
 *@brief    ����BDF����
 *@details  �û��Զ���BDF��ʽ�����壬֧�ּ��ض��BDF�����ļ�
 *@param    unFontID �Զ�������ID,��0,1,2��,NDK_ScrDispBDFTextʹ�á����uFontID��֮ǰ���õ�BDF������ͬ�����滻֮ǰBDF����
 *@param    pszFile BDF�ļ�·��+�ļ���
 *@param    punWidth ��ȡ��BDF����������ؿ�
 *@param    punHeight ��ȡ��BDF����������ظ�
 *@return
 *@li   NDK_OK  �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR     BDF�����ļ���ʽ�޷�ʶ��
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
 *@brief    ʹ���Ѽ��ص�BDF������ʾ����
 *@details
 *@param    uFontID NDK_ScrLoadBDF���������ID��
 *@param    unX ��ʾ�ַ���λ�õĺ�����
 *@param    unY ��ʾ�ַ���λ�õ�������
 *@param    pszText ��ʾ���ַ�������ֵ��BDF�ļ����ַ����Ӧ�ı���ֵ�������UNICODE��������BDF�ļ�����
 *@return
 *@li   NDK_OK  �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR     BDF�����ļ���ʽ�޷�ʶ��
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
 *@brief    ���д���У׼����
 *@return
 *@li       NDK_OK    У׼�ɹ�
 *@li       \ref NDK_ERR_NO_DEVICES     "NDK_ERR_NO_DEVICES"        POS�޴����豸
 *@li       \ref NDK_ERR_QUIT           "NDK_ERR_QUIT"              �û��˳�
 *@li       \ref NDK_ERR                "NDK_ERR"                   ����ʧ��
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
			case 3://���봥������Ĳ����Ƿ�
			case 1://��������ִ��ʧ��
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
 *@brief    ͼƬ�������ݱ��浽ָ������
 *@param    pszFile ͼƬ�ļ�����·��,ͼƬ�ļ�֧�ָ�ʽ:png��jpg��bmp
 *@retval   pOutput ������ͼƬ���ݻ���
 *@return
 *@li       NDK_OK    �����ɹ�
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        ��������
 *@li       \ref NDK_ERR_PATH	  "NDK_ERR_PATH"  ͼƬ�ļ�������
 *@li		\ref NDK_ERR	"NDK_ERR" ����ʧ��
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
 *@brief    �ͷ�ָ����ͼƬ����
 *@details  ��\ref NDK_ScrImgDecode "NDK_ScrImgDecode"���ʹ��
 *@retval   pImg ���ͷŵ�ͼƬ���ݻ���,��\ref NDK_ScrImgDecode "NDK_ScrImgDecode"���
 *@return
 *@li       NDK_OK    �����ɹ�
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        ��������
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
 *@brief    ��ָ��λ�ÿ�ʼ��ʾͼƬ����
 *@details  ���ͼƬ��Χ������Ļ��Χ��ͨ��NDK_ScrSetViewPort���õ��û�ʹ������ʱ����ú���������Ч�����ص���ʧ�ܡ�
 *@param    unX ͼƬ��ʾ�������ϽǺ����꣨��λ�����أ�
 *@param    unY ͼƬ��ʾ�������Ͻ������꣨��λ�����أ�
 *@param    unW ͼƬ��ʾ�������λ�����أ�
 *@param    unH ͼƬ��ʾ����ߣ���λ�����أ�
 *@param    pImg ͼƬ���ݻ��棬��\ref NDK_ScrImgDecode "NDK_ScrImgDecode"���
 *@return
 *@li       NDK_OK    �����ɹ�
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        ��������
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
 *@brief    ���������ַ���ȡ��ϵͳ��ǰ������Ӧ�ĵ��󻺳�
 *@param    pszText �ַ���
 *@retval   psOutbuf   ���ص�������Ϊrgb565��ʽ����1���ض�Ӧ��16bits(2�ֽ�),��16x16���ֵ��󻺳��С����Ϊ16x16*2��512�ֽڡ�
 *@param    unBuf_x ��������󻺳����psOutbuf�е����Ͻ���ʼ��x���꣬
 *@param    unBuf_y ��������󻺳����psOutbuf�е����Ͻ���ʼ��y����
 *@param    unBuf_w psOutbuf �������ؿ�
 *@param    unBuf_h psOutbuf �������ظ�
 *@param    usColor �ֿ���ʾɫ�ʣ�rgb565��ʽ��0��ʾ�ڣ�0xFFFF��ʾ��

 *@return
 *@li       NDK_OK    �����ɹ�
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        ��������
 *@li       \ref NDK_ERR_OVERFLOW  "NDK_ERR_OVERFLOW"  psOutbuf��С����
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
