#include <iostream>
#include "opencv2/opencv.hpp"  // OpenCV's C++ Interface

using namespace cv;
using namespace std;

#if !defined VK_ESCAPE
    #define VK_ESCAPE 0x1B      // Escape character (27)
#endif

const char *windowName = "CameraAccess";   // GUI window name.
const int CAMERA_WIDTH = 640;
const int CAMERA_HEIGHT = 480;


int main(int argc, char *argv[])
{
    Mat cameraFrame;
 
    cout << "Basic video access, December 2014." << endl;
    cout << "Compiled with OpenCV version " << CV_VERSION << endl;
    cout << endl;

    /// Access camera.
    VideoCapture vc(argv[1]);

    /// Check if the file was opened properly
    if(!vc.isOpened())
    {
		cout << "Capture could not be opened succesfully" << endl;
		return -1;
    }

    vc.set(CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
    vc.set(CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);

    /// Create a GUI window for display on the screen.
    namedWindow(windowName);  


    while (true) {
       
        vc >> cameraFrame;

        // Check if the video is over
        if(cameraFrame.empty())
        {
                cout << "Video over" << endl;
                break;
        }

        /// Do processing here....

        /// Display camera frames
        imshow(windowName, cameraFrame);

        /// Wait for at least 20 ms, so that the image can be displayed on the screen!
        char keypress = waitKey(20);  

        if (keypress == VK_ESCAPE || keypress == 'q') {   // Escape Key or q
            break; // quit
        }

      
     }//end while
    destroyAllWindows();
    return EXIT_SUCCESS;
}
/*
$ ./VideoAccess ~/Videos/TheElderScrolls.mp4
Basic video access, December 2014.
Compiled with OpenCV version 3.0.0-dev
*/

