#pragma once

class LinearQuad {
public:
	int start;
	int end;
	std::vector<int> overflow;

	void Reset() {
		start = end = 0;
		overflow.clear();
	}
};
