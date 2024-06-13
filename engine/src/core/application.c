#include "application.h"
#include "game_types.h"

#include "logger.h"

#include "platform/platform.h"
#include "core/kmemory.h"
#include "core/event.h"
#include "core/input.h"

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

// keyboard event handlers
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

b8 application_create(game* game_inst) {
    if (initialized) {
        KERROR("Application_create() called more than once");
        return FALSE;
    }

    app_state.game_inst = game_inst;
    //initialize subsystems
    initialize_logging();
    input_initialize();

    //remove in the future
    /*
    KFATAL("A test msg: %f", 6.66f);
    KERROR("A test msg: %f", 6.66f);
    KWARN("A test msg: %f", 6.66f);
    KINFO("A test msg: %f", 6.66f);
    KDEBUG("A test msg: %f", 6.66f);
    KTRACE("A test msg: %f", 6.66f);
    */

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if(!event_initialize()) {
        KERROR("Event system failed initialization. App cant continue");
        return FALSE;
    }

    // start to listen for events
    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

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
    //this is for testing memory allocation tracking delete after 6/13/24
    KINFO(get_memory_usage_str());

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

            // input/state copying should be handled after any input should be recorded
            //input should be the last this to be updated before the end of frame
            input_update(0);
        }
    }

    //sanity check to make sure false is returned when app sate is shut down
    app_state.is_running = FALSE;

    // shutdown various systems
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    event_shutdown();
    input_shutdown();

    platform_shutdown(&app_state.platform);

    return TRUE;
}
// press esc to quit x11 window
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code){
        case EVENT_CODE_APPLICATION_QUIT: {
            KINFO("Event code quit recieved. shutiting down.\n");
            app_state.is_running = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

/*
TODO:: rewrite all of this stuff with more consiten logic
this is mostly test code any way no reason to track if a r b key 
is pressed in a "release" build
*/

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            //technically firing an event to itself but there maybe others listening
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            //block anything else from processing this
            return TRUE;
        } else if (key_code == KEY_A) {
            // example on checking for a key
            KDEBUG("Explicit - A key pressed 1");
        }else {
            KDEBUG("'%c' key pressed in window 2", key_code);
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        // example on checking for a key
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_B) {
            KDEBUG("Explicit - B key released 3");
        }else {
            KDEBUG("'%c' key released in window 4", key_code);
        }
    }
    return FALSE;
}