#pragma once

#include "print.h"
#include <iostream>
#include <fstream>
#include <SDL_render.h>
#include <string>
#include <vector>
#include <cmath>
#include <SDL.h>
#include <unordered_map>
#include "color.h"
#include "imageLoader.h"
#include <omp.h>

using namespace std;

const Color B = {0, 0, 0};
const Color W = {255, 255, 255};

const int WIDTH = 23;
const int HEIGHT = 11;
const int MAPBLOCK = 15;
const int MAPWIDTH = MAPBLOCK * WIDTH;
const int MAPHEIGHT = MAPBLOCK * HEIGHT;
const int BLOCK = 30;
const int SCREEN_WIDTH = BLOCK * WIDTH;
const int SCREEN_HEIGHT = BLOCK * HEIGHT;

struct Player {
    int x;
    int y;
    int mapx;
    int mapy;
    float a;
    float mapA;
    float fov;
};


struct Impact {
    float d;
    string mapHit;
    int ofx;
};

class Raycaster {
public:
    Raycaster(SDL_Renderer* renderer)
            : renderer(renderer) {
        player.x = BLOCK + BLOCK / 2;
        player.y = BLOCK + BLOCK / 2;
        player.mapx = static_cast<int>(static_cast<int>(BLOCK/3) + static_cast<int>(BLOCK/3) / 2);
        player.mapy = static_cast<int>(static_cast<int>(BLOCK/3) + static_cast<int>(BLOCK/3) / 2) ;
        player.a = M_PI / 4.0f;
        player.mapA = ((M_PI) / 4.0f);
        player.fov = M_PI /4.0f;
        scale = 40;
        textSize = 128;
    }

    Player player;

    void load_map(const string& filename) {
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            map.push_back(line);
        }
        file.close();
    }

    void point(int x, int y, Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderDrawPoint(renderer, x, y);
    }

// Nueva función altamente optimizada para dibujar múltiples puntos a la vez con colores variables
    void points(const vector<pair<int, int>>& pointList, const vector<Color>& colors) {
        int count = static_cast<int>(pointList.size());
        vector<SDL_Point> sdlPoints(count);

        #pragma omp parallel for
        for (int i = 0; i < count; i++) {
            sdlPoints[i] = {pointList[i].first, pointList[i].second};
        }

        vector<Uint8> r(count);
        vector<Uint8> g(count);
        vector<Uint8> b(count);
        vector<Uint8> a(count);


        for (int i = 0; i < count; i++) {
            r[i] = colors[i].r;
            g[i] = colors[i].g;
            b[i] = colors[i].b;
            a[i] = colors[i].a;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color de fondo (puedes cambiarlo)

        int startIndex = 0;
        int endIndex = 1;

        while (endIndex < count) {
            while (endIndex < count && r[endIndex] == r[startIndex] && g[endIndex] == g[startIndex] && b[endIndex] == b[startIndex] && a[endIndex] == a[startIndex]) {
                endIndex++;
            }

            // Establece el color una vez y traza todos los puntos del mismo color juntos
            SDL_SetRenderDrawColor(renderer, r[startIndex], g[startIndex], b[startIndex], a[startIndex]);
            SDL_RenderDrawPoints(renderer, sdlPoints.data() + startIndex, endIndex - startIndex);

            startIndex = endIndex;
            endIndex++;
        }

        // Trazar el último grupo de puntos
        SDL_SetRenderDrawColor(renderer, r[startIndex], g[startIndex], b[startIndex], a[startIndex]);
        SDL_RenderDrawPoints(renderer, sdlPoints.data() + startIndex, count - startIndex);
    }

    void rect(int x, int y, const string& mapHit) {
        vector<pair<int, int>> pointsToDraw;
        vector<Color> pointColors;

        for (int cx = x; cx < x + static_cast<int>(BLOCK / 3); cx++) {
            for (int cy = y; cy < y + static_cast<int>(BLOCK / 3); cy++) {
                int tx = ((cx - x) * textSize) / static_cast<int>(BLOCK / 3);
                int ty = ((cy - y) * textSize) / static_cast<int>(BLOCK / 3);
                Color c = ImageLoader::getPixelColor(mapHit, tx, ty);
                pointsToDraw.push_back({cx, cy});
                pointColors.push_back(c);
            }
        }

        // Dibuja todos los puntos a la vez con sus colores correspondientes
        points(pointsToDraw, pointColors);
    }


    Impact cast_ray(float a) {
        float d = 0;
        string mapHit;
        int tx;
        int x = static_cast<int>(player.x + d * cos(a));
        int y = static_cast<int>(player.y + d * sin(a));

        while (true) {
            int i = static_cast<int>(x / BLOCK);
            int j = static_cast<int>(y / BLOCK);

            if (map[j][i] != ' ' && map[j][i] != '.') {
                mapHit = map[j][i];
                int hitx = x - i * BLOCK;
                int hity = y - j * BLOCK;
                int maxHit;
                if (hitx == 0 || hitx == BLOCK - 1) {
                    maxHit = hity;
                } else {
                    maxHit = hitx;
                }
                tx = maxHit * textSize / BLOCK;
                break;
            }

            d += 2;
            x = static_cast<int>(player.x + d * cos(a));
            y = static_cast<int>(player.y + d * sin(a));
        }
        return Impact{d, mapHit, tx};
    }

    Impact cast_ray_map(float a) {
        float d = 0;
        string mapHit;
        int tx;
        int x = static_cast<int>(player.mapx + d * cos(a));
        int y = static_cast<int>(player.mapy + d * sin(a));

        vector<pair<int, int>> pointsToDraw; // Para almacenar los puntos a dibujar
        vector<Color> pointColors; // Para almacenar los colores de los puntos

        while (true) {
            int i = static_cast<int>(x / (BLOCK / 3));
            int j = static_cast<int>(y / (BLOCK / 3));

            if (map[j][i] != ' ' && map[j][i] != '.') {
                mapHit = map[j][i];
                int hitx = x - i * static_cast<int>(BLOCK / 3);
                int hity = y - j * static_cast<int>(BLOCK / 3);
                int maxHit;
                if (hitx == 0 || hitx == static_cast<int>(BLOCK / 3) - 1) {
                    maxHit = hity;
                } else {
                    maxHit = hitx;
                }
                tx = maxHit * textSize / static_cast<int>(BLOCK / 3);
                break;
            }

            d += 1;

            // Agrega el punto a la lista de puntos a dibujar
            pointsToDraw.push_back({x, y});
            pointColors.push_back(W); // Color de los puntos en el mapa (puedes cambiarlo si es necesario)

            x = static_cast<int>(player.mapx + d * cos(a));
            y = static_cast<int>(player.mapy + d * sin(a));
        }

        // Dibuja todos los puntos en el mapa a la vez con sus colores correspondientes
        points(pointsToDraw, pointColors);

        return Impact{d, mapHit, tx};
    }

    void draw_stake(int x, float h, Impact i) {
        float start = SCREEN_HEIGHT / 2.0f - h / 2.0f;
        float end = start + h;
        for (int y = static_cast<int>(start); y < static_cast<int>(end); y++) {
            int ty = static_cast<int>(((y - start) * textSize) / h);
            Color c = ImageLoader::getPixelColor(i.mapHit, i.ofx, ty);
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    void draw_stake_minimap(int x, float h, Impact i) {
        float start = MAPHEIGHT / 2.0f - h / 2.0f;
        float end = start + h;
        for (int y = static_cast<int>(start); y < static_cast<int>(end); y++) {
            int ty = static_cast<int>(((y - start) * textSize) / h);
            Color c = ImageLoader::getPixelColor(i.mapHit, i.ofx, ty);
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
    void minimap_bg(SDL_Renderer* renderer) {
        int size = 230;
        ImageLoader::render(renderer, "mm", 0, 0, size, 110);
    }

    // Función para verificar si el jugador ha ganado
    bool has_won() {
        int player_x = static_cast<int>(player.x / BLOCK);
        int player_y = static_cast<int>(player.y / BLOCK);

        // Comprueba si el jugador está en una posición de victoria ('y' en el mapa)
        if (map[player_y][player_x] == '.') {
            return true;
        }

        return false;
    }

    // Nueva función para mostrar la pantalla de victoria
    void draw_victory_screen() {
        // Limpia el renderizador
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibuja la pantalla de victoria (imagen o texto)
        ImageLoader::render(renderer, "8c", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // Refresca el renderizador
        SDL_RenderPresent(renderer);
    }



    void render() {


        const int numRays = SCREEN_WIDTH;
        const double deltaAngle = player.fov / numRays;
        const double halfFov = player.fov / 2;
        const double playerCosA = cos(player.a);
        const double playerSinA = sin(player.a);

        // Calcular los vértices del frustum
        double leftFrustum = player.a - halfFov;
        double rightFrustum = player.a + halfFov;

        // Recopila todos los puntos a dibujar junto con sus colores
        vector<pair<int, int>> pointsToDraw;
        vector<Color> pointColors;

        for (int i = 0; i < numRays; i++) {
            double a = player.a + halfFov - deltaAngle * i;
            if (a < leftFrustum || a > rightFrustum) {
                // El rayo está fuera del frustum, no lo procesamos
                continue;
            }

            Impact impact = cast_ray(a, playerCosA, playerSinA);
            float d = impact.d;
            Color c = Color(255, 0, 0);
            if (d == 0) {
                print("you lose");
                exit(1);
            }

            int x = i;
            float h = static_cast<float>(SCREEN_HEIGHT) / (d * cos(a - player.a)) * static_cast<float>(scale);

            float start = SCREEN_HEIGHT / 2.0f - h / 2.0f;
            float end = start + h;

            // Agrega los puntos y sus colores a las listas correspondientes
            #pragma omp parallel for // Inicio de la sección paralela
            for (int y = static_cast<int>(start); y < static_cast<int>(end); y++) {
                int ty = static_cast<int>(((y - start) * textSize) / h);
                Color pointColor = ImageLoader::getPixelColor(impact.mapHit, impact.ofx, ty);
                int px = x;
                int py = y;
                pointsToDraw.push_back({px, py});
                pointColors.push_back(pointColor);
            }
        }

        // Dibuja los puntos en batch con sus colores correspondientes
        points(pointsToDraw, pointColors);

        minimap_bg(renderer);

        // draw left side of the screen
        for (int x = 0; x < static_cast<int>(SCREEN_WIDTH /3); x += static_cast<int>(BLOCK /3)) {
            for (int y = 0; y < static_cast<int>(SCREEN_HEIGHT /3); y += static_cast<int>(BLOCK /3)) {
                int i = static_cast<int>(x / static_cast<int>(BLOCK /3));
                int j = static_cast<int>(y / static_cast<int>(BLOCK /3));
                if (map[j][i] != ' ' && map[j][i] != '.') {
                    string mapHit;
                    mapHit = map[j][i];
                    rect(x, y, mapHit);
                }
            }
        }

        for (int i = 1; i < static_cast<int>(SCREEN_HEIGHT/3); i+=1) {
            float a = player.mapA + player.fov / 2 - player.fov * i / static_cast<int>(SCREEN_HEIGHT/3);
            cast_ray_map(a);
        }

        // Puedes usar la función `has_won` o cualquier otro método que desees.

    }

private:
    int scale;
    SDL_Renderer* renderer;
    vector<string> map;
    int textSize;
    Impact cast_ray(float a, double playerCosA, double playerSinA) {
        float d = 0;
        string mapHit;
        int tx;
        int x = static_cast<int>(player.x);
        int y = static_cast<int>(player.y);
        double cosA = cos(a);
        double sinA = sin(a);

        while (true) {
            x = static_cast<int>(player.x + d * cosA);
            y = static_cast<int>(player.y + d * sinA);

            if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
                // El rayo está fuera de la pantalla, detén la búsqueda.
                break;
            }

            int i = static_cast<int>(x / BLOCK);
            int j = static_cast<int>(y / BLOCK);

            if (map[j][i] != ' ' && map[j][i] != '.') {
                mapHit = map[j][i];
                int hitx = x - i * BLOCK;
                int hity = y - j * BLOCK;
                int maxHit;
                if (hitx == 0 || hitx == BLOCK - 1) {
                    maxHit = hity;
                } else {
                    maxHit = hitx;
                }
                tx = maxHit * textSize / BLOCK;
                break;
            }

            d += 1;
        }

        return Impact{d, mapHit, tx};
    }


};

class Fog {
public:
    Fog(SDL_Renderer* renderer, int opacity, Color color)
            : renderer(renderer), opacity(opacity), color(color) {}

    void draw() {
        // Dibuja un rectángulo de niebla en toda la pantalla
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, opacity);
        SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &rect);
    }



private:
    SDL_Renderer* renderer;
    int opacity;
    Color color;
};
