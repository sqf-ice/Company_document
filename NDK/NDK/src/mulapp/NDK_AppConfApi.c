#include "NDK.h"
#include "NDK_debug.h"
extern  int AppGetInfoFrom(const uchar *pszAppName, int nPos, ST_APPINFO *pstAppInfo, int nSizeofInfo);
extern  int AppDel(const char *pszAppName);

/**
 *@brief    获取应用信息
 *@param    pszAppName  应用名称, 可以输入为NULL。
 *@param    nPos        偏移，该偏移位置为在系统中信息表偏移，和下载顺序相关。
 *@param    nSizeofInfo 输出应用信息结构长度。
 *@retval   pstAppInfo  获取到的应用信息
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR     操作失败(应用配置文件操作失败)
 *@li   NDK_ERR_APP_NOT_EXIST       应用项不存在
*/
int NDK_AppGetInfo(const uchar *pszAppName, int nPos, ST_APPINFO *pstAppInfo, int nSizeofInfo)
{
    return AppGetInfoFrom(pszAppName,nPos, pstAppInfo, nSizeofInfo);
}


/**
 *@brief    删除应用信息
 *@param    pszAppName  应用名称
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszAppName为NULL)
 *@li   NDK_ERR         操作失败(读写应用配置文件失败)
 *@li   NDK_ERR_APP_NOT_EXIST   应用项不存在
*/
int NDK_AppDel(const char *pszAppName)
{
	Set_NDK_flag();
    return AppDel(pszAppName);
}

