#pragma once

#include <sstream>
#include "Color.h"

class PsTexture {
public:
	PsTexture(int width, int height);
	~PsTexture();
	
	void SetPixel(int x, int y, Color color) const;
	Color GetPixel(int x, int y) const;

	void WriteToDisk(std::string fileName) const;

	int GetHeight() const;
	int GetWidth() const;

private:
	const int channels = 4;
	
	int width;
	int height;
	int stride;	
	int totalLength;

	uint8_t* interntalBuffer;

	int ConverToIndex(int x, int y) const;
};
