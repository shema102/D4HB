#include <HPDetector.hpp>
#include <ScreenCapturer.hpp>
#include <iostream>
#include <CircleDetector.hpp>

HPDetector* HPDetector::GetInstance()
{
	static HPDetector instance;
	return &instance;
}

cv::Point HPDetector::center = cv::Point(0, 0);
int HPDetector::radius = 0;

bool HPDetector::SetCircle(cv::Point center, int radius)
{
	HPDetector::center = center;
	HPDetector::radius = radius;

	return true;
}

int findEdgeLocation(cv::Mat& image) {
	for (int y = 5; y < image.rows - 6; y++)
	{
		int intensity = image.at<cv::Vec3b>(y, 0)[2];
		if (intensity > 200)
		{
			return image.rows - y;
		}
	}

	return image.rows;
}

int HPDetector::GetHPPercent()
{
	if (radius == 0)
		return -1;

	Mat screenshot = ScreenCapturer::GetInstance()->TakeScreenshot(center.x, center.y - radius, 1, radius * 2);

	cv::Mat canny = CannyImage(screenshot, 80, 20);

	int edgeLocation = findEdgeLocation(canny);
	std::cout << "Edge location: " << edgeLocation << std::endl;
	int height = screenshot.rows;
	std::cout << "Height: " << height << std::endl;

	return edgeLocation * 100 / height;
}

cv::Point HPDetector::GetCircleCenter()
{
	return center;
}

int HPDetector::GetCircleRadius()
{
	return radius;
}
