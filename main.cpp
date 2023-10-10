#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <string>  // Para trabajar con cadenas de texto
#include "print.h"
#include "color.h"
#include "raycaster.h"

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;

void clear() {
    SDL_SetRenderDrawColor(renderer, B.r, B.g, B.b, B.a);
    SDL_RenderClear(renderer);
}

int main(int argc, char* argv[])  {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("DOOM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Raycaster r = { renderer };
    r.load_map("../assets/map.txt");

    bool running = true;
    Uint32 frameStart;  // Para medir el tiempo de inicio de cada cuadro
    const int SCREEN_FPS = 60;  // FPS objetivo
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;  // Duración de un cuadro en milisegundos
    int countedFrames = 0;  // Contador de cuadros
    Uint32 capTimer = 0;  // Temporizador para el límite de FPS

    while (running) {
        frameStart = SDL_GetTicks();  // Obtener el tiempo de inicio de este cuadro

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym ){
                    case SDLK_LEFT:
                        r.player.x -= 10;
                        break;
                    case SDLK_RIGHT:
                        r.player.x += 10;
                        break;
                    case SDLK_UP:
                        r.player.y -= 10;
                        break;
                    case SDLK_DOWN:
                        r.player.y += 10;
                        break;
                    case SDLK_a:
                        r.player.a += 3.14/12;
                        break;
                    case SDLK_d:
                        r.player.a -= 3.14/12;
                        break;
                    default:
                        break;
                }
            }
        }

        clear();
        r.render();
        SDL_RenderPresent(renderer);

        // Calcular FPS y actualizar el título de la ventana
        ++countedFrames;
        int frameTicks = SDL_GetTicks() - frameStart;
        if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }

        if (SDL_GetTicks() - capTimer >= 1000) {
            capTimer = SDL_GetTicks();
            string windowTitle = "DOOM - FPS: " + to_string(countedFrames);
            SDL_SetWindowTitle(window, windowTitle.c_str());
            countedFrames = 0;
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
