#pragma once

#include <thread>
#include <windows.h>
#include <gdiplus.h>
#include <opencv2/core.hpp>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class ScreenCapturer
{
protected:
	ScreenCapturer();
	~ScreenCapturer();
	static ULONG_PTR m_gdiplusToken;


public:
	ScreenCapturer(ScreenCapturer& other) = delete;
	void operator=(const ScreenCapturer&) = delete;

	static ScreenCapturer* GetInstance();

	cv::Mat TakeScreenshot(int x = 0, int y = 0, int regionWidth = -1, int regionHeight = -1);
};

