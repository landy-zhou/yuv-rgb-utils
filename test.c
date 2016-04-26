
#include <stdio.h>
#include "rgb_yuv_utils.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define WIDTH 	1920
#define HEIGHT	1080

char yuv_buf[WIDTH*HEIGHT*2];
char rgb_buf[WIDTH*HEIGHT*4];
char out_yuv_buf[WIDTH*HEIGHT*3];

int main(int argc,char **argv)
{
    FILE *yuv_file;
    FILE *rgb_file;
    FILE *out_yuv_file;
    int width = WIDTH;
    int height = HEIGHT;
    struct timeval start_time,end_time;
    char out_yuv_path[30];

    IplImage *rgb_frame = cvCreateImageHeader(cvSize(WIDTH, HEIGHT),IPL_DEPTH_8U,4);
    IplImage *yuv_frame = cvCreateImageHeader(cvSize(WIDTH, HEIGHT),IPL_DEPTH_8U,3);

    if( 3 > argc )
    {
	printf("Usage: exe in_yuv_file_path out_rgb_file_path\n");
	return -1;
    }

    //open files
    sprintf(out_yuv_path,"%s-out-yuv.yuv",argv[2]);
    yuv_file = fopen(argv[1],"r");
    rgb_file = fopen(argv[2],"w");
    out_yuv_file = fopen(out_yuv_path,"w");

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

    //nv21 --> rgba
    nv21_to_rgba_init();
    gettimeofday(&start_time,NULL);
    nv21_to_rgba(yuv_buf,rgb_buf,width,height);
    gettimeofday(&end_time,NULL);
    printf( "size %dx%d, nv21_to_rgba time cost : %ld us\n",width,height,measure_us(&start_time,&end_time) );
    // creat opencv window 
    cvNamedWindow("rgba", CV_WINDOW_AUTOSIZE);
    cvSetData(rgb_frame,rgb_buf,rgb_frame->widthStep); 
    cvShowImage( "rgba", rgb_frame );	//in bgr order
    cvWaitKey(0);
    //write to rgb file
    if(0 > fwrite(rgb_buf,1,width*height*4,rgb_file))
    {
	perror("fwrite");
	return -1;
    }

    //rgba -->yuv
    gettimeofday(&start_time,NULL);
    rgba_to_yuv(rgb_buf,out_yuv_buf,width,height);
    gettimeofday(&end_time,NULL);
    printf( "size %dx%d, rgba_to_yuv time cost : %ld us\n",width,height,measure_us(&start_time,&end_time) );
    // creat opencv window 
    cvNamedWindow("yuv", CV_WINDOW_AUTOSIZE);
    cvSetData(yuv_frame,out_yuv_buf,yuv_frame->widthStep); 
    cvShowImage( "yuv", yuv_frame );	//in bgr order
    cvWaitKey(0);
    //write to yuv out file
    if(0 > fwrite(out_yuv_buf,1,width*height*3,out_yuv_file))
    {
	perror("fwrite");
	return -1;
    }

    // close file handlers
    fflush(rgb_file);
    fclose(yuv_file);
    fclose(rgb_file);
    fclose(out_yuv_file);

    cvDestroyWindow( "rgba" );
    cvDestroyWindow( "yuv" );
    //cvReleaseImageheader(frame);
    return 0;
}


