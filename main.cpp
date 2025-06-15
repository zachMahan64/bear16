#include <iostream>
#include "assembly.h"
#include "emu.h"
#include "SDL2/SDL.h"

void runSDLWindow() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Hello, SDL2!",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640, 480,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    SDL_Event e;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT ||
                (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
                }
        }

        SDL_Delay(16);  // ~60 FPS idle loop
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    //assembly::asmMnemonicSetGenerator();
    runSDLWindow();
    emu::Emulator emulator {};
    std::string path = "../programs/asm_test.asm";
    int exitCode = emulator.assembleAndRun(path);
    return exitCode;
}

