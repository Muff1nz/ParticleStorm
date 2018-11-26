#include "Rect.h"


Rect::Rect() = default;

Rect::Rect(const int x_, const int  y_, const int w_, const int h_) {
	x = x_; y = y_;
	w = w_; h = h_;
	halfW = w_ / 2.0f; halfH = h_ / 2.0f;
	centerX = x_ + w_ / 2.0f; centerY = y_ + h / 2.0f;
}

Rect::~Rect() = default;
