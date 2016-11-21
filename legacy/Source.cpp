#include <stdio.h>
#include <iostream>
#include <time.h>
#include <string.h>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"

#include <raspicam/raspicam_cv.h>


using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

bool try_raspicam = false;
int main(int, char**)
{

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
	{
		printf("VideoCapture was not opened!\n");
		try_raspicam = true;
	}
	if (try_raspicam == true)
	{
		printf("Using raspicam for Raspberry Pi camera module OpenCV input");
/*
		raspicam::RaspiCam_Cv Camera;
		Mat image;
		Camera.set(cv::CAP_PROP_FORMAT, CV_8UC1);
*/
	
		return 0;
	}
	else
		return -1;


	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    Mat edges;
//    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
//        imshow("edges", edges);
		
//		src = edges;
//		string matAsString (src.begin<unsigned char>(), src.end<unsigned char>());

		string matAsString (edges.begin<unsigned char>(), \
							edges.end<unsigned char>());

		cout << matAsString << endl;

//		cout << edges.tostring() << endl;
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
