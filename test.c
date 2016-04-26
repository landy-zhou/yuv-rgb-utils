
#include <stdio.h>
#include "rgb_yuv_utils.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define WIDTH 	1920
#define HEIGHT	1080

char yuv_buf[WIDTH*HEIGHT*2];
char rgb_buf[WIDTH*HEIGHT*4];

int main(int argc,char **argv)
{
    FILE *yuv_file;
    FILE *rgb_file;
    int width = WIDTH;
    int height = HEIGHT;

    IplImage *frame = cvCreateImageHeader(cvSize(WIDTH, HEIGHT),IPL_DEPTH_8U,4);
    /* creat opencv window */
    cvNamedWindow("test", CV_WINDOW_AUTOSIZE);

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

    cvSetData(frame,rgb_buf,frame->widthStep); 
    cvShowImage( "test", frame );	//in bgr order

    if(0 > fwrite(rgb_buf,1,width*height*4,rgb_file))
    {
	perror("fwrite");
	return -1;
    }
    fflush(rgb_file);
    fclose(yuv_file);
    fclose(rgb_file);

    cvWaitKey(0);
    cvDestroyWindow( "test" );
    cvReleaseImageheader(frame);
    return 0;
}


