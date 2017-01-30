#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

const char *winName = "Image Display";

int main(int argc, char** argv)
{
	cv::Mat img;
	img = cv::imread("C:\\Users\\cobalt\\Desktop\\Dovahkin.jpg", cv::IMREAD_COLOR);
	if (img.empty()) {
		std::cout << "Couldn't open image!" << std::endl;
		exit(1);
	}
	cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
	cv::imshow(winName, img);
	cv::waitKey(0);
	return 0;
}
