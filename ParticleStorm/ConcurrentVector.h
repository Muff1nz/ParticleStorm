#pragma once

#include <mutex>


template <typename T>
class ConcurrentVector {
public:
	void push_back(T const value) {
		std::unique_lock<std::mutex> lock(mutex);
		vector.push_back(value);
	}

	int size() {
		std::unique_lock<std::mutex> lock(mutex);
		return vector.size();
	}

	void clear() {
		vector.clear();
	}

	T& operator[] (const int index) {
		return vector[index];
	}

private:
	std::mutex mutex;
	std::vector<T> vector;
};