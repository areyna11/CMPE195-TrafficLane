#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv/cv.h"
#include <iostream>
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h> 
using namespace cv;
using namespace std;
/*
g++ camera_v3.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o test3


./test3 to run. 
*/
Mat img;



//function to caputre images from either the video feeds.
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	static int count = 0;	
	stringstream ss;
	ss << count;
	string img_count = ss.str();
	string path = "./images/image_" + img_count + ".jpg";
	if  ( event == EVENT_LBUTTONDOWN )
	{
		cout << "Image Captured" << endl;
		imwrite(path,img);
		++count;
	}
	if (count ==10)
		count = 0;
}

//function to load and play a video from file
void img_frm_file()
{
	namedWindow("Video File", 1);	
	setMouseCallback("Video File", CallBackFunc, NULL);	
	cout << "Showing a  list of files in the working directory.\n\n";
	cout << "Type in the name of the file you want to open (with avi or mp4 extension).\n\n";
	string file_name = "";
	//showing the file in the working directory	
	DIR *d;
	struct dirent *dir;
	d = opendir("./videos/");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			printf("%s\n", dir->d_name);
		}
		closedir(d);
	}
	cout << endl;
	cin >> file_name;
	cout << "Showing "<<file_name <<"\n\n";

	VideoCapture input_video(("./videos/" + file_name));
	for(;;)
	{
		if(!input_video.read(img))
		{
			destroyWindow("Video File");
			break;
		}

		imshow("Video File",img);
		
		if (waitKey(30) >= 0 )
		{
			destroyWindow("Video File");			
			input_video.release();			
		}		
	}
}

//function to show a live caputre from the webcam
void img_frm_live()
{
	namedWindow("Live Video Feed", 1);	
	setMouseCallback("Live Video Feed", CallBackFunc, NULL);	
	cout << "Showing a live video fee.\n\n";		
	VideoCapture input_live(0);
	for(;;)
	{
		if(!input_live.read(img))
		{
			destroyWindow("Live Video Feed");
			break;
		}
		imshow("Live Video Feed",img);
		if (waitKey(30) >= 0 )
		{
			destroyWindow("Live Video Feed");			
			input_live.release();			
		}
	}	
}


//main function
int main()
{
	int choice = 0;	

	cout << "What would you like to do?\n";
	cout << "Press 1 to show a video from file.\n";
	cout << "Press 2 to show a live video feed.\n";
	cin >> choice;
	for(;;)
	{
		if (choice == 1)
			img_frm_file();
		else if (choice == 2)
			img_frm_live();
		else
		{
			cout << "Enter 1 or 2.\n";

		}
		cout << "Press 1 to show a video from file.\n";
		cout << "Press 2 to show a live video feed.\n";
		cin >> choice;

		if (waitKey(30) >= 0 )
			break;	
	}
}
