#pragma once

#include <Windows.h>
#include <opencv2/core.hpp>

using cv::Point;

class ScreenPainter
{
protected:
	ScreenPainter();
	~ScreenPainter();
	static HDC hdc;
	static HPEN pen;

public:
	ScreenPainter(ScreenPainter const&) = delete;
	void operator=(ScreenPainter const&) = delete;

	static ScreenPainter* GetInstance();
	void DrawCircle(cv::Point center, int radius);
	void DrawText(cv::Point position, std::wstring text);
	void Clear();
	static HWND overlayWindow;
};