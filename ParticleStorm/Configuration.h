#pragma once

struct Configuration : BaseEntity {
	//Technical data, belongs here
	int workerThreadCount = -1;

	int screenWidth = -1;
	int screenHeight = -1;

	int seed = -1;
};
