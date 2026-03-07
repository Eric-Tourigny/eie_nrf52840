#include "screen.h"
#include "game_object.h"

extern const lv_image_dsc_t SpriteRockTile;

static tile_info_t rock_tile = {
    &SpriteRockTile, 
    15,
    15
};

static game_object_t* game_objects[MAX_NUM_GAME_OBJECTS + 1];

void init_screen(screen_t* screen) {
    int i;
    for (i = 0; i < screen->num_objects; i++) {
        game_object_info_t* object_info = &screen->objects[i];
        game_objects[i] = create_game_object(object_info->x, object_info->y, object_info->sprite.w, object_info->sprite.h, object_info->sprite.sprite);
    }
    game_objects[i] = NULL;
}

const game_object_info_t SCREEN1[] = {
    {}
};