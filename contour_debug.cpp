#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

const char *winName = "Webcam";

int main(int argc, char** argv)
{
	cv::Mat frame, gray, edges;
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

	cv::RNG rng(12345);

	cv::Mat drawing;
	std::vector<std::vector<cv::Point2f> > contours;
	std::vector<cv::Vec4i > hierarchy;

	cv::namedWindow(winName);
	while (1) {
		cap >> frame;
		if (frame.empty()) {
			std::cerr << "ERROR: No more frames " << std::endl;
			exit(1);
		}
		cv::cvtColor(frame, gray, CV_BGR2GRAY);
		cv::Canny(gray, edges, 60, 180);
		
		cv::findContours(edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point());
		//drawing = cv::Mat::zeros(edges.size(), CV_8UC3);
		//for (int i = 0; i < contours.size(); ++i) {
		//	cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//	cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
		//}
		cv::imshow(winName, edges);
		char keypress = cv::waitKey(25);
		if (keypress == 'q') {
			break;
		}
	}
	cvDestroyAllWindows();
	return 0;
}
