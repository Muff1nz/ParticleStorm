#include "IdGenerator.h"

int IdGenerator::nextId = 0;

int IdGenerator::GetNewId() {
	return nextId++;
}