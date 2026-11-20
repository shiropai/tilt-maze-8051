/* stdutils.h — 精簡自課堂 OLED_MPU6050 實驗（ExploreEmbedded，BSD license） */
#ifndef _STD_UTIL_H_
#define _STD_UTIL_H_

typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef signed int         int16_t;
typedef unsigned int       uint16_t;
typedef signed long int    int32_t;
typedef unsigned long int  uint32_t;

#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE  1u
#endif
#ifndef FALSE
#define FALSE 0u
#endif

#define util_GetAbsolute(x)  (((x) < 0) ? -(x) : (x))

#endif
