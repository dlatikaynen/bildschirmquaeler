#include <SDL3/SDL.h>
#include <Windows.h>
#include <io.h>
#include <iostream>
#include <math.h>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <string>

#include "Orchestrator.h"
#include "RandomSquares.h"

int Orchestrate(
	std::vector<SDL_Rect>* boundss,
	std::vector<SDL_Renderer*>* renders
) {
	return RandomSquares(
		boundss,
		renders
	);
}