#pragma once

struct PhysicsHyperParameters {
public:
	int maxParticles = 100;
	int maxDepth = 10;
	int particlesPerThreadLevel1 = 800;
	int particlesPerThreadLevel2 = particlesPerThreadLevel1 * 4;
	int particlesPerThreadLevel3 = particlesPerThreadLevel2 * 4;

	double score = 0;

	void Mutate() {
		MutateDataPoint(maxParticles);
		MutateDataPoint(maxDepth);
		MutateDataPoint(particlesPerThreadLevel1);
		MutateDataPoint(particlesPerThreadLevel2);
		MutateDataPoint(particlesPerThreadLevel3);
	}

	PhysicsHyperParameters MakeChildGeneAvg(PhysicsHyperParameters partner) {
		PhysicsHyperParameters child;
		child.maxParticles = (maxParticles + partner.maxParticles) / 2;
		child.maxDepth = (maxDepth + partner.maxDepth) / 2;
		child.particlesPerThreadLevel1 = (particlesPerThreadLevel1 + partner.particlesPerThreadLevel1) / 2;
		child.particlesPerThreadLevel2 = (particlesPerThreadLevel2 + partner.particlesPerThreadLevel2) / 2;
		child.particlesPerThreadLevel3 = (particlesPerThreadLevel3 + partner.particlesPerThreadLevel3) / 2;
		return child;
	}

	PhysicsHyperParameters MakeChildGeneSplicing(PhysicsHyperParameters partner) {
		PhysicsHyperParameters child;
		child.maxParticles = GetRandomGene(maxParticles, partner.maxParticles);
		child.maxDepth = GetRandomGene(maxDepth, partner.maxDepth);
		child.particlesPerThreadLevel1 = GetRandomGene(particlesPerThreadLevel1, partner.particlesPerThreadLevel1);
		child.particlesPerThreadLevel2 = GetRandomGene(particlesPerThreadLevel2, partner.particlesPerThreadLevel2);
		child.particlesPerThreadLevel3 = GetRandomGene(particlesPerThreadLevel3, partner.particlesPerThreadLevel3);
		return child;
	}

	static bool SortByScore(const PhysicsHyperParameters& lhs, const PhysicsHyperParameters& rhs) { return lhs.score < rhs.score; }

	std::string ToString() {
		return " maxParticles: " + std::to_string(maxParticles)
			+ " maxDepth: " + std::to_string(maxDepth)
			+ " particlesPerThreadLevel1: " + std::to_string(particlesPerThreadLevel1)
			+ " particlesPerThreadLevel2: " + std::to_string(particlesPerThreadLevel2)
			+ " particlesPerThreadLevel3: " + std::to_string(particlesPerThreadLevel3);
	}

private:
	inline static NumberGenerator numberGenerator{};

	void MutateDataPoint(int& dataPoint) const {
		const int dataPointScrambleRange = dataPoint / 4;
		dataPoint += numberGenerator.GenerateInt(-dataPointScrambleRange, dataPointScrambleRange);
		dataPoint = dataPoint < 1 ? 1 : dataPoint;
	}
	
	static int GetRandomGene(int one, int two) {
		if (numberGenerator.GenerateFloat(0.0f, 1.0f) > 0.5f)
			return one;
		return two;
	}
};