#pragma once
#include "Environment.h"
#include "SDL_Circles.h"

#define _2PI 6.28f

/**
* Me stealy wheely https://forums.libsdl.org/viewtopic.php?p=46027
**/
void SDL_Circles::render_circle(const glm::vec2 & center, float radius, const SDL_Color & color, unsigned int sides) {
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
		render_line(start, end, color);
	}
}




bool SDL_Circles::render_line(const glm::vec2 start, const glm::vec2 end, const SDL_Color& color) {
	SDL_Color oldc;
	get_render_color(Environment::renderer, oldc);
	set_render_color(Environment::renderer, color);

	int ww, wh;
	SDL_GetWindowSize(Environment::window,
		&ww,
		&wh);
	// Draw a line 
	//--- 
	int ret =
		SDL_RenderDrawLine(
			Environment::renderer, // SDL_Renderer* renderer: the renderer in which draw 
			start.x,               // int x1: x of the starting point 
			wh - start.y,          // int y1: y of the starting point 
			end.x,                 // int x2: x of the end point 
			wh - end.y);           // int y2: y of the end point 
	if (ret != 0) {
		const char *error = SDL_GetError();
		if (*error != '\0') {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not renderDrawLine. SDL Error: %s at line #%d of file %s/n", error, __LINE__, __FILE__);
			SDL_ClearError();
		}
		set_render_color(Environment::renderer, oldc);
		return false;
	}
	set_render_color(Environment::renderer, oldc);
	return true;
}


void SDL_Circles::set_render_color(SDL_Renderer* renderer, const SDL_Color& color) {
	int ret = SDL_SetRenderDrawColor(renderer,
		color.r,
		color.g,
		color.b,
		color.a);
#ifdef DEBUG 
	DCHECKSDLERROR(ret);
#endif 
}

void SDL_Circles::get_render_color(SDL_Renderer* renderer, SDL_Color& oldc) {
	SDL_GetRenderDrawColor(renderer,
		&oldc.r,
		&oldc.g,
		&oldc.b,
		&oldc.a);
}

// in the header... 

#ifdef DEBUG 
#define DCHECKSDLERROR(ret) if (ret != 0)\ 
{\
const char *error = SDL_GetError(); \
if (*error != '\0')\
{\
SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL error: %s at line #%d of file %s", error, __LINE__, __FILE__); \
SDL_ClearError(); \
}\
}
#else 
#define DCHECKSDLERROR(ret) 
#endif 