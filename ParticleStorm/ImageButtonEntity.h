#pragma once

#include <functional>
#include <vector>

#include "GraphicsEntity.h"
#include "IdGenerator.h"
#include "Rect.h"

struct ImageButtonEntity : GraphicsEntity {
	ImageButtonEntity() {
		id = IdGenerator::GetNewId();
		type = ET_ImageButton;

		rect = Rect(0, 0, 100, 100);
	}

	ImageButtonEntity(std::string texture, int width, int height, glm::vec2 pos) {
		id = IdGenerator::GetNewId();
		type = ET_ImageButton;

		texturePath = texture;

		rect = Rect(pos.x, pos.y, width, height);

		position = new glm::vec2(width / 2 + pos.x, height / 2 + pos.y);
		scale = new glm::vec2(width / 2, height / 2);
	}

	void RegisterOnClick(std::function<void()> onClick) {
		onClickEvents.push_back(onClick);
	}

	void TriggerOnClick() {
		for (auto onClickEvent : onClickEvents)
			onClickEvent();
	}

	Rect rect;
	
	std::vector<std::function<void()>> onClickEvents;
};
