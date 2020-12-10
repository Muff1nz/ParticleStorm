#pragma once

class Timer {
public:
	Timer(double maxDeltaTime = 99999999.0);
	~Timer();

	double DeltaTime();
	double RealTimeDifference() const;

	void Start();
	void Stop();
	void Restart();

	double ElapsedSeconds();
	double ElapsedMilliseconds();
	double ElapsedMicroseconds();

private:
	static double SecondsToMicroseconds(double seconds);
	static double NowSeconds() noexcept;

	const double maxDeltaTime;
	double realTimeDifference;

	double nowDeltaTime;
	double lastDeltaTime;

	double now;
	double last;

	bool stopWatchRunning;
};

