#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV's C++ Interface

using namespace cv;
using namespace std;

#if !defined VK_ESCAPE
    #define VK_ESCAPE 0x1B      // Escape character (27)
#endif

const char *windowName = "LaplacianwThreshold";   // GUI window name.
const int CAMERA_WIDTH = 640;
const int CAMERA_HEIGHT = 480;

/// Get access to the webcam.
void initWebcam(VideoCapture &videoCapture, int cameraNumber)
{
    /// Get access to the default camera.
    try {   
        videoCapture.open(cameraNumber);
    } catch (cv::Exception &e) {}
    if ( !videoCapture.isOpened() ) {
        cerr << "ERROR: Could not access the camera!" << endl;
        exit(1);
    }
    cout << "Successfully loaded camera " << cameraNumber << "." << endl;
}




void processImage(Mat srcColor, Mat &dst){

    Size size = srcColor.size();
    Mat gray = Mat(size, CV_8U);
    Mat edges = Mat(size, CV_8U);
 
    cvtColor(srcColor, gray, COLOR_BGR2GRAY);

    const int LAPLACIAN_FILTER_SIZE = 5;
    Laplacian(gray, edges, CV_8U, LAPLACIAN_FILTER_SIZE);

    const int EDGES_THRESHOLD = 80;
    threshold(edges, dst, EDGES_THRESHOLD, 255, THRESH_BINARY_INV);

}



int main(int argc, char *argv[])
{
    Mat cameraFrame, displayedFrame;
 
    cout << windowName << " Demo, December 2014." << endl;
    cout << "Compiled with OpenCV version " << CV_VERSION << endl;
    cout << endl;

    int cameraNumber = 0;    
    if (argc > 1) {
        cameraNumber = atoi(argv[1]);
    }

    /// Access camera.
    VideoCapture camera;
    initWebcam(camera, cameraNumber);

    camera.set(CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
    camera.set(CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);

    /// Create a GUI window for display on the screen.
    namedWindow(windowName);  


    while (true) {
       
        camera >> cameraFrame;

        if( cameraFrame.empty() ) {
            cerr << "ERROR: Couldn't grab the next frame." << endl;
            exit(1);
        }

        /// Do processing here.... 
        displayedFrame = Mat(cameraFrame.size(), CV_8UC3);
        processImage(cameraFrame, displayedFrame);           

        /// Display output camera frames
        imshow(windowName, displayedFrame);

        /// Wait for at least 20 ms, so that the image can be displayed on the screen!
        char keypress = waitKey(20);  

        if (keypress == VK_ESCAPE) {   // Escape Key
            break; // quit
        }
     }//end while
    return EXIT_SUCCESS;
}

/*
$ ./LaplacianwThresholdDemo 
LaplacianwThreshold Demo, December 2014.
Compiled with OpenCV version 3.0.0-dev
*/
