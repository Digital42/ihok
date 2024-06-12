#include "game.h"
#include <entry.h>

//remove this later
//#include <platform/platform.h>
#include<core/kmemory.h>

// func definition to create game
b8 create_game(game* out_game){

    // application config
    application_config config;
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_pos_width = 1280;
    out_game->app_config.start_pos_height = 720;
    out_game->app_config.name = "Engine testbed"; 
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->initialize = game_initialize;
    out_game->on_resize = game_on_resize;

    // create the game state
    out_game->state = kallocate(sizeof(game_state), MEMORY_TAG_GAME);

    return TRUE;
}