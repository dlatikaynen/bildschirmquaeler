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

    const auto startTime = std::chrono::steady_clock::now();
    constexpr const int maxIter = 0xff;
    int i;
    int frame = 0;
    int pow = 2;
    SDL_Event event;

    while (!(SDL_PollEvent(&event) == 1 && (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)))
    {
        int display = 0;

        ++frame;
        ++pow;

        // timing
        if (frame % 16 == 0) {
            const auto curTick = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(curTick - startTime).count();

            if (elapsed > 20) {
                break;
            }
        }

        for (const auto& renderer : *renders) {
            const auto& bounds = boundss->at(display);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
            SDL_RenderClear(renderer);
            for (double x = -2.0; x <= 2.0; x += 0.02) {
                for (double y = -2.0; y <= 2.0; y += 0.02) {
                    c = { x, y };
                    z = { 0, 0 };

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
                    for (i = 0; i < maxIter; ++i) {
                        // See that's why I love C++ so much
                        // Can your Rust do that?
                        // ♫ ♪ ♫ Can your Rust do that? ♫ ♪ ♫
                        // I don't fricking think so.
                        z = std::pow(z, pow / 10.0) + c;

                        if (z.real() * z.real() + z.imag() * z.imag() > 4) {
                            SDL_SetRenderDrawColor(
                                renderer,
                                static_cast<int>(std::abs(z) * 255) % 0xff,
                                0xff - (static_cast<int>(std::abs(c) * 255) % 0xff),
                                static_cast<int>(std::abs(x * 128.0)) % 0xff,
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