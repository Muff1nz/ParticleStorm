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

	this->done = done;
	for (int i = 0; i < threadCount; ++i) {
		workerThreads.emplace_back([=] {WorkerThreadRun(); });
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
	while (!*done && (!work.Empty() || busyThreadCount > 0)) {

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

void WorkerThreadPool::PartitionForWorkers(int size, std::vector<Range>& range, int extraThreads) const {
	const int totalThreads = workerThreads.size() + extraThreads;
	const int unitsPerThread = size / totalThreads;
	range.clear();
	for (int i = 0; i < totalThreads; ++i) {
		int start = i * unitsPerThread;
		int end = (i + 1) * unitsPerThread;
		if (i == totalThreads - 1)
			end = size;
		range.emplace_back(Range(start, end));
	}
}

void WorkerThreadPool::WorkerThreadRun() {
	++busyThreadCount;
	while (!*done) {
		--busyThreadCount;
		auto workUnit = work.Pop();
		++busyThreadCount;
		workUnit();
	}
	--busyThreadCount;
}
