#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV's C++ Interface
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#if !defined VK_ESCAPE
    #define VK_ESCAPE 0x1B      // Escape character (27)
#endif

const char *windowName = "Canny Edge";   // GUI window name.
const int CAMERA_WIDTH = 640;
const int CAMERA_HEIGHT = 480;



void processImage(Mat srcColor, Mat &dst, int lowThresh, int highThresh){
    Size size = srcColor.size();
    Mat gray = Mat(size, CV_8UC1);
    Mat edge = Mat(size, CV_8UC1);

    cvtColor(srcColor, gray, COLOR_BGR2GRAY);
    //GaussianBlur(gray, gray, Size(7,7), 1.5, 1.5); 
    Canny(gray, edge, lowThresh, highThresh);
    srcColor.copyTo(dst, edge);

    stringstream s;
    s << "LowThresh: " << lowThresh <<" ; HighThresh: "<< highThresh;
    putText(dst, s.str(), cv::Point(10, size.height - 10), FONT_HERSHEY_PLAIN, 1.5, Scalar(255,255,255));    
}


Mat append2(Mat srca, Mat srcb, bool isHorizontal){
   const int border = 2;
   int width = 0, height = 0;

   Size a = srca.size();
   Size b = srcb.size();

   if(isHorizontal){
        width = border + a.width + border + b.width + border;
        height = border + max(a.height, b.height) + border;      
   }
   else {
        width = border + max(a.width, b.width) + border;
        height = border + a.height + border + b.height + border;
   }

   Size c = Size(width, height);
   Mat out = Mat(c, srca.type());

   if (isHorizontal) {
         srca.copyTo(Mat(out, Rect(border, border, a.width, a.height)));
         srcb.copyTo(Mat(out, Rect(border + a.width + border, border, b.width, b.height)));
	}
   else {
	srca.copyTo(Mat(out, Rect(border, border, a.width, a.height)));
	srcb.copyTo(Mat(out, Rect(border, border + a.height + border, b.width, b.height)));
	}
   return out;
}


Mat scale2Half(Mat srcColor){
    Size halfSize;
    halfSize.width = srcColor.size().width/2;
    halfSize.height = srcColor.size().height/2;
    Mat halfImg = Mat(halfSize, CV_8UC3);
    resize(srcColor, halfImg, halfSize, 0,0, INTER_LINEAR);
    return halfImg;
}


Mat append4(Mat srca, Mat srcb, Mat srcc, Mat srcd){
   const int border = 0;
   int width = 0, height = 0;

   Size a = srca.size();
 
   width = border + a.width + border + a.width + border;
   height = border + a.height + border + a.height + border;      

   Size e = Size(width, height);
   Mat out = Mat(e, srca.type());
   
   Mat srcb_3, srcc_3, srcd_3;
   srcb.convertTo(srcb_3, CV_8UC3);
   srcc.convertTo(srcc_3, CV_8UC3);
   srcd.convertTo(srcd_3, CV_8UC3);

   srca.copyTo(Mat(out, Rect(border, border, a.width, a.height)));
   srcb_3.copyTo(Mat(out, Rect(border, border + a.height + border, a.width, a.height)));
   srcc_3.copyTo(Mat(out, Rect(border + a.width + border, border, a.width, a.height)));
   srcd_3.copyTo(Mat(out, Rect(border + a.width + border, border + a.height + border, a.width, a.height)));
   return out;
}





int main(int argc, char *argv[])
{
    Mat cameraFrame, rawFrame, displayedFrameA, displayedFrameB, displayedFrameC, outputFrame;
    bool isHorizontal = true;    
 
    cout << windowName << " Demo, December 2014." << endl;
    cout << "Compiled with OpenCV version " << CV_VERSION << endl;
    cout << endl;

    /// Access video.
    VideoCapture inputVideo(argv[1]);

    /// Check if the file was opened properly
    if(!inputVideo.isOpened())
    {
		cout << "Capture could not be opened succesfully" << endl;
		return -1;
    }

     // Get size of frames
       Size frameSize = Size((int) inputVideo.get(CAP_PROP_FRAME_WIDTH), (int) inputVideo.get(CAP_PROP_FRAME_HEIGHT));

      // Make a video writer object and initialize it
      // VideoWriter put("./Video/output.mpg", VideoWriter::fourcc('M','J','P','G'), 30, frameSize); doesn't work
      // VideoWriter put("./Video/output.mpg", VideoWriter::fourcc('D','I','V','X'), 30, frameSize); // DivX MPEG-4 codec
      // VideoWriter put("./Video/output.avi",VideoWriter::fourcc('P','I','M','1'), 30, frameSize);// bad quality
       VideoWriter put("./Video/output.mpg", VideoWriter::fourcc('M','P','E','G'), 30, frameSize); // works

	if(!put.isOpened())
	{
		cout << "File could not be created for writing. Check permissions" << endl;
		return -1;
	}

    /// Create a GUI window for display on the screen.
    namedWindow(windowName);  

    while (true) {
       
        inputVideo >> cameraFrame;
        cameraFrame.copyTo(rawFrame);
 
        if(cameraFrame.empty())
        {
                cout << "Video is over!!!" << endl;
                break;
        }

        /// Do processing here.... 
        displayedFrameA = Mat(cameraFrame.size(), CV_8UC1);
        displayedFrameB = Mat(cameraFrame.size(), CV_8UC1);
        displayedFrameC = Mat(cameraFrame.size(), CV_8UC1);

        processImage(cameraFrame, displayedFrameA, 2, 50);
        processImage(cameraFrame, displayedFrameB, 20, 100);
        processImage(cameraFrame, displayedFrameC, 200, 250);
  
        //put.write(displayedFrame);
        //cout<< "displayedFrame.type() = "<< displayedFrame.type() <<endl; // 16 or CV_8UC3 
      
        stringstream text;
        text << "Raw Video";
        putText(rawFrame, text.str(), cv::Point(10, cameraFrame.size().height - 10), FONT_HERSHEY_PLAIN, 1.5, Scalar(0,0,0)); 
           
        /// Display output camera frames        
        //outputFrame = append2(cameraFrame, displayedFrame, isHorizontal);
        outputFrame = append4( scale2Half(rawFrame),
                               scale2Half(displayedFrameA),
                               scale2Half(displayedFrameB),
                               scale2Half(displayedFrameC));
        //cout<< "outputFrame.type() = "<< outputFrame.type() <<endl; // 16 or CV_8UC3
        imshow(windowName, outputFrame);  
        put << outputFrame;      

        /// Wait for at least 20 ms, so that the image can be displayed on the screen!
        char keypress = waitKey(1);  

        if (keypress == VK_ESCAPE || keypress == 'q') {   // Escape Key or q
            break; // quit
        }
     }//end while
    destroyAllWindows();
    return EXIT_SUCCESS;
}
