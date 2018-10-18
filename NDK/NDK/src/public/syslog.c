#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>

#include "config.h"
#include "NDK_debug.h"

static int syslog_enable = -1;
static int syslog_level = -1;

void NDK_LOG_DEBUG(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;

    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL7|LOG_DEBUG, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_LOG_WARNING(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;

    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL6|LOG_WARNING, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_LOG_ERR(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;

    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL6|LOG_ERR, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_LOG_CRIT(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;

    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL6|LOG_CRIT, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_LOG_ALERT(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;

    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL6|LOG_ALERT, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_LOG_EMERG(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;

    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL6|LOG_EMERG, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_LOG_INFO(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }
    if(syslog_level==-1){
        if(ndk_getconfig("syslog","level",CFG_INT,&syslog_level)!=0)
            syslog_level=0;
    }

    if(syslog_enable!=1)
        return;
    if(syslog_level==1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;


    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL7|LOG_INFO, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_LOG_NOTICE(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;


    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL7|LOG_NOTICE, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_COMM_LOG_DEBUG(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;

    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL5|LOG_DEBUG, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_COMM_LOG_INFO(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }
    if(syslog_level==-1){
        if(ndk_getconfig("syslog","level",CFG_INT,&syslog_level)!=0)
            syslog_level=0;
    }

    if(syslog_enable!=1)
        return;
    if(syslog_level==1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;


    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL5|LOG_INFO, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}
void NDK_COMM_LOG_NOTICE(char *module,const char *format, ...)
{
    if(syslog_enable==-1){
        if(ndk_getconfig("syslog","enable",CFG_INT,&syslog_enable)!=0)
            syslog_enable=0;
    }

    if(syslog_enable!=1)
        return;

    va_list ap;
    char tmp_str[1024];

    if (format == NULL)
        return;


    va_start(ap, format);
    vsprintf(tmp_str, format, ap);
    va_end(ap);
    syslog(LOG_LOCAL5|LOG_NOTICE, "[%s]<pid:%d>%s",module,getpid(),tmp_str);
    closelog();
    return;
}