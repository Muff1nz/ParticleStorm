#include "Timer.h"
#include <thread>
#include <windows.h>

Timer::Timer(double maxDeltaTime) : maxDeltaTime(maxDeltaTime) {
	nowDeltaTime = NowSeconds();
	now = NowSeconds();

	lastDeltaTime = nowDeltaTime;
	last = now;


	stopWatchRunning = false;
}

Timer::~Timer() = default;

double Timer::DeltaTime() {
	lastDeltaTime = nowDeltaTime;
	nowDeltaTime = NowSeconds();
	const double deltaTime = nowDeltaTime - lastDeltaTime;
	double result = deltaTime < maxDeltaTime ? deltaTime : maxDeltaTime;
	realTimeDifference = result / deltaTime * 100;
	return result;
}

double Timer::RealTimeDifference() const {
	return realTimeDifference >= 0.0f && realTimeDifference <= 100 ? realTimeDifference : 100.0f;
}

void Timer::Start() {
	if (stopWatchRunning) return;
	Restart();
}

void Timer::Stop() {
	stopWatchRunning = false;
	now = NowSeconds();
}

void Timer::Restart() {
	stopWatchRunning = true;
	now = NowSeconds();
	last = now;
}

double Timer::ElapsedSeconds() {
	if (stopWatchRunning)
		now = NowSeconds();
	return now - last;
}

double Timer::ElapsedMilliseconds() {
	if (stopWatchRunning)
		now = NowSeconds();
	return (now - last) * 1000.0;
}

double Timer::ElapsedMicroseconds() {
	if (stopWatchRunning)
		now = NowSeconds();
	return SecondsToMicroseconds(now - last);
}


double Timer::SecondsToMicroseconds(double seconds) {
	const double factor = 1000000;
	return seconds * factor;
}


double Timer::NowSeconds() noexcept {
	LARGE_INTEGER ticks, frequency;
	if (QueryPerformanceCounter(&ticks) && QueryPerformanceFrequency(&frequency))
		return double(ticks.QuadPart) / double(frequency.QuadPart);
	return 1.0f;
}

