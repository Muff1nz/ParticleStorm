#include "Timer.h"
#include <chrono>
#include <thread>

Timer::Timer(float maxDeltaTime) : maxDeltaTime(maxDeltaTime) {
	lastDeltaTime = SDL_GetPerformanceCounter();
	last = SDL_GetPerformanceCounter();

	std::this_thread::sleep_for(std::chrono::microseconds(10));

	nowDeltaTime = SDL_GetPerformanceCounter();
	now = SDL_GetPerformanceCounter();

	stopWatchRunning = false;
}

Timer::~Timer() = default;

float Timer::DeltaTime() {
	lastDeltaTime = nowDeltaTime;
	nowDeltaTime = SDL_GetPerformanceCounter();
	const float deltaTime = TicksToMilliseconds(nowDeltaTime - lastDeltaTime);
	const float result = deltaTime < maxDeltaTime ? deltaTime : maxDeltaTime;
	realTimeDifference = result / deltaTime;
	return result;
}

float Timer::RealTimeDifference() const {
	return realTimeDifference;
}

void Timer::Start() {
	if (stopWatchRunning) return;
	Restart();
}

void Timer::Stop() {
	stopWatchRunning = false;
}

void Timer::Restart() {
	stopWatchRunning = true;
	now = SDL_GetPerformanceCounter();
	last = now;
}

float Timer::ElapsedMilliseconds() {
	if (stopWatchRunning)
		now = SDL_GetPerformanceCounter();
	return TicksToMilliseconds(now - last);
}

float Timer::TicksToMilliseconds(Uint64 ticks) {
	return ticks * 1000 / float(SDL_GetPerformanceFrequency());
}
