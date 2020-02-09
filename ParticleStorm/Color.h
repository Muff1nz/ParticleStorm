#pragma once

#include <sstream>

struct Color {	
	Color() {
		r = 255;
		g = 0;
		b = 225;
		a = 255;
	}

	Color(uint8_t r, uint8_t g, uint8_t b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};
