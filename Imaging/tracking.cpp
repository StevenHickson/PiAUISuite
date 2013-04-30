#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

#define _USE_MATH_DEFINES
#define THRESH 0.4f

//local #defines
#define B_MEAN 99.1442f
#define R_MEAN 138.0654f        
#define PI 3.141592653589793238462643383279502884f

//this is actually the inverse of the covariance matrix
#define COV_11 0.00376353477484179f //552.6847
#define COV_12 -0.00330727927436376f //326.5669
#define COV_21 -0.00330727927436376f //326.5669
#define COV_22 0.00559726810338469f //371.6184

#define COV_DET 9.87418612943281e+04

#define WINDOW_NAME "Webcam"

inline void SkinSegment1(IplImage *frame, CvMemStorage *pStorage);
inline void SkinSegment2(IplImage *frame, Mat &kernel, CvMemStorage *pStorage);

int main( int argc, char** argv ) {
    cvNamedWindow( WINDOW_NAME, CV_WINDOW_NORMAL);

    CvCapture* capture = cvCreateCameraCapture(0) ;
    IplImage* frame;
    CvMemStorage* pStorage = cvCreateMemStorage(0);
    
    float data[9] = {0.111111111f,0.111111111f,0.111111111f,0.111111111f,0.111111111f,0.111111111f,0.111111111f,0.111111111f,0.111111111f};
    Mat kernel = Mat(3, 3, CV_32FC1, &data);
    //Mat kernel2 = (cv::Mat_<uchar>(3,3) << 0, 1, 0, 0, 1, 0, 0, 1, 0);

    int args = 0;
    if(argc > 1) {
        if(atoi(argv[1]) == 1)
            args = 1;
        else if(atoi(argv[1]) == 2)
            args = 2;
    }
    
    while(1) {
        frame = cvQueryFrame( capture );
        if( !frame ) break;
        
        if(args == 1) {
            SkinSegment1(frame,pStorage);
        } else
            SkinSegment2(frame,kernel,pStorage);
        char c = cvWaitKey(33);
        if( c == 27 ) break;

    }
    cvReleaseCapture( &capture );
    cvDestroyWindow( "Webcam" );
}

inline void SkinSegment1(IplImage *frame, CvMemStorage *pStorage) {
    Mat hsv,bw,tmp;
    Mat src(frame);
    cvtColor(src, hsv, CV_BGR2HSV);
    inRange(hsv, Scalar(0, 10, 60), Scalar(20, 150, 255), bw);
    erode(bw,tmp,Mat());
    //dilate(tmp,bw,Mat());
    IplImage copy = tmp;
    cvShowImage( WINDOW_NAME, &copy);
}

inline void SkinSegment2(IplImage *frame, Mat& kernel, CvMemStorage *pStorage) {
    uint8_t* pixelPtr = (uint8_t*)frame->imageData;
    char r,g,b;
    float cr,cb,tmp;
    Mat conv(frame->height, frame->width, CV_32FC1);
    float *convPtr = (float *)conv.data;
    float *convEnd = (float *)conv.dataend;
    Mat filter, final;
    while( convPtr < convEnd ) {
        //r = pixelPtr[i*frame->widthStep*cn + j*cn + 0];
        r = *pixelPtr;
        ++pixelPtr;
        g = *pixelPtr;
        ++pixelPtr;
        b = *pixelPtr;
        ++pixelPtr;
        //convert from rgb to ycbcr and subtract mean, ignore y, taken from wikipedia page on ycbcr
        cb = -0.16874f * r - 0.33126f * g + 0.50000f * b  + 128.0f - B_MEAN;
        cr =  0.50000f * r - 0.41869f * g - 0.08131f * b  + 128.0f - R_MEAN;
        //matrix multiplication [cr , cb] * inv(cov) * [cr ; cb]
        tmp = cr * (COV_11 * cr + COV_21 * cb) + cb * (COV_12 * cr + COV_22 * cb);
        *convPtr = float(exp(-0.5f * tmp) / (2.0f*PI*pow(COV_DET,5.0e-1)));
        ++convPtr;
    }
    filter2D(conv,filter,-1,kernel,cvPoint(-1,-1),0.0f,BORDER_DEFAULT);
    float *filterPtr = (float *)filter.data;
    float *filterEnd = (float *)filter.dataend;
    float *max = max_element(filterPtr,filterEnd);
    //printf("%f\n",*max);
    while( filterPtr < filterEnd ) {
        *filterPtr = (*filterPtr / *max);
        ++filterPtr;
    }
    threshold(filter,final,THRESH,1,THRESH_BINARY);
    dilate(final,filter,Mat());
    IplImage copy = filter;
    cvShowImage( WINDOW_NAME, &copy);
}