#pragma once
#include <string>
#include <vector>

class Environment;
class Stats;

class SessionManager {
public:
	const std::string shorTitle = "MegaBenchmark";
	const std::string longTitle = "MegaBenchmark";
	const std::string statsOutputDir = "C:/C++ Projects/ParticleStorm_Stats/";
	const std::string singleStatsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/StatsGrapher.py";
	const std::string multiStatsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/MultiStatsGrapher.py";

	SessionManager();
	~SessionManager();

	void Sandbox() const;
	std::string Benchmark(int particleCount = 20000, int particleRadius = 5, int threadCount = 16, bool suppress = false) const;
	void ThreadingBenchmark() const;

private:
	void OutputSingleRunToFile(const std::string& sessionString) const;
	void OutputMultiRunToFile(const std::string& sessionString) const;
	std::string SessionToString(const Stats& stats, const std::vector<std::string>& perSecondStats,
	                            const Environment& environment) const;
	static char* FileTime();
};

