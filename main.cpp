#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <windows.h>
#include <gdiplus.h>
#include <opencv2/opencv.hpp>
#include <ScreenCapturer.hpp>
#include <CircleDetector.hpp>
#include <ScreenPainter.hpp>
#include <HPDetector.hpp>

using namespace cv;


cv::Point translateRoi(cv::Point point, cv::Rect roi) {
	return cv::Point(point.x + roi.x, point.y + roi.y);
}

std::mutex mtx;
std::condition_variable pollingHP;
bool proceed = false;

std::atomic<bool> continueHPPolling(true);

void HPPollingThread(ScreenPainter* painter, HPDetector* detector) {
	int lastHP = -1;

	while (continueHPPolling) {
		std::unique_lock<std::mutex> lock(mtx);
		pollingHP.wait(lock, [] {return proceed; });

		int hp = detector->GetHPPercent();

		if (hp == lastHP) {
			Sleep(10);
			continue;
		}

		lastHP = hp;

		painter->Clear();

		cv::Point center = detector->GetCircleCenter();

		int radius = detector->GetCircleRadius();

		cv::Point middleLeft = cv::Point(center.x - radius, center.y);

		std::wstring HPStr = std::to_wstring(hp) + L"%";

		painter->DrawText(middleLeft, HPStr);

		Sleep(10);

		lock.unlock();
		std::this_thread::yield();
	}
}

std::atomic<bool> pollingKeyboard(true);

void KeyStatePollingThread(ScreenPainter* painter, ScreenCapturer* capturer, HPDetector* detector) {

	std::thread hpPollingThread(HPPollingThread, painter, detector);

	while (pollingKeyboard) {
		if (GetAsyncKeyState(VK_F6) & 0x8000) {
			std::cout << "F6 pressed" << std::endl;

			painter->Clear();

			cv::Mat screenshot = capturer->TakeScreenshot();

			int width = screenshot.cols;
			int height = screenshot.rows;

			cv::Rect roi(cv::Point(width / 4, 2 * height / 3), cv::Point(width / 2, height));

			cv::Mat cropped = screenshot(roi);

			std::vector<cv::Vec3f> circles = DetectCircles(cropped, 80, 100);

			std::cout << "Found " << circles.size() << " circles" << std::endl;

			for (cv::Vec3f circle : circles) {
				cv::Point center = translateRoi(cv::Point(circle[0], circle[1]), roi);
				int radius = circle[2];

				detector->SetCircle(center, radius);

				painter->DrawCircle(center, radius);
				std::cout << "Circle detected at: " << center << " with radius: " << radius << std::endl;
			}

			Sleep(200);
		}

		if (GetAsyncKeyState(VK_F7) & 0x8000) {
			std::cout << "F7 pressed" << std::endl;

			painter->Clear();
			Sleep(200);
		}

		if (GetAsyncKeyState(VK_F8) & 0x8000) {
			std::cout << "F8 pressed" << std::endl;

			painter->Clear();

			{
				std::lock_guard<std::mutex> lock(mtx);
				proceed = !proceed;
			}

			pollingHP.notify_one();

			Sleep(200);
		}

		Sleep(10);
	}

	hpPollingThread.join();
}

int main()
{
	ScreenPainter* painter = ScreenPainter::GetInstance();
	ScreenCapturer* capturer = ScreenCapturer::GetInstance();
	HPDetector* detector = HPDetector::GetInstance();


	std::thread keyStateThread(KeyStatePollingThread, painter, capturer, detector);

	// Message loop
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	continueHPPolling = false;
	pollingKeyboard = false;
	keyStateThread.join();

	return 0;
}