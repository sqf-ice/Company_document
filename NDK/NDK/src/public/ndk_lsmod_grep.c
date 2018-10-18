#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
static void replace(char *s, char what, char with)
{
        while (*s) {
                if (what == *s)
                        *s = with;
                ++s;
        }
}
static char* replace_underline(char *s)
{
        replace(s, '-', '_');
        return s;
}
int ndk_ifunloaddrv(char *driver)
{
        char sys_driver_file[64] = {0};

        /* 判断/sys/module目录是否存在相应的驱动目录, 从而判断驱动是否已加载或卸载 */
        snprintf(sys_driver_file, sizeof(sys_driver_file), "/sys/module/%s", driver);
        replace_underline(sys_driver_file);
        if (access(sys_driver_file, F_OK)<0) {
                return -1;
        }
        return 0;
}

