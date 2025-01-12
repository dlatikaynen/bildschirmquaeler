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

#include "GoL.h"

typedef struct cell {
    SDL_FRect onScreen;
    cell* neighbors[8];
    bool isAliveEven;
    bool isAliveOddd;
    int killCount; // how often another cell was killed because of this
    int longevity; // number of frames this cell has survived
} cell;

constexpr const int NW = 0;
constexpr const int WW = 1;
constexpr const int SW = 2;
constexpr const int SS = 3;
constexpr const int SE = 4;
constexpr const int EE = 5;
constexpr const int NE = 6;
constexpr const int NN = 7;

int width = 0;
int hight = 0;

static int indexAt(int x, int y) noexcept {
    if (x == -1) {
        x = width - 1;
    }
    else if (x == width) {
        x = 0;
    }

    if (y == -1) {
        y = hight - 1;
    }
    else if (y == hight) {
        y = 0;
    }

    return x * hight + y;
}

int GoL(
    std::vector<SDL_Rect>* boundss,
    std::vector<SDL_Renderer*>* renders 
) {
    const auto& cells = std::make_unique<std::vector<cell>>();
    const auto& lives = std::make_unique<std::vector<SDL_FRect>>();

    const int expectedX = (boundss->at(0).w + 14) / 17 + 1;
    const int expectedY = (boundss->at(0).h + 14 / 17) + 1;
    const auto expectedNumberOfElements = static_cast<std::vector<SDL_FRect, std::allocator<SDL_FRect>>::size_type>(expectedY)* expectedX;

    cells->reserve(expectedNumberOfElements);

    int gridX = 0;
    int gridY = 0;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution dis(0, 5);

    cell cell{};
    for (int x = -7; x < boundss->at(0).w + 7; x += 17) {
        ++width;
        for (int y = -7; y < boundss->at(0).h + 7; y += 17) {
            cell.isAliveEven = false;
            cell.isAliveOddd = dis(gen) > 3;
            cell.killCount = 0;
            cell.longevity = 0;
            cell.onScreen.x = static_cast<float>(x);
            cell.onScreen.y = static_cast<float>(y);
            cell.onScreen.w = 15;
            cell.onScreen.h = 15;
            cells->push_back(cell);

            if (gridX == 0) {
                ++gridY;
                ++hight;
            }
        }

        ++gridX;
    }

    // compute neighbors
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < hight; ++y) {
            auto& center = cells->at(indexAt(x, y));

            center.neighbors[NW] = &cells->at(indexAt(x - 1, y - 1));
            center.neighbors[WW] = &cells->at(indexAt(x - 1, y));
            center.neighbors[SW] = &cells->at(indexAt(x - 1, y + 1));
            center.neighbors[SS] = &cells->at(indexAt(x, y + 1));
            center.neighbors[SE] = &cells->at(indexAt(x + 1, y + 1));
            center.neighbors[EE] = &cells->at(indexAt(x + 1, y));
            center.neighbors[NE] = &cells->at(indexAt(x + 1, y - 1));
            center.neighbors[NN] = &cells->at(indexAt(x, y - 1));
        }
    }

    const int countSquares{ static_cast<int>(cells->size()) };
    static std::uniform_int_distribution ttl(0, countSquares);
    const auto startTime = std::chrono::steady_clock::now();
    const int threePerc = static_cast<int>(countSquares / 100.0 * 17.1);
    int frame = 0;
    int pass = 0;
    SDL_Event event;

    // prepare a palette for the longevity colorization
    
    while (!(SDL_PollEvent(&event) == 1 && event.type == SDL_EVENT_QUIT))
    {
        // update
        if ((frame % 5) == 0) {
            const bool isEven = (pass++ % 2) == 0;
            const size_t livingBefore = lives->size();
            const bool doSpawn = livingBefore < threePerc;

            lives->clear();
            for (auto& entry : *cells) {
                // count the neighbors
                int numNeigh = 0;
                if (isEven)
                {
                    numNeigh =
                        (entry.neighbors[0]->isAliveOddd ? 1 : 0) +
                        (entry.neighbors[1]->isAliveOddd ? 1 : 0) +
                        (entry.neighbors[2]->isAliveOddd ? 1 : 0) +
                        (entry.neighbors[3]->isAliveOddd ? 1 : 0) +
                        (entry.neighbors[4]->isAliveOddd ? 1 : 0) +
                        (entry.neighbors[5]->isAliveOddd ? 1 : 0) +
                        (entry.neighbors[6]->isAliveOddd ? 1 : 0) +
                        (entry.neighbors[7]->isAliveOddd ? 1 : 0);

                    if (!entry.isAliveOddd && numNeigh == 3) {
                        entry.isAliveEven = true;
                    }
                    else if (entry.isAliveOddd && numNeigh != 2 && numNeigh != 3) {
                        entry.isAliveEven = false;
                    }
                    else {
                        entry.isAliveEven = entry.isAliveOddd;
                        ++entry.longevity;
                    }
                }
                else {
                    numNeigh =
                        (entry.neighbors[0]->isAliveEven ? 1 : 0) +
                        (entry.neighbors[1]->isAliveEven ? 1 : 0) +
                        (entry.neighbors[2]->isAliveEven ? 1 : 0) +
                        (entry.neighbors[3]->isAliveEven ? 1 : 0) +
                        (entry.neighbors[4]->isAliveEven ? 1 : 0) +
                        (entry.neighbors[5]->isAliveEven ? 1 : 0) +
                        (entry.neighbors[6]->isAliveEven ? 1 : 0) +
                        (entry.neighbors[7]->isAliveEven ? 1 : 0);

                    if (!entry.isAliveEven && numNeigh == 3) {
                        entry.isAliveOddd = true;
                    }
                    else if (entry.isAliveEven && numNeigh != 2 && numNeigh != 3) {
                        entry.isAliveOddd = false;
                    }
                    else {
                        entry.isAliveOddd = entry.isAliveEven || (doSpawn && ttl(gen) < threePerc);
                        ++entry.longevity;
                    }
                }

                if ((isEven && entry.isAliveEven) || (!isEven && entry.isAliveOddd)) {
                    lives->push_back(entry.onScreen);
                }
            }
        }

        // timing
        if (frame % 16 == 0) {
            const auto curTick = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(curTick - startTime).count();

            //if (elapsed > 300) {
            //    break;
            //}
        }

        // render
        int display = 0;
        for (const auto& renderer : *renders) {
            if (display == 0) {
                if (!SDL_SetRenderDrawColor(renderer, 0x33, 0xdd, 0x12, 0xFF)) {
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
                if (!SDL_SetRenderDrawColor(renderer, 0xff, 0xdd, 0xbb, 0xff)) {
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

            const auto& nRects = (int)lives->size();
            const auto& firstRect = lives->data();

            if (nRects != 0) {
                if (!SDL_RenderFillRects(renderer, firstRect, nRects)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }

            if (display == 0) {
                if (!SDL_SetRenderDrawColor(renderer, 0xf0, 0xd0, 0xb0, 0xbb)) {
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

            if (nRects != 0) {
                if (!SDL_RenderRects(renderer, firstRect, nRects)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }

            if (!SDL_RenderPresent(renderer)) {
                std::cerr << SDL_GetError();

                return 1;
            }

            ++display;
        }

        ++frame;
    }

    return 0;
}