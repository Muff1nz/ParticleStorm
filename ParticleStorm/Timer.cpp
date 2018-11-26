#include "Timer.h"

Timer::Timer(float maxDeltaTime) : maxDeltaTime(maxDeltaTime) {
	nowDeltaTime = SDL_GetPerformanceCounter();
	lastDeltaTime = nowDeltaTime;

	now = SDL_GetPerformanceCounter();
	last = now;

	stopWatchRunning = false;
}

Timer::~Timer() = default;

float Timer::DeltaTime() {
	lastDeltaTime = nowDeltaTime;
	nowDeltaTime = SDL_GetPerformanceCounter();
	const auto deltaTime = TicksToMilliseconds(nowDeltaTime - lastDeltaTime);
	const auto result = deltaTime < maxDeltaTime ? deltaTime : maxDeltaTime;
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
