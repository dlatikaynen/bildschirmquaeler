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
#include "GoL.h"

int Orchestrate(
	std::vector<SDL_Rect>* boundss,
	std::vector<SDL_Renderer*>* renders
) {
	const auto rsr = RandomSquares(boundss, renders);
	if (rsr != 0)
	{
		return rsr;
	}

	const auto gol = GoL(boundss, renders);
	if (gol != 0)
	{
		return gol;
	}
}