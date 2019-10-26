#pragma once

template <typename  T, typename U>
class PsTuple {
public:
	PsTuple(T first, U second) {
		this->first = first;
		this->second = second;
	}

	T first;
	U second;
};