#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV's C++ Interface

using namespace cv;
using namespace std;

#if !defined VK_ESCAPE
    #define VK_ESCAPE 0x1B      // Escape character (27)
#endif

const char *OutputWindowName = "Harris Corners";   // GUI output window name.
const char *InputWindowName = "Raw Video";   // GUI output window name.
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

    Mat gray = Mat(srcColor.size(), CV_8U);
    Mat edges = Mat(srcColor.size(), CV_8U);
    Mat tmp_norm = Mat(srcColor.size(), CV_8U);
    Mat tmp = Mat::zeros(srcColor.size(), CV_32FC1);
 
    /// Convert to grayscale
    cvtColor( srcColor, gray, COLOR_BGR2GRAY );

    /// Detect corners
    cornerHarris( gray, tmp, 7, 5, 0.05, BORDER_DEFAULT );
 
    /// Normalize
    normalize( tmp, tmp_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( tmp_norm, dst );
  
    const int threshold = 150;
     /// Draw a circle around corners
    for( int j = 0; j < tmp_norm.rows ; j++ )
    { 
        for( int i = 0; i < tmp_norm.cols; i++ )
        { 
           if( (int) tmp_norm.at<float>(j,i) > threshold )
                circle( dst, Point( i, j ), 5,  Scalar(0,0,255), 2, 8, 0 );    
        }
    }
}



int main(int argc, char *argv[])
{
    Mat cameraFrame, displayedFrame;
 
    cout << OutputWindowName << " Demo, December 2014." << endl;
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
    namedWindow(OutputWindowName);  
    namedWindow(InputWindowName);

    while (true) {
       
        camera >> cameraFrame;

        if( cameraFrame.empty() ) {
            cerr << "ERROR: Couldn't grab the next frame." << endl;
            exit(1);
        }

        /// Do processing here.... 
        displayedFrame = Mat(cameraFrame.size(), CV_8UC3);
        processImage(cameraFrame, displayedFrame); 

        /// Display raw camera frames
        imshow(InputWindowName, cameraFrame);          

        /// Display output camera frames
        imshow(OutputWindowName, displayedFrame);

        /// Wait for at least 20 ms, so that the image can be displayed on the screen!
        char keypress = waitKey(20);  

        if (keypress == VK_ESCAPE) {   // Escape Key
            break; // quit
        }
     }//end while
    destroyAllWindows();
    return EXIT_SUCCESS;
}

/*
$ ./HarrisCorners 
Harris Corners Demo, December 2014.
Compiled with OpenCV version 3.0.0-dev
//SLOWWWWWW.....

*/
