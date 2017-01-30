#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

const char *colorWinName = "Color Display";
const char *grayWinName = "Gray Display";

int main(int argc, char** argv)
{
	cv::Mat img, gray;
	img = cv::imread("C:\\Users\\cobalt\\Desktop\\Dovahkin.jpg", cv::IMREAD_COLOR);
	if (img.empty()) {
		std::cout << "Couldn't open image!" << std::endl;
		exit(1);
	}
	cv::cvtColor(img, gray, CV_BGR2GRAY);
	cv::namedWindow(colorWinName, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(grayWinName, cv::WINDOW_AUTOSIZE);
	cv::imshow(colorWinName, img);
	cv::imshow(grayWinName, gray);
	cv::waitKey(0);
	return 0;
}
