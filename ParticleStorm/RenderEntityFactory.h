#pragma once
#include "RenderEntity.h"

class RenderEntityFactory {
public:
	RenderEntityFactory();
	~RenderEntityFactory();

	RenderEntity* CreateRenderEntity(RenderTransform transform, bool isStatic, std::string vertexShader, std::string fragmentShader);
};

