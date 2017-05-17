#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv/cv.h"
#include <iostream>
#include <time.h>
using namespace cv;
using namespace std;
int main()
{
	//timer variables
	time_t end;
	time_t start= time(0);
	
	//CPU variables to have the frames in CPU memory
	Mat img;
	//CV function to hold the video that will be proccessed
	VideoCapture input(0);
	//CV video writer function that encodes the video into avi format, at the same size of the 
	//original video
	cv::VideoWriter output(
	"my_clip.avi",
	CV_FOURCC('M','J','P','G'),
	15,
	cv::Size(640,480));
	for(;;)
	{
		if(!input.read(img))
			break;
		//shows the img from the unproccessed video		
		imshow("Video Capture",img);		
		//writes the processed frame into the new video file		
		output.write(img);
		end = time(0);
		if (waitKey(30) >= 0 || (difftime(end,start) > 120))
			break;
	}
}