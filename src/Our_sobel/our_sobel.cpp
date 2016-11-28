#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
using namespace std;
using namespace cv;
using namespace cv::gpu;
int main()
{
	//CPU variables to have the frames in CPU memory
	Mat img;
	Mat dst;
	//GPU variables to allocate them into GPU memory	
	GpuMat d_img(img);
	GpuMat d_dst;
	
	//CV function to hold the video that will be proccessed
	VideoCapture input("vid_trim.mp4");
	
	//CV video writer function that encodes the video into avi format, at the same size of the 
	//original video
	cv::VideoWriter output(
	"sobel_lane.avi",
	CV_FOURCC('X','V','I','D'),
	30,
	cv::Size(input.get(CV_CAP_PROP_FRAME_WIDTH),
	input.get(CV_CAP_PROP_FRAME_HEIGHT))
	);
		
	
	for(;;)
	{
		if(!input.read(img))
			break;
		//trasferring the CPU img and loading it into GPU memory		
		d_img.upload(img);
		//GPU, CUDA enhanced sobel/2d convolution filterting done on every inputted 			// video frame, done on 5*5 kernel		
		Sobel(d_img,d_dst,CV_8U,1,1,5);
		// get the procceses image and trasnfers it from GPU to CPU memory		
		d_dst.download(dst);
		//writes the processed frame into the new video file		
		output.write(dst);
		//shows the img from the unproccessed video		
		imshow("img",img);
		char c = waitKey(30);
		if(c == ' ')
			break;
	}
}

// g++ our_sobel.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -o test
