#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include <iostream>
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h> 
using namespace cv;
using namespace std;

/*
g++ video_d_e.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o video_d_e


./video_d_e to run. 
*/

/*
 * KL (April 17, 2017): Function that performs a series of filtering operations on a image loaded from a file.
 */
void image_filtering()
{
	// Allows user to load image from file
	cout << "Showing a  list of files in the working directory.\n\n";
	cout << "Type in the name of the file you want to open.\n\n";
	string file_name = "";
	// Shows the file in the working directory	
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
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

	// VideoCapture variable that loads the video from a video file
	VideoCapture input_video(file_name);
	
	Size img_size(500, 500);
	
	// OpenCV Mat variable that stores the source input image
	Mat img(img_size, CV_32F);
	
	// Kernel used in filtering operations
	Mat kernel;
	int kernel_size = 5; // 5x5 kernel
	
	// OpenCV Mat variables to contain the image matrixes after each filter is applied
	Mat img_erosion(img_size, CV_32F), img_dilation(img_size, CV_32F), img_diff(img_size, CV_32F), gray_diff(img_size, CV_32F), img_diff_erosion(img_size, CV_32F);
	
	int niters = 3; // Number of iterations
	
	// The threshold value variable
	int thresh, maxValue;
	
	// OpenCV Mat variable that contains the final filtered image matrix
	Mat diff_e_thredhold(img_size, CV_32F);
	
	// OpenCV Mat variables used to cropped the image
	Mat croppedRef, croppedImage;
	
	// ----------- Filtering Operations Start Here ----------------------------
	
	for(;;)
	{
		// Reads the video file and stores the image matrix into an OpenCV Mat variable
		if(!input_video.read(img))
		{
			destroyWindow("Video File");
			break;
		}

		// Resizes the image to make it fit the screen
		resize(img, img, img_size, 0, 0, INTER_AREA);
		
		// Cropping Region of Interest (ROI)
		Rect cropROI((int)(img_size.width*0.25), (int)(img_size.height*0.45),(int)(img_size.width*(0.75-0.25)),(int)(img_size.height*(0.8-0.45)));
		
		// Creates a reference to the image region, but does not also copy the image data
		croppedRef = img(cropROI);

		// Copies the image data inside the cropping region to a new matrix
		croppedRef.copyTo(croppedImage);
		
		// Sets up the kernel
		kernel = Mat::ones( kernel_size, kernel_size, CV_32F )/ (float)(kernel_size*kernel_size);
		
		// Performs Erosion on the image
		erode(croppedImage, img_erosion, kernel, Point(-1,-1), niters);
		
		// Performs Dilation on the image
		dilate(croppedImage, img_dilation, kernel, Point(-1,-1), niters);
		
		// Computes the difference between the Image Dilation and Image Erosion and stores it
		img_diff = img_dilation - img_erosion;
		
		// Sets the image difference to grayscale
		cvtColor(img_diff, gray_diff, COLOR_BGR2GRAY, 0);
		
		// Performs an additional Erosion on the image
		erode(gray_diff, img_diff_erosion, kernel, Point(-1,-1), niters);
		
		thresh = 50;
        maxValue = 255;
		
		// Performs thresholding on the eroded image difference
        threshold(img_diff_erosion,diff_e_thredhold,thresh,maxValue,THRESH_BINARY);

		// Shows both the original video and the filtered video in new windows
		imshow("Original Video File", croppedImage);
		imshow("Filtered Video File", diff_e_thredhold);

		if (waitKey(30) >= 0 )
		{
			destroyAllWindows();
			input_video.release();	
		}		
	}
}

// Main function - Kevin Lai
int main()
{
	for(;;)
	{
		image_filtering();

		if (waitKey(30) >= 0 )
			break;	
	}
}