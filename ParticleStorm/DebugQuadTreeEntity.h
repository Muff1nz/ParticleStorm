#pragma once

#include "GameEntity.h"
#include "IdGenerator.h"

struct DebugQuadTreeEntity : GameEntity {
	DebugQuadTreeEntity() {
		type = ET_QuadTreeDebugEntity;

		id = IdGenerator::GetNewId();
		count = 400;
		position = new glm::vec2[count];
		scale = new glm::vec2[count];
	}
};
