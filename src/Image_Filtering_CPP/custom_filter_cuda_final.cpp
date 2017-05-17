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
#include "stack.h"
using namespace cv;
using namespace std;

// For OpenCV 2.4
using namespace cv::gpu;

/*
g++ custom_filter_cuda_final.cpp stack.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o custom_filter_cuda_final

./custom_filter_cuda_final to run. 
*/

#define ROWS	500	// number of rows in images
#define COLUMNS	500	// number of columns in images

int zero_crossing_data[ROWS][COLUMNS];  // binary image to be floodfilled  
int red_array[ROWS][COLUMNS];           // red image plane 
int green_array[ROWS][COLUMNS];         // green image plane 
int blue_array[ROWS][COLUMNS];          // blue image plane 
 
int flood_size[ROWS*COLUMNS];           //to count area size for filtering    

int label = 0;                          //to colour each region  
int i,j;    

int threshold_r = 150;     // for red  
int threshold_g = 150;     // for green    
int threshold_b = 150;     // for blue  
int threshold_size = 2;    // for size   

int threshold_size_l = 10;	// for lower threshold bound
int threshold_size_h = 50;	// for upper threshold bound

bool flood_fill_done = false;
bool flood_filter_done = false;
  
void flood_fill(int r, int c){
    unsigned long itemp, jtemp;
    int count = 0;  // to count number of pixels adjecent to each other, 
                    // to be used for thresholding colour regions  

    for(i = 0; i < r ; i++){
       for (j = 0; j < c ; j++) {
          if (zero_crossing_data[i][j] == 255){ // seek seed poit of a cluster   
            label++;
            push(i,j);
            count++;
            zero_crossing_data[i][j] = label;
            flood_size[label] = count; //HL: 2015-6-5 to count the area (size)  
            }
                // check 8-connected neighbors
                while (pop(&itemp,&jtemp)){
                    //--- North west
                    if(zero_crossing_data[itemp-1][jtemp-1] == 255) {
                        push(itemp-1,jtemp-1);
                        count++;
                        zero_crossing_data[itemp-1][jtemp-1] = label;
                        flood_size[label] = count; //HL: 2015-6-5 
                    }
                    //--- North
                    if(zero_crossing_data[itemp-1][jtemp] == 255) {
                        push(itemp-1,jtemp);
                        count++;
                        zero_crossing_data[itemp-1][jtemp] = label;
                        flood_size[label] = count; //HL: 2015-6-5 
                    }
                    //--- North east
                    if(zero_crossing_data[itemp-1][jtemp+1] == 255) {
                        push(itemp-1,jtemp+1);
                        count++;
                        zero_crossing_data[itemp-1][jtemp+1] = label;
                        flood_size[label] = count; //HL: 2015-6-5 
                    }

                    //--- west
                    if(zero_crossing_data[itemp][jtemp-1] == 255) {
                        push(itemp,jtemp-1);
                        count++;
                        zero_crossing_data[itemp][jtemp-1] = label;
                        flood_size[label] = count; //HL: 2015-6-5 
                    }
                    //--- east
                    if(zero_crossing_data[itemp][jtemp+1] == 255) {
                        push(itemp,jtemp+1);
                        count++;
                        zero_crossing_data[itemp][jtemp+1] = label;
                        flood_size[label] = count; //HL: 2015-6-5 
                    }

                    //--- south west
                    if(zero_crossing_data[itemp+1][jtemp-1] == 255) {
                        push(itemp+1,jtemp-1);
                        count++;
                        zero_crossing_data[itemp+1][jtemp-1] = label;
                        flood_size[label] = count; //HL: 2015-6-5 
                    }
                    //--- south
                    if(zero_crossing_data[itemp+1][jtemp] == 255) {
                        push(itemp+1,jtemp);
                        count++;
                        zero_crossing_data[itemp+1][jtemp] = label;
                        flood_size[label] = count; //HL: 2015-6-5 
                    }
                    //--- south east
                    if(zero_crossing_data[itemp+1][jtemp+1] == 255) {
                        push(itemp+1,jtemp+1);
                        count++;
                        zero_crossing_data[itemp+1][jtemp+1] = label;
                        flood_size[label] = count; //HL: 2015-6-5 
                    }
                }
          count = 0;
        }
    }
    flood_fill_done = true;
}
 
void flood_filter(int r, int c){
    unsigned int topx, topy=0, bottomx, bottomy=1000; 
    unsigned int leftx=1000, lefty, rightx=0, righty;

    for(i = 0; i < r ; i++){
       for (j = 0; j < c ; j++) {
            if((red_array[i][j] >= threshold_r)               //keep if red above 
               && ( green_array[i][j] >= threshold_g)		  //keep if green above
               && ( blue_array[i][j] >= threshold_b)		  //keep if blue above  
               && ( flood_size[zero_crossing_data[i][j]] >= threshold_size_l) //and size above 
			   && ( flood_size[zero_crossing_data[i][j]] <= threshold_size_h) ) { //and size below 
            }
            else zero_crossing_data[i][j] = 0;
        } // double for loop  
    }
    flood_filter_done = true;
}

//Function that peforms the floodfill filtering after user clicks on an area to fill.
int main()
{
	namedWindow("Live Video Feed", 1);	
	cout << "Showing a live video feed.\n\n";
	VideoCapture input_live(0);
	
	Size img_size(500, 500);
	
	// OpenCV Mat variable that stores the source input image
	Mat img(img_size, CV_8U, Scalar(0,0,0));

	// Kernel used in filtering operations
	Mat_<int> kernel(3,3);
	kernel  << 1,1,1,1,1,1,1,1,1;
	int kernel_size = 3; // 3x3 kernel	

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
	Mat finalImage(img_size, CV_8U, Scalar(0,0,0));

	// CUDA GpuMat variables used to store uploaded OpenCV Mat variables that will be used in GPU filtering operations
	//All GPUMats changed to CV_8U, for them to work in the Erode, Dilate GPU functions 
	GpuMat img_src(img_size, CV_8U), croppedImage_src(img_size, CV_8U) , NewcroppedImage_src(img_size, CV_8U), diff_e_threshold_src(img_size, CV_8U);

	// CUDA GpuMat variables to upload OpenCV Mat variables into for performing filtering operations on the GPU
	//All GPUMats changed to CV_8U, for them to work in the Erode, Dilate GPU functions
	GpuMat img_erosion_src(img_size, CV_8U), img_dilation_src(img_size, CV_8U), img_diff_src(img_size, CV_8U), gray_diff_src(img_size, CV_8U), img_diff_erosion_src(img_size, CV_8U);
	
	for(;;)
	{
		if(!input_live.read(img)){
			destroyWindow("Live Video Feed");
			break;
		}
		
		// ----------- Filtering Operations Start Here ----------------------------

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
		cv::split(croppedImage, colors);
		
		croppedImage_src.upload(croppedImage);

		//changed the croppedImage_src from a 3 channel (8UC3 to 8UC4, the expected input for the erode/dilate functions)
		gpu::cvtColor(croppedImage_src, NewcroppedImage_src, CV_BGR2BGRA, 1);

		// Performs Erosion on the image
		gpu::erode(NewcroppedImage_src, img_erosion_src, kernel, Point(-1,-1), niters);

		// Performs Dilation on the image	
		gpu::dilate(NewcroppedImage_src, img_dilation_src, kernel, Point(-1,-1), niters);
		
		// Computes the difference between the Image Dilation and Image Erosion and stores it
		img_erosion_src.download(img_erosion);
		img_dilation_src.download(img_dilation);
		img_diff = img_dilation - img_erosion;
		img_diff_src.upload(img_diff);
	
		// Sets the image difference to grayscale
		//Change from a 4 channel to a singe channel 8U mat, (CV8UC4 -> CV8UC1)		
		gpu::cvtColor(img_diff_src, gray_diff_src, CV_BGRA2GRAY, 1);

		// Performs an additional Erosion on the image
		gpu::erode(gray_diff_src, img_diff_erosion_src, kernel, Point(-1,-1), niters);

		// Performs thresholding on the eroded image difference
		gpu::threshold(img_diff_erosion_src, diff_e_threshold_src, thresh, maxValue, THRESH_BINARY);

		diff_e_threshold_src.download(diff_e_threshold);
		finalImage = diff_e_threshold.clone();
		
		// Loading the floodfill arrays with the image data
		for (int x = 0; x < diff_e_threshold.rows; x++){
			for (int y = 0; y < diff_e_threshold.cols; y++){
				// Loading object array with data
				if(((int)diff_e_threshold.at<unsigned char>(x,y)*0) != 0){
					zero_crossing_data[x][y] = 0;
				}
				else if(abs((int)diff_e_threshold.at<unsigned char>(x,y)) > thresh){
					zero_crossing_data[x][y] = 255;
				}
				else{
					zero_crossing_data[x][y] = 0;
				}
				
				// Loading color arrays with image data
				blue_array[x][y] = abs((int)colors[0].at<unsigned char>(x,y));
				green_array[x][y] = abs((int)colors[1].at<unsigned char>(x,y));
				red_array[x][y] = abs((int)colors[2].at<unsigned char>(x,y));
			}
		}

		// Setting the thresholds for the floodfill filtering
		threshold_b = 100;            // for blue
		threshold_g = 100;            // for green
		threshold_r = 100;            // for red  
		threshold_size_l = 2000;      // for lower bound size 
		threshold_size_h = 5000;      // for upper bound size
		
		// Performs floodfill filtering
		flood_fill(diff_e_threshold.rows, diff_e_threshold.cols);
		flood_filter(diff_e_threshold.rows, diff_e_threshold.cols);
		
		for (int x = 0; x < finalImage.rows; x++){
			for (int y = 0; y < finalImage.cols; y++){
				// Fills the filtered pixels with no label
				if(zero_crossing_data[x][y] == 0){
					finalImage.at<unsigned char>(x,y) = 0;
				}
			}
		}
		
		namedWindow("FloodFill Video Feed", 1);

		// Shows both the original cropped video feed and the filtered video feed in new windows
		imshow("Original Cropped Video Feed", croppedImage);
		imshow("Filtered Video Feed", diff_e_threshold);
		imshow("FloodFill Video Feed", finalImage);
		imshow("Live Video Feed",img);
				
		if (waitKey(30) >= 0 )
			break;	

		}
	
}
