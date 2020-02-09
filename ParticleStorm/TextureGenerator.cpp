#include "TextureGenerator.h"

#include <iostream>

#include "PsTexture.h"
#include "SimplexNoise.h"
#include "Timer.h"


TextureGenerator::TextureGenerator() = default;


TextureGenerator::~TextureGenerator() = default;

std::string TextureGenerator::GetBackgroungTexture(Environment* environment, int width, int height) const {
	std::string fileName = "Textures/Background.png";

	const int resFactor = 10;
	width /= resFactor;
	height /= resFactor;

	int maxSide = width > height ? width : height;
	float frequency = 7.54f / maxSide;

	SimplexNoise simplex;

	PsTexture texture(width, height);		

	Timer timer;
	timer.Start();
	std::cout << "\nBACKGROUND TEXTURE: Started generating";

	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			float noise = simplex.Simplex2D(glm::vec3(x, y, 0), frequency);
			noise = simplex.scale01(noise);
			const uint8_t grayScaleValue = 155 + 100 * noise;
			texture.SetPixel(x, y, Color(grayScaleValue, grayScaleValue, grayScaleValue, 255));
		}
	}

	texture.WriteToDisk(fileName);

	timer.Stop();
	std::cout << "\nBACKGROUND TEXTURE: Finished generating in " << timer.ElapsedMilliseconds() << "ms";

	return fileName;
}

std::string TextureGenerator::GetParticleTexture() const {
	std::string fileName = "Textures/Circle.png";

	int radius = 65;
	int outlineThickness = 2;

	int width = radius * 2;
	int height = radius * 2;
		
	SimplexNoise simplex;

	PsTexture texture(width, height);

	Timer timer;
	timer.Start();
	std::cout << "\nPARTICLE TEXTURE: Started generating";

	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			int distanceToCenter = distance(glm::vec2(x, y), glm::vec2(radius, radius));
			if (distanceToCenter >= radius - outlineThickness && distanceToCenter < radius) {
				texture.SetPixel(x, y, Color(0, 0, 0, 255));
			}
			else if (distanceToCenter < radius) {
				float noise = simplex.Simplex2D(glm::vec3(x, y, 0), 0.071f);
				noise = simplex.scale01(noise);
				const uint8_t grayScaleValue = 0 + 50 * noise;
				texture.SetPixel(x, y, Color(grayScaleValue, grayScaleValue, grayScaleValue, 255));
			} else {
				texture.SetPixel(x, y, Color(0, 0, 0, 0));
			}
		}
	}
	
	texture.WriteToDisk(fileName);

	timer.Stop();
	std::cout << "\nPARTICLE TEXTURE: Finished generating in " << timer.ElapsedMilliseconds() << "ms";

	return fileName;
}
