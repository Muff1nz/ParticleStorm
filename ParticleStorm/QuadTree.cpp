#include "QuadTree.h"
#include <iostream>
#include "CollisionChecker.h"

QuadTree::QuadTree(QuadTree* parent, Rect rect_) {
	this->parent = parent;

	rect = rect_;

	if (parent != nullptr)
		depth = parent->depth + 1;
	else
		depth = 0;
}

QuadTree::~QuadTree() {
	if (subTree != nullptr) {
		for (int i = 0; i < 4; ++i) {
			delete secretSubTree[i];
		}
	}
	delete subTree;
}

bool QuadTree::IsRoot() const {
	return parent == nullptr;
}

int QuadTree::Size() const {
	return particlesInQuad.size();
}

void QuadTree::ClearContent() {
	particlesInQuad.clear();
	particlesInQuadThreaded.Reset(1);
}

void QuadTree::RemoveChildren() {
	subTree = nullptr;
}

void QuadTree::CreateSubTrees() {
	if (subTree == nullptr) {
		if (secretSubTree != nullptr) {
			subTree = secretSubTree;
		} else {
			subTree = new QuadTree*[4];
			subTree[0] = new QuadTree(this, Rect(rect.x, rect.y, rect.w / 2, rect.h / 2));
			subTree[1] = new QuadTree(this, Rect(rect.x + rect.w / 2, rect.y, rect.w / 2, rect.h / 2));
			subTree[2] = new QuadTree(this, Rect(rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2));
			subTree[3] = new QuadTree(this, Rect(rect.w / 2 + rect.x, rect.y + rect.h / 2, rect.w / 2, rect.h / 2));
			secretSubTree = subTree;
		}
	}
}

void QuadTree::AddParticle(int particle) {
	particlesInQuad.push_back(particle);
}

void QuadTree::AddParticle(int particle, int thread) {
	particlesInQuadThreaded.Add(particle, thread);
}

int QuadTree::GetParticle(int index) {
	return particlesInQuad[index];
}

QuadTree* QuadTree::GetParent() {
	return parent;
}

QuadTree* QuadTree::GetSubTree(int index) {
	return subTree[index];
}

void QuadTree::ResetSubTrees() {
	ResetSubTrees(1);
}

void QuadTree::ResetSubTrees(int threadCount) {
	for (int i = 0; i < 4; ++i) {
		subTree[i]->particlesInQuad.clear();
		subTree[i]->particlesInQuadThreaded.Reset(threadCount);
		subTree[i]->finishedThreads = 0;
	}
}

void QuadTree::MergeThreadVectors() {
	particlesInQuad = particlesInQuadThreaded.Merge();
}

void QuadTree::Lock() {
	QuadLock.lock();
}

void QuadTree::Unlock() {
	QuadLock.unlock();
}

void QuadTree::CompleteThread() {
	++finishedThreads;
}

int QuadTree::CompletedThreads() {
	return finishedThreads;
}

Rect QuadTree::GetRect() {
	return rect;
}

int QuadTree::GetDepth() {
	return depth;
}
