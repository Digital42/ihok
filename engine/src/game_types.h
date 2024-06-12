#pragma once

#include "core/application.h"

typedef struct game {
    // app config
    application_config app_config;

    //function pointer to games init function
    b8 (*initialize)(struct game* game_inst);

    //function pointer to games update function
    b8 (*update)(struct game* game_inst, f32 delta_time);

    //function pointer to games render function
    b8 (*render)(struct game* game_inst, f32 delta_tim);

    //function pointer to manage resizes
    void (*on_resize)(struct game* game_inst, u32 width, u32 height);   

    // game specific game state created & managed by the game
    void* state;
} game;
