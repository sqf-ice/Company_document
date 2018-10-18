#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define __USE_GNU
#include <pthread.h>
#include "NDK.h"
#include "NDK_debug.h"
//#include "NDK_threading.h"

typedef void        ( *pvFUNC )( void );

static void *start_routine(void *p);

NEXPORT int NDK_CreateThread( NDK_HANDLE *handle, const char* name, unsigned stackSize,
                              void (*entry_point)( void ) )
{
    int retval = NDK_ERR;

    // Check that we got a valid pointer to store the thread handle.
    // and we have a valid thread entry point
    if( handle && entry_point ) {
        // Allocate memory for the thread object
        pthread_t *new_thread = (pthread_t *)malloc( sizeof( pthread_t ) );

        if( new_thread ) {
            pthread_attr_t attr;
            pthread_attr_init( &attr );
            int rc;

            // Set the name and stack size if required
#ifdef  pthread_setname_np
            if( name ) {
                pthread_setname_np( *new_thread, name );
            }
#endif
            if( stackSize ) {
                pthread_attr_setstacksize( &attr, stackSize );
            }
            // Now create the thread
            rc = pthread_create( new_thread, &attr, start_routine, (void *)entry_point );
            if ( rc ) {
                fprintf( stdout, "ERROR; return code from pthread_create() is %d\n", rc);
                retval = NDK_ERR;
            } else {
                // success
                retval = NDK_OK;
            }
        } else {
            // Memory allocation failed
            retval = NDK_ERR_MACLLOC;
        }
    } else {
        // Invalid parameters supplied
        retval = NDK_ERR_PARA;
    }
    return retval;
}

NEXPORT int NDK_ResumeThread( NDK_HANDLE handle )
{
    // Not available in POSIX
    return NDK_ERR_THREAD_CMDUNSUPPORTED;
}

NEXPORT int NDK_SuspendThread( NDK_HANDLE handle )
{
    // Not available in POSIX
    return NDK_ERR_THREAD_CMDUNSUPPORTED;
}

NEXPORT int NDK_TerminateThread( NDK_HANDLE handle )
{
    int retval = NDK_ERR;

    if( handle ) {
        pthread_cancel( *((pthread_t *)handle) );
        free( handle );
        retval = NDK_OK;
    }
    return retval;
}

NEXPORT int NDK_Relinquish( void )
{
    pthread_yield( );
    return NDK_OK;
}

NEXPORT int NDK_GetThreadState( NDK_HANDLE handle, EM_THREAD_STATE *state )
{
    // Not available in POSIX
    return NDK_ERR_THREAD_CMDUNSUPPORTED;
}

NEXPORT int NDK_ThreadSleep( int ms_wait )
{
    struct timespec req;
    int sec;

    // Convert the number of ms to a timespec value
    sec = ms_wait/1000;
    ms_wait = ms_wait - sec*1000;

    req.tv_sec = sec;
    req.tv_nsec = ms_wait*1000000;
    req.tv_sec+=req.tv_nsec/1000000000 + sec;
    req.tv_nsec=req.tv_nsec%1000000000;

    // Loop until we've finished the delay
    do {
        // Wait the required time
        if( 0 != nanosleep( &req, &req ) ) {
            // If interrupted by EINTR, continue, otherwise exit.
            if(errno != EINTR)
                return NDK_ERR;
        } else {
            // Wait finished, exit the loop
            break;
        }
    } while( req.tv_sec > 0 || req.tv_nsec > 0 );

    return NDK_OK;
}

NEXPORT int NDK_CreateSemaphore( NDK_HANDLE *handle )
{
    int retval = NDK_ERR;
    pthread_mutex_t *mutex = 0;

    if( handle ) {
        mutex = malloc( sizeof( pthread_mutex_t ) );
        if( mutex ) {
            memset( mutex, 0, sizeof( pthread_mutex_t ) );
            pthread_mutex_init( mutex, 0 );
            *handle = mutex;
            retval = NDK_OK;
        } else {
            retval = NDK_ERR_THREAD_ALLOC;
        }
    } else {
        retval = NDK_ERR_THREAD_PARAM;
    }
    return retval;
}

NEXPORT int NDK_LockSemaphore( NDK_HANDLE handle )
{
    if(handle==NULL)
        return NDK_ERR_THREAD_PARAM;
    if( handle ) {
        pthread_mutex_lock( (pthread_mutex_t *)handle );
    }
    return NDK_OK;
}

NEXPORT int NDK_ReleaseSemaphore( NDK_HANDLE handle )
{
    if(handle==NULL)
        return NDK_ERR_THREAD_PARAM;
    if( handle ) {
        pthread_mutex_unlock( (pthread_mutex_t *)handle );
    }
    return NDK_OK;
}

NEXPORT int NDK_DestroySemaphore( NDK_HANDLE handle )
{
    if(handle==NULL)
        return NDK_ERR_THREAD_PARAM;
    if( handle ) {
        pthread_mutex_destroy( (pthread_mutex_t *)handle );
        free( handle );
    }
    return NDK_OK;
}

static void *start_routine(void *p)
{
    pvFUNC fptr = ( pvFUNC )p;
    if( p ) {
        fptr( );
    }
    return NULL;
}
