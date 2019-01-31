#pragma once
#include <SDL2/SDL.h>
#include <limits>

class Timer {
public:
	Timer(float maxDeltaTime = std::numeric_limits<float>::max(), float minDeltaTime = 0);
	~Timer();

	static bool unhinged;

	float DeltaTime();
	float RealTimeDifference() const;

	void Start();
	void Stop();
	void Restart();
	float ElapsedSeconds();
private:
	static float TicksToSeconds(Uint64 ticks);
	static int SecondsToMicroseconds(float seconds);

	const float maxDeltaTime;
	const float minDeltaTime;
	float realTimeDifference;

	Uint64 nowDeltaTime;
	Uint64 lastDeltaTime;

	Uint64 now;
	Uint64 last;

	bool stopWatchRunning;
};

