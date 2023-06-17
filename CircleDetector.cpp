#include <CircleDetector.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;



cv::Mat ExtractRedChannel(cv::Mat image)
{
	cv::Mat redChannel;
	cv::extractChannel(image, redChannel, 2);

	return redChannel;
}

std::vector<cv::Vec3f> DetectCircles(cv::Mat image, int minRadius, int maxRadius, int scale)
{
	Mat redChannel = ExtractRedChannel(image);

	vector<cv::Vec3f> circles;
	cv::HoughCircles(redChannel, circles, cv::HOUGH_GRADIENT, 1, image.rows / (2 * scale), 200, 50, minRadius / scale, maxRadius / scale);

	return circles;
}

cv::Mat CannyImage(cv::Mat image, int threshold1, int threshold2)
{

	cv::Mat gray;
	cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	cv::Mat blurred;
	cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 1.5);

	cv::Mat canny;

	cv::Canny(blurred, canny, threshold1, threshold2, 3, true);

	return canny;
}
