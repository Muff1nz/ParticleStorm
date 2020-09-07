#pragma once
#include <atomic>
#include "EntityType.h"

struct BaseEntity {
	int id;
	EntityType type;

	BaseEntity() {
		type = ET_Undefined;
		id = -1;
		observers = 0;
	}

	BaseEntity(BaseEntity& entity) {
		id = entity.id;
		observers = int(entity.observers);
	}

	int GetObservers() const {
		return observers;
	}

	void RegisterAsObserver() {
		observers += 1;
	}

	void UnregisterAsObserver() {
		observers -= 1;
	}
private:
	std::atomic_int observers{};
};
