#pragma once
#include "Environment.h"

class SessionManager {
public:
	const std::string shorTitle = "Benchmark";
	const std::string longTitle = "Currently making benchmark for ParticleStorm";

	SessionManager();
	~SessionManager();

	void Sandbox();
	void Benchmark();

private:
	void OutputStatsToFile(const Stats& stats, const std::vector<std::string>& perSecondStats, const Environment& environment);
	char* FileTime();
};

