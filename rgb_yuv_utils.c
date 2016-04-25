#include <stdio.h>
#include <string.h>
#include <sys/time.h>

char yuv_buf[1920*1080*2];
char rgb_buf[1920*1080*4];
int nv21_to_rgba(unsigned char *src,unsigned char *rgb,int width,int height );

int main(int argc,char **argv)
{
    FILE *yuv_file;
    FILE *rgb_file;
    int width = 1920;
    int height = 1080;

    if( 3 > argc )
    {
	printf("Usage: exe in_yuv_file_path out_rgb_file_path\n");
	return -1;
    }

    yuv_file = fopen(argv[1],"r");
    rgb_file = fopen(argv[2],"w");
    if((!yuv_file)||(!rgb_file))
    {
	perror("fopen");
	return -1;
    }
    if(0 > fread(yuv_buf,1,width*height*3/2,yuv_file))
    {
	perror("fread");
	return -1;
    }

    nv21_to_rgba(yuv_buf,rgb_buf,width,height);

    if(0 > fwrite(rgb_buf,1,width*height*4,rgb_file))
    {
	perror("fwrite");
	return -1;
    }
    fflush(rgb_file);
    fclose(yuv_file);
    fclose(rgb_file);

    return 0;
}

static int yuv_tbl_ready_flag=0;
static int y1192_tbl[256];
static int v1634_tbl[256];
static int v833_tbl[256];
static int u400_tbl[256];
static int u2066_tbl[256];
int nv21_to_rgba(unsigned char *src_buf,unsigned char *rgb_buf,int width,int height )
{

    int frame_size =width*height;
    unsigned char (*src_y)[width],(*src_vu)[width];
    int i,j;

    if((!rgb_buf)||(!src_buf)){
	return -1;
    }

    src_y = src_buf;
    src_vu = src_buf + frame_size;

    unsigned char (*rgb)[width*4] = rgb_buf;

    if(yuv_tbl_ready_flag==0){
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
	yuv_tbl_ready_flag=1;
    }

    for(i=0; i<height; i+=2){
	for(j=0; j<width; j+=2){
	    unsigned char y11,y12,y21,y22,u,v;
	    int i_v = i>>1;  // index for vu buf
	    int j_r = 4*j;  //index for rgba

	    y11 = src_y[i][j];
	    y12 = src_y[i][j+1];
	    y21 = src_y[i+1][j];
	    y22 = src_y[i+1][j+1];

	    v = src_vu[i_v][j];
	    u = src_vu[i_v][j+1];

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

	    rgb[i][j_r+0] = r11>255 ? 255 : r11<0 ? 0 : r11;
	    rgb[i][j_r+1] = g11>255 ? 255 : g11<0 ? 0 : g11;
	    rgb[i][j_r+2] = b11>255 ? 255 : b11<0 ? 0 : b11;
	    rgb[i][j_r+3] = 0xff;

	    rgb[i][j_r+4] = r12>255 ? 255 : r12<0 ? 0 : r12;
	    rgb[i][j_r+5] = g12>255 ? 255 : g12<0 ? 0 : g12;
	    rgb[i][j_r+6] = b12>255 ? 255 : b12<0 ? 0 : b12;
	    rgb[i][j_r+7] = 0xff;

	    rgb[i+1][j_r+0] = r21>255 ? 255 : r21<0 ? 0 : r21;
	    rgb[i+1][j_r+1] = g21>255 ? 255 : g21<0 ? 0 : g21;
	    rgb[i+1][j_r+2] = b21>255 ? 255 : b21<0 ? 0 : b21;
	    rgb[i+1][j_r+3] = 0xff;

	    rgb[i+1][j_r+4] = r22>255 ? 255 : r22<0 ? 0 : r22;
	    rgb[i+1][j_r+5] = g22>255 ? 255 : g22<0 ? 0 : g22;
	    rgb[i+1][j_r+6] = b22>255 ? 255 : b22<0 ? 0 : b22;
	    rgb[i+1][j_r+7] = 0xff;
	}
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
