#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv/cv.h"
#include <iostream>
using namespace cv;
using namespace std;

VideoCapture feed(0); 
Mat frame;
cv::VideoWriter output(
		"my_clip.avi",
		CV_FOURCC('X','V','I','D'),
		25,
		cv::Size(feed.get(CV_CAP_PROP_FRAME_WIDTH),
		feed.get(CV_CAP_PROP_FRAME_HEIGHT)));
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	

	if  ( event == EVENT_LBUTTONDOWN )
	{
		cout << "Recording" << endl;
		for(;;)
		{
			feed.read(frame);
			imshow("feed", frame);
			output.write(frame);
			if (waitKey(30) >= 0)
				break;
		}		
		
		
	}
	
}
int main()
{
	namedWindow("feed", 1);
	setMouseCallback("feed", CallBackFunc, NULL);
	//unconditional loop
	for(;;)
	{
		feed.read(frame);
		imshow("feed", frame);
		
		if (waitKey(30) >= 0)
			break;
	}
	return 0;
}