#include "coins.h"

bool coin_callback(game_object_t* context) {
    deactivate_game_object(context);

    // Cancel collision
    return false;   
}
