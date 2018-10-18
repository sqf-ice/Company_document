#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

#include "mdm_drv.h"

void TRACE(const char *format, ...)
{
    char time[32] = { 0 };

    // print time
    mdm_get_datetime(time);
    fprintf(stderr, "[%s]: ", time);
    // print message
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
