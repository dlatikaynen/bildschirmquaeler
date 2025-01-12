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
#include "SinWavesThere.h"
#include "SinWavesBack.h"
#include "GoL.h"
#include "Mandel.h"

int Orchestrate(
	std::vector<SDL_Rect>* boundss,
	std::vector<SDL_Renderer*>* renders
) {
	const constexpr bool doSquares = true;
	const constexpr bool doSinWavesThere = true;
	const constexpr bool doSinWavesBack = true;
	const constexpr bool doMandel = true;
	const constexpr bool doGoL = true;

	if (doSquares) {
		const auto rsr = RandomSquares(boundss, renders);
		if (rsr != 0)
		{
			return rsr;
		}
	}

	if (doSinWavesThere) {
		const auto swt = SinWavesThere(boundss, renders);
		if (swt != 0)
		{
			return swt;
		}
	}

	if (doSinWavesBack) {
		const auto swb = SinWavesBack(boundss, renders);
		if (swb != 0)
		{
			return swb;
		}
	}

	if (doMandel) {
		const auto mbr = Mandel(boundss, renders);
		if (mbr != 0)
		{
			return mbr;
		}
	}

	if (doGoL) {
		const auto gol = GoL(boundss, renders);
		if (gol != 0)
		{
			return gol;
		}
	}

	return 0;
}