#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "libconfig.h"
#include "config.h"
#include "parsecfg.h"
#include "gui.h"

#define USERBINFS_ROOT              "/appfs"
#define USER_CONFIG_PATH    USERBINFS_ROOT"/etc/"
const char * ndk_conf_filename = USER_CONFIG_PATH"sys.conf";
static config_t ndk_cfg;
static config_setting_t *ndk_root_setting;

/******************** global config variables *********************************/
unsigned int g_language=1;  /* 1 english, 0 chinese */
unsigned int g_beepvolumn=0;    /* 0: big, 1: small, 2: none */
unsigned int g_startclock=0;    /* 0: date & time, 1: time only */
unsigned int g_autorun=1;       /* 0: no autorun, 1: autorun */

unsigned int g_lcd_contrast=20;     /* lcd contrast */
unsigned int g_lcd_coloffset=0;     /* lcd col offset */
unsigned int g_lcd_lrreverse=0;         /* lcd col direction */
unsigned int g_lcd_ubreverse=0;         /* lcd row direction */
unsigned int g_lcd_blswitch=0;

//字体正常显示颜色 by zhengk [2/16/2011]
unsigned int g_fg_normal_R=0;
unsigned int g_fg_normal_G=0;
unsigned int g_fg_normal_B=0;
//字体反显颜色 by zhengk [2/16/2011]
unsigned int g_fg_reverse_R=255;
unsigned int g_fg_reverse_G=255;
unsigned int g_fg_reverse_B=255;
//字体反显背景颜色 by zhengk [2/16/2011]
unsigned int g_reverse_bg_R=0;
unsigned int g_reverse_bg_G=0;
unsigned int g_reverse_bg_B=0;
//背景色 by zhengk [2/16/2011]
unsigned int g_bg_R=255;
unsigned int g_bg_G=255;
unsigned int g_bg_B=255;

unsigned int g_lcd_bldelay=0;
unsigned int g_commun_type=0;           /* comunication type */
unsigned int g_wireless_type=0;         /* wiless modem type: gprs or cdma */
unsigned int g_hip_compensation = 3;         /* hip compensation value *//* 0~7 */
unsigned int g_hip_printer_quality = 0;      /* hip printer quality */
unsigned int g_prt_heatdelay = 3;

char * g_ipaddr="192.168.1.156";        /* ethernet ipaddr */
char * g_netmask="255.255.255.0";       /* ethernet netmask */
char * g_gateway="192.168.1.1";     /* ethernet gateway */
int g_keysensitive = 0;
unsigned int g_sq_limit=8;  //无线信号限制值



static int _config_init(void)
{
    static int init_flag=0;

    if (!init_flag) {
        init_flag=1;
        config_init(&ndk_cfg);
        if (config_read_file(&ndk_cfg, ndk_conf_filename) == CONFIG_FALSE) {
            fprintf(stderr,"%s,%d %s\n",__FUNCTION__,__LINE__,config_error_text(&ndk_cfg)) ;
            config_destroy(&ndk_cfg);
            exit(-1);
        }
    }
    return 0;
}

static int _getconfig(config_setting_t *dev_class, const char * confname, cfgValueType type, void * confvalue)
{
    if (dev_class) {
        switch (type) {
            case CFG_INT:
                if (config_setting_lookup_int(dev_class, confname, (int *)confvalue) == CONFIG_TRUE)
                    return 0;
                break;
            case CFG_LONG:
            case CFG_UINT:
            case CFG_ULONG:
                if (config_setting_lookup_int64(dev_class, confname, (long long*)confvalue) == CONFIG_TRUE)
                    return 0;
                break;
            case CFG_FLOAT:
            case CFG_DOUBLE:
                if (config_setting_lookup_float(dev_class, confname, (double *)confvalue) == CONFIG_TRUE)
                    return 0;
                break;
            case CFG_STRING:
                if (config_setting_lookup_string(dev_class, confname, (const char **)confvalue) == CONFIG_TRUE) {
                    return 0;
                }
                break;
            case CFG_BOOL:
                if (config_setting_lookup_bool(dev_class, confname, (int *)confvalue) == CONFIG_TRUE)
                    return 0;
                break;
            default:
                break;
        }
    }
    return -1;
}

int ndk_getconfig(const char * optname, const char * confname, cfgValueType type, void * confvalue)
{
    int count,i,ret = -1;
    config_setting_t *dev_class;

    _config_init();
    ndk_root_setting = config_root_setting(&ndk_cfg);
    if (ndk_root_setting == NULL) {
        return -1;
    }
    if (optname == NULL) {
        count = config_setting_length(ndk_root_setting);
        //PDEBUG("count = %d\n",count);
        for(i = 0; i < count; i++) {
            dev_class = config_setting_get_elem(ndk_root_setting, i);
            if (dev_class) {
                ret = _getconfig(dev_class, confname, type, confvalue);
                if (ret == 0)
                    break;
            }
        }
    } else {
        dev_class = config_lookup(&ndk_cfg, optname);
        if (dev_class)
            ret = _getconfig(dev_class, confname, type, confvalue);
    }
    if (ret == -1) {
        return -1;
    }
    return 0;
}

static int _setconfig(config_setting_t *dev_class, cfgValueType type, const void * confvalue)
{
    if (dev_class) {
        switch (type) {
            case CFG_INT:
                if (config_setting_set_int(dev_class, *(int*)confvalue) == CONFIG_TRUE) {
                    return 0;
                }
                break;
            case CFG_LONG:
            case CFG_UINT:
            case CFG_ULONG:
                if (config_setting_set_int64(dev_class,*(long long*)confvalue) == CONFIG_TRUE) {
                    return 0;
                }
                break;
            case CFG_FLOAT:
            case CFG_DOUBLE:
                if (config_setting_set_float(dev_class,*(double*)confvalue) == CONFIG_TRUE) {
                    return 0;
                }
                break;
            case CFG_STRING:
                if (config_setting_set_string(dev_class,(char *)confvalue) == CONFIG_TRUE) {
                    return 0;
                }
                break;
            case CFG_BOOL:
                if (config_setting_set_bool(dev_class, *(int*)confvalue) == CONFIG_TRUE) {
                    return 0;
                }
            default:
                break;
        }
    }
    return -1;
}

int ndk_setconfig(const char *optname, const char * confname, cfgValueType type, const void * confvalue)
{
    int count,i,ret = -1;
    config_setting_t *dev_class;

    _config_init();
    ndk_root_setting = config_root_setting(&ndk_cfg);
    if (ndk_root_setting == NULL) {
        return -1;
    }
    if (optname == NULL) {
        count = config_setting_length(ndk_root_setting);

        for(i = 0; i < count; i++) {
            dev_class = config_setting_get_elem(ndk_root_setting, i);
            dev_class = config_setting_get_member(dev_class, confname);
            if (dev_class) {
                ret = _setconfig(dev_class, type, confvalue);
                if (ret == 0)
                    break;
            }
        }
    } else {
        dev_class = config_lookup(&ndk_cfg, optname);
        if(dev_class) {
            dev_class = config_setting_get_member(dev_class, confname);
            if (dev_class)
                ret = _setconfig(dev_class, type, confvalue);
        }

    }

    if (ret == -1) {
        return -1;
    }

    if (! config_write_file(&ndk_cfg, ndk_conf_filename)) {
        return -1;
    }

//  sync();
    return 0;
}

int ndk_removeconfig(const char * optname, const char * confname)
{
    int ret = -1;
    config_setting_t *dev_class;

    _config_init();
    ndk_root_setting = config_root_setting(&ndk_cfg);
    if (ndk_root_setting == NULL) {
        return -1;
    }
    if (optname == NULL || confname ==NULL) {
        return -1;

    } else {
        dev_class = config_lookup(&ndk_cfg, optname);
        if (dev_class)
            ret = config_setting_remove(dev_class, confname);
    }
    if (ret == -1) {
        return -1;
    }
    if (! config_write_file(&ndk_cfg, ndk_conf_filename)) {
        return -1;
    }

//  sync();

    return 0;
}

static int _addconfig(config_setting_t *dev_class, const char * confname ,cfgValueType type, const void * confvalue)
{
    config_setting_t *setting;
    if (dev_class) {
        switch (type) {
            case CFG_INT:
                setting = config_setting_add(dev_class, confname, CONFIG_TYPE_INT);
                if(setting) {
                    if (config_setting_set_int(setting, *(int*)confvalue) == CONFIG_TRUE) {
                        return 0;
                    }
                }
                break;
            case CFG_LONG:
            case CFG_UINT:
            case CFG_ULONG:
                setting = config_setting_add(dev_class, confname, CONFIG_TYPE_INT64);
                if(setting) {
                    if (config_setting_set_int64(setting, *(long long*)confvalue) == CONFIG_TRUE) {
                        return 0;
                    }
                }
                break;
            case CFG_FLOAT:
            case CFG_DOUBLE:
                setting = config_setting_add(dev_class, confname, CONFIG_TYPE_FLOAT);
                if(setting) {
                    if (config_setting_set_float(setting, *(double*)confvalue) == CONFIG_TRUE) {
                        return 0;
                    }
                }
                break;
            case CFG_STRING:
                setting = config_setting_add(dev_class, confname, CONFIG_TYPE_STRING);
                if(setting) {
                    if (config_setting_set_string(setting, (char*)confvalue) == CONFIG_TRUE) {
                        return 0;
                    }
                }
                break;
            case CFG_BOOL:
                setting = config_setting_add(dev_class, confname, CONFIG_TYPE_BOOL);
                if(setting) {
                    if (config_setting_set_bool(setting, *(int*)confvalue) == CONFIG_TRUE) {
                        return 0;
                    }
                }
            default:
                break;
        }
    }
    return -1;
}

int ndk_addconfig(const char *optname, const char * confname, cfgValueType type, const void * confvalue)
{
    int ret = -1;
    config_setting_t *dev_class;

    _config_init();
    ndk_root_setting = config_root_setting(&ndk_cfg);
    if (ndk_root_setting == NULL) {
        return -1;
    }

    if ((optname != NULL) &&(confname!=NULL)) {
        dev_class = config_lookup(&ndk_cfg, optname);
        if(dev_class==NULL)
            dev_class = config_setting_add(ndk_root_setting, optname, CONFIG_TYPE_GROUP);
        if(dev_class)
            ret = _addconfig(dev_class, confname, type,confvalue);
        if(ret==0) {
            if (! config_write_file(&ndk_cfg, ndk_conf_filename)) {
                return -1;
            }

//          sync();
        }

    }
    return ret;
}

int ndk_config_init(void)
{
    static int init_flag=0;
    extern int init_gui(void);
    extern color_t rgb_color(int r, int g, int b);

    if (!init_flag) {
        init_flag=1;
        if (init_gui() < 0) {
            return -1;
        }
        _config_init();
        ndk_getconfig("sys", "language", CFG_INT, &g_language);
        ndk_getconfig("sys", "autorun", CFG_INT, &g_autorun);
        ndk_getconfig("lcd", "lcdcontrast", CFG_INT, &g_lcd_contrast);
        ndk_getconfig("lcd", "lcd_lrresever", CFG_INT, &g_lcd_lrreverse);
        ndk_getconfig("lcd", "lcd_blswitch", CFG_INT, &g_lcd_blswitch);
        ndk_getconfig("lcd", "lcd_bldelay", CFG_INT, &g_lcd_bldelay);
        ndk_getconfig("lcd", "lcd_udresever", CFG_INT, &g_lcd_ubreverse);
        //读取字体颜色 by zhengk [2/16/2011]
        ndk_getconfig("disp", "lcd_normal_fgR", CFG_INT, &g_fg_normal_R);
        ndk_getconfig("disp", "lcd_normal_fgG", CFG_INT, &g_fg_normal_G);
        ndk_getconfig("disp", "lcd_normal_fgB", CFG_INT, &g_fg_normal_B);
        ndk_getconfig("disp", "lcd_reverse_fgR", CFG_INT, &g_fg_reverse_R);
        ndk_getconfig("disp", "lcd_reverse_fgG", CFG_INT, &g_fg_reverse_G);
        ndk_getconfig("disp", "lcd_reverse_fgB", CFG_INT, &g_fg_reverse_B);
        ndk_getconfig("disp", "lcd_reverse_bgR", CFG_INT, &g_reverse_bg_R);
        ndk_getconfig("disp", "lcd_reverse_bgG", CFG_INT, &g_reverse_bg_G);
        ndk_getconfig("disp", "lcd_reverse_bgB", CFG_INT, &g_reverse_bg_B);
        //读取背景颜色 by zhengk [2/16/2011]
        ndk_getconfig("disp", "lcd_bg_R", CFG_INT, &g_bg_R);
        ndk_getconfig("disp", "lcd_bg_G", CFG_INT, &g_bg_G);
        ndk_getconfig("disp", "lcd_bg_B", CFG_INT, &g_bg_B);
        ndk_getconfig("eth", "ipaddr", CFG_STRING, &g_ipaddr);
        ndk_getconfig("eth", "netmask", CFG_STRING, &g_netmask);
        ndk_getconfig("eth", "gateway", CFG_STRING, &g_gateway);

        ndk_getconfig("prn", "prt_heatdelay", CFG_INT, &g_prt_heatdelay);
        ndk_getconfig("prn", "hip_printer_quality", CFG_INT, &g_hip_printer_quality);
        ndk_getconfig("prn", "hip_compensation", CFG_INT, &g_hip_compensation);

        ndk_getconfig("wls", "wirelesstype", CFG_INT, &g_wireless_type);
        ndk_getconfig("wls", "sq_limit", CFG_INT, &g_sq_limit);
        ndk_getconfig("kdb", "keysensitive", CFG_INT, &g_keysensitive);
        ndk_getconfig("kdb", "beepvolumn", CFG_INT, &g_beepvolumn);

        sys.view.x = 0;
        sys.view.y = sys.theme->tit_bar.height; /* status bar height */
        sys.view.w = sys.video->sur->width;
        sys.view.h = sys.video->sur->height - sys.theme->tit_bar.height;
        sys.fg = rgb_color(g_fg_normal_R, g_fg_normal_G, g_fg_normal_B);
        sys.bg = rgb_color(g_bg_R, g_bg_G, g_bg_B);
        sys.fb = rgb_color(g_reverse_bg_R, g_reverse_bg_G, g_reverse_bg_B);
        sys.backimg = NULL;

    }
    return 0;
}

void ndk_config_exit()
{
    extern void uninit_gui(void);
    uninit_gui();
    return;
}


////////////////////////////////////////////////////////////////////////////////
//配置文件可设接口
///////////////////////////////////////////////////////////////////////////////
int ndk_initconfig_customize(char *path,config_t *cfg_file)
{

    if(access(path, F_OK) < 0)
        return -1;
    config_init(cfg_file);
    if (config_read_file(cfg_file, path) == CONFIG_FALSE) {
        fprintf(stderr,"%s,%d %s\n",__FUNCTION__,__LINE__,config_error_text(cfg_file)) ;
        config_destroy(cfg_file);
        return -1;
    }
    return 0;
}

int ndk_getconfig_customize(config_t * cfg_file,const char * optname, const char * confname, cfgValueType type, void * confvalue)
{
    int count,i,ret = -1;
    config_setting_t *dev_class;

    ndk_root_setting = config_root_setting(cfg_file);
    if (ndk_root_setting == NULL) {
        return -1;
    }
    if (optname == NULL) {
        count = config_setting_length(ndk_root_setting);
        //PDEBUG("count = %d\n",count);
        for(i = 0; i < count; i++) {
            dev_class = config_setting_get_elem(ndk_root_setting, i);
            if (dev_class) {
                ret = _getconfig(dev_class, confname, type, confvalue);
                if (ret == 0)
                    break;
            }
        }
    } else {
        dev_class = config_lookup(cfg_file, optname);
        if (dev_class)
            ret = _getconfig(dev_class, confname, type, confvalue);
    }
    if (ret == -1) {
        return -1;
    }
    return 0;
}


int ndk_destoryconfig_customize(config_t *cfg_file)
{
    config_destroy(cfg_file);
    return 0;
}
