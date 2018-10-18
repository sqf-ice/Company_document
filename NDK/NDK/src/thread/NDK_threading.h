/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：	产品开发部
* 日    期：	2013-02-17
* 版	本：	V1.00
* 最后修改人：
* 最后修改日期：
*/
#ifndef __NDKTHREAD__H
#define __NDKTHREAD__H

#include "NDK.h"
#include "NDK_debug.h"

int NDK_CreateSemaphore( NDK_HANDLE *handle );
int NDK_CreateThread( NDK_HANDLE *handle, const char* name, unsigned stackSize, void (*entry_point)( void ) );
int NDK_DestroySemaphore( NDK_HANDLE handle );
int NDK_GetThreadState( NDK_HANDLE handle, NDK_THREAD_STATE *state );
int NDK_LockSemaphore( NDK_HANDLE handle );
int NDK_ReleaseSemaphore( NDK_HANDLE handle );
int NDK_Relinquish( void );
int NDK_ResumeThread( NDK_HANDLE handle );
int NDK_SuspendThread( NDK_HANDLE handle );
int NDK_TerminateThread( NDK_HANDLE handle );
int NDK_ThreadSleep( int ms_wait );




#endif
/* End of this file */

