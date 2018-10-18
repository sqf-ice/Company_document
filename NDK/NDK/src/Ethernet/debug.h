#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <time.h>
#include <unistd.h>

#undef DPRINTF

#define LCD 0
#define COM 1

#ifdef DEBUG
#if OUTPUT == LCD
#define DPRINTF(fmt, args...) pos_printf("[%d]%s,%s,%d:"fmt, (int)time((time_t *)0), __FILE__, __FUNCTION__, __LINE__, ##args)
#elif OUTPUT == COM
#define DPRINTF(fmt, args...) \
		{\
		struct timeval tv;\
		gettimeofday(&tv, (void *)0);\
		fprintf(stderr, "[%d-%d]%s,%s,%d:"fmt, (int)tv.tv_sec, (int)tv.tv_usec, __FILE__, __FUNCTION__, __LINE__, ##args);}
#elif OUTPUT == COM1
#define DPRINTF(fmt, args...) \
		{\
		struct timeval tv;\
		char debug_buf[2048];\
		gettimeofday(&tv, (void *)0);\
		fprintf(debug_buf, "[%d-%d]%s,%s,%d:"fmt, (int)tv.tv_sec, (int)tv.tv_usec, __FILE__, __FUNCTION__, __LINE__, ##args);\
		portwrite(0, strlen(debug_buf), debug_buf);}\
	#elif OUTPUT == FILE
#define DPRINTF(fmt, args...)
#endif
#else
#define DPRINTF(fmt, args...)
#endif

#undef DPRINTFF
#define DPRINTFF(fmt, args...)

#endif
