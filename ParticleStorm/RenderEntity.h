#pragma once
#include "RenderDataCore.h"
#include "RenderDataSingular.h"
#include "RenderDataInstanced.h"

class RenderEntity {
public:
	RenderEntity(RenderDataCore* renderDataCore, RenderDataSingular* renderDataSingular, RenderDataInstanced* renderDataInstanced, bool isStatic);
	~RenderEntity();

	void Dispose();
private:
	bool isDisposed = false;
	const bool isStatic = false;

	RenderDataCore* renderDataCore;
	RenderDataSingular* renderDataSingular;
	RenderDataInstanced* renderDataInstanced;

};
