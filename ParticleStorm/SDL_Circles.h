#pragma once
#include <SDL2/SDL.h>
#include <glm.hpp>

namespace SDL_Circles {
	void render_circle(const glm::vec2& center, const float radius, unsigned sides);
	void render_line(const glm::vec2 start, const glm::vec2 end);
	void set_render_color(SDL_Renderer* renderer, const SDL_Color& color);
	void get_render_color(SDL_Renderer* renderer, SDL_Color& oldc);
}
