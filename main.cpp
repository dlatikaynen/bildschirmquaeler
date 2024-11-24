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

#define SDL_VENDOR_INFO "SDL 3.1.6 statically linked for single-file distribution"
#undef main

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        const auto& err = SDL_GetError();
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

    Orchestrate(boundss.get(), renders.get());

    return 0;
}
