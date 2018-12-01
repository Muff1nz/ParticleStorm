#include "Timer.h"

Timer::Timer(float maxDeltaTime) : maxDeltaTime(maxDeltaTime) {
	nowDeltaTime = SDL_GetPerformanceCounter();
	now = SDL_GetPerformanceCounter();

	lastDeltaTime = nowDeltaTime;
	last = now;


	stopWatchRunning = false;
}

Timer::~Timer() = default;

float Timer::DeltaTime() {
	lastDeltaTime = nowDeltaTime;
	nowDeltaTime = SDL_GetPerformanceCounter();
	const float deltaTime = TicksToSeconds(nowDeltaTime - lastDeltaTime);
	const float result = deltaTime < maxDeltaTime ? deltaTime : maxDeltaTime;
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
}

void Timer::Restart() {
	stopWatchRunning = true;
	now = SDL_GetPerformanceCounter();
	last = now;
}

float Timer::ElapsedSeconds() {
	if (stopWatchRunning)
		now = SDL_GetPerformanceCounter();
	return TicksToSeconds(now - last);
}

float Timer::TicksToSeconds(Uint64 ticks) {
	return ticks / float(SDL_GetPerformanceFrequency());
}
