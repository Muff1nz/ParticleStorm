#include "NumberGenerator.h"
#include <ctime>


NumberGenerator::NumberGenerator() {
	generator.seed(int(time(nullptr)));
}

NumberGenerator::NumberGenerator(const int seed) {
	if (seed != -1)
		generator.seed(seed);
	else 
		generator.seed(int(time(nullptr)));
}

NumberGenerator::~NumberGenerator() = default;

float NumberGenerator::GenerateFloat(const float min, const float max) {
	return distribution(generator) * (max - min) + min;
}

int NumberGenerator::GenerateInt(int min, int max) {
	return static_cast<int>(GenerateFloat(min, max));
}