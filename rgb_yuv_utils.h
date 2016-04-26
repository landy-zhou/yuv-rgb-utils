#ifndef __RGB_YUV_UTILS_H__
#define __RGB_YUV_UTILS_H__

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

void nv21_to_rgba_init(void);
int nv21_to_abgr(unsigned char *src,unsigned char *rgb,int width,int height);
unsigned long measure_us(struct timeval *start, struct timeval *stop);

#ifdef __cplusplus
}
#endif

#endif 

