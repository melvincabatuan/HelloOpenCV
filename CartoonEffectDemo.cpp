#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV's C++ Interface

using namespace cv;
using namespace std;

#if !defined VK_ESCAPE
    #define VK_ESCAPE 0x1B      // Escape character (27)
#endif

const char *windowName = "Cartoon Effect";   // GUI window name.
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


Mat scale2Half(Mat srcColor){
    Size halfSize;
    halfSize.width = srcColor.size().width/2;
    halfSize.height = srcColor.size().height/2;
    Mat halfImg = Mat(halfSize, CV_8UC3);
    resize(srcColor, halfImg, halfSize, 0,0, INTER_LINEAR);
    return halfImg;
}


void cartoonEffect(Mat &srcColor, int repetitions){
     Mat tmp = Mat(srcColor.size(), CV_8UC3);
     for (int i=0; i<repetitions; i++) {
        int size = 9;           // Filter size. Has a large effect on speed.
        double sigmaColor = 9;  // Filter color strength.
        double sigmaSpace = 7;  // Positional strength. Effects speed.
        bilateralFilter(srcColor, tmp, size, sigmaColor, sigmaSpace);
        bilateralFilter(tmp, srcColor, size, sigmaColor, sigmaSpace);
    }
}



void processImage(Mat srcColor, Mat &dst){

    Size size = srcColor.size();

    /// Part I: Prepare the mask

    Mat gray = Mat(size, CV_8U);
    Mat edges = Mat(size, CV_8U);
    Mat mask = Mat(size, CV_8U);
 
    cvtColor(srcColor, gray, COLOR_BGR2GRAY); // Grayscale conversion

    medianBlur(gray, gray, 7); // Remove noise by Median Filter 

    const int LAPLACIAN_FILTER_SIZE = 5;
    Laplacian(gray, edges, CV_8U, LAPLACIAN_FILTER_SIZE); // Edge detection 

    const int EDGES_THRESHOLD = 80;
    threshold(edges, mask, EDGES_THRESHOLD, 255, THRESH_BINARY_INV); // Thresholding

    
    /// Part II: Cartoon effect

    Mat smallImg = scale2Half(srcColor); // resize to half scale

    int repetitions = 4;
    cartoonEffect(smallImg, repetitions); // Perform many iterations of weak bilateral filtering  

    resize(smallImg, srcColor, size, 0,0, INTER_LINEAR); // resize to original scale.
    
    memset((char*)dst.data, 0, dst.step * dst.rows); // Clear the output image to black, 
                                              // so that the cartoon line drawings will be black.   
    srcColor.copyTo(dst, mask);
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
