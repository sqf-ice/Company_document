#include "NDK_debug.h"

extern int AppLoad(const unsigned char *pszFilename, int nRebootFlag);
extern int AppGetReboot(int *nRebootFlag);
extern void Set_NDK_flag();

/**
 *@brief    装载应用
 *@param    pszAppName      NLD文件名称
 *@param    nRebootFlag     重启标志，除传统POS终端外，其他终端该参数无意义
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它NDK_ERRCODE     操作失败
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

