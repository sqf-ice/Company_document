#include "NDK_debug.h"

extern int AppLoad(const unsigned char *pszFilename, int nRebootFlag);
extern int AppGetReboot(int *nRebootFlag);
extern void Set_NDK_flag();

/**
 *@brief    װ��Ӧ��
 *@param    pszAppName      NLD�ļ�����
 *@param    nRebootFlag     ������־������ͳPOS�ն��⣬�����ն˸ò���������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����NDK_ERRCODE     ����ʧ��
*/
NEXPORT int NDK_AppLoad(const unsigned char *pszFilename, int nRebootFlag)
{
    Set_NDK_flag();
    return AppLoad(pszFilename,nRebootFlag);
}
NEXPORT int NDK_AppGetReboot(int *nRebootFlag)
{
    return AppGetReboot(nRebootFlag);
}

