#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
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
g++ video_filter_cuda.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o video_filter_cuda


./video_filter_cuda to run. 
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

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

int main()
{
	namedWindow("Live Video Feed", 1);	

	cout << "Showing a live video feed.\n\n";		
	VideoCapture input_live(0);
	
	Size img_size(500, 500);	
	// OpenCV Mat variable that stores the source input image
	Mat img(img_size, CV_8U, Scalar(0,0,0));

	// Read image from file and store in Mat img
	//img = imread(file_name, CV_LOAD_IMAGE_COLOR);

	// Kernel used in filtering operations
	Mat_<int> kernel(3,3);
	kernel  << 1,1,1,1,1,1,1,1,1;
	int kernel_size = 3; // 3x3 kernel

		

	// Sets up the kernel
	//cout << "Before Kernel.\n\n";	
		
	//kernel = Mat::ones(kernel_size, kernel_size, CV_8U)/
			  //(kernel_size*kernel_size);

	// OpenCV Mat variables to contain the image matrixes after each filter is applied
	Mat img_erosion(img_size, CV_8U, Scalar(0,0,0)),
	    img_dilation(img_size, CV_8U, Scalar(0,0,0)),
	    img_diff(img_size, CV_8U, Scalar(0,0,0)),
	    gray_diff(img_size, CV_8U, Scalar(0,0,0)),
	    img_diff_erosion(img_size, CV_8U, Scalar(0,0,0));

	int niters = 3; // Number of iterations

	// The threshold value variable
	
	int thresh = 50;
	int maxValue = 255;

	// OpenCV Mat variable that contains the filtered image matrix before floodfill
	Mat diff_e_threshold(img_size, CV_8U, Scalar(0,0,0));

	// OpenCV Mat variables used to cropped the image
	Mat croppedRef(img_size, CV_8U, Scalar(0,0,0)),
	    croppedImage(img_size, CV_8U, Scalar(0,0,0));

	// OpenCV Mat Array that contains the individual RGB arrays of the image.
	// Each color is stored in its own Mat Variable in the array. Default MAT channel format = BGR.
	Mat colors[3];

	// OpenCV Mat variable that contains the filtered image matrix after floodfill
	Mat finalImage(img_size, CV_8U, Scalar(0,0,0)),
	    finalRef(img_size, CV_8U, Scalar(0,0,0));

	// CUDA GpuMat variables used to store uploaded OpenCV Mat variables that will be used in GPU filtering operations
	//All GPUMats changed to CV_8U, for them to work in the Erode, Dilate GPU functions 
	GpuMat img_src(img_size, CV_8U), croppedImage_src(img_size, CV_8U) , NewcroppedImage_src(img_size, CV_8U), diff_e_threshold_src(img_size, CV_8U);

	// CUDA GpuMat variables to upload OpenCV Mat variables into for performing filtering operations on the GPU
	//All GPUMats changed to CV_8U, for them to work in the Erode, Dilate GPU functions
	GpuMat img_erosion_src(img_size, CV_8U), img_dilation_src(img_size, CV_8U), img_diff_src(img_size, CV_8U), gray_diff_src(img_size, CV_8U), img_diff_erosion_src(img_size, CV_8U);	
	for(;;)
	{
	// ----------- Filtering Operations Start Here ----------------------------

		if(!input_live.read(img))
		{
			destroyWindow("Live Video Feed");
			break;
		}		

		img_src.upload(img);

		// Resizes the image to make it fit the screen
		//cout << "Before Resisze.\n\n";		
		gpu::resize(img_src, img_src, img_size, 0, 0,INTER_LINEAR);

		img_src.download(img);

		// Cropping Region of Interest (ROI)
		Rect cropROI((int)(img_size.width*0.25),
			    (int)(img_size.height*0.45),
			    (int)(img_size.width*(0.75-0.25)),
			    (int)(img_size.height*(0.8-0.45)));

		// Creates a reference to the image region, but does not also copy the image data

		//cout << "Before uploads.\n\n";	
		croppedRef = img(cropROI);

		// Copies the image data inside the cropping region to a new matrix
		croppedRef.copyTo(croppedImage);

	
		//string ty =  type2str( croppedImage.type() );
		//printf("Matrix: %s %dx%d \n", ty.c_str(), croppedImage.cols, croppedImage.rows );

		//cout << "Before split.\n\n";
	
		// Splits the image matrix into 3 separate color arrays. 1 for R, 1 for G, and 1 for B.		
		cv::split(croppedImage, colors);
		
		croppedImage_src.upload(croppedImage);

		//ty =  type2str( croppedImage.type() );
		//printf("Matrix: %s %dx%d \n", ty.c_str(), croppedImage.cols, croppedImage.rows );

		// Performs Erosion on the image
		//cout << "Before erode.\n\n";
		//changed the croppedImage_src from a 3 channel (8UC3 to 8UC4, the expected input for the erode/dilate functions)
		gpu::cvtColor(croppedImage_src, NewcroppedImage_src, CV_BGR2BGRA, 1);
		//ty =  type2str( greycroppedImage_src.type() );
		//printf("Matrix: %s %dx%d \n", ty.c_str(), greycroppedImage_src.cols, greycroppedImage_src.rows );

		gpu::erode(NewcroppedImage_src, img_erosion_src, kernel, Point(-1,-1), niters);

		//cout << "After erode.\n\n";	

		// Performs Dilation on the image
		//cout << "Before dilate.\n\n";
	
		gpu::dilate(NewcroppedImage_src, img_dilation_src, kernel, Point(-1,-1), niters);

		//cout << "After Dilate.\n\n";	

		//cout << "Before differences.\n\n";	
		img_erosion_src.download(img_erosion);
		img_dilation_src.download(img_dilation);
		// Computes the difference between the Image Dilation and Image Erosion and stores it
		img_diff = img_dilation - img_erosion;

		img_diff_src.upload(img_diff);

//string ty =  type2str( img_diff_src.type() );
		//printf("Matrix: %s %dx%d \n", ty.c_str(), //img_diff_src.cols, img_diff_src.rows );


		//cout << "Before cvtColor.\n\n";	
		// Sets the image difference to grayscale
		//Change from a 4 channel to a singe channel 8U mat, (CV8UC4 -> CV8UC1)		
		
		gpu::cvtColor(img_diff_src, gray_diff_src, CV_BGRA2GRAY, 1);

		//cout << "After cvtColor.\n\n";
	
		//cout << "Before erode-2.\n\n";

		// Performs an additional Erosion on the image
		gpu::erode(gray_diff_src, img_diff_erosion_src, kernel, Point(-1,-1), niters);

		//cout << "After erode-2.\n\n";	
		//cout << "Before erode-2.\n\n";

		// Performs thresholding on the eroded image difference
		gpu::threshold(img_diff_erosion_src, diff_e_threshold_src, thresh, maxValue, THRESH_BINARY);

		//diff_e_threshold = Mat(diff_e_threshold_src);
		diff_e_threshold_src.download(diff_e_threshold);

		niceImageK = diff_e_threshold.clone();

		namedWindow("FloodFill Image File", 1);
		cv::setMouseCallback("FloodFill Image File", onMouse, 0);

		// Shows both the original image and the filtered image in new windows
		imshow("Original Image File", croppedImage);
		imshow("Filtered Image File", diff_e_threshold);
		imshow("Live Video Feed",img);
		if (waitKey(30) >= 0 )
			break;	

	}
}
