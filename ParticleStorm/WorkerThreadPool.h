#pragma once
#include <functional>
#include "ConcurrentQueue.h"
#include <atomic>
#include "Range.h"

class WorkerThreadPool {
public:
	WorkerThreadPool();
	WorkerThreadPool(int threadCount, bool* done);
	~WorkerThreadPool();

	void Init(int threadCount, bool* done);

	void JoinWorkerThreads();
	void AddWork(std::function<void()> workUnit);
	void CloseWorkerThreads();
	void PartitionForWorkers(int size, std::vector<Range>& range, int extraThreads) const;
private:
	bool isInitialized;
	std::vector<std::thread> workerThreads;
	ConcurrentQueue<std::function<void()>> work;
	std::atomic_int busyThreadCount;

	void WorkerThreadRun(const bool* done);

};

