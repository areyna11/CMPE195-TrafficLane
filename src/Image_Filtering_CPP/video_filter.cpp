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

/*
g++ video_filter.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o video_filter


./image_filter to run. 
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

/*
 * Main function - Kevin Lai
 * KL (April 17, 2017): Function that performs a series of filtering operations on a image loaded from a file.
 * KL (April 20, 2017): Modified previous version to now only load image from file for easy testing. Also integrated flood_fill filtering.
 */
int main()
{
	
/*		namedWindow("Live Video Feed", 1);	
	
		cout << "Showing a live video fee.\n\n";		
		VideoCapture input_live(0);*/
	namedWindow("Video File", 1);	
	
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


		Size img_size(500, 500);
	
		// OpenCV Mat variable that stores the source input image
		Mat img(img_size, CV_32F);
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
		for(;;)
	{
		/*if(!input_live.read(img))
		{
			destroyWindow("Live Video Feed");
			break;
		}*/

		if(!input_video.read(img))
		{
			destroyWindow("Video File");
			break;
		}
		// Read image from file and store in Mat img
		//img = imread(file_name, CV_LOAD_IMAGE_COLOR);
	

	
		// ----------- Filtering Operations Start Here ----------------------------
	
		// Resizes the image to make it fit the screen
		resize(img, img, img_size, 0, 0, INTER_AREA);
		
		// Cropping Region of Interest (ROI)
		Rect cropROI((int)(img_size.width*0.25),
			    (int)(img_size.height*0.45),
			    (int)(img_size.width*(0.75-0.25)),
			    (int)(img_size.height*(0.8-0.45)));
		
		// Creates a reference to the image region, but does not also copy the image data
		croppedRef = img(cropROI);

		// Copies the image data inside the cropping region to a new matrix
		croppedRef.copyTo(croppedImage);
		
		// Splits the image matrix into 3 separate color arrays. 1 for R, 1 for G, and 1 for B.
		
split(croppedImage, colors);


		// Sets up the kernel
		kernel = Mat::ones(kernel_size, kernel_size, CV_32F)/
				  (int)(kernel_size*kernel_size);
		
		// Performs Erosion on the image
		erode(croppedImage, img_erosion, kernel, Point(-1,-1), niters);
		
		// Performs Dilation on the image
		dilate(croppedImage, img_dilation, kernel, Point(-1,-1), niters);
		
		// Computes the difference between the Image Dilation and Image Erosion and stores it
		img_diff = img_dilation - img_erosion;
		
		// Sets the image difference to grayscale
		cvtColor(img_diff, gray_diff, COLOR_BGR2GRAY, 1);
		
		// Performs an additional Erosion on the image
		erode(gray_diff, img_diff_erosion, kernel, Point(-1,-1), niters);
		
		thresh = 50;
        	maxValue = 255;
		// Performs thresholding on the eroded image difference
        	threshold(img_diff_erosion, diff_e_threshold, thresh, maxValue, THRESH_BINARY);
			
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
