#include "stdafx.h"

using namespace cv;
using namespace std;

int main(int argc, const char** argv)
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

	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
	namedWindow("Video", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	while (1)
	{
		Mat frame;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}


		cv::Mat grayscaleMat(frame.size(), CV_8U);
		cv::cvtColor(frame, grayscaleMat, CV_BGR2GRAY);
		imshow("MyVideo", grayscaleMat); //show the frame in "MyVideo" window
		cv::Mat binaryMat(grayscaleMat.size(), grayscaleMat.type());
		cv::threshold(grayscaleMat, binaryMat, 130, 255, cv::THRESH_BINARY);
		imshow("Video", binaryMat); //show the frame in "MyVideo" window

		cv::Mat skel(frame.size(), CV_8UC1, cv::Scalar(0));
		cv::Mat temp(frame.size(), CV_8UC1);

		cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

		/*
		bool done;
		do
		{
			cv::morphologyEx(binaryMat, temp, cv::MORPH_OPEN, element);
			cv::bitwise_not(temp, temp);
			cv::bitwise_and(binaryMat, temp, temp);
			cv::bitwise_or(skel, temp, skel);
			cv::erode(binaryMat, binaryMat, element);

			double max;
			cv::minMaxLoc(binaryMat, 0, &max);
			done = (max == 0);
		} while (!done);

		*/







		//imshow("MyVideo", skel); //show the frame in "MyVideo" window

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}

