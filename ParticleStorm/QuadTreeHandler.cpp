#include "QuadTreeHandler.h"
#include "LinearQuad.h"

QuadTreeHandler::QuadTreeHandler(WorkerThreadPool* workerThreads, Stats* stats) {
	this->workerThreads = workerThreads;
	this->stats = stats;
}

QuadTreeHandler::~QuadTreeHandler() {
	delete tree;
}

void QuadTreeHandler::Init(PhysicsParticlesEntity* particles, WorldEntity* world) {
	delete tree;

	this->particles = particles;
	this->world = world;

	tree = new QuadTree(nullptr, Rect(0, 0, world->width, world->height));
	for (int i = 0; i < particles->count; ++i) {
		tree->AddParticle(i);
	}

	collisionChecker = CollisionChecker(particles->radius);
}


void QuadTreeHandler::BuildLinearQuadTree(std::vector<LinearQuad*>* linearQuads, std::vector<Range>& quadSections) {
	//TODO: Solve linearQuads using "Function Injection" and return like in RenderEngineVulkanBackend
	BuildQuadTree();
	workerThreads->JoinWorkerThreads();
	
	workerThreads->PartitionForWorkers(quads.size(), quadSections);
	ResizeLinearQuads(linearQuads);
	for (auto quadSection : quadSections)
		workerThreads->AddWork([=] { CalculateLinearQuads(linearQuads, quadSection.lower, quadSection.upper); });
	workerThreads->JoinWorkerThreads();
}

void QuadTreeHandler::PopulateQuadData(DebugQuadTreeEntity* quadTreeDebugEntity) {
	for (int i = 0; i < quadTreeDebugEntity->count; ++i) {
		if (i < quads.size()) {
			auto rect = quads[i]->GetRect();
			quadTreeDebugEntity->position[i] = { rect.x + rect.halfW, rect.y + rect.halfH };
			quadTreeDebugEntity->scale[i] = {rect.halfW, rect.halfH};
		} else {
			quadTreeDebugEntity->position[i] = { 0, 0 };
			quadTreeDebugEntity->scale[i] = { 0, 0 };
		}
	}
}

void QuadTreeHandler::ResizeLinearQuads(std::vector<LinearQuad*>* linearQuads) {
	//TODO: Make this scale better with higher particle/thread counts
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
		LinearQuad* linearQuad = (*linearQuads)[i];
		linearQuad->Clear();
		for (int j = 0; j < quad->Size(); ++j) {
			const int globalParticle = quad->GetParticle(j);
			if (particles->particleQuadCount[globalParticle] > 1) {
				linearQuad->externalParticle.push_back(globalParticle);
			} else {
				linearQuad->internalParticle.push_back(globalParticle);
				linearQuad->internalParticlePos.push_back(particles->position[globalParticle]);
			}
		}
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
		workerThreads->AddWork([=] { Build(tree->GetSubTree(i)); });
	}
}


void QuadTreeHandler::PopulateBigSubTreesThreaded(QuadTree* tree) {
	int threads = tree->Size() / particlesPerThreadLevel2;
	if (threads > workerThreads->GetThreadCount())
		threads = workerThreads->GetThreadCount();
	if (threads < 2)
		threads = 2;

	std::vector<Range> partitions;
	workerThreads->PartitionForWorkers(tree->Size(), partitions, threads);

	tree->ResetSubTrees(threads);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < partitions.size(); ++j) {
			workerThreads->AddWork([=] { BuildThreaded(tree->GetSubTree(i), partitions[j].lower, partitions[j].upper, threads, j); });
		}
	}
}

void QuadTreeHandler::PopulateQuadTreeWithParticles(QuadTree* tree) const {
	const int size = tree->GetParent()->Size();
	const auto allParticles = particles->position;

	tree->ClearContent();
	QuadTree* parent = tree->GetParent();

	for (int i = 0; i < size; i++) {
		if (collisionChecker.CircleRectCollision(allParticles[parent->GetParticle(i)], tree->GetRect())) {
			tree->AddParticle(parent->GetParticle(i));
		}
	}
}

void QuadTreeHandler::PopulateQuadTreeWithParticlesThreaded(QuadTree* tree, const int start, const int end,	const int threadNumber) const {
	const auto allParticles = particles->position;
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
			++particles->particleQuadCount[tree->GetParticle(i)];
		}
	}
}

bool QuadTreeHandler::QuadLimitReached(QuadTree* tree) const {
	return tree->Size() >= maxParticles && tree->GetDepth() <= maxDepth;
}