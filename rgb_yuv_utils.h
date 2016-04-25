#ifndef __RGB_YUV_UTILS_H__
#define __RGB_YUV_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

int yuv422_to_rgb888(unsigned char *YUYV_image, unsigned char *RGB_image, unsigned int nWidth, unsigned int nHeight);
void yuyv422toABGRY(unsigned char *src,unsigned char *rgb,int width,int height );
unsigned long measure_us(struct timeval *start, struct timeval *stop);

#ifdef __cplusplus
}
#endif

#endif 

