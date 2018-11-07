#pragma once
#include <string>

class Test {
public:
	std::string GetSecretString();

	int changeMe = 0;
private:
	std::string secret = "WHUT";
};