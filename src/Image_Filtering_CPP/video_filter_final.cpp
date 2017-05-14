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
#include "stack.h"
using namespace cv;
using namespace std;

/*
g++ image_d_e.cpp stack.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o image_d_e


./image_d_e to run. 
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

int threshold_r = 150;       // for red  
int threshold_g = 150;       // for green    
int threshold_b = 150;       // for blue  
int threshold_size = 2;      // for size   

int threshold_size_l = 10;	// for lower threshold bound
int threshold_size_h = 50;	// for upper threshold bound

bool flood_fill_done = false;
bool flood_filter_done = false;

void printB(int r, int c)
{
	int x, y;

        printf("Binary image\n"); 

	for (x=0; x<r; x++) { // for each row in binary image B
	for (y=0; y<c; y++) {// for each column in binary image B
        printf(" %4d", zero_crossing_data[x][y]);
	}
	printf("\n");
}

	printf("Colour image\n");

        for (x=0; x<r; x++) {       // for each row in binary image B
        for (y=0; y<c; y++) {    // for each column in binary image B
        printf(" %4d", red_array[x][y]);  //red image array  
        }
        printf("\n");		
}

} 

// HL April 22, 2017: floodfill engine from my 8 connected neighbours 
//     from my paperMill project. Starting below
  
//  The flood fill is updated to allow the size of each individual 
//  region to be counted for. HL June5, 2015 
//   
void flood_fill(int r, int c){
    unsigned long itemp, jtemp;
    int count = 0;  // to count number of pixels adjecent to each other, 
                    // to be used for thresholding colour regions  

    for(i = 0; i < r ; i++){
       for (j = 0; j < c ; j++) {
          if (zero_crossing_data[i][j] == 255){ // seek seed poit of a cluster  
            //if(label == 254) label++;  //HL Check this again ???  
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
    //}
    flood_fill_done = true;
}


//
// HL: Modify this code to make it for tape inspection, Jun 5, 2015  
//     
/*
bool is_color(int y, int x){
    if(     image->data[y][(x*3)] > 120     && image->data[y][(x*3)] < 210
        &&  image->data[y][(x*3)+1] > 25    && image->data[y][(x*3)+1] < 120
        &&  image->data[y][(x*3)+2] < 50    )   return true;
    return false;
}
*/

// 
//  Modify this code to make it for tape inspection 
//  HL: 2015-6-5 
//

// Filtering: 
//  1. when the r,g,b colour intensity checked for color vs. colour threshold 
//  2. when the size of each colour region of the binary image checked for size threhold 
//     the filtering is applied, e.g., to turn foreground to the background 
 
void flood_filter(int r, int c){
    unsigned int topx, topy=0, bottomx, bottomy=1000; 
    unsigned int leftx=1000, lefty, rightx=0, righty;

        //printf("threshold_r %d\n", threshold_r);
        //printf("threshold_size %d\n", threshold_size);

    for(i = 0; i < r ; i++){
       for (j = 0; j < c ; j++) {
            //if(flood_size[label] > flood_threshold ){
            //if(red_array[zero_crossing_data[i][j]] > threshold && is_color(i,j)){

            if((red_array[i][j] >= threshold_r)               //keep if (1) red above 
               && ( green_array[i][j] >= threshold_g)		  //keep if green above
               && ( blue_array[i][j] >= threshold_b)		  //keep if blue above  
               && ( flood_size[zero_crossing_data[i][j]] >= threshold_size_l)
			   && ( flood_size[zero_crossing_data[i][j]] <= threshold_size_h) ) { //and (2) size above  
            }
            else zero_crossing_data[i][j] = 0;
        } // double for loop  
    }
    flood_filter_done = true;
}
// HL floodfill engine for 8 connected neighbours ends here 

/*
 * Main function - Kevin Lai
 * KL (April 17, 2017): Function that performs a series of filtering operations on a image loaded from a file.
 * KL (April 20, 2017): Modified previous version to now only load image from file for easy testing. Also integrated flood_fill filtering.
 */
int main()
{
		Size img_size(500, 500);
	
		// OpenCV Mat variable that stores the source input image
		Mat img(img_size, CV_32F, Scalar(0,0,0));
	
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
		Mat finalImage(img_size, CV_32F, Scalar(0,0,0));
	
		// Parameter passed to floodfill operation.
		Rect rectRegion;
		
		// Point to start floodfill at
		Point mPoint;
		
		// Mode of floodfill operation. Default mode = 4 connected-neighbors.
		int connected_neighbors_mode = 8;		
	
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
		
		VideoCapture input_video((file_name));
	
	for(;;)
	{
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
		threshold_b = 100;      // for blue
		threshold_g = 100;      // for green
		threshold_r = 100;      // for red  
		threshold_size_l = 10;  // for size 
		threshold_size_h = 500;	
		
		// Performs floodfill filtering
		flood_fill(diff_e_threshold.rows, diff_e_threshold.cols);
		flood_filter(diff_e_threshold.rows, diff_e_threshold.cols);
		
		rectRegion = Rect(Point(), finalImage.size());
		
		for (int x = 0; x < finalImage.rows; x++){
			for (int y = 0; y < finalImage.cols; y++){
				
				if(zero_crossing_data[x][y] == 0){
					
					mPoint = Point(x, y);
					
					if(rectRegion.contains(mPoint)){
						// Performs the floodfill filter operation on the cluster.
						floodFill(finalImage, mPoint, Scalar(0), &rectRegion, Scalar(), Scalar(), connected_neighbors_mode);
					}
				}

			}
		}

		// Shows both the original cropped image and the filtered image in new windows
		imshow("Original Cropped Image File", croppedImage);
		imshow("Filtered Image File", diff_e_threshold);
		imshow("FloodFill Image File", finalImage);
		imshow("Video File", img);
			
		if (waitKey(30) >= 0 )
			break;	

	}
}