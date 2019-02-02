#pragma once
#include <functional>
#include "ConcurrentQueue.h"
#include <atomic>

class WorkerThreadPool {
public:
	WorkerThreadPool();
	WorkerThreadPool(int threadCount, bool* done);
	~WorkerThreadPool();

	void Init(int threadCount, bool* done);

	void JoinWorkerThreads();
	void AddWork(std::function<void()> workUnit);
	void CloseWorkerThreads();
private:
	bool isInitialized;
	std::vector<std::thread> workerThreads;
	ConcurrentQueue<std::function<void()>> work;
	std::atomic_int busyThreadCount;

	void WorkerThreadRun(const bool* done);

};

