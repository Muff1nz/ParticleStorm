#include "NumberGenerator.h"
#include <ctime>


NumberGenerator::NumberGenerator() {
	generator.seed(int(time(nullptr)));
}

NumberGenerator::NumberGenerator(const int seed) {
	generator.seed(seed);
}


NumberGenerator::~NumberGenerator() = default;


float NumberGenerator::GenerateFloat(const float min, const float max) {
	return distribution(generator) * (max - min) + min;
}