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

#include "RandomSquares.h"

int RandomSquares(
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
        for (int y = -7; y < boundss->at(0).h + 7; y += 17) {
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

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution dis(-0.2, 0.87);
    static std::uniform_int_distribution col(0xef, 0xff);
    static std::uniform_int_distribution col2(55, 66);
    static std::uniform_int_distribution trans(0xee, 0xf3);

    const auto countSquares = templateRects->size();
    const auto startTime = std::chrono::steady_clock::now();
    int prevIndex = -1;
    int curIndex = 0;
    SDL_Event event;

    while (!(SDL_PollEvent(&event) == 1 && (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)))
    {
        // update
        renderedRects->clear();

        if (prevIndex != -1)
        {
            templateRects->at(prevIndex).w = 15;
            templateRects->at(prevIndex).h = 15;
        }

        templateRects->at(curIndex).w = 100;
        templateRects->at(curIndex).h = 100;
        std::swap(templateRects->at(curIndex), templateRects->at(countSquares - 1));

        for (const auto& entry : *templateRects) {
            if (sin(gridX++) * dis(gen) > 0) {
                renderedRects->push_back(entry);
            }
        }

        prevIndex = curIndex;
        curIndex = (curIndex + 1) % countSquares;

        // timing
        if (curIndex % 16 == 0) {
            const auto curTick = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(curTick - startTime).count();

            if (elapsed > 4) {
                break;
            }
        }

        // render
        int display = 0;
        for (const auto& renderer : *renders) {
            if (display == 0) {
                if (!SDL_SetRenderDrawColor(renderer, col2(gen), 0x00, 0xFF, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else if (display == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else {
                if (!SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }

            if (!SDL_RenderClear(renderer)) {
                std::cerr << SDL_GetError();

                return 1;
            }

            if (display == 0) {
                if (!SDL_SetRenderDrawColor(renderer, col(gen), col2(gen), 0x88, trans(gen))) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else if (display == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0x00, 0x88, 0xFF, 0xef)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else {
                if (!SDL_SetRenderDrawColor(renderer, 0x88, 0xFF, 0x00, 0xef)) {
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
                if (!SDL_SetRenderDrawColor(renderer, 0x55, 0x66, 0x77, 0xbb)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else if (display == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0x66, 0x55, 0x77, 0xbb)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }
            else {
                if (!SDL_SetRenderDrawColor(renderer, 0x55, 0x77, 0x66, 0xbb)) {
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