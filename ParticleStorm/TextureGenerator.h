#pragma once

#include "PsTexture.h"
#include "Range.h"

class TextureGenerator {
public:
	TextureGenerator();
	~TextureGenerator();

	std::string GetBackgroungTexture(int width, int height) const;
	void GenerateBackground(Range range, PsTexture* texture) const;
	std::string GetParticleTexture() const;
};

