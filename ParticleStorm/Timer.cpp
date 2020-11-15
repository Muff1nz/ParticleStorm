#include "Timer.h"
#include <thread>
#include <windows.h>

bool Timer::unhinged = false;

Timer::Timer(float maxDeltaTime, float minDeltaTime) : maxDeltaTime(maxDeltaTime), minDeltaTime(minDeltaTime) {
	nowDeltaTime = NowSeconds();
	now = NowSeconds();

	lastDeltaTime = nowDeltaTime;
	last = now;


	stopWatchRunning = false;
}

Timer::~Timer() = default;

float Timer::DeltaTime() {
	lastDeltaTime = nowDeltaTime;
	nowDeltaTime = NowSeconds();
	const float deltaTime = nowDeltaTime - lastDeltaTime;
	float result = deltaTime < maxDeltaTime ? deltaTime : maxDeltaTime;
	if (!unhinged && deltaTime < minDeltaTime) {
		std::this_thread::sleep_for(std::chrono::microseconds(SecondsToMicroseconds(minDeltaTime - deltaTime)));
		result = minDeltaTime;
	}
	realTimeDifference = result / deltaTime * 100;
	return result;
}

float Timer::RealTimeDifference() const {
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

int Timer::ElapsedMilliseconds() {
	if (stopWatchRunning)
		now = NowSeconds();
	return int((now - last) * 1000);
}

int Timer::ElapsedMicroseconds() {
	if (stopWatchRunning)
		now = NowSeconds();
	return SecondsToMicroseconds(now - last);
}

int Timer::ElapsedNanoseconds() {
	if (stopWatchRunning)
		now = NowSeconds();
	return SecondsToNanoseconds(now - last);
}


int Timer::SecondsToMicroseconds(float seconds) {
	const float factor = 1000 * 1000;
	return int(seconds * factor);
}

int Timer::SecondsToNanoseconds(float seconds) {
	const float factor = 1000 * 1000 * 1000;
	return int(seconds * factor);
}


double Timer::NowSeconds() noexcept {
	LARGE_INTEGER ticks, frequency;
	if (QueryPerformanceCounter(&ticks) && QueryPerformanceFrequency(&frequency))
		return double(ticks.QuadPart) / double(frequency.QuadPart);
	return 1.0f;
}

