#pragma once

#include <deque>

template <typename T>
class Queue {
public:
	void Push(T const& value) {
		queue.push_front(value);
	}

	T Pop() {
		T ret = queue.back();
		queue.pop_back();
		return ret;
	}

	int Size() {
		return queue.size();
	}

	bool Empty() {
		return queue.empty();
	}

private:
	std::deque<T> queue;
};
