#include <ScreenPainter.hpp>
#include <Windows.h>
#include <opencv2/opencv.hpp>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HWND ScreenPainter::overlayWindow = nullptr;
HDC ScreenPainter::hdc = nullptr;
HPEN ScreenPainter::pen = nullptr;

ScreenPainter::ScreenPainter()
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.lpszClassName = L"OverlayWindow";
	RegisterClass(&wc);

	// Create the overlay window
	overlayWindow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		L"OverlayWindow", L"Transparent Window",
		WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr,
		GetModuleHandle(nullptr), nullptr);

	// Set the window to be transparent
	SetLayeredWindowAttributes(overlayWindow, RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
	ShowWindow(overlayWindow, SW_SHOW);

	hdc = GetDC(overlayWindow);
	pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));

	HFONT hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

	SelectObject(hdc, hFont);
}

ScreenPainter::~ScreenPainter()
{
	// Clean up
	SelectObject(hdc, pen);
	ReleaseDC(overlayWindow, hdc);
	DestroyWindow(overlayWindow);
	UnregisterClass(L"OverlayWindow", GetModuleHandle(nullptr));

	HFONT oldFont = (HFONT)SelectObject(hdc, GetStockObject(SYSTEM_FONT));
	DeleteObject(oldFont);
}

ScreenPainter* ScreenPainter::GetInstance()
{
	static ScreenPainter instance;
	return &instance;
}

void ScreenPainter::DrawCircle(cv::Point center, int radius)
{
	// Draw the circle
	SelectObject(hdc, pen);
	Ellipse(hdc, center.x - radius, center.y - radius, center.x + radius, center.y + radius);
}

void ScreenPainter::DrawText(cv::Point position, std::wstring text)
{
	// Draw the text
	SetTextColor(hdc, RGB(255, 255, 255)); // Set text color to white
	SetBkMode(hdc, TRANSPARENT); // Set background mode to transparent
	TextOut(hdc, position.x, position.y, text.c_str(), static_cast<int>(text.length()));
}

void ScreenPainter::Clear()
{
	RECT rect;
	GetClientRect(overlayWindow, &rect);
	HBRUSH transparentBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hdc, &rect, transparentBrush);
	DeleteObject(transparentBrush);
}