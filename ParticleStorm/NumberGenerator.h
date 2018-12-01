#pragma once
#include <random>

class NumberGenerator {
public:
	NumberGenerator();
	NumberGenerator(int seed);
	~NumberGenerator();
	float GenerateFloat(float min, float max);

private:
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
};

