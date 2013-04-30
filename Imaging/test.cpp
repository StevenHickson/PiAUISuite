#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void FindFaces(IplImage *frame, CvMemStorage *pStorage, CvHaarClassifierCascade *cascade);

#define WINDOW_NAME "Webcam"

int main( int argc, char** argv ) {
    cvNamedWindow( WINDOW_NAME, CV_WINDOW_NORMAL);

    CvCapture* capture = cvCreateCameraCapture(0) ;
    IplImage* frame;
    CvMemStorage* pStorage = cvCreateMemStorage(0);
    CvHaarClassifierCascade* face_cascade = (CvHaarClassifierCascade *)cvLoad("face.xml",0,0,0);
    
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
            FindFaces(frame,pStorage,face_cascade);
        } else
            cvShowImage( WINDOW_NAME, frame );
        char c = cvWaitKey(33);
        if( c == 27 ) break;

    }
    cvReleaseCapture( &capture );
    cvDestroyWindow( "Webcam" );
}

void FindFaces(IplImage *frame, CvMemStorage *pStorage, CvHaarClassifierCascade *cascade) {
    //find faces using haar recognition
    CvSeq * pFaceRectSeq = cvHaarDetectObjects
        (frame, cascade, pStorage,
        1.1,                       // increase search scale by 10% each pa
        3,                         // merge groups of three detections
        CV_HAAR_DO_CANNY_PRUNING,  // skip regions unlikely to contain a face
        cvSize(40,40));            // smallest size face to detect = 40x40

    //draw rectangles around the face
    for(int i=0;i<(pFaceRectSeq? pFaceRectSeq->total:0); i++ ) {
        CvRect* r = (CvRect*)cvGetSeqElem(pFaceRectSeq, i);
        CvPoint pt1 = { r->x, r->y };
        CvPoint pt2 = { r->x + r->width, r->y + r->height };
        cvRectangle(frame, pt1, pt2, CV_RGB(0,255,0), 3, 4, 0);
    }
    cvShowImage( WINDOW_NAME, frame );

}
