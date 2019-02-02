#pragma once

#include <mutex>
#include <condition_variable>
#include <deque>

//Stolen from stackoverflow: https://stackoverflow.com/questions/12805041/c-equivalent-to-javas-blockingqueue
template <typename T>
class ConcurrentQueue {
public:
	void Push(T const& value) {
		{
			std::unique_lock<std::mutex> lock(mutex);
			queue.push_front(value);
		}
		this->condition.notify_one();
	}

	T Pop() {
		std::unique_lock<std::mutex> lock(mutex);
		condition.wait(lock, [=] { return !queue.empty(); });
		T ret = queue.back();
		queue.pop_back();
		return ret;
	}

	T NonBlockPop() {
		std::unique_lock<std::mutex> lock(mutex);
		if (queue.empty())
			return NULL;
		T ret = queue.back();
		queue.pop_back();
		return ret;
	}

	int Size() {
		std::unique_lock<std::mutex> lock(mutex);
		return queue.size();
	}

	bool Empty() {
		std::unique_lock<std::mutex> lock(mutex);
		return queue.empty();
	}

private:
	std::mutex              mutex;
	std::condition_variable condition;
	std::deque<T>           queue;
};