#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "Range.h"
#include "Queue.h"


class WorkerThreadPool {
public:
	WorkerThreadPool(int threadCount);
	~WorkerThreadPool();

	void SetThreadCount(int threadCount);

	void JoinWorkerThreads();
	void AddWork(std::function<void()> workUnit);
	void CloseWorkerThreads();
	void PartitionForWorkers(int size, std::vector<Range>& range, int threads = 0) const;
	std::string StateString();

	int GetThreadCount() const;
private:
	bool isInitialized{};
	bool done = false;

	std::vector<std::thread> workerThreads;
	Queue<std::function<void()>> work;

	std::mutex mutex;
	std::condition_variable consumeCondition;
	std::condition_variable joinCondition;
	int busyThreads = 0;

	void WorkerThreadRun();

};

