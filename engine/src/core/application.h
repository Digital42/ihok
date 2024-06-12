#pragma once 

#include "defines.h"
//#include "game_types.h"

struct game;

// application config
typedef struct application_config {
    // window starting postion on x axis 
    i16 start_pos_x;

    // window starting postion on y axis 
    i16 start_pos_y;

    // window width 
    i16 start_pos_width;

     // window height 
    i16 start_pos_height;

    //applicaiton name used in window
    char* name; 
} application_config;
 

KAPI b8 application_create(struct game* game_inst);

KAPI b8 application_run();