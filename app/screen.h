#pragma once

#define MAX_NUM_GAME_OBJECTS 100        // The maximum number of game objects appearing on a screen

struct __tile_info_t {
    const lv_image_dsc_t* sprite;
    uint32_t w;
    uint32_t h;
} typedef tile_info_t;

struct __game_object_info_t {
    uint32_t x;
    uint32_t y;
    tile_info_t sprite;
} typedef game_object_info_t;

struct __screen_t {
    game_object_info_t* objects;
    uint32_t num_objects;
} typedef screen_t;

extern const screen_t SCREEN1;

void init_screen(screen_t* screen);
