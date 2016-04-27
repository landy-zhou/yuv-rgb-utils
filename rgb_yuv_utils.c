
#include "rgb_yuv_utils.h"

#define SYSTEM_BITS_IN_BYTES  (sizeof(void*))
#define LOOPS_PER_LONG	(SYSTEM_BITS_IN_BYTES/2)

//#define UCHAR_RANGE(x)	((0>x)?0:(x>255)?255:x)
//#define UCHAR_RANGE(x)		(x)
#define UCHAR_RANGE(x)	((unsigned long)uchar_range_tbl[x+300])

static int yuv_tbl_ready_flag=0;
static int loop_times_once=2;
static int y1192_tbl[256];
static int v1634_tbl[256];
static int v833_tbl[256];
static int u400_tbl[256];
static int u2066_tbl[256];

static unsigned char uchar_range_tbl[1000];

void nv21_to_rgba_init(void)
{
    int i;
    if(0 == yuv_tbl_ready_flag){
	for(i=0; i<256; i++){
	    y1192_tbl[i] = 1192*(i-16);
	    if(y1192_tbl[i] < 0){
		y1192_tbl[i] = 0;
	    }
	    v1634_tbl[i] = 1634*(i-128);
	    v833_tbl[i] = 833*(i-128);
	    u400_tbl[i] = 400*(i-128);
	    u2066_tbl[i] = 2066*(i-128);
	}
	//-276~534,set tbl offset to 300
	for(i=0; i<1000; i++){
	    if(i<300)
		uchar_range_tbl[i] = 0;
	    else if(i>555)
		uchar_range_tbl[i] = 255;
	    else
		uchar_range_tbl[i] = i-300;
	    //printf("%d,",uchar_range_tbl[i]);
	}
	yuv_tbl_ready_flag = 1;
    }
    //printf("SYSTEM_BITS_IN_BYTES=%d\n",SYSTEM_BITS_IN_BYTES);
}

int nv21_to_rgba(unsigned char *src_buf,unsigned char *rgb_buf,int width,int height)
{

    int frame_size =width*height;
    unsigned long *src_y,*src_vu;
    int i,j;

    if((!rgb_buf)||(!src_buf)){
	return -1;
    }

    src_y = (unsigned long *)src_buf;
    src_vu = (unsigned long *)(src_buf + frame_size);

    if(0 == yuv_tbl_ready_flag)
	nv21_to_rgba_init();

    int cnt_long = 0;
    int longs_per_line = width/SYSTEM_BITS_IN_BYTES;
    unsigned long y1 = *(src_y+cnt_long);
    unsigned long y2 = *(src_y+cnt_long+longs_per_line);
    unsigned long vu = *(src_vu+cnt_long);

    unsigned char *rgb1 = rgb_buf;
    unsigned char *rgb2 = rgb1+4*width;
    //unsigned long *rgb1 = (unsigned long *)rgb_buf;
    //unsigned long *rgb2 = rgb1+4*width/SYSTEM_BITS_IN_BYTES;

    //process 2 lines X 1 long size once
    for(i=0; i<frame_size; i+=2*SYSTEM_BITS_IN_BYTES){
	cnt_long++;
	//if reached the end of the line
	if(cnt_long >= longs_per_line)
	{
	    //src buf skip to next two line
	    src_y += 2*longs_per_line;
	    src_vu += longs_per_line;

	    //dst buf skip to next two line
	    rgb1 += 4*width;
	    rgb2 = rgb1+4*width;
	    //rgb1 += 4*width/SYSTEM_BITS_IN_BYTES;
	    //rgb2 = rgb1+4*width/SYSTEM_BITS_IN_BYTES;

	    cnt_long = 0;
	}
	//read src buf per long size
	y1 = *(src_y+cnt_long);
	y2 = *(src_y+cnt_long+longs_per_line);
	vu = *(src_vu+cnt_long);

	//convert one long size per loop
	for(j=0; j<LOOPS_PER_LONG; j+=1){
	    unsigned char y11,y12,y21,y22,u,v;
	    int k = j<<4;
	    //y11 = (y1>>(8*2*j))&0xff;
	    //y12 = (y1>>(8*(2*j+1)))&0xff;
	    //y21 = (y2>>(8*2*j))&0xff;;
	    //y22 = (y2>>(8*(2*j+1)))&0xff;
	    y11 = (y1>>k)&0xff;
	    y12 = (y1>>(k+8))&0xff;
	    y21 = (y2>>k)&0xff;;
	    y22 = (y2>>(k+8))&0xff;

	    //u = (vu>>(8*2*j))&0xff;
	    //v = (vu>>(8*(2*j+1)))&0xff;
	    u = (vu>>k)&0xff;
	    v = (vu>>(k+8))&0xff;

	    int y1192_11=y1192_tbl[y11];
	    int r11 = (y1192_11 + v1634_tbl[v])>>10;
	    int g11 = (y1192_11 - v833_tbl[v] - u400_tbl[u])>>10;
	    int b11 = (y1192_11 + u2066_tbl[u])>>10;

	    int y1192_12=y1192_tbl[y12];
	    int r12 = (y1192_12 + v1634_tbl[v])>>10;
	    int g12 = (y1192_12 - v833_tbl[v] - u400_tbl[u])>>10;
	    int b12 = (y1192_12 + u2066_tbl[u])>>10;

	    int y1192_21=y1192_tbl[y21];
	    int r21 = (y1192_21 + v1634_tbl[v])>>10;
	    int g21 = (y1192_21 - v833_tbl[v] - u400_tbl[u])>>10;
	    int b21 = (y1192_21 + u2066_tbl[u])>>10;

	    int y1192_22=y1192_tbl[y22];
	    int r22 = (y1192_22 + v1634_tbl[v])>>10;
	    int g22 = (y1192_22 - v833_tbl[v] - u400_tbl[u])>>10;
	    int b22 = (y1192_22 + u2066_tbl[u])>>10;

	    *rgb1++ = UCHAR_RANGE(r11);
	    *rgb1++ = UCHAR_RANGE(g11);
	    *rgb1++ = UCHAR_RANGE(b11);
	    *rgb1++ = 0xff;
	    
	    *rgb1++ = UCHAR_RANGE(r12);
	    *rgb1++ = UCHAR_RANGE(g12);
	    *rgb1++ = UCHAR_RANGE(b12);
	    *rgb1++ = 0xff;

	    *rgb2++ = UCHAR_RANGE(r21);
	    *rgb2++ = UCHAR_RANGE(g21);
	    *rgb2++ = UCHAR_RANGE(b21);
	    *rgb2++ = 0xff;

	    *rgb2++ = UCHAR_RANGE(r22);
	    *rgb2++ = UCHAR_RANGE(g22);
	    *rgb2++ = UCHAR_RANGE(b22);
	    *rgb2++ = 0xff;

	    //*rgb1++ = UCHAR_RANGE(r11)|UCHAR_RANGE(g11)<<8|UCHAR_RANGE(b11)<<16|0xff<<24| \
		    UCHAR_RANGE(r12)<<32|UCHAR_RANGE(g12)<<40|UCHAR_RANGE(b12)<<48|0xff<<56;

	    //printf("%16x,",*(--rgb1));
	    //if(SYSTEM_BITS_IN_BYTES)

	    //*rgb2++ = UCHAR_RANGE(r21)|UCHAR_RANGE(g21)<<8|UCHAR_RANGE(b21)<<16|0xff<<24| \
		    UCHAR_RANGE(r22)<<32|UCHAR_RANGE(g22)<<40|UCHAR_RANGE(b22)<<48|0xff<<56;
	    //printf("i=%d,j=%d,rgb1=%d,rgb2=%d\n",i,j,rgb1,rgb2);
	}
    }
    //printf("i=%d\n",i);
    return 0;
}

int rgba_to_yuv(unsigned char *src_buf,unsigned char *yuv_buf,int width,int height)
{

    int i,j;
    unsigned char (*rgb)[width*4] = src_buf;
    unsigned char (*yuv)[width*3] = yuv_buf;

    if((!yuv_buf)||(!src_buf)){
	return -1;
    }

    for(i=0; i<height; i+=1){
	for(j=0; j<width; j+=1){
	    int y,u,v;
	    float r,g,b;
	    int j_r = j<<2; //rgb index
	    int j_y = j*3;  //yuv index

	    r = (float)rgb[i][j_r+0];
	    g = (float)rgb[i][j_r+1];
	    b = (float)rgb[i][j_r+2];

	    y = (int)( 0.257*r + 0.504*g + 0.098*b) + 16;
	    u = (int)(-0.148*r - 0.291*g + 0.439*b) + 128;
	    v = (int)( 0.439*r - 0.368*g - 0.071*b) + 128;


	    yuv[i][j_y+0] = y;
	    yuv[i][j_y+1] = u;
	    yuv[i][j_y+2] = v;
	    //printf("i=%d,j_r=%d\n",i,j_r);
	}
	//printf("i=%d\n",i);
    }

    return 0;
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
