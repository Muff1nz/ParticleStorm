#include "QuadTreeHandler.h"
#include "LinearQuad.h"

QuadTreeHandler::QuadTreeHandler(Environment* environment) {
	this->environment = environment;
	tree = new QuadTree(nullptr, Rect(0, 0, environment->worldWidth, environment->worldHeight));
	for (int i = 0; i < environment->particleCount; ++i) {
		tree->AddParticle(i);
	}

	collisionChecker = CollisionChecker(environment->particleRadius);
}


QuadTreeHandler::~QuadTreeHandler() = default;


void QuadTreeHandler::BuildLinearQuadTree(std::vector<LinearQuad*>* linearQuads, std::vector<Range>& quadSections) {
	BuildQuadTree();
	environment->workerThreads.JoinWorkerThreads();
	
	environment->workerThreads.PartitionForWorkers(quads.size(), quadSections);
	ResizeLinearQuads(linearQuads);
	for (auto quadSection : quadSections)
		environment->workerThreads.AddWork([=] { CalculateLinearQuads(linearQuads, quadSection.lower, quadSection.upper); });
	environment->workerThreads.JoinWorkerThreads();
}

void QuadTreeHandler::ResizeLinearQuads(std::vector<LinearQuad*>* linearQuads) {

	for (auto& linearQuad : *linearQuads) {
		linearQuad->Clear();
	}

	if (quads.size() > linearQuads->size()) {
		for (int i = linearQuads->size(); i < quads.size(); ++i) {
			linearQuads->push_back(new LinearQuad());
		}
	} else if (quads.size() < linearQuads->size()) {
		for (int i = linearQuads->size() - 1; i > quads.size(); --i) {
			delete (*linearQuads)[i];
			linearQuads->pop_back();
		}
	}
}

void QuadTreeHandler::CalculateLinearQuads(std::vector<LinearQuad*>* linearQuads, const int start, const int end) {
	for (int i = start; i < end; ++i) {
		QuadTree* quad = quads[i];
		LinearQuad* linearQuad = new LinearQuad();
		for (int j = 0; j < quad->Size(); ++j) {
			const int globalParticle = quad->GetParticle(j);
			if (environment->particleQuadCount[globalParticle] > 1) {
				linearQuad->externalParticle.push_back(globalParticle);
			} else {
				linearQuad->internalParticle.push_back(globalParticle);
				linearQuad->internalParticlePos.push_back(environment->particlePos[globalParticle]);
			}
		}
		(*linearQuads)[i] = linearQuad;
	}
}




void QuadTreeHandler::BuildQuadTree() {
	quads.clear();
	HandleSubTrees(tree);
}


void QuadTreeHandler::Build(QuadTree* tree) {
	PopulateQuadTreeWithParticles(tree);
	HandleSubTrees(tree);
}

void QuadTreeHandler::BuildThreaded(QuadTree* tree, const int start, const int end, const int threadCount, const int ThreadNumber) {
	PopulateQuadTreeWithParticlesThreaded(tree, start, end, ThreadNumber);

	tree->Lock();
	tree->CompleteThread();
	if (tree->CompletedThreads() != threadCount) {
		tree->Unlock();
		return;
	}
	tree->Unlock();

	tree->MergeThreadVectors();

	HandleSubTrees(tree);
}

void QuadTreeHandler::PopulateSubTrees(QuadTree* tree) {
	if (tree->Size() >= particlesPerThreadLevel3)
		PopulateBigSubTreesThreaded(tree);
	else if (tree->Size() >= particlesPerThreadLevel2)
		PopulateSubTreesThreaded(tree);
	else
		PopulateSubTreesNotThreaded(tree);
}

void QuadTreeHandler::PopulateSubTreesNotThreaded(QuadTree* tree) {
	for (int i = 0; i < 4; ++i) {
		Build(tree->GetSubTree(i));
	}
}

void QuadTreeHandler::PopulateSubTreesThreaded(QuadTree* tree) {
	for (int i = 0; i < 4; ++i) {
		environment->workerThreads.AddWork([=] { Build(tree->GetSubTree(i)); });
	}
}


void QuadTreeHandler::PopulateBigSubTreesThreaded(QuadTree* tree) {
	int threads = tree->Size() / particlesPerThreadLevel2;
	if (threads > environment->workerThreadCount)
		threads = environment->workerThreadCount;
	if (threads < 2)
		threads = 2;

	std::vector<Range> partitions;
	environment->workerThreads.PartitionForWorkers(tree->Size(), partitions, threads);

	tree->ResetSubTrees(threads);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < partitions.size(); ++j) {
			environment->workerThreads.AddWork([=] { BuildThreaded(tree->GetSubTree(i), partitions[j].lower, partitions[j].upper, threads, j); });
		}
	}
}

void QuadTreeHandler::PopulateQuadTreeWithParticles(QuadTree* tree) const {
	const int size = tree->GetParent()->Size();
	const auto allParticles = environment->particlePos;

	tree->ClearContent();
	QuadTree* parent = tree->GetParent();

	for (int i = 0; i < size; i++) {
		if (collisionChecker.CircleRectCollision(allParticles[parent->GetParticle(i)], tree->GetRect())) {
			tree->AddParticle(parent->GetParticle(i));
		}
	}
}

void QuadTreeHandler::PopulateQuadTreeWithParticlesThreaded(QuadTree* tree, const int start, const int end,	const int threadNumber) const {
	const auto allParticles = environment->particlePos;
	QuadTree* parent = tree->GetParent();

	for (int i = start; i < end; i++) {
		if (collisionChecker.CircleRectCollision(allParticles[parent->GetParticle(i)], tree->GetRect())) {
			tree->AddParticle(parent->GetParticle(i), threadNumber);
		}
	}	
}

void QuadTreeHandler::HandleSubTrees(QuadTree* const tree) {
	if (QuadLimitReached(tree)) {
		tree->CreateSubTrees();
		PopulateSubTrees(tree);
	} else {
		tree->RemoveChildren();
		quads.push_back(tree);
		for (int i = 0; i < tree->Size(); ++i) {
			++environment->particleQuadCount[tree->GetParticle(i)];
		}
	}
}

bool QuadTreeHandler::QuadLimitReached(QuadTree* tree) const {
	return tree->Size() >= maxParticles && tree->GetDepth() <= maxDepth;
}