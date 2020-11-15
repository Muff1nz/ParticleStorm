#pragma once
#include <random>

class NumberGenerator {
public:
	NumberGenerator();
	NumberGenerator(int seed);
	~NumberGenerator();
	float GenerateFloat(float min, float max);
	int GenerateInt(int min, int max);

private:
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution;
};

