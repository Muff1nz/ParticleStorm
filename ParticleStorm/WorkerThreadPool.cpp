#include "WorkerThreadPool.h"


WorkerThreadPool::WorkerThreadPool() {
	isInitialized = false;
}


WorkerThreadPool::WorkerThreadPool(int threadCount, bool* done) {
	Init(threadCount, done);
}

void WorkerThreadPool::Init(int threadCount, bool* done) {
	if (isInitialized) {
		throw std::runtime_error("ThreadPool is already Initialized!!");
	}
	
	for (int i = 0; i < threadCount; ++i) {
		workerThreads.emplace_back([=] {WorkerThreadRun(done); });
	}
	isInitialized = true;
}

WorkerThreadPool::~WorkerThreadPool() {

}

void WorkerThreadPool::JoinWorkerThreads() {
	while (!work.Empty()) {
		auto workUnit = work.NonBlockPop();
		if (workUnit != NULL)
			workUnit();
	}
	while (!work.Empty() || busyThreadCount > 0) {

	}

}
void WorkerThreadPool::AddWork(std::function<void()> workUnit) {
	work.Push(workUnit);
}

void WorkerThreadPool::CloseWorkerThreads() {
	JoinWorkerThreads();
	for (int i = 0; i < workerThreads.size(); ++i) {
		work.Push([=] {});
	}

	for (int i = 0; i < workerThreads.size(); ++i) {
		workerThreads[i].join();
	}
}

void WorkerThreadPool::WorkerThreadRun(const bool* done) {
	busyThreadCount++;
	while (!*done) {
		busyThreadCount--;
		auto workUnit = work.Pop();
		busyThreadCount++;
		workUnit();
	}
	busyThreadCount--;
}
