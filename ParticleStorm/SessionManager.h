#pragma once
#include <string>
#include <vector>

class Environment;
class Stats;

class SessionManager {
public:
	const std::string shorTitle = "ThreadingFix";
	const std::string longTitle = "Fixing multi threading joining logic";
	const std::string statsOutputDir = "C:/C++ Projects/ParticleStorm_Stats/";
	const std::string singleStatsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/StatsGrapher.py";
	const std::string multiStatsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/MultiStatsGrapher.py";
	const std::string physicsDetailedGrapherDir = "C:/Python Projects/ParticleStorm_Tools/PhysicsDetailedGrapher.py";

	SessionManager();
	~SessionManager();

	void Sandbox() const;
	void Benchmark() const;

private:
	void OutputSingleRunToFile(const std::string& sessionString) const;
	void OutputMultiRunToFile(const std::string& sessionString) const;
	std::string SessionToString(const Stats& stats, const std::vector<std::string>& perSecondStats,
	                            const Environment& environment) const;
	static char* FileTime();

	std::string Benchmark(int particleCount, int particleRadius, int threadCount) const;
};

