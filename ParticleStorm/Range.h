#pragma once

class Range {
public:
	Range() {
		lower = 0;
		upper = 1;
	}

	Range(int lower, int upper) {
		this->lower = lower;
		this->upper = upper;
	}

	int lower;
	int upper;
};
