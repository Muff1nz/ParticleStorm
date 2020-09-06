#pragma once
class IdGenerator {
public:
	static int GetNewId();
private:
	static int nextId;
};