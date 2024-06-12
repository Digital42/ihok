#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

//externally defined function to create game
//defined in main.c
extern b8 create_game(game* out_game);

// NEW ENTRY FOR POINT FOR MAIN //
/////////////////////////////////
int main(void){

    //request game instance from application
    game game_inst;
    if (!create_game(&game_inst)) {
        KFATAL("Could not create game.");
        return -1;
    }

    // sanity check to make sure function pointers exist
    if (!game_inst.render || !game_inst.update || !game_inst.initialize || !game_inst.on_resize ) {
        KFATAL("Function pointers are not assigned");
        return -2;
    }
    //sanity check for app creation
    if (!application_create(&game_inst)) {
       KINFO("Application failed to create\n")
       return 1;
    }
    //sanity check for application shutdown
    if (!application_run()){
        KINFO("Application failed to shutdown gracefully\n");
        return 2;   
    }
    
    return 0;
}