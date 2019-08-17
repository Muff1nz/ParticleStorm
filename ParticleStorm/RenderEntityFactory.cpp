#include "RenderEntityFactory.h"

RenderEntityFactory::RenderEntityFactory() = default;

RenderEntityFactory::~RenderEntityFactory() = default;

RenderEntity* RenderEntityFactory::CreateRenderEntity(RenderTransform transform, bool isStatic, std::string vertexShader, std::string fragmentShader) {
	return nullptr;
}
