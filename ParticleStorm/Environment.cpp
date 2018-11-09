#include "Environment.h"



Environment::Environment() = default;
Environment::~Environment() = default;

SDL_Renderer* Environment::renderer = nullptr;
SDL_Window* Environment::window = nullptr;
