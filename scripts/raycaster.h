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

    void rect(int x, int y, const string& mapHit) {
        for(int cx = x; cx < x + static_cast<int>(BLOCK/3); cx++){
            for(int cy = y; cy < y + static_cast<int>(BLOCK/3); cy++){
                int tx = ((cx - x) * textSize) / static_cast<int>(BLOCK /3) ;
                int ty = ((cy - y) * textSize) / static_cast<int>(BLOCK /3);
                Color c = ImageLoader::getPixelColor(mapHit, tx, ty);
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
                SDL_RenderDrawPoint(renderer, cx, cy);
            }
        }
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

            if (map[j][i] != ' ') {
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

        while (true) {
            int i = static_cast<int>(x / (BLOCK/3));
            int j = static_cast<int>(y / (BLOCK/3));

            if (map[j][i] != ' ') {
                mapHit = map[j][i];
                int hitx = x - i * static_cast<int>(BLOCK/3);
                int hity = y - j * static_cast<int>(BLOCK/3);
                int maxHit;
                if (hitx == 0 || hitx == static_cast<int>(BLOCK/3) - 1) {
                    maxHit = hity;
                } else {
                    maxHit = hitx;
                }
                tx = maxHit * textSize / static_cast<int>(BLOCK/3);
                break;
            }

            d += 1;
            point(x, y, W);

            x = static_cast<int>(player.mapx + d * cos(a));
            y = static_cast<int>(player.mapy + d * sin(a));
        }
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

    void render() {
        const int numRays = SCREEN_WIDTH; // Número de rayos
        const double deltaAngle = player.fov / numRays;
        // draw right side of the screen
        for (int i = 0; i < numRays; i++) {
            double a = player.a + player.fov / 2 - deltaAngle * i;
            Impact impact = cast_ray(a);
            float d = impact.d;
            Color c = Color(255,0,0);
            if (d == 0) {
                print("you lose");
                exit(1);
            }
            int x = i;
            float h = static_cast<float>(SCREEN_HEIGHT) / (d * cos(a - player.a)) * static_cast<float>(scale);
            draw_stake(x, h, impact);
        }

        minimap_bg(renderer);

        // draw left side of the screen
        for (int x = 0; x < static_cast<int>(SCREEN_WIDTH /3); x += static_cast<int>(BLOCK /3)) {
            for (int y = 0; y < static_cast<int>(SCREEN_HEIGHT /3); y += static_cast<int>(BLOCK /3)) {
                int i = static_cast<int>(x / static_cast<int>(BLOCK /3));
                int j = static_cast<int>(y / static_cast<int>(BLOCK /3));
                if (map[j][i] != ' ') {
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
    }

private:
    int scale;
    SDL_Renderer* renderer;
    vector<string> map;
    int textSize;
};
