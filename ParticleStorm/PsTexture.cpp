#include "PsTexture.h"

#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

PsTexture::PsTexture(int width, int height) {
	this->width = width;
	this->height = height;
	stride = width * channels;
	totalLength = width * height * channels;

	interntalBuffer = new uint8_t[totalLength];
}

PsTexture::~PsTexture() {
	delete interntalBuffer;
}

void PsTexture::SetPixel(int x, int y, Color color) const {
	int index = ConverToIndex(x, y);
	interntalBuffer[index] = color.r;
	interntalBuffer[index + 1] = color.g;
	interntalBuffer[index + 2] = color.b;
	interntalBuffer[index + 3] = color.a;
}

Color PsTexture::GetPixel(int x, int y) const {
	int index = ConverToIndex(x, y);
	Color color;
	color.r = interntalBuffer[index];
	color.g = interntalBuffer[index + 1];
	color.b = interntalBuffer[index + 2];
	color.a = interntalBuffer[index + 3];
	return color;
}

int PsTexture::ConverToIndex(int x, int y) const {
	return (x + y * width) * channels;
}

void PsTexture::WriteToDisk(std::string fileName) const {
	std::fstream textureFile(fileName);
	textureFile.close();

	stbi_write_png(&fileName[0], width, height, channels, interntalBuffer, stride);
}

int PsTexture::GetHeight() const {
	return height;
}

int PsTexture::GetWidth() const {
	return width;
}
