#include "stdafx.h"

using namespace cv;
using namespace std;

Mat process(Mat &frame)
{
	/*cv::Mat grayscaleMat(frame.size(), frame.type());
	cv::cvtColor(frame, grayscaleMat, CV_BGR2GRAY);*/
	return frame;
	/*cv::Mat binaryMat(frame.size(), frame.type());
	cv::threshold(frame, binaryMat, 130, 255, cv::THRESH_BINARY);*/

	/*cv::Mat skel(frame.size(), CV_8UC1);
	cv::Mat temp(frame.size(), CV_8UC1);

	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));*/

	/*bool done = false;
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

	// return grayscaleMat;
}

int main(int argc, const char** argv)
{
	VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); // get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); // get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	namedWindow("Chifoumi", CV_WINDOW_AUTOSIZE); // create a window called "MyVideo"

	while (1)
	{
		Mat frameLeft;
		Mat frameRight;

		bool bSuccess = cap.read(frameLeft); // read a new frame from video

		if (!bSuccess) // if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		else
		{
			cout << "test0" << endl;

			frameRight = frameLeft.clone();
			frameRight = process(frameRight);

			cout << "test1" << endl;

			// IMAGE PROCESSING
			// ...
			// IMAGE PROCESSING END

			// DISPLAYING
			IplImage iplLeft = frameLeft;
			IplImage iplRight = frameRight;
			IplImage* dst = cvCreateImage(cvSize(iplLeft.width + iplRight.width, iplLeft.height), IPL_DEPTH_8U, 3);
			cvSetImageROI(dst, cvRect(0, 0, iplLeft.width, iplLeft.height));
			cvCopy(&iplLeft, dst, NULL);
			cvResetImageROI(dst);
			cvSetImageROI(dst, cvRect(iplLeft.width, 0, iplRight.width, iplRight.height));
			cvCopy(&iplRight, dst, NULL);
			cvResetImageROI(dst);

			cvShowImage("Chifoumi", dst);
			// DISPLAYING END
		}

		if (waitKey(30) == 27) // wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;
}