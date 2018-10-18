#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
//#include "NDK_threading.h"
#include "NDK.h"
#include "NDK_debug.h"

typedef struct event_ctx {
	pthread_cond_t evt_var;
	pthread_mutex_t evt_mutex;
	volatile unsigned long value;
    volatile unsigned clients;
}EVENT_CTX;

static bool RetrievePredicate( unsigned long value, unsigned long mask,
								unsigned long *signaled );

static EVENT_CTX ndk_event;



int NDK_InitEvents( void )
{
	int retval = NDK_ERR;

	// Attempt to initialise the mutex
	if( 0 == pthread_mutex_init( &ndk_event.evt_mutex, NULL ) )
	{
		// Now initiailise the variable
		if( 0 == pthread_cond_init( &ndk_event.evt_var, NULL ) )
		{
			ndk_event.value = 0;
			ndk_event.clients = 0;
			retval = NDK_OK;
		}
		else
		{
			// Destroy the mutex
			pthread_mutex_destroy( &ndk_event.evt_mutex );
			memset( &ndk_event.evt_mutex, 0, sizeof( ndk_event.evt_mutex ) );
			fprintf( stdout, "pthread_cond_init: %s\n", strerror( errno ) );
		}
	}
	else
	{
		fprintf( stdout, "pthread_mutex_init: %s\n", strerror( errno ) );
	}
	return retval;
}

void NDK_DeleteEvents( void )
{
	// Destroy the condition variable
	if( 0 == pthread_cond_destroy( &ndk_event.evt_var ) )
	{
		memset( &ndk_event.evt_var, 0, sizeof( ndk_event.evt_var ) );
	}
	else
	{
		fprintf( stdout, "pthread_cond_destroy: %s\n", strerror( errno ) );
	}
	// Destroy the mutex
	if( 0 == pthread_mutex_destroy( &ndk_event.evt_mutex ) )
	{
		memset( &ndk_event.evt_mutex, 0, sizeof( ndk_event.evt_mutex ) );
	}
	else
	{
		fprintf( stdout, "pthread_mutex_destroy: %s\n", strerror( errno ) );
	}
}

NEXPORT	int NDK_SetEvents( ulong ulEventId )
{
    unsigned long old_value;

    pthread_mutex_lock( &ndk_event.evt_mutex );

	old_value = ndk_event.value;

    ndk_event.value = old_value | ulEventId;

    if( ndk_event.value != old_value && ndk_event.clients )
	{
       	pthread_cond_broadcast( &ndk_event.evt_var );
	}

    pthread_mutex_unlock( &ndk_event.evt_mutex );

    return NDK_OK;
}

NEXPORT	int NDK_ClearEvents( ulong ulEventId )
{
    unsigned long old_value;

    pthread_mutex_lock( &ndk_event.evt_mutex );

	old_value = ndk_event.value;

    ndk_event.value = ~ulEventId & old_value;

    if( ndk_event.value != old_value && ndk_event.clients )
	{
       	pthread_cond_broadcast( &ndk_event.evt_var );
	}

    pthread_mutex_unlock( &ndk_event.evt_mutex );

    return NDK_OK;
}

NEXPORT	int NDK_WaitForEvents( ulong ulEventId, ulong *pEventsRetrieved, unsigned int uiTimeOut )
{
	int retval = NDK_ERR;
	struct timespec time;
	int status = 0;

	// First initialise the time variable
	if( clock_gettime( CLOCK_REALTIME, &time ) )
	{
		// Something is very wrong
		return retval;
	}

    if( uiTimeOut && uiTimeOut != NDK_SUSPEND )
    {
		time.tv_sec += uiTimeOut / 100; // change to OS_TIMES
		time.tv_nsec += ( uiTimeOut % 100 ) * ( 1000000 );
		if( time.tv_nsec >= 1000000000 )
		{
			time.tv_sec++;
			time.tv_nsec -= 1000000000;
		}
    }

    pthread_mutex_lock( &ndk_event.evt_mutex );

	do
    {
     	if( RetrievePredicate( ndk_event.value, ulEventId, pEventsRetrieved ) )
		{
			break;
		}

   		ndk_event.clients++;

		if( uiTimeOut == NDK_SUSPEND )
		{
       		status = pthread_cond_wait( &ndk_event.evt_var, &ndk_event.evt_mutex );
		}
       	else if( uiTimeOut )
		{
       		status = pthread_cond_timedwait( &ndk_event.evt_var, &ndk_event.evt_mutex, &time );
		}
		ndk_event.clients--;

    } while( uiTimeOut && !status );

    if( status == ETIMEDOUT )
			RetrievePredicate( ndk_event.value, ulEventId, pEventsRetrieved );

    pthread_mutex_unlock( &ndk_event.evt_mutex );

    return ( status == ETIMEDOUT )? NDK_ERR_TIMEOUT : NDK_OK;
}

NEXPORT	int NDK_CheckAnyEvents( ulong ulEventId, ulong *pEventsRetrieved )
{
	int retval = NDK_ERR;
	ulong events;
	// Use zero time for non-blocking operation
	retval = NDK_WaitForEvents( ulEventId, &events, 0 );
	if( NDK_OK == retval )
	{
		*pEventsRetrieved = events;
	}
	return retval;
}

NEXPORT	int NDK_CheckAllEvents( ulong ulEventId, ulong *pEventsRetrieved )
{
	int retval = NDK_ERR;
	ulong events;
	// Use zero time for non-blocking operation
	retval = NDK_WaitForEvents( ulEventId, &events, 0 );
	if( NDK_OK == retval )
	{
		if( events == ulEventId )
			*pEventsRetrieved = events;
		else
			retval = NDK_ERR_TIMEOUT;
	}
	return retval;
}

static bool RetrievePredicate( unsigned long value, unsigned long mask,
								unsigned long *signaled )
{
	if( value & mask )
	{
   		*signaled = value & mask;
	}
	return ( value & mask )?true:false;
}


