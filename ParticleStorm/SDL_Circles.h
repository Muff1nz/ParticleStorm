#pragma once
#include <SDL2/SDL.h>
#include <glm.hpp>

namespace SDL_Circles {
	void render_circle(const glm::vec2 & center, float radius, const SDL_Color & color, unsigned int sides);
	bool render_line(const glm::vec2 start, const glm::vec2 end, const SDL_Color& color);
	void set_render_color(SDL_Renderer* renderer, const SDL_Color& color);
	void get_render_color(SDL_Renderer* renderer, SDL_Color& oldc);
}
