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

const int WIDTH = 13;
const int HEIGHT = 11;
const int BLOCK = 40;
const int SCREEN_WIDTH = BLOCK * WIDTH;
const int SCREEN_HEIGHT = BLOCK * HEIGHT;

struct Player {
    int x;
    int y;
    float a;
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
        player.a = M_PI / 4.0f;
        player.fov = M_PI /2.0f;
        scale = 30;
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

    void print_map() {
        for (const string& line : map) {
            cout << line << endl;
        }
    }

    void point(int x, int y, Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderDrawPoint(renderer, x, y);
    }

    void rect(int x, int y, const string& mapHit) {
        for(int cx = x; cx < x + BLOCK; cx++){
            for(int cy = y; cy < y + BLOCK; cy++){
                int tx = ((cx - x) * textSize) / BLOCK;
                int ty = ((cy - y) * textSize) / BLOCK;
                Color c = ImageLoader::getPixelColor(mapHit, tx, ty);
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
                //SDL_Rect rect = { x, y, BLOCK, BLOCK };
                SDL_RenderDrawPoint(renderer, cx, cy);
            }
        }
    }

    Impact cast_ray(float a) {
        float d = 0;
        string mapHit;
        while(true) {
            int x = static_cast<int>(player.x + d * cos(a));
            int y = static_cast<int>(player.y + d * sin(a));

            int i = static_cast<int>(x / BLOCK);
            int j = static_cast<int>(y / BLOCK);

            if (map[j][i] != ' ') {
                mapHit = map[j][i];
                break;
            }

            point(x, y, W);

            d += 5;
        }
        return Impact{d, "+", 0};
    }

    void draw_stake(int x, float h, Color c) {
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        float start = SCREEN_HEIGHT/2.0f - h/2.0f;
        SDL_Rect rect = { x, static_cast<int>(start), 1, static_cast<int>(h) };
        SDL_RenderFillRect(renderer, &rect);
    }

    void render() {
        // draw left side of the screen
        for (int x = 0; x < SCREEN_WIDTH; x += BLOCK) {
            for (int y = 0; y < SCREEN_HEIGHT; y += BLOCK) {
                int i = static_cast<int>(x / BLOCK);
                int j = static_cast<int>(y / BLOCK);
                if (map[j][i] != ' ') {
                    string mapHit;
                    mapHit = map[j][i];
                    rect(x, y, mapHit);
                }
            }
            // Avanzar a la siguiente columna del mapa
        }

        for (int i = 1; i < SCREEN_WIDTH ; i++) {
            float a = player.a + player.fov / 2 - player.fov * i / SCREEN_WIDTH;
            cast_ray(a);
        }
        // draw right side of the screen
        for (int i = 1; i < SCREEN_WIDTH; i++) {
            double a = player.a + player.fov / 2.0 - player.fov * i / SCREEN_WIDTH;
            Impact impact = cast_ray(a);
            float d = impact.d;
            Color c = Color(255,0,0);
            if (d == 0) {
                print("you lose");
                exit(1);
            }
            int x = SCREEN_WIDTH + i;
            float h = static_cast<float>(SCREEN_HEIGHT)/static_cast<float>(d) * static_cast<float>(scale);
            draw_stake(x, h, c);
        }
    }

private:
    int scale;
    SDL_Renderer* renderer;
    vector<string> map;
    int textSize;
};