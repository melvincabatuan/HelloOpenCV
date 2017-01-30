#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

const char *winName = "Webcam";

int main(int argc, char** argv)
{
	cv::Mat frame;
	cv::VideoCapture cap;
	int cameraNum = 0;
	try {
		cap.open(cameraNum);
	}
	catch (cv::Exception e) {}
	if (!cap.isOpened()) {
		std::cerr << "Couldn't open the default camera!" << std::endl;
		exit(1);
	}
	cv::namedWindow(winName);
	while (1) {
		cap >> frame;
		if (frame.empty()) {
			std::cerr << "ERROR: No more frames " << std::endl;
			exit(1);
		}
		cv::imshow(winName, frame);
		char keypress = cv::waitKey(25);
		if (keypress == 'q') {
			break;
		}
	}
	cvDestroyAllWindows();
	return 0;
}
