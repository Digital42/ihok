#include "application.h"
#include "game_types.h"

#include "logger.h"
#include "platform/platform.h"

typedef struct application_state {
    game* game_inst;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;
} application_state;


//static means this app state is local 
static b8 initialized = FALSE;
static application_state app_state;

b8 application_create(game* game_inst) {
    if (initialized) {
        KERROR("Application_create() called more than once");
        return FALSE;
    }

    app_state.game_inst = game_inst;
    //initialize subsystems
    initialize_logging();

    //remove in the future
    KFATAL("A test msg: %f", 6.66f);
    KERROR("A test msg: %f", 6.66f);
    KWARN("A test msg: %f", 6.66f);
    KINFO("A test msg: %f", 6.66f);
    KDEBUG("A test msg: %f", 6.66f);
    KTRACE("A test msg: %f", 6.66f);

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if (!platform_startup(
            &app_state.platform, 
            game_inst->app_config.name, 
            game_inst->app_config.start_pos_x, 
            game_inst->app_config.start_pos_y, 
            game_inst->app_config.start_pos_width, 
            game_inst->app_config.start_pos_height)){
        return FALSE;

    }

    //init the game
    if (!app_state.game_inst->initialize(app_state.game_inst)) {
        KFATAL("Game failed to init");
        return FALSE;
    }

    // on resize event 
    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);

    initialized = TRUE;

    return TRUE;
}

b8 application_run() {
    while(app_state.is_running){
        if (!platform_pump_messages(&app_state.platform)){
            app_state.is_running = FALSE;
        }
        // check if game is suspended and shut down 
        if (!app_state.is_suspended) {
            if (!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
                KFATAL("Game update failed, shutting down");
                app_state.is_running = FALSE;
                break;
            }
        }
        // call renderer and shutdown if something happened
        if (!app_state.is_suspended) {
            if (!app_state.game_inst->render(app_state.game_inst, (f32)0)) {
                KFATAL("Game render failed, shutting down");
                app_state.is_running = FALSE;
                break;
            }
        }
    }

    //sanity check to make sure false is returned when app sate is shut down
    app_state.is_running = FALSE;

    platform_shutdown(&app_state.platform);

    return TRUE;
}