//============================================================================
// Name        : cuda_kevinlai.cu
// Author      : Kevin Lai
// Version     : 1.0
// Copyright   : Kevin Lai
// Description : Simple CUDA Program
//============================================================================

#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/core/cuda.hpp"

using namespace std;
using namespace cv;
using namespace cv::gpu;
using namespace cv::cuda;


/* 
 * OpenCV GPU module is written using CUDA.
 * Therefore, I used the OpenCV GPU libraries to load and display an image.
 * - Kevin Lai
 *
*/
int main (int argc, char* argv[])
{
	// Checks if the library is compiled without CUDA. This function does not throw exceptions if the library is compiled without Cuda.
	int num_devices = getCudaEnabledDeviceCount();
	
	// If the library is compiled without CUDA, then print out an error.
    if (num_devices == 0)
    {
        cerr << "No GPU found or the library was compiled without CUDA support";
        return -1;
    }
	
    try
    {
		// Opens the image file. Reference to image file is saved into "image" variable.
        Mat image = imread("cuda.png", CV_LOAD_IMAGE_COLOR);
        
		// Handles the image file being passed to the GPU.
		GpuMat onGPU, toGPU;
        
		// Uploads the image onto the GPU.
		toGPU.upload(image);
		
		// Sets GPU parameters.
        threshold(toGPU, onGPU, 128.0, 255.0, CV_THRESH_BINARY);

		// Grabs image from GPU to display on screen.
        Mat displayImage;
		
		// Downloads the image to display on screen.
        onGPU.download(displayImage);

		// Creates a new window for the displayed image.
		namedWindow( "CUDA", WINDOW_AUTOSIZE )
        
		// Displays the image on the new window.
		imshow("Display CUDA Logo", displayImage);
        waitKey();
    }
    catch(const Exception& ex)
    {
		// Checks for any errors and prints what error occurred.
        cout << "Error: " << ex.what() << endl;
    }
    return 0;
}