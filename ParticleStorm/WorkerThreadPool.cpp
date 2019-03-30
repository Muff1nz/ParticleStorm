#include "WorkerThreadPool.h"
#include <string>


WorkerThreadPool::WorkerThreadPool() {
	isInitialized = false;
}


WorkerThreadPool::WorkerThreadPool(int threadCount) {
	Init(threadCount);
}

void WorkerThreadPool::Init(int threadCount) {
	if (isInitialized) {
		throw std::runtime_error("ThreadPool is already Initialized!!");
	}
	
	for (int i = 0; i < threadCount; ++i) {
		workerThreads.emplace_back([=] {WorkerThreadRun(); });
	}
	isInitialized = true;
}

WorkerThreadPool::~WorkerThreadPool() = default;

void WorkerThreadPool::JoinWorkerThreads() {
	std::unique_lock<std::mutex> lock(mutex);
	joinCondition.wait(lock, [=] { return work.Empty() && busyThreads == 0; });
}

void WorkerThreadPool::AddWork(std::function<void()> workUnit) {
	std::unique_lock<std::mutex> lock(mutex);
	work.Push(workUnit);
	consumeCondition.notify_one();
}

void WorkerThreadPool::CloseWorkerThreads() {
	JoinWorkerThreads();

	std::unique_lock<std::mutex> lock(mutex);
	done = true;
	for (int i = 0; i < workerThreads.size() - busyThreads; ++i) {
		work.Push([=] {});
	}
	consumeCondition.notify_all();
	lock.unlock();

	for (int i = 0; i < workerThreads.size(); ++i) {
		workerThreads[i].join();
	}
}

void WorkerThreadPool::PartitionForWorkers(const int size, std::vector<Range>& range, const int threads) const {
	const int totalThreads = threads != 0 ? threads: workerThreads.size();
	const int unitsPerThread = size / totalThreads;
	range.clear();
	for (int i = 0; i < totalThreads; ++i) {
		int start = i * unitsPerThread;
		int end = (i + 1) * unitsPerThread;
		if (i == totalThreads - 1)
			end = size;
		range.emplace_back(start, end);
	}
}

std::string WorkerThreadPool::StateString() {
	std::unique_lock<std::mutex> lock(mutex);
	return "Busy threads: " + std::to_string(busyThreads) + " Work units in queue: " + std::to_string(work.Size());
}

void WorkerThreadPool::WorkerThreadRun() {
	std::unique_lock<std::mutex> lock(mutex);
	++busyThreads;
	lock.unlock();

	while (!done) {
		lock.lock();
		--busyThreads;
		if (work.Empty())
			joinCondition.notify_all();
		consumeCondition.wait(lock, [=] { return !work.Empty(); });
		++busyThreads;

		auto workUnit = work.Pop();
		lock.unlock();

		workUnit();
	}

	lock.lock();
	--busyThreads;
	joinCondition.notify_all();
	lock.unlock();
}
