
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <string>
#include "print.h"
#include "color.h"
#include "raycaster.h"
#include "imageLoader.h"
#include <thread>

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;

bool isMusicPlaying = true;  // Variable para controlar la reproducción de música

// Función que maneja la reproducción de música en un hilo separado
int PlayMusicThread() {
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8* wavBuffer;

    if (SDL_LoadWAV("../assets/fondo.wav", &wavSpec, &wavBuffer, &wavLength) == nullptr) {
        // Manejar el error en caso de que la carga de música falle.
        return 1;
    }

    if (SDL_OpenAudio(&wavSpec, nullptr) < 0) {
        // Manejar el error en caso de que la apertura de audio falle.
        return 1;
    }

    while (isMusicPlaying) {
        SDL_QueueAudio(1, wavBuffer, wavLength);
        SDL_PauseAudio(0);

        // Espera a que termine la reproducción actual
        SDL_Delay(wavLength * 1000 / wavSpec.freq);

        // Vuelve a cargar la música para reproducirla en bucle
        SDL_QueueAudio(1, wavBuffer, wavLength);
        SDL_PauseAudio(0);
    }

    // Libera los recursos al final de la reproducción
    SDL_CloseAudio();
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();
}

void clear(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);
}

void draw_floor(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 112, 112, 255);
    SDL_Rect rect = {
            SCREEN_WIDTH,
            SCREEN_HEIGHT / 2,
            SCREEN_WIDTH,
            SCREEN_HEIGHT / 2
    };
    SDL_RenderFillRect(renderer, &rect);
}

void draw_bg(SDL_Renderer* renderer) {
    int size = 256;
    ImageLoader::render(renderer, "b", 0, 0, 690, 180);
}

void draw_ui(SDL_Renderer* renderer, string playerImage, int width, int height) {
    int size = 64;
    ImageLoader::render(renderer, playerImage, SCREEN_WIDTH / 2.0f - width / 2.0f, SCREEN_HEIGHT - height, width, height);
}

int main(int argc, char* argv[])  {
    SDL_Init(SDL_INIT_VIDEO);
    ImageLoader::init();

    window = SDL_CreateWindow("DOOM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    ImageLoader::loadImage("+","../assets/skullN2.png");
    ImageLoader::loadImage("|","../assets/skullN1.png");
    ImageLoader::loadImage("-","../assets/skullN2.png");
    ImageLoader::loadImage("/","../assets/capsuleN1.png");
    ImageLoader::loadImage("1","../assets/churchN2.png");
    ImageLoader::loadImage("2","../assets/churchN5.png");
    ImageLoader::loadImage("5","../assets/skullN2.png");
    ImageLoader::loadImage("*","../assets/doorChurch.png");
    ImageLoader::loadImage("g","../assets/wallN2.png");
    ImageLoader::loadImage("p","../assets/gunV.png");
    ImageLoader::loadImage("pL","../assets/gunleftV.png");
    ImageLoader::loadImage("pR","../assets/gunrightV.png");
    ImageLoader::loadImage("b","../assets/sol.jpg");
    ImageLoader::loadImage("m","../assets/monster.png");
    ImageLoader::loadImage("mm","../assets/fondomp.png");


    Raycaster r = { renderer };
    r.load_map("../assets/map.txt");
    Color c = Color(20, 0, 0);
    Fog f = {renderer, 120, c};

    bool running = true;
    Uint32 frameStart;
    const int SCREEN_FPS = 144;  // Aumentado a 144 FPS
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
    Uint32 capTimer = 0;
    int countedFrames = 0;
    float speed = 10.0f;
    string playerImage = "p"; // Imagen predeterminada
    int widthP = 80;
    int heightP = 80;
    bool isMovingLeft = false;
    bool isMovingRight = false;

    bool keys[SDL_NUM_SCANCODES] = {false};

    std::thread musicThread(PlayMusicThread);
    while (running) {
        frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                keys[event.key.keysym.scancode] = true;
            }
            if (event.type == SDL_KEYUP) {
                keys[event.key.keysym.scancode] = false;
            }
        }

        if (keys[SDL_SCANCODE_LEFT]) {
            r.player.a += 3.14/24;
            r.player.mapA += (M_PI/24);
            playerImage = "pL"; // Cambia a la imagen de movimiento hacia la izquierda
            widthP = 120;
            heightP = 80;
            isMovingLeft = true;
        } else if (keys[SDL_SCANCODE_RIGHT]) {
            r.player.a -= 3.14/24;
            r.player.mapA -= (M_PI/24);
            playerImage = "pR"; // Cambia a la imagen de movimiento hacia la derecha
            widthP = 120;
            heightP = 80;
            isMovingRight = true;
        } else {
            // Cuando no se está moviendo a la izquierda o derecha, vuelve a la imagen predeterminada.
            if (isMovingLeft) {
                playerImage = "p"; // Cambia a la imagen predeterminada
                widthP = 80;
                heightP = 80;
                isMovingLeft = false;
            } else if (isMovingRight) {
                playerImage = "p"; // Cambia a la imagen predeterminada
                widthP = 80;
                heightP = 80;
                isMovingRight = false;
            }
        }
        if (keys[SDL_SCANCODE_UP]) {
            r.player.x += static_cast<int>(speed * cos(r.player.a));
            r.player.y += static_cast<int>(speed * sin(r.player.a));
            r.player.mapx = static_cast<int>(r.player.x / 3);
            r.player.mapy = static_cast<int>(r.player.y / 3);
        }
        if (keys[SDL_SCANCODE_DOWN]) {
            r.player.x -= static_cast<int>(speed * cos(r.player.a));
            r.player.y -= static_cast<int>(speed * sin(r.player.a));
            r.player.mapx = static_cast<int>(r.player.x / 3);
            r.player.mapy = static_cast<int>(r.player.y / 3);
        }

        clear(renderer);
        draw_floor(renderer);
        draw_bg(renderer);
        r.render();

        draw_ui(renderer, playerImage, widthP, heightP);
        f.draw();
        SDL_RenderPresent(renderer);

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
    musicThread.join();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}