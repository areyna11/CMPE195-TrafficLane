#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include <iostream>
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <vector>
using namespace cv;
using namespace std;

/*
g++ pixelTest.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o pixelTest


./pixelTest to run. 
*/

// Main function - Kevin Lai
int main(){

	Size img_size(500, 500);

	Mat img1(img_size, CV_32F), img2(img_size, CV_32F);

	img1 = imread("5.jpg", CV_LOAD_IMAGE_COLOR);
	img2 = imread("3.jpg", CV_LOAD_IMAGE_COLOR);

	// OpenCV Mat variables used to cropped the image
	Mat croppedRef1,croppedRef2, croppedImage1, croppedImage2;
	
	// Cropping Region of Interest (ROI)
	Rect cropROI1((int)(img1.size().width*0.3), (int)(img1.size().height*0.4),(int)(img1.size().width*(0.7-0.3)),(int)(img1.size().height*(0.8-0.4)));
		
	// Cropping Region of Interest (ROI)
	Rect cropROI2((int)(img2.size().width*0.3), (int)(img2.size().height*0.4),(int)(img2.size().width*(0.7-0.3)),(int)(img2.size().height*(0.8-0.4)));

	// Creates a reference to the image region, but does not also copy the image data
	croppedRef1 = img1(cropROI1);

	// Copies the image data inside the cropping region to a new matrix
	croppedRef1.copyTo(croppedImage1);

	// Creates a reference to the image region, but does not also copy the image data
	croppedRef2 = img2(cropROI2);

	// Copies the image data inside the cropping region to a new matrix
	croppedRef2.copyTo(croppedImage2);
	
	Vec3f firstMatrix, secondMatrix, diffMatrix;
	
	for(int i = 0; i < croppedImage2.size().width; i++){
		for(int j = 0; j < croppedImage2.size().height; j++){
			
			firstMatrix = croppedImage1.at<Vec3f>(j, i);
			secondMatrix = croppedImage2.at<Vec3f>(j, i);
			
			// Computes the difference between two image matrixes
			diffMatrix = firstMatrix - secondMatrix;
			
			// Prints the individual RGB values for each pixel in the matrix
			cout << "R: " << diffMatrix[0] << ", G: " << diffMatrix[1] << ", B: " << diffMatrix[2] << "\n" << endl;
			
		}
	
	}
}
	
