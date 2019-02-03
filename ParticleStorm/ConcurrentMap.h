#pragma once

#include <mutex>


template <typename T, typename U>
class ConcurrentMap {
public:
	void Insert(T const& key, U const& value) {
		std::unique_lock<std::mutex> lock(mutex);
		if (map.find(key) == map.end())
			map.insert(key, std::vector<U>());
		map[key].push_back(value);
	}

	bool HasKey(T const& key) {
		std::unique_lock<std::mutex> lock(mutex);
		return map.find(key) != map.end();
	}

	void Clear() {
		map.clear();
	}

	std::vector<U>& operator[] (const T index) {
		return map[index];
	}

private:
	std::mutex mutex;
	std::unordered_map<T, std::vector<U>> map;
};
