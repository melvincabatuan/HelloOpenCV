#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;

const char *windowName = "Background Segmentation";   // GUI window name.

static void help()
{
 printf("\nDo background segmentation, especially demonstrating the use of cvUpdateBGStatModel().\n"
"Learns the background at the start and then segments.\n"
"Learning is togged by the space key. Will read from file or camera\n"
"Usage: \n"
"			./bgfg_segm [--camera]=<use camera, if this key is present>, [--file_name]=<path to movie file> \n\n");
}


Mat scale2Half(Mat srcColor){
    Size halfSize;
    halfSize.width = srcColor.size().width/2;
    halfSize.height = srcColor.size().height/2;
    Mat halfImg = Mat(halfSize, CV_8UC3);
    resize(srcColor, halfImg, halfSize, 0,0, INTER_LINEAR);
    return halfImg;
}


Mat convertToColoredGray(Mat gray){

  if (gray.type() != CV_8UC1)
     return gray;

  vector<Mat> images(3);

  Mat blue = Mat::zeros(gray.size(), gray.type());
  Mat red = Mat::zeros(gray.size(), gray.type());

  images.at(0) = blue;
  images.at(1) = gray;
  images.at(2) = red;

  Mat color;
  merge(images, color);
  return color;
}



Mat append4(Mat srca, Mat srcb, Mat srcc, Mat srcd){

   int width = 0, height = 0;

   Size a = srca.size(); 
   int OUT_width = 2 * a.width;
   int OUT_height = 2 * a.height;  
   Size outSize = Size(OUT_width, OUT_height);    
   Mat out = Mat(outSize, srca.type());
   
   srca.copyTo(Mat(out, Rect(0, 0, a.width, a.height)));
   convertToColoredGray(srcb).copyTo(Mat(out, Rect(0, a.height, a.width, a.height)));
   convertToColoredGray(srcc).copyTo(Mat(out, Rect(a.width, 0, a.width, a.height)));
   convertToColoredGray(srcd).copyTo(Mat(out, Rect(a.width, a.height, a.width, a.height)));
   return out;
}





const char* keys =
{
    "{c  camera   |         | use camera or not}"
    "{m  method   |mog2     | method (knn or mog2) }"
    "{s  smooth   |         | smooth the mask }"
    "{fn file_name|../data/tree.avi | movie file        }"
};

//this is a sample for foreground detection functions
int main(int argc, const char** argv)
{
    help();

    CommandLineParser parser(argc, argv, keys);
    bool useCamera = parser.has("camera");
    bool smoothMask = parser.has("smooth");
    string file = parser.get<string>("file_name");
    string method = parser.get<string>("method");
    VideoCapture cap;
    bool update_bg_model = true;

    if( useCamera )
        cap.open(0);
    else
        cap.open(file.c_str());

    parser.printMessage();

    if( !cap.isOpened() )
    {
        printf("can not open camera or video file\n");
        return -1;
    }



     /// Set background subtractor object 
     Ptr<BackgroundSubtractor> bg_model = method == "knn" ?
            createBackgroundSubtractorKNN().dynamicCast<BackgroundSubtractor>() :
            createBackgroundSubtractorMOG2().dynamicCast<BackgroundSubtractor>();

    

    /// Set VideoWriter object
    Size frameSize = Size((int) cap.get(CAP_PROP_FRAME_WIDTH), (int) cap.get(CAP_PROP_FRAME_HEIGHT));
    VideoWriter put("./backgroundSubtraction.mpg", VideoWriter::fourcc('M','P','E','G'), 30, frameSize); // works

	if(!put.isOpened())
	{
		cout << "File could not be created for writing. Check permissions" << endl;
		return -1;
	}

 
   Mat img0, img, fgmask, fgimg, outputFrame;

    for(;;)
    {
        cap >> img0;

        if( img0.empty() )
            break;

        /// Reduce to 1/4 of the original size
        resize(img0, img, Size(frameSize.width/2, frameSize.height/2), INTER_LINEAR);

        if( fgimg.empty() )
          fgimg.create(img.size(), img.type());

        /// Update the background model
        bg_model->apply(img, fgmask, update_bg_model ? -1 : 0);
        if( smoothMask )
        {
            GaussianBlur(fgmask, fgmask, Size(25, 25), 3.5, 0);
            threshold(fgmask, fgmask, 60, 255, THRESH_BINARY);
        }

        /// Get foreground image
        fgimg = Scalar::all(0);
        img.copyTo(fgimg, fgmask);

        /// Get background image
        Mat bgimg;
        bg_model->getBackgroundImage(bgimg);

     
        stringstream text1, text2, text3, text4;
        text1 << "Raw Video";
        putText(img, text1.str(), cv::Point(10, img.size().height - 10), FONT_HERSHEY_PLAIN, 1.5, Scalar(0,255,0)); 
        text2 << "Mask";
        putText(fgmask, text2.str(), cv::Point(10, img.size().height - 10), FONT_HERSHEY_PLAIN, 1.5, Scalar(255,255,255));
        text3 << "Foreground";
        putText(fgimg, text3.str(), cv::Point(10, img.size().height - 10), FONT_HERSHEY_PLAIN, 1.5, Scalar(0,255,0));
        text4 << "Background";
        putText(bgimg, text4.str(), cv::Point(10, img.size().height - 10), FONT_HERSHEY_PLAIN, 1.5, Scalar(0,255,0));
           
        /// Display output camera frames        
         outputFrame = append4( img,
                               fgmask,
                               fgimg,
                               bgimg); 
        
         imshow(windowName, outputFrame);  
         put << outputFrame;   
       
       

        char k = (char)waitKey(30);
        if( k == 27 ) break;
        if( k == ' ' )
        {
            update_bg_model = !update_bg_model;
            if(update_bg_model)
                printf("Background update is on\n");
            else
                printf("Background update is off\n");
        }
    }

    return 0;
}

