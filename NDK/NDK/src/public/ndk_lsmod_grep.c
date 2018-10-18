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

        /* �ж�/sys/moduleĿ¼�Ƿ������Ӧ������Ŀ¼, �Ӷ��ж������Ƿ��Ѽ��ػ�ж�� */
        snprintf(sys_driver_file, sizeof(sys_driver_file), "/sys/module/%s", driver);
        replace_underline(sys_driver_file);
        if (access(sys_driver_file, F_OK)<0) {
                return -1;
        }
        return 0;
}

