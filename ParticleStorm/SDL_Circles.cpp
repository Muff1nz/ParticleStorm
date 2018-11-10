#pragma once
#include "Environment.h"
#include "SDL_Circles.h"

#define _2PI 6.28f
#define SCREEN_WIDTH 2400
#define SCREEN_HEIGHT 1300

/**
* Me stealy wheely https://forums.libsdl.org/viewtopic.php?p=46027
**/
void SDL_Circles::render_circle(const glm::vec2 & center, const float radius, unsigned int sides) {
	if (sides == 0) {
		sides = _2PI*radius / 2;
	}

	float d_a = _2PI / sides, angle = d_a;

	glm::vec2 start, end;
	end.x = radius;
	end.y = 0.0f;
	end = end + center;
	for (int i = 0; i != sides; i++) {
		start = end;
		end.x = cos(angle) * radius;
		end.y = sin(angle) * radius;
		end = end + center;
		angle += d_a;
		render_line(start, end);
		render_line(center, end);
	}
}




void SDL_Circles::render_line(const glm::vec2 start, const glm::vec2 end) {
	// Draw a line 
	//--- 
	int ret =
		SDL_RenderDrawLine(
			Environment::renderer, // SDL_Renderer* renderer: the renderer in which draw 
			start.x,               // int x1: x of the starting point 
			SCREEN_HEIGHT - start.y,          // int y1: y of the starting point 
			end.x,                 // int x2: x of the end point 
			SCREEN_HEIGHT - end.y);           // int y2: y of the end point 
}


void SDL_Circles::set_render_color(SDL_Renderer* renderer, const SDL_Color& color) {
	int ret = SDL_SetRenderDrawColor(renderer,
		color.r,
		color.g,
		color.b,
		color.a);
}

void SDL_Circles::get_render_color(SDL_Renderer* renderer, SDL_Color& oldc) {
	SDL_GetRenderDrawColor(renderer,
		&oldc.r,
		&oldc.g,
		&oldc.b,
		&oldc.a);
}