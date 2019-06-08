#pragma once
#include "Environment.h"
#include "Rect.h"
#include "MultiIntVector.h"
#include "ConcurrentVector.h"


class QuadTree {
public:
	QuadTree(QuadTree* parent, Rect rect_);
	~QuadTree();

	bool IsRoot() const;
	int Size() const;

	void AddParticle(int particle);
	void AddParticle(int particle, int thread);
	int GetParticle(int index);

	QuadTree* GetParent();
	QuadTree* GetSubTree(int index);

	void ClearContent();
	void RemoveChildren();
	void CreateSubTrees();

	void ResetSubTrees();
	void ResetSubTrees(int threadCount);

	Rect GetRect();
	int GetDepth();

	void MergeThreadVectors();

	void Lock();
	void Unlock();

	void CompleteThread();
	int CompletedThreads();

private:
	int depth;

	std::vector<int> particlesInQuad;

	int finishedThreads;
	MultiIntVector particlesInQuadThreaded;
	std::mutex QuadLock;

	Rect rect;

	QuadTree* parent = nullptr;
	QuadTree** subTree = nullptr;
	QuadTree** secretSubTree = nullptr;
};

