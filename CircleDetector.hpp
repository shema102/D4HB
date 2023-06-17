#pragma once

#include <opencv2/core.hpp>

using cv::Mat;

std::vector<cv::Vec3f> DetectCircles(Mat image, int minRadius, int maxRadius, int scale = 1);

cv::Mat CannyImage(cv::Mat image, int threshold1, int threshold2);
