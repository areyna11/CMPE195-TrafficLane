#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/gpu/gpu.hpp>
using namespace std;
using namespace cv;
using namespace cv::gpu;
int main()
{
	
	Mat img;
	Mat dst;
	GpuMat d_img(img);
	GpuMat d_dst;
	
	VideoCapture input("vid_trim.mp4");
	
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
		d_img.upload(img);
		Sobel(d_img,d_dst,CV_8U,1,1,5);
		d_dst.download(dst);
				imshow("dst",dst);
		output.write(dst);
		imshow("img",img);
		
		char c = waitKey(30);
		
		if(c == ' ')
			break;
	}
}
