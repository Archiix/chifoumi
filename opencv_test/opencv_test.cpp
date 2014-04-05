// opencv_test.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

#include "opencv2\opencv.hpp"

#include <vector>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

int thresh = 230;
int max_thresh = 255;
RNG rng(12345);

int _tmain(int argc, _TCHAR* argv[])
{
	VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	namedWindow("BlackWhite", CV_WINDOW_AUTOSIZE);
	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	while (1)
	{
		Mat frame;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		// process
		Mat frameGray;
		cvtColor(frame, frameGray, CV_BGR2GRAY);
		blur(frameGray, frameGray, Size(9,9));
		threshold(frameGray, frameGray, 80, 255, THRESH_BINARY);

		imshow("BlackWhite", frameGray);

		Mat canny_output;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		Canny(frameGray, canny_output, thresh, thresh * 2, 3);

		findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		cout << "contours size: " << contours.size() << endl;

		Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
		for( int i = 0; i< contours.size(); i++ )
	    {
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
		    drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
         }

		imshow("MyVideo", drawing);


		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}

