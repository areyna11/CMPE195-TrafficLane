#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include <iostream>
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
using namespace cv;
using namespace std;

// For OpenCV 2.4
using namespace cv::gpu;

// For OpenCV 3.0
// using namespace cv::cuda;

/*
g++ image_filter_cuda.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o image_filter_cuda


./image_filter_cuda to run. 
*/


Mat niceImageK;

/*
 * Mouse Callback function - Kevin Lai
 * KL (April 29, 2017): Function that peforms the floodfill filtering after user clicks on an area to fill.
 */
void onMouse(int event, int x, int y, int flags, void *param)
{
	// If left mouse button is clicked, then execute.
	if(event == EVENT_LBUTTONDOWN) {
		
		// Parameter passed to floodfill operation.
		Rect rectRegion;
		
		// Point to start floodfill at
		Point mousePoint(x, y);
		
		// Mode of floodfill operation. Default mode = 4 connected-neighbors.
		int connected_neighbors_mode = 8;
		 
		// Performs the floodfill filter operation on the clicked on cluster.
		floodFill(niceImageK, mousePoint, Scalar(0), &rectRegion, Scalar(), Scalar(), connected_neighbors_mode);
    }
	
	// Displays the floodfilled image
	imshow("FloodFill Image File", niceImageK);
}

/*
 * Main function - Kevin Lai
 * KL (April 17, 2017): Function that performs a series of filtering operations on a image loaded from a file.
 * KL (April 20, 2017): Modified previous version to now only load image from file for easy testing. Also integrated flood_fill filtering.
 */
int main()
{
	for(;;)
	{
		// Allows user to load image from file
		cout << "Showing a list of files in the working directory.\n\n";
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
	
		Size img_size(500, 500);
	
		// OpenCV Mat variable that stores the source input image
		Mat img(img_size, CV_32F, Scalar(0,0,0));
	
		// Read image from file and store in Mat img
		img = imread(file_name, CV_LOAD_IMAGE_COLOR);
	
		// Kernel used in filtering operations
		Mat kernel;
		int kernel_size = 5; // 5x5 kernel
	
		// OpenCV Mat variables to contain the image matrixes after each filter is applied
		Mat img_erosion(img_size, CV_32F, Scalar(0,0,0)),
		    img_dilation(img_size, CV_32F, Scalar(0,0,0)),
		    img_diff(img_size, CV_32F, Scalar(0,0,0)),
		    gray_diff(img_size, CV_32F, Scalar(0,0,0)),
		    img_diff_erosion(img_size, CV_32F, Scalar(0,0,0));
	
		int niters = 3; // Number of iterations
	
		// The threshold value variable
		int thresh, maxValue;
	
		// OpenCV Mat variable that contains the filtered image matrix before floodfill
		Mat diff_e_threshold(img_size, CV_32F, Scalar(0,0,0));

		// OpenCV Mat variables used to cropped the image
		Mat croppedRef(img_size, CV_32F, Scalar(0,0,0)),
		    croppedImage(img_size, CV_32F, Scalar(0,0,0));
	
		// OpenCV Mat Array that contains the individual RGB arrays of the image.
		// Each color is stored in its own Mat Variable in the array. Default MAT channel format = BGR.
		Mat colors[3];
	
		// OpenCV Mat variable that contains the filtered image matrix after floodfill
		Mat finalImage(img_size, CV_32F, Scalar(0,0,0)),
		    finalRef(img_size, CV_32F, Scalar(0,0,0));
	
		// CUDA GpuMat variables used to store uploaded OpenCV Mat variables that will be used in GPU filtering operations 
		GpuMat img_src, croppedImage_src, diff_e_threshold_src;
	
		// CUDA GpuMat variables to upload OpenCV Mat variables into for performing filtering operations on the GPU
		GpuMat img_erosion_src, img_dilation_src, img_diff_src, gray_diff_src, img_diff_erosion_src;
	
	
		// ----------- Filtering Operations Start Here ----------------------------
	
		img_src.upload(img);
	
		// Resizes the image to make it fit the screen
		gpu::resize(img_src, img_src, img_size, 0, 0, INTER_AREA);
		
		img = Mat(img_src);
		
		// Cropping Region of Interest (ROI)
		Rect cropROI((int)(img_size.width*0.25),
			    (int)(img_size.height*0.45),
			    (int)(img_size.width*(0.75-0.25)),
			    (int)(img_size.height*(0.8-0.45)));
		
		// Creates a reference to the image region, but does not also copy the image data
		croppedRef = img(cropROI);

		// Copies the image data inside the cropping region to a new matrix
		croppedRef.copyTo(croppedImage);
		
		croppedImage_src.upload(croppedImage);
		
		// Splits the image matrix into 3 separate color arrays. 1 for R, 1 for G, and 1 for B.
		cv::split(croppedImage, colors);
		
		// Sets up the kernel
		kernel = Mat::ones(kernel_size, kernel_size, CV_32F)/
				  (int)(kernel_size*kernel_size);
		
		// Performs Erosion on the image
		gpu::erode(croppedImage_src, img_erosion_src, kernel, Point(-1,-1), niters);
		
		// Performs Dilation on the image
		gpu::dilate(croppedImage_src, img_dilation_src, kernel, Point(-1,-1), niters);
		
		img_erosion = Mat(img_erosion_src);
		img_dilation = Mat(img_dilation_src);
		
		// Computes the difference between the Image Dilation and Image Erosion and stores it
		img_diff = img_dilation - img_erosion;
		
		img_diff_src.upload(img_diff);
		
		// Sets the image difference to grayscale
		gpu::cvtColor(img_diff_src, gray_diff_src, COLOR_BGR2GRAY, 1);
		
		// Performs an additional Erosion on the image
		gpu::erode(gray_diff_src, img_diff_erosion_src, kernel, Point(-1,-1), niters);
		
		thresh = 50;
        	maxValue = 255;
		
		// Performs thresholding on the eroded image difference
        	gpu::threshold(img_diff_erosion_src, diff_e_threshold_src, thresh, maxValue, THRESH_BINARY);
		
		diff_e_threshold = Mat(diff_e_threshold_src);
		
		niceImageK = diff_e_threshold.clone();
		
		namedWindow("FloodFill Image File", 1);
		cv::setMouseCallback("FloodFill Image File", onMouse, 0);
		
		// Shows both the original image and the filtered image in new windows
		imshow("Original Image File", croppedImage);
		imshow("Filtered Image File", diff_e_threshold);
		
		waitKey(0);
		
	}
}