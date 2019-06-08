#pragma once

#include <unordered_set>

template <typename T>
class HashSet {
public:	
	void Add(T item) {
		set.emplace(item);
	}

	bool Contains(T item) {
		return set.find(item) != set.end();
	}

	void Clear() {
		set.clear();
	}

private:
	std::unordered_set<T> set;
};
