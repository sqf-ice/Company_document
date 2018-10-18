/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：    产品开发部
* 日    期：    2012-08-17
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
*/

#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_App.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdarg.h>
#include <assert.h>
#include <dirent.h>
#include <signal.h>

#define EVENT_FILE  "/tmp/.ndk_event"

extern void ndk_getguifocus(void);
extern int AppGetInfo(int nAppPostion,char *pKeyWords,char*pValue);
extern int ndk_file_cp(char *sourceFile,char *desFile);

CallbackMock App_CallbackMock = NULL;

int App_GetEventFile(char *pEventFile)
{
    if(pEventFile == NULL)
        return NDK_ERR;
    strcpy(pEventFile,EVENT_FILE);
    return NDK_OK;
}
int App_GetCallBack(CallbackMock *NDK_EventMain)
{
    if(App_CallbackMock == NULL)
        return NDK_ERR_PARA;
    *NDK_EventMain = App_CallbackMock;
    return NDK_OK;
}
void App_EmptyFile(char *path)
{
    FILE *fp;
    fp = fopen(path,"w+");
    fclose(fp);
    remove(path);
    return ;
}
int NDK_AppSetEventCallBack(CallbackMock NDK_EventMain)
{
    if(NDK_EventMain == NULL)
        return NDK_ERR_PARA;
    App_CallbackMock = NDK_EventMain;
    return NDK_OK;
}
/** @addtogroup 应用管理
* @{
*/

/**
 *@brief    运行应用程序
 *@param    pszAppName  应用名称。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszAppName为NULL)
 *@li   NDK_ERR_APP_MAX_CHILD   子应用运行数超过最大运行数目
 *@li   NDK_ERR         操作失败(读写应用配置文件失败)
 *@li   NDK_ERR_APP_NOT_EXIST   应用项不存在
 *@li   NDK_ERR_READ    读文件失败
 *@li   NDK_ERR_WRITE   写文件失败
 *@li   NDK_ERR_APP_CREAT_CHILD 等待子进行结束错误
*/
int NDK_AppRun(const uchar *pszAppName)
{
    int nPid;
    char szAppPath[NDK_APP_PATH_MAX+1];
    int nChildPid=-1;
    static int nChildCount=0;
    int nPosition;
    int ret;
    FILE *fp = NULL;
    char *p,tmppath[NDK_APP_PATH_MAX]= {0},appname[NDK_APP_PATH_MAX]= {0};
    if(pszAppName == NULL)
        return NDK_ERR_PARA;
    if (nChildCount>=NDK_APP_MAX_CHILD) {
        return NDK_ERR_APP_MAX_CHILD;
    }
    if ((ret=App_IsExist((const char *)pszAppName,&nPosition))!=NDK_OK) {
        return ret;
    }
    ret=AppGetInfo(nPosition,"Main",tmppath);//查找可执行文件路径
    if(ret!=NDK_OK)
        return ret;
    if((tmppath == NULL)||(strlen(tmppath)==0)) {
        fprintf(stderr,"%s %d Get Main path null\n",__FUNCTION__,__LINE__);
        return NDK_ERR_PARA;
    }
    strcpy(szAppPath,tmppath);
    PDEBUG("[%s][%d] %s\n",__func__,__LINE__,szAppPath);
    ndk_file_cp("/etc/ld.so.conf","/tmp/.ld.so.conf");
    if((p=strstr(tmppath,(char *)pszAppName))!=NULL) {
        *(p+strlen((char *)pszAppName))='\0';
        tmppath[strlen(tmppath)]='\0';
        sprintf(tmppath,"%s\r\n",tmppath);
        PDEBUG("[%s][%d] %s\n",__func__,__LINE__,tmppath);
    }
    fp=fopen("/etc/ld.so.conf","a+");
    if(fp==NULL) {
        ndk_file_cp("/tmp/.ld.so.conf","/etc/ld.so.conf");
        remove("/tmp/.ld.so.conf");
        return NDK_ERR_READ;
    }
    if(fwrite("/appfs/apps/share\r\n",strlen("/appfs/apps/share\r\n"),1,fp)<0) {
        fclose(fp);
        ndk_file_cp("/tmp/.ld.so.conf","/etc/ld.so.conf");
        remove("/tmp/.ld.so.conf");
        return NDK_ERR_WRITE;
    }
    if(fwrite(tmppath,strlen(tmppath),1,fp)<0) {
        fclose(fp);
        ndk_file_cp("/tmp/.ld.so.conf","/etc/ld.so.conf");
        remove("/tmp/.ld.so.conf");
        return NDK_ERR_WRITE;
    }
    fclose(fp);
    system("sudo /sbin/ldconfig");

    nPid = fork();
    if (nPid>0) {
        nChildPid = nPid;
        nChildCount ++;
        if (waitpid(nChildPid, &nPid, 0) < 0) {
            ndk_file_cp("/tmp/.ld.so.conf","/etc/ld.so.conf");
            remove("/tmp/.ld.so.conf");
            remove("/tmp/.ld.so.conf");
            ndk_getguifocus();// by zhengk  增加子进程退出后，父进程获取gui焦点 [12/11/2012]
            if(errno == ECHILD) { //by hanjf 子进程不存在，已经结束，则返回成功
                nChildCount--;
                return NDK_OK;
            }
            nChildCount--;
            return NDK_ERR_APP_WAIT_CHILD;
        }
        ndk_getguifocus();// by zhengk  增加子进程退出后，父进程获取gui焦点 [12/11/2012]
        nChildCount--;
    } else if (nPid==0) {
        memset(tmppath,0,sizeof(tmppath));
        sprintf(tmppath, "%s%s", NDK_APP_FILE_PATH, pszAppName);
        if (chdir(tmppath)) {
            exit(-1);
        }
        setenv("HOME",tmppath,1);
        if((p=strstr(szAppPath,(char *)pszAppName))!=NULL)
            strcpy(appname,p+strlen((char *)pszAppName)+1);
        PDEBUG("%s %d %s %s %s\n",__FUNCTION__,__LINE__,szAppPath,tmppath,appname);
        execlp(szAppPath,appname, (char *)0);
        exit(0);
    } else { //nPid<0
        return NDK_ERR_APP_CREAT_CHILD;
    }

    ndk_file_cp("/tmp/.ld.so.conf","/etc/ld.so.conf");
    remove("/tmp/.ld.so.conf");
    system("sudo /sbin/ldconfig");
    if(pszAppName!=NULL)
        NDK_LOG_INFO(NDK_LOG_MODULE_APP,"%s succ,app name is %s\n",__func__,pszAppName);
    return NDK_OK;
}

/**
 *@brief    执行事件操作
 *@param    pszFilename     应用名称
 *@param    psInEventMsg    输入事件消息
 *@param    nInlen          输入事件长度
 *@param    nMaxOutLen      允许的输出事件的长度
 *@retval   psOutEventMsg   获取到的应用信息
 *@retval   pnOutLen        实际输出的数据长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_APP_MAX_CHILD   子应用运行数超过最大运行数目
 *@li   NDK_ERR         操作失败(读写应用配置文件失败)
 *@li   NDK_ERR_APP_NOT_EXIST   应用项不存在
 *@li   NDK_ERR_READ    读文件失败
 *@li   NDK_ERR_WRITE   写文件失败
 *@li   NDK_ERR_APP_CREAT_CHILD 等待子进行结束错误
*/
int NDK_AppDoEvent(const uchar *pszFilename,int nModuleID,const void *psInEventMsg, int nInlen, void *psOutEventMsg, int nMaxOutLen, int *pnOutLen)
{
    int iret = -1,nResult;
    char szBuff[256]= {0};
    FILE *fp;
    if((pszFilename == NULL)||(psInEventMsg == NULL)||(nInlen == 0)||(psOutEventMsg == NULL)||(pnOutLen == NULL))
        return NDK_ERR_PARA;
    sprintf(szBuff,"touch %s;chmod 777 %s",EVENT_FILE,EVENT_FILE);
    system(szBuff);
    memset(szBuff,0,sizeof(szBuff));
    fp = fopen(EVENT_FILE,"w+");
    if(fp == NULL) {
        PDEBUG("%s %d\n",__func__,__LINE__);
        return NDK_ERR_WRITE;
    }
    fwrite(&nModuleID,4,1,fp);//文件格式为nModuleID+nInlen+psInEventMsg
    fwrite(&nInlen,4,1,fp);
    fwrite(psInEventMsg,nInlen,1,fp);
    fclose(fp);
    if((iret = NDK_AppRun(pszFilename))!=NDK_OK) {
        App_EmptyFile(EVENT_FILE);
        PDEBUG("%s %d %d %s\n",__func__,__LINE__,iret,pszFilename);
        return iret;
    }
    fp = fopen(EVENT_FILE,"r");
    if(fp == NULL) {
        fclose(fp);
        App_EmptyFile(EVENT_FILE);
        PDEBUG("%s %d\n",__func__,__LINE__);
        return NDK_ERR_READ;
    }
    fseek(fp, 0, SEEK_END);
    int nLen = ftell(fp);
    fseek(fp,0L,SEEK_SET);
    fread(szBuff,1,nLen,fp);
    NDK_LOG_INFO(NDK_LOG_MODULE_APP,"%s %d %d %s\n",__func__,__LINE__,nLen,szBuff);
    PDEBUG("%s %d %d %s\n",__func__,__LINE__,nLen,szBuff);
    *pnOutLen = nLen-1;
    fclose(fp);
    nResult = szBuff[0];
    NDK_LOG_INFO(NDK_LOG_MODULE_APP,"%s %d nResult = %d\n",__func__,__LINE__,nResult);
    PDEBUG("%s %d nResult = %d\n",__func__,__LINE__,nResult);
    if(nResult !=NDK_OK) {
        App_EmptyFile(EVENT_FILE);
        return (nResult-256);
    }
    if((nLen-1)>nMaxOutLen)
        nLen = nMaxOutLen+1;
    memcpy(psOutEventMsg,szBuff+1,nLen-1);
    App_EmptyFile(EVENT_FILE);
    return NDK_OK;
}

/**
 *@brief    运行后台服务程序
 *@param    pszAppName  应用名称。
 *@param    argv 应用运行参数
 *@param    block:  1 阻塞直到后台应用退出
                    0 并行运行

 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li   NDK_ERR_APP_NOT_EXIST       应用项不存在
 *@li   NDK_ERR_READ        读文件失败
 *@li   NDK_ERR_WRITE       写文件失败
*/
int NDK_AppEXECV(const uchar *pszAppName, char * const argv[],char  block)
{
    int nPid;
    char szAppPath[NDK_APP_PATH_MAX+1],tmppath[256]= {0},*p;
    FILE *fp = NULL;
    extern void ndk_close_client(void);
    if(pszAppName == NULL)
        return NDK_ERR_PARA;
    if((block!=0)&&(block!=1))
        return NDK_ERR_PARA;
    sprintf(szAppPath, "%s%s", NDK_APP_FILE_PATH, pszAppName);
    if (access(szAppPath,F_OK)<0)
        return NDK_ERR_APP_NOT_EXIST;

    ndk_file_cp("/etc/ld.so.conf","/tmp/.ld.so.conf");
    p = strrchr(szAppPath, '/');
    memcpy(tmppath,szAppPath,p-szAppPath);
    strcat(tmppath,"\n");
    PDEBUG("[%s][%d] %s %s %s %s\n",__func__,__LINE__,tmppath,szAppPath,p,pszAppName);

    fp=fopen("/etc/ld.so.conf","a+");
    if(fp==NULL) {
        remove("/tmp/.ld.so.conf");
        return NDK_ERR_READ;
    }
    if(fwrite("/appfs/apps/share\r\n",strlen("/appfs/apps/share\r\n"),1,fp)<0) {
        fclose(fp);
        remove("/tmp/.ld.so.conf");
        return NDK_ERR_WRITE;
    }
    if(fwrite(tmppath,strlen(tmppath),1,fp)<0) {
        fclose(fp);
        remove("/tmp/.ld.so.conf");
        return NDK_ERR_WRITE;
    }
    fclose(fp);
    system("sudo /sbin/ldconfig");

    nPid = fork();
    if (nPid==0) {
        sprintf(szAppPath, "%s%s", NDK_APP_FILE_PATH, pszAppName);
        PDEBUG("%s\n",szAppPath);
        if (access(szAppPath,F_OK)<0) {
            fprintf(stderr,"access ------%s fail\n",szAppPath);
            exit(-1);
        }

        ndk_close_client();
        execv(szAppPath, argv);
        exit(0);
    } else { //nPid<0
        if(block) {
            waitpid(nPid,NULL, 0);
            ndk_getguifocus();// by zhengk  增加子进程退出后，父进程获取gui焦点 [12/11/2012]
            ndk_file_cp("/tmp/.ld.so.conf","/etc/ld.so.conf");
            remove("/tmp/.ld.so.conf");
            system("sudo /sbin/ldconfig");
            return NDK_OK;
        }
    }
    ndk_file_cp("/tmp/.ld.so.conf","/etc/ld.so.conf");
    remove("/tmp/.ld.so.conf");
    system("sudo /sbin/ldconfig");
    if(pszAppName!=NULL)
        NDK_LOG_INFO(NDK_LOG_MODULE_APP,"%s succ,app name is %s\n",__func__,pszAppName);
    return NDK_OK;
}


#define  APPLIBRARYPATH   "/appfs/apps/share/"
#define  APPSHARALIBRARYPATH   "/appfs/apps/share/"
#define  APPTMPPATH   "/tmp/"
/**
 *@brief    获取应用管理相关目录
 *@param    emPathId  要获取的目录ID
 *@retval   pszPath   获取到的应用目录
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(emPathId不在枚举范围内，pszPath为NULL)
*/
int NDK_AppGetPathById(EM_APPPATH emPathId,char *pszPath)
{
    if(emPathId<APPPATH_LIARARY||emPathId>APPPATH_TMP||pszPath==NULL)
		return NDK_ERR_PARA;
	switch(emPathId){
		case APPPATH_LIARARY:
			strcpy(pszPath,APPLIBRARYPATH);
			pszPath[strlen(pszPath)]='\0';
			break;
		case APPPATH_SHARA_LIBRARY:
			strcpy(pszPath,APPSHARALIBRARYPATH);
			pszPath[strlen(pszPath)]='\0';
			break;
		case APPPATH_TMP:
			strcpy(pszPath,APPTMPPATH);
			pszPath[strlen(pszPath)]='\0';
			break;
		default:
			break;
	}
    return NDK_OK;
}
/**
 *@brief    获取应用程序安装目录
 *@param    pszAppName  应用名
 *@retval   pszAppPath  获取到的应用安装目录
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszAppName为NULL,pszAppPath为NULL)
 *@li   NDK_ERR_APP_FILE_NOT_EXIST        应用不存在(不存在该应用程序)
*/
int NDK_AppGetPathByAppName(const char *pszAppName,char *pszAppPath)
{
	int tmppath[256] = {0};
	
    if(pszAppName==NULL||pszAppPath==NULL)
		return NDK_ERR_PARA;
	if(strlen(pszAppName)==0)
		return NDK_ERR_APP_FILE_NOT_EXIST;
	sprintf(tmppath,"/appfs/apps/%s/",pszAppName);
	if(access(tmppath,F_OK)!=0)
		return NDK_ERR_APP_FILE_NOT_EXIST;
	else{
		strcpy(pszAppPath,tmppath);
		pszAppPath[strlen(pszAppPath)]='\0';
	}
    return NDK_OK;
}
/** @} */ // 应用管理模块结束

/* End of this file */
