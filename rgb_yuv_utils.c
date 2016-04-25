#include <stdio.h>
#include <string.h>
#include <sys/time.h>

int yuv422_to_rgb888(unsigned char *YUYV_image, unsigned char *RGB_image, unsigned int nWidth, unsigned int nHeight)
{
    int x, y, yuyv[4],yuv[6],bgr[6];
    unsigned char *pin, *pout;
	for(y=0; y< nHeight; y++)
	{
	    pin = YUYV_image + y * nWidth * 2;
	    pout = RGB_image + y * nWidth * 3;
	    for(x=0; x<nWidth; x+=2)
	    {
			yuyv[0]=*(pin++);
			yuyv[1]=*(pin++);
			yuyv[2]=*(pin++);
			yuyv[3]=*(pin++);

			yuv[0] = yuyv[0];
			yuv[1] = yuyv[1]-128;
			yuv[2] = yuyv[3]-128;
			yuv[3] = yuyv[2];
			yuv[4] = yuyv[1]-128;
			yuv[5] = yuyv[3]-128;
			bgr[0]=yuv[1]+yuv[0];
			bgr[2]=yuv[2]+yuv[0];
			bgr[1]=yuv[0]-((3141*yuv[1]+8350*yuv[2])>>14);
			bgr[3]=yuv[4]+yuv[3];
			bgr[5]=yuv[5]+yuv[3];
			bgr[4]=yuv[3]-((3141*yuv[4]+8350*yuv[5])>>14);

			if(bgr[0]<0)bgr[0]=0;   if(bgr[0]>255)bgr[0]=255;
			*(pout++)=(unsigned char) bgr[0];

			if(bgr[1]<0)bgr[1]=0;   if(bgr[1]>255)bgr[1]=255;
			*(pout++)=(unsigned char) bgr[1];

			if(bgr[2]<0)bgr[2]=0;   if(bgr[2]>255)bgr[2]=255;
			*(pout++)=(unsigned char) bgr[2];

			if(bgr[3]<0)bgr[3]=0;   if(bgr[3]>255)bgr[3]=255;
			*(pout++)=(unsigned char) bgr[3];

			if(bgr[4]<0)bgr[4]=0;   if(bgr[4]>255)bgr[4]=255;
			*(pout++)=(unsigned char) bgr[4];

			if(bgr[5]<0)bgr[5]=0;   if(bgr[5]>255)bgr[5]=255;
			*(pout++)=(unsigned char) bgr[5];
		}
	}
	return 0;
}

static int yuv_tbl_ready=0;
static int y1192_tbl[256];
static int v1634_tbl[256];
static int v833_tbl[256];
static int u400_tbl[256];
static int u2066_tbl[256];
void yuyv422toABGRY(unsigned char *src,unsigned char *rgb,int width,int height )
{

    int frameSize =width*height*2;

    int i;

    if((!rgb)){
        return;
    }
    int *lrgb = NULL;
    int *lybuf = NULL;

    lrgb = (int *)&rgb[0];
    //lybuf = &ybuf[0];

    if(yuv_tbl_ready==0){
        for(i=0 ; i<256 ; i++){
            y1192_tbl[i] = 1192*(i-16);
            if(y1192_tbl[i]<0){
                y1192_tbl[i]=0;
            }

            v1634_tbl[i] = 1634*(i-128);
            v833_tbl[i] = 833*(i-128);
            u400_tbl[i] = 400*(i-128);
            u2066_tbl[i] = 2066*(i-128);
        }
        yuv_tbl_ready=1;
    }

    for(i=0 ; i<frameSize ; i+=4){
        unsigned char y1, y2, u, v;
        y1 = src[i];
        u = src[i+1];
        y2 = src[i+2];
        v = src[i+3];

        int y1192_1=y1192_tbl[y1];
        int r1 = (y1192_1 + v1634_tbl[v])>>10;
        int g1 = (y1192_1 - v833_tbl[v] - u400_tbl[u])>>10;
        int b1 = (y1192_1 + u2066_tbl[u])>>10;

        int y1192_2=y1192_tbl[y2];
        int r2 = (y1192_2 + v1634_tbl[v])>>10;
        int g2 = (y1192_2 - v833_tbl[v] - u400_tbl[u])>>10;
        int b2 = (y1192_2 + u2066_tbl[u])>>10;

        r1 = r1>255 ? 255 : r1<0 ? 0 : r1;
        g1 = g1>255 ? 255 : g1<0 ? 0 : g1;
        b1 = b1>255 ? 255 : b1<0 ? 0 : b1;
        r2 = r2>255 ? 255 : r2<0 ? 0 : r2;
        g2 = g2>255 ? 255 : g2<0 ? 0 : g2;
        b2 = b2>255 ? 255 : b2<0 ? 0 : b2;

        *lrgb++ = 0xff000000 | b1<<16 | g1<<8 | r1;
        *lrgb++ = 0xff000000 | b2<<16 | g2<<8 | r2;

        if(lybuf!=NULL){
            *lybuf++ = y1;
            *lybuf++ = y2;
        }
    }

}

unsigned long measure_us(struct timeval *start, struct timeval *stop)
{
    unsigned long sec, usec, time;
    sec = stop->tv_sec - start->tv_sec;

    if (stop->tv_usec >= start->tv_usec) {
	usec = stop->tv_usec - start->tv_usec;
    } else {
	usec = stop->tv_usec + 1000000 - start->tv_usec;
	sec--;
    }

    time = (sec * 1000000) + usec;

    return time;
}
