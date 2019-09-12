#pragma once

class EnumUtils {
public:
	template<class T> static bool HasFlag(T e, T f) {
		return (static_cast<int>(e) & static_cast<int>(f)) != 0;
	}

	template<class T> static T AddFlag(T e, T f) {
		return static_cast<T>(static_cast<int>(e) | static_cast<int>(f));
	}
};