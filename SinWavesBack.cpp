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

#include "SinWavesBack.h"

int SinWavesBack(
    std::vector<SDL_Rect>* boundss,
    std::vector<SDL_Renderer*>* renders
) {
    const auto& templateRects = std::make_unique<std::vector<SDL_FRect>>();
    const auto& renderedRects = std::make_unique<std::vector<SDL_FRect>>();

    const int expectedX = (boundss->at(0).w + 14) / 17 + 1;
    const int expectedY = (boundss->at(0).h + 14 / 17) + 1;
    const auto expectedNumberOfElements = static_cast<std::vector<SDL_FRect, std::allocator<SDL_FRect>>::size_type>(expectedY)* expectedX;

    templateRects->reserve(expectedNumberOfElements);
    renderedRects->reserve(expectedNumberOfElements);

    SDL_FRect rect{};
    int gridX = 0;
    int gridY = 0;

    for (int x = -7; x < boundss->at(0).w + 7; x += 17) {
        for (int y = boundss->at(0).h - 17 + 7; y > -7; y -= 17) {
            rect.x = static_cast<float>(x);
            rect.y = static_cast<float>(y);
            rect.w = 15;
            rect.h = 15;
            templateRects->push_back(rect);

            if (gridX == 0) {
                ++gridY;
            }
        }

        ++gridX;
    }

    const int countSquares = templateRects->size();
    const auto startTime = std::chrono::steady_clock::now();
    int curIndex = 0;

    while (true) {
        // update
        renderedRects->clear();
        for (const auto& entry : *templateRects) {
            if (abs(sin(gridY--)) > 0.2) {
                renderedRects->push_back(entry);
            }
        }

        curIndex = (curIndex + 1) % countSquares;

        // timing
        if (curIndex % 16 == 0) {
            const auto curTick = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(curTick - startTime).count();

            if (elapsed > 3) {
                break;
            }
        }

        // render
        int display = 0;
        for (const auto& renderer : *renders) {
            if (display == 0) {
                if (!SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x23, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else if (display == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else {
                if (!SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }

            if (!SDL_RenderClear(renderer)) {
                std::cerr << SDL_GetError();

                return 1;
            }

            if (display == 0) {
                if (!SDL_SetRenderDrawColor(renderer, 0x88, 0x7f, 0x25, 0xff)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else if (display == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0xFF, 0x88, 0x00, 0xef)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else {
                if (!SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x88, 0xef)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }

            const auto& nRects = (int)renderedRects->size();
            const auto& firstRect = renderedRects->data();

            if (!SDL_RenderFillRects(renderer, firstRect, nRects)) {
                std::cerr << SDL_GetError();

                return 1;
            }

            if (display == 0) {
                if (!SDL_SetRenderDrawColor(renderer, 0x77, 0x66, 0x55, 0xbb)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else if (display == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0x77, 0x55, 0x66, 0xbb)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else {
                if (!SDL_SetRenderDrawColor(renderer, 0x66, 0x77, 0x55 , 0xbb)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }

            if (!SDL_RenderRects(renderer, firstRect, nRects)) {
                std::cerr << SDL_GetError();

                return 1;
            }

            if (!SDL_RenderPresent(renderer)) {
                std::cerr << SDL_GetError();

                return 1;
            }

            ++display;
        }
    }

    return 0;
}