#include "NDK.h"
#include "NDK_debug.h"
extern  int AppGetInfoFrom(const uchar *pszAppName, int nPos, ST_APPINFO *pstAppInfo, int nSizeofInfo);
extern  int AppDel(const char *pszAppName);

/**
 *@brief    ��ȡӦ����Ϣ
 *@param    pszAppName  Ӧ������, ��������ΪNULL��
 *@param    nPos        ƫ�ƣ���ƫ��λ��Ϊ��ϵͳ����Ϣ��ƫ�ƣ�������˳����ء�
 *@param    nSizeofInfo ���Ӧ����Ϣ�ṹ���ȡ�
 *@retval   pstAppInfo  ��ȡ����Ӧ����Ϣ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li   NDK_ERR     ����ʧ��(Ӧ�������ļ�����ʧ��)
 *@li   NDK_ERR_APP_NOT_EXIST       Ӧ�������
*/
int NDK_AppGetInfo(const uchar *pszAppName, int nPos, ST_APPINFO *pstAppInfo, int nSizeofInfo)
{
    return AppGetInfoFrom(pszAppName,nPos, pstAppInfo, nSizeofInfo);
}


/**
 *@brief    ɾ��Ӧ����Ϣ
 *@param    pszAppName  Ӧ������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszAppNameΪNULL)
 *@li   NDK_ERR         ����ʧ��(��дӦ�������ļ�ʧ��)
 *@li   NDK_ERR_APP_NOT_EXIST   Ӧ�������
*/
int NDK_AppDel(const char *pszAppName)
{
	Set_NDK_flag();
    return AppDel(pszAppName);
}

