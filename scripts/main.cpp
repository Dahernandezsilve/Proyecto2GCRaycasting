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

void drawImages(SDL_Renderer* renderer, string playerImage, int width, int height) {
}

void applyChromaticAberration(SDL_Renderer* renderer) {
    int offsetR = 2; // Desplazamiento en el canal rojo
    int offsetG = 0; // Desplazamiento en el canal verde
    int offsetB = 2; // Desplazamiento en el canal azul

    // Captura la textura actual del renderer
    SDL_Texture* currentTexture = SDL_GetRenderTarget(renderer);

    // Crea una textura temporal para aplicar el efecto de aberración cromática
    SDL_Texture* tempTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, tempTexture);

    // Copia la textura original en la textura temporal con desplazamiento de canales
    SDL_SetTextureColorMod(currentTexture, 255, 0, 0); // Canal rojo
    SDL_RenderCopy(renderer, currentTexture, NULL, NULL);
    SDL_SetTextureColorMod(currentTexture, 0, 255, 0); // Canal verde
    SDL_RenderCopy(renderer, currentTexture, NULL, NULL);
    SDL_SetTextureColorMod(currentTexture, 0, 0, 255); // Canal azul
    SDL_RenderCopy(renderer, currentTexture, NULL, NULL);

    // Restablece el render target original
    SDL_SetRenderTarget(renderer, currentTexture);

    // Dibuja la textura temporal en la pantalla
    SDL_RenderCopy(renderer, tempTexture, NULL, NULL);

    // Limpia la textura temporal
    SDL_DestroyTexture(tempTexture);
}

void applyWavesDistortion(SDL_Renderer* renderer) {
    double amplitude = 10.0; // Amplitud de las ondas
    double frequency = 0.1; // Frecuencia de las ondas

    // Captura la textura actual del renderer
    SDL_Texture* currentTexture = SDL_GetRenderTarget(renderer);

    // Crea una textura temporal para aplicar el efecto de ondas
    SDL_Texture* tempTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, tempTexture);

    // Aplica la distorsión sinusoidal a la imagen
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        int offsetX = static_cast<int>(amplitude * sin(frequency * x));
        SDL_Rect sourceRect = {x, 0, 1, SCREEN_HEIGHT};
        SDL_Rect destRect = {x + offsetX, 0, 1, SCREEN_HEIGHT};
        SDL_RenderCopyEx(renderer, currentTexture, &sourceRect, &destRect, 0.0, NULL, SDL_FLIP_NONE);
    }

    // Restablece el render target original
    SDL_SetRenderTarget(renderer, currentTexture);

    // Dibuja la textura temporal en la pantalla
    SDL_RenderCopy(renderer, tempTexture, NULL, NULL);

    // Limpia la textura temporal
    SDL_DestroyTexture(tempTexture);
}

void showWelcomeScreen(SDL_Renderer* renderer, bool& showWelcome) {
    std::vector<std::string> imageKeys = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};

    Uint32 frameInterval = 250;
    int frameIndex = 0;  // Índice del cuadro actual

    bool quit = false;
    while (!quit) {
        // Dibuja el cuadro actual en la ventana
        ImageLoader::render(renderer, imageKeys[frameIndex], 0, 0, 780, 330);
        SDL_RenderPresent(renderer);

        // Espera el tiempo de intervalo entre cuadros
        SDL_Delay(frameInterval);

        // Avanza al siguiente cuadro
        frameIndex++;
        if (frameIndex >= imageKeys.size()) {
            frameIndex = 0;  // Reiniciar la animación al llegar al final
        }

        // Maneja eventos, como la salida de la ventana y eventos de teclado
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Salir si el usuario cierra la ventana
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                // Al presionar cualquier tecla, ocultar la pantalla de bienvenida
                quit = true;
                showWelcome = false;
            }
        }
    }
}


bool hasWon = false;

bool showWelcome = true;

int main(int argc, char* argv[])  {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_Init(SDL_INIT_VIDEO);
    ImageLoader::init();

    window = SDL_CreateWindow("DOOM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
     // Captura el mouse

    ImageLoader::loadImage("+","../assets/skullN2.png");
    ImageLoader::loadImage("|","../assets/skullN1.png");
    ImageLoader::loadImage("-","../assets/skullN2.png");
    ImageLoader::loadImage("/","../assets/capsuleN1.png");
    ImageLoader::loadImage("x","../assets/churchN2.png");
    ImageLoader::loadImage("y","../assets/churchN5.png");
    ImageLoader::loadImage("z","../assets/skullN2.png");
    ImageLoader::loadImage("*","../assets/doorChurch.png");
    ImageLoader::loadImage("g","../assets/wallN2.png");
    ImageLoader::loadImage("p","../assets/gunV.png");
    ImageLoader::loadImage("pL","../assets/gunleftV.png");
    ImageLoader::loadImage("pR","../assets/gunrightV.png");
    ImageLoader::loadImage("b","../assets/sol.jpg");
    ImageLoader::loadImage("m","../assets/monster.png");
    ImageLoader::loadImage("mm","../assets/fondomp.png");
    ImageLoader::loadImage("0","../assets/welcomeScreen/0.png");
    ImageLoader::loadImage("1","../assets/welcomeScreen/1.png");
    ImageLoader::loadImage("2","../assets/welcomeScreen/2.png");
    ImageLoader::loadImage("3","../assets/welcomeScreen/3.png");
    ImageLoader::loadImage("4","../assets/welcomeScreen/4.png");
    ImageLoader::loadImage("5","../assets/welcomeScreen/5.png");
    ImageLoader::loadImage("6","../assets/welcomeScreen/6.png");
    ImageLoader::loadImage("7","../assets/welcomeScreen/7.png");
    ImageLoader::loadImage("8","../assets/welcomeScreen/8.png");
    ImageLoader::loadImage("0c","../assets/loseScreen/0C.png");
    ImageLoader::loadImage("1c","../assets/loseScreen/1C.png");
    ImageLoader::loadImage("2c","../assets/loseScreen/2C.png");
    ImageLoader::loadImage("3c","../assets/loseScreen/3C.png");
    ImageLoader::loadImage("4c","../assets/loseScreen/4C.png");
    ImageLoader::loadImage("5c","../assets/loseScreen/5C.png");
    ImageLoader::loadImage("6c","../assets/loseScreen/6C.png");
    ImageLoader::loadImage("7c","../assets/loseScreen/7C.png");
    ImageLoader::loadImage("8c","../assets/loseScreen/8C.png");


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
    showWelcomeScreen(renderer, showWelcome);

    std::thread musicThread(PlayMusicThread);
    int prevMouseX = 0;

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

            // Captura los movimientos del mouse
            if (event.type == SDL_MOUSEMOTION) {
                int mouseX = event.motion.x;
                print(event.motion.x);
                int mouseXDelta = mouseX - prevMouseX;
                prevMouseX = mouseX;
                // Ajusta la dirección de vista del jugador en función del movimiento del mouse
                r.player.a -= static_cast<float>(mouseXDelta) * 0.02f; // Puedes ajustar el factor de sensibilidad
                r.player.mapA -= static_cast<float>(mouseXDelta) * 0.02f;
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


        if (hasWon) {
            // El jugador ha ganado, muestra la pantalla de victoria.
            r.draw_victory_screen();
        }

        if (r.has_won()) {
            hasWon = true;
        }

        clear(renderer);
        applyChromaticAberration(renderer);
        applyWavesDistortion(renderer);
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