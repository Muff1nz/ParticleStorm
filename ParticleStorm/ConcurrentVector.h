#pragma once

#include <mutex>


template <typename T>
class ConcurrentVectror {
public:
	void Push(T const& value) {
		std::unique_lock<std::mutex> lock(mutex);
		vector.push_back(value);
	}

	int Size() {
		std::unique_lock<std::mutex> lock(mutex);
		return vector.size();
	}

	void Clear() {
		vector.clear();
	}

	T& operator[] (const int index) {
		return vector[index];
	}

private:
	std::mutex mutex;
	std::vector<T> vector;
};