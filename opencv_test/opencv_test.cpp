// opencv_test.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"

#include "opencv2\opencv.hpp"

#include <vector>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

void displayText(string text, int x, int y, Mat &drawing)
{
	putText(drawing, text, cvPoint(x, y), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250));
}

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

int getArea(int xmin, int xmax, int ymin, int ymax, Mat &frameBlackWhite)
{
	int area = 0;

	if (xmin >= 0 && xmax < frameBlackWhite.size().width && ymin >= 0 && ymax < frameBlackWhite.size().height)
	{
		for (int j = ymin; j < ymax; j++)
		{
			for (int i = xmin; i < xmax; i++)
			{
				bool pixel = frameBlackWhite.at<bool>(j, i);
				
				area = pixel ? area + 1 : area;
			}
		}
	}

	return area;
}

string getResult(double rapF, double rapPerimeterArea)
{
	if (rapF > 200.0)
	{
		return "Caillou";
	}
	else if (rapPerimeterArea > 1.0)
	{
		return "Ciseaux";
	}
	else
	{
		return "Papier";
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

	// namedWindow("BlackWhite", CV_WINDOW_AUTOSIZE);
	namedWindow("Gray", CV_WINDOW_AUTOSIZE);
	namedWindow("Blur", CV_WINDOW_AUTOSIZE);
	namedWindow("BlackWhite", CV_WINDOW_AUTOSIZE);
	namedWindow("Canny", CV_WINDOW_AUTOSIZE);
	namedWindow("FindContours", CV_WINDOW_AUTOSIZE);
	namedWindow("Result", CV_WINDOW_AUTOSIZE);

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
		Mat frameBlur;
		Mat frameBlackWhite;
		Mat frameCanny;

		Mat frameGrayResize;
		Mat frameBlurResize;
		Mat frameBlackWhiteResize;
		Mat frameCannyResize;

		cvtColor(frame, frameGray, CV_BGR2GRAY);
		blur(frameGray, frameBlur, Size(9, 9));
		threshold(frameBlur, frameBlackWhite, 80, 255, THRESH_BINARY);

		// calcul de l'air (calculer le nombre de pixels blancs)

		// imshow("BlackWhite", frameGray);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		Canny(frameBlackWhite, frameCanny, thresh, thresh * 2, 3);

		findContours(frameCanny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		int index = 0;
		vector<Point> bestContour;
		int perimeter = getPerimeter(contours, index, bestContour);
		int xmin = INT_MAX, xmax = -INT_MAX, ymin = INT_MAX, ymax = -INT_MAX;
		getHandArea(bestContour, xmin, xmax, ymin, ymax);
		int area = getArea(xmin, xmax, ymin, ymax, frameBlackWhite);

		// rapport aire / périmètre
		double rap1 = (double)area / (double)((xmax - xmin) * (ymax - ymin) - area);
		double rap2 = (double)area / (double)perimeter;

		// std::cout << perimeter << std::endl;

		// cout << "contours size: " << contours.size() << endl;
		Mat frameFindContours = Mat::zeros(frameCanny.size(), CV_8UC3);
		Mat drawing = Mat::zeros(frameCanny.size(), CV_8UC3);
		
		Mat frameFindContoursResize;
		Mat drawingResize;

		for( int i = 0; i< contours.size(); i++ )
	    {
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
			drawContours(frameFindContours, contours, i, color, 2, 8, hierarchy, 0, Point());
        }
		
		// Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		Scalar color = Scalar(200, 200, 250);
		drawContours(drawing, contours, index, color, 2, 8, hierarchy, 0, Point());

		double rapF = rap1 * rap2;

		displayText("Perimeter = " + std::to_string(perimeter), 10, 20, drawing);
		displayText("Area = " + std::to_string(area), 10, 50, drawing);
		displayText("Rapport pixel blanc noir = " + std::to_string(rap1), 10, 80, drawing);
		displayText("Rapport perimeter area = " + std::to_string(rap2), 10, 110, drawing);
		displayText("Rapport final = " + std::to_string(rapF), 10, 140, drawing);
		displayText("Result = " + getResult(rapF, rap1), 10, 170, drawing);

		rectangle(drawing, cvPoint(xmin, ymin), cvPoint(xmax, ymax), cvScalar(255, 0, 0));

		// resize
		// old 640x480
		// new 320x240 or 160x120
		Size newSize = Size(500, 340);

		resize(frameGray, frameGrayResize, newSize);
		resize(frameBlur, frameBlurResize, newSize);
		resize(frameBlackWhite, frameBlackWhiteResize, newSize);
		resize(frameCanny, frameCannyResize, newSize);
		resize(frameFindContours, frameFindContoursResize, newSize);
		resize(drawing, drawingResize, newSize);

		imshow("Gray", frameGrayResize);
		imshow("Blur", frameBlurResize);
		imshow("BlackWhite", frameBlackWhiteResize);
		imshow("Canny", frameCannyResize);
		imshow("FindContours", frameFindContoursResize);
		imshow("Result", drawingResize);

		int offsetX = newSize.width + 30;
		int offsetY = newSize.height + 50;

		moveWindow("Gray", 0, 0);
		moveWindow("Blur", offsetX, 0);
		moveWindow("BlackWhite", 2 * offsetX, 0);
		moveWindow("Canny", 0, offsetY);
		moveWindow("FindContours", offsetX, offsetY);
		moveWindow("Result", 2 * offsetX, offsetY);

		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}

