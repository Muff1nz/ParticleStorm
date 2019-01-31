#pragma once
#include <string>
#include <vector>

class Environment;
class Stats;
class PhysicsEngine;
class RenderEngineSDL;

class SessionManager {
public:
	const std::string shorTitle = "VulkanRenderEngine";
	const std::string longTitle = "Making a render engine based on vulkan";
	const std::string statsOutputDir = "C:/C++ Projects/ParticleStorm_Stats/";
	const std::string statsGrapherDir = "C:/Python Projects/ParticleStorm_Tools/StatsGrapher.py";

	SessionManager();
	~SessionManager();

	void Sandbox() const;
	void Benchmark() const;

private:
	void OutputStatsToFile(const Stats& stats, const std::vector<std::string>& perSecondStats, const Environment& environment) const;
	static char* FileTime();
};

