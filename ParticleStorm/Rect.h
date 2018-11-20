#pragma once
class Rect {
public:
	Rect();
	Rect(int x_, int y_, int w_, int h_);
	//Rect(Rect rect);
	~Rect();

	int x{};
	int y{};
	int w{};
	int h{};

	float halfW{};
	float halfH{};

	float centerX{};
	float centerY{};
};

