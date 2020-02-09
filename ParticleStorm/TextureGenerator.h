#pragma once
#include "Environment.h"
#include "PsTexture.h"

class Environment;

class TextureGenerator {
public:
	TextureGenerator();
	~TextureGenerator();

	std::string GetBackgroungTexture(Environment* environment, int width, int height) const;
	void GenerateBackground(Range range, PsTexture* texture) const;
	std::string GetParticleTexture() const;
};

