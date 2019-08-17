#include "RenderEntity.h"

RenderEntity::RenderEntity(RenderDataCore* renderDataCore, RenderDataSingular* renderDataSingular,	RenderDataInstanced* renderDataInstanced, const bool isStatic) : isStatic(isStatic) {
	this->renderDataCore = renderDataCore;
	this->renderDataSingular = renderDataSingular;
	this->renderDataInstanced = renderDataInstanced;
}

RenderEntity::~RenderEntity() {
	Dispose();
}

bool RenderEntity::IsStatic() const {
	return isStatic;
}

void RenderEntity::Dispose() {
	if (isDisposed)
		return;

	//TODO: Vulkan cleanup shit
}
