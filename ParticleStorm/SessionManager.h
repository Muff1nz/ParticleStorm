#pragma once
#include <string>
#include <vector>

class Environment;
class Stats;

class SessionManager {
public:
	const std::string shorTitle = "Graphics Bench";
	const std::string longTitle = "Graphics Bench";
	const std::string statsOutputDir = "C:/C++ Projects/ParticleStorm_Stats/";
	const std::string graphicsBenchGrapherDir = "C:/Python Projects/ParticleStorm_Tools/GraphicsBenchGrapher.py";
	const std::string multiStatsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/MultiStatsGrapher.py";
	const std::string physicsDetailedGrapherDir = "C:/Python Projects/ParticleStorm_Tools/PhysicsDetailedGrapher.py";

	SessionManager();
	~SessionManager();

	void Sandbox() const;
	void PhysBench() const;
	void GraphBench() const;
private:
	void OutputPhysBenchRunToFile(const std::string& sessionString) const;
	void OutputGraphBenchRunToFile(const std::string& sessionString) const;
	std::string SessionToString(const std::vector<std::string>& perSecondStats, const Environment& environment) const;
	static char* FileTime();

	std::string Benchmark(int particleCount, int particleRadius, int threadCount) const;
};

