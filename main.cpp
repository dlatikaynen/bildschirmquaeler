#include <SDL3/SDL.h>
#include <Windows.h>
#include <io.h>
#include <iostream>
#include <math.h>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <string>

#define SDL_VENDOR_INFO "SDL 3.1.6 statically linked for single-file distribution"
#undef main

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        const auto &err = SDL_GetError();
        std::cerr 
            << "SDL_Init failed\n"
            << err;

        return 1;
    }

    int nDisplays = 0;
    const auto &displays = SDL_GetDisplays(&nDisplays);

    if (nDisplays == 0) {
        std::cerr << "Congratulations, you attempted to start a screen saver on a machine with no display";

        return 1;
    }

    const auto &boundss = std::make_unique<std::vector<SDL_Rect>>();
    const auto &windows = std::make_unique<std::vector<SDL_Window *>>();
    const auto &renders = std::make_unique<std::vector<SDL_Renderer *>>();
    for (int i = nDisplays - 1; i >= 0; --i) {
        const auto display = displays[i];
        SDL_Rect bounds;
        SDL_GetDisplayBounds(display, &bounds);

        SDL_Window *window = nullptr;
        SDL_Renderer *renderer = nullptr;

        if (!SDL_CreateWindowAndRenderer(
            "JUST WAIT",
            bounds.w,
            bounds.h,
            SDL_WINDOW_FULLSCREEN,
            &window,
            &renderer)
        ) {
            std::cerr << SDL_GetError();

            return 1;
        }

        const auto &name = SDL_GetRendererName(renderer);
        const auto &props = SDL_GetRendererProperties(renderer);
        const auto &driver = SDL_GetStringProperty(props, SDL_PROP_RENDERER_NAME_STRING, name);
        std::stringstream s;

        s << "I torture #";
        s << display;
        s << " with ";
        s << driver;

        auto str = s.str();

        std::transform(str.begin(), str.end(), str.begin(), ::toupper);

        const auto &sister = str.c_str();

        SDL_SetWindowTitle(window, sister);
        boundss->push_back(bounds);
        windows->push_back(window);
        renders->push_back(renderer);
        SDL_SetWindowPosition(window, bounds.x, bounds.y);
        SDL_ShowWindow(window);
        if (i == 0) {
            SDL_HideCursor();
        }

        if (!SDL_SetWindowFullscreenMode(window, NULL)) {
            std::cerr << SDL_GetError();

            return 1;
        }

        if (!SDL_SyncWindow(window)) {
            std::cerr << SDL_GetError();

            return 1;
        }
    }

    const auto &templateRects = std::make_unique<std::vector<SDL_FRect>>();
    const auto &renderedRects = std::make_unique<std::vector<SDL_FRect>>();

    const int expectedX = (boundss->at(0).w + 14) / 17 + 1;
    const int expectedY = (boundss->at(0).h + 14 / 17) + 1;
    const auto expectedNumberOfElements = static_cast<std::vector<SDL_FRect, std::allocator<SDL_FRect>>::size_type>(expectedY) * expectedX;

    templateRects->reserve(expectedNumberOfElements);
    renderedRects->reserve(expectedNumberOfElements);

    SDL_FRect rect{};
    int gridX = 0;
    int gridY = 0;

    for (int x = -7; x < boundss->at(0).w + 7; x += 17) {
        for (int y = -7; y < boundss->at(0).h + 7; y += 17) {
            rect.x = (float)x;
            rect.y = (float)y;
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
    static std::uniform_int_distribution col(0, 0xff);
    static std::uniform_int_distribution col2(55, 0xac);
    static std::uniform_int_distribution trans(0xee, 0xff);

    while (true) {
        // update
        renderedRects->clear();
        for (const auto &entry : *templateRects) {
            if (sin(gridX++) * dis(gen) > 0) {
                renderedRects->push_back(entry);
            }
        }

        // render
        int pass = 0;
        for (const auto &renderer : *renders) {
            if (pass == 0) {
                if (!SDL_SetRenderDrawColor(renderer, col2(gen), 0x00, 0xFF, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            } else if (pass == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            } else {
                if (!SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0xFF)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }

            if (!SDL_RenderClear(renderer)) {
                std::cerr << SDL_GetError();

                return 1;
            }

            if (pass == 0) {
                if (!SDL_SetRenderDrawColor(renderer, col(gen), col2(gen), 0x88, trans(gen))) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            } else if (pass == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0x00, 0x88, 0xFF, 0xef)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            } else {
                if (!SDL_SetRenderDrawColor(renderer, 0x88, 0xFF, 0x00, 0xef)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            }

            const auto &nRects = (int)renderedRects->size();
            const auto &firstRect = renderedRects->data();

            if (!SDL_RenderFillRects(renderer, firstRect, nRects)) {
                std::cerr << SDL_GetError();

                return 1;
            }

            if (pass == 0) {
                if (!SDL_SetRenderDrawColor(renderer, 0x55, 0x66, 0x77, 0xbb)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            } else if (pass == 1) {
                if (!SDL_SetRenderDrawColor(renderer, 0x66, 0x55, 0x77, 0xbb)) {
                    std::cerr << SDL_GetError();

                    return 1;
                }
            } else {
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

            ++pass;
        }
    }

    return 0;
}
