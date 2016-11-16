//OpenCV 3.1
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/cuda.hpp"
#include "opencv2/cudaarithm.hpp"


using namespace cv;
using namespace cv::cuda;

int main (int argc, char* argv[])
{
    try
    {
	//load image from file 
        cv::Mat src_host = cv::imread("image.png", IMREAD_UNCHANGED);
        cv::cuda::GpuMat dst, src;
        src.upload(src_host);

        cv::cuda::threshold(src, dst, 128.0, 255.0, THRESH_BINARY);

        cv::Mat result_host;
        dst.download(result_host);
        //cv::imshow("Result", result_host);
        //cv::waitKey();
    }
    catch(const cv::Exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return 0;
}
