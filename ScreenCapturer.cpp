#include <ScreenCapturer.hpp>
#include <iostream>
#include <windows.h>
#include <gdiplus.h>
#include <opencv2/core.hpp>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

ULONG_PTR ScreenCapturer::m_gdiplusToken = NULL;

ScreenCapturer::ScreenCapturer() {
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

ScreenCapturer::~ScreenCapturer() {
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

ScreenCapturer* ScreenCapturer::GetInstance() {
	static ScreenCapturer instance;
	return &instance;
}


cv::Mat ScreenCapturer::TakeScreenshot(int x, int y, int regionWidth, int regionHeight)
{
	// Get screen dimensions
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Set region dimensions to screen dimensions if not specified
	if (regionWidth < 0)
	{
		regionWidth = screenWidth;
	}
	if (regionHeight < 0)
	{
		regionHeight = screenHeight;
	}

	// Create GDI+ Bitmap
	HDC hdcScreen = GetDC(NULL);

	HDC hdcMemory = CreateCompatibleDC(hdcScreen);

	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, regionWidth, regionHeight);

	HGDIOBJ oldObject = SelectObject(hdcMemory, hBitmap);

	BitBlt(hdcMemory, 0, 0, regionWidth, regionHeight, hdcScreen, x, y, SRCCOPY);

	Bitmap bitmap(hBitmap, NULL);

	// Convert GDI+ Bitmap to cv::Mat
	BitmapData bitmapData;
	Rect rect(0, 0, bitmap.GetWidth(), bitmap.GetHeight());

	bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

	cv::Mat screenMat(bitmap.GetHeight(), bitmap.GetWidth(), CV_8UC4);

	if (bitmapData.Scan0 && screenMat.data) {
		std::copy(static_cast<BYTE*>(bitmapData.Scan0),
			static_cast<BYTE*>(bitmapData.Scan0) + bitmapData.Stride * bitmap.GetHeight(),
			screenMat.data);
		bitmap.UnlockBits(&bitmapData);
	}
	else {
		std::cerr << "Error: Null pointer encountered during memory copy." << std::endl;
	}

	// Unlock the GDI+ Bitmap
	bitmap.UnlockBits(&bitmapData);

	// Cleanup
	SelectObject(hdcMemory, oldObject);
	DeleteObject(hBitmap);
	ReleaseDC(NULL, hdcScreen);
	DeleteDC(hdcMemory);

	return screenMat;
}