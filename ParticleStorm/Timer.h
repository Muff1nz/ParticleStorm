#pragma once
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
	int ElapsedMilliseconds();
	int ElapsedMicroseconds();

	private:
	static int SecondsToMicroseconds(float seconds);
	static float NowSeconds() noexcept;

	const float maxDeltaTime;
	const float minDeltaTime;
	float realTimeDifference;

	float nowDeltaTime;
	float lastDeltaTime;

	float now;
	float last;

	bool stopWatchRunning;
};

