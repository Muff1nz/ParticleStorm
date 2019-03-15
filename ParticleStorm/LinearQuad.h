#pragma once

class LinearQuad {
public:
	int start;
	int end;
	LazyVector<int> overflow;

	void Reset() {
		start = end = 0;
		overflow.Clear();
	}
};
