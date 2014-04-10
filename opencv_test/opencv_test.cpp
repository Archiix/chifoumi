// opencv_test.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

#include "opencv2\opencv.hpp"

#include <vector>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

int getPerimeter(vector<vector<Point> > &contours, int &index, vector<Point> &bestContour)
{
	int max = -INT_MAX;
	for (int i = 0; i < contours.size(); i++)
	{
		int contourSize = contours[i].size();
		if (contourSize > max)
		{
			max = contourSize;
			index = i;
			bestContour = contours[i];
		}
	}
	return max;
}

void getHandArea(vector<Point> &bestContour, int &xmin, int &xmax, int &ymin, int &ymax)
{
	for (int i = 0; i < bestContour.size(); i++)
	{
		if (bestContour[i].x < xmin) xmin = bestContour[i].x;
		if (bestContour[i].x > xmax) xmax = bestContour[i].x;
		if (bestContour[i].y < ymin) ymin = bestContour[i].y;
		if (bestContour[i].y > ymax) ymax = bestContour[i].y;
	}
}

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

		// calcul de l'air (calculer le nombre de pixels blancs)

		imshow("BlackWhite", frameGray);

		Mat canny_output;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		Canny(frameGray, canny_output, thresh, thresh * 2, 3);

		findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		int index = 0;
		vector<Point> bestContour;
		int perimeter = getPerimeter(contours, index, bestContour);
		int xmin = INT_MAX, xmax = -INT_MAX, ymin = INT_MAX, ymax = -INT_MAX;
		getHandArea(bestContour, xmin, xmax, ymin, ymax);
		// std::cout << perimeter << std::endl;

		// cout << "contours size: " << contours.size() << endl;

		Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
		/*
		for( int i = 0; i< contours.size(); i++ )
	    {
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
		    drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
         }
		 */
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, index, color, 2, 8, hierarchy, 0, Point());

		putText(drawing, "Perimeter: " + std::to_string(perimeter), cvPoint(10, 20), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250));
		rectangle(drawing, cvPoint(xmin, ymin), cvPoint(xmax, ymax), cvScalar(255, 0, 0));

		imshow("MyVideo", drawing);


		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}

