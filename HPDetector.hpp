#pragma once

#include <opencv2/core.hpp>

using cv::Mat;

class HPDetector
{
protected:
	HPDetector() {
	}
	~HPDetector() {
	}
	static cv::Point center;
	static int radius;

public:
	HPDetector(HPDetector const&) = delete;
	void operator=(HPDetector const&) = delete;

	static HPDetector* GetInstance();
	int GetHPPercent();
	bool SetCircle(cv::Point center, int radius);
	cv::Point GetCircleCenter();
	int GetCircleRadius();
};