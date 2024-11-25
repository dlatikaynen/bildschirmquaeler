#include <SDL3/SDL.h>
#include <Windows.h>
#include <io.h>
#include <iostream>
#include <math.h>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <complex>

#include "Mandel.h"

int Mandel(
    std::vector<SDL_Rect>* boundss,
    std::vector<SDL_Renderer*>* renders
) {
    std::complex<double> z{};
    std::complex<double> c{};

    constexpr const int maxIter = 0xff;
    int i;

    while (true) {
        int display = 0;

        for (const auto& renderer : *renders) {
            const auto& bounds = boundss->at(display);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
            SDL_RenderClear(renderer);
            for (double x = -2.0; x <= 1.0; x += 0.02) {
                for (double y = -2.0; y <= 2.0; y += 0.02) {
                    c = { x, y };
                    z = { 0, 0 };

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
                    for (i = 0; i < maxIter; ++i) {
                        z = z * z + c;

                        if (abs(z) > 2) {
                            SDL_SetRenderDrawColor(
                                renderer,
                                i % 0xff,
                                0xff - (i % 0xff),
                                i % 0xff,
                                0xFF
                            );

                            break;
                        }
                    }

                    SDL_RenderPoint(renderer, bounds.w / 2 + 200 * x, bounds.h / 2 + 200 * y);
                }
            }

            SDL_RenderPresent(renderer);
            ++display;
        }
    }

    return 0;
}