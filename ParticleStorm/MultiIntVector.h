#pragma once

#include <vector>

class MultiIntVector {
public:
	void Reset(int count) {
		if (count != vectorCount) {
			for (int i = 0; i < vectorCount; ++i) {
				delete vectors[i];
			}
			delete vectors;

			vectors = new std::vector<int>*[count];
			for (int i = 0; i < count; ++i) {
				vectors[i] = new std::vector<int>;
			}
		} else {
			for (int i = 0; i < count; ++i) {
				vectors[i]->clear();
			}
		}
		vectorCount = count;
	}

	void Add(int item, int vector) {
		vectors[vector]->push_back(item);
	}


	std::vector<int> Merge() {
		int totalSize = 0;
		for (int i = 0; i < vectorCount; ++i) {
			totalSize += vectors[i]->size();
		}

		std::vector<int> mergedVector;
		mergedVector.resize(totalSize);
		int current = 0;
		for (int i = 0; i < vectorCount; ++i) {
			std::move(vectors[i]->begin(), vectors[i]->end(), mergedVector.begin() + current);
			current += vectors[i]->size();
		}
		return mergedVector;
	}

private:	
	std::vector<int>** vectors = nullptr;
	int vectorCount = 0;
};