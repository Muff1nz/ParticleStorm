#pragma once

struct Configuration : BaseEntity {
	int workerThreadCount = -1;

	int screenWidth = -1;
	int screenHeight = -1;

	int worldWidth = -1;
	int worldHeight = -1;

	int particleCount = -1;
	int particleRadius = -1;
	int seed = -1;
};
