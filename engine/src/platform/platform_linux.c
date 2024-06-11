// this is for x11 only if you use wayland this wont work.
#include "platform.h"

//make sure its linux platform
#if KPLATFORM_LINUX

#include "core/logger.h"

// libaries for linux
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>    // sudo apt-get install libx11-dev
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>  // sudo apt-get install libxkbcommon-x11-dev libx11-xcb-dev
#include <sys/time.h>


// this macro make sure you get the right library for sleep depending on distro
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else
#include <unistd.h> //usleep
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct internal_state{
    Display *display;
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
} internal_state;

b8 platform_startup(
    platform_state* plat_state,
    const char* application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height){

    //create the internal state
    plat_state->internal_state = malloc(sizeof(internal_state));
    //type cast
    internal_state *state = (internal_state *)plat_state->internal_state;

    // start connection to x server
    state->display = XOpenDisplay(NULL);

    // turn off key repeats
    // x11 is done is such a way this function call is global across the entire x
    XAutoRepeatOff(state->display);

    // get connection from the display
    state->connection = XGetXCBConnection(state->display);

    if (xcb_connection_has_error(state->connection)) {
        KFATAL("Failed to connect to x server from XCB.");
        return FALSE;
    }

    // get data from x server
    const struct xcb_setup_t *setup = xcb_get_setup(state->connection);

    // iterate thru screens
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    int screen_p = 0;
    for (i32 s = screen_p; s > 0; s--){
        xcb_screen_next(&it);
    }

    // after screens have been looped thru assign it
    state->screen = it.data;

    // generate XID for new window
    state->window = xcb_generate_id(state->connection);

    // Register event types.
    // XCB_CW_BACK_PIXEL = filling then window bg with a single colour
    // XCB_CW_EVENT_MASK is required.
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // Listen for keyboard and mouse buttons
    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    // values sent over xcb (bg color, events)
    u32 value_list[] = {state->screen->black_pixel, event_values};

    // create window cookie
    xcb_void_cookie_t cookie = xcb_create_window(
        state->connection,
        XCB_COPY_FROM_PARENT,           //depth
        state->window,
        state->screen->root,            // parent
        x,
        y,
        width,
        height,
        0,                              // no border
        XCB_WINDOW_CLASS_INPUT_OUTPUT,  //class
        state->screen->root_visual,
        event_mask,
        value_list);

    // change title of window
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,                      //set to view 8 bits at a time
        strlen(application_name),
        application_name);
        
    // tell x11 when the window manager tries to terminate the window
    xcb_intern_atom_cookie_t wm_delete_cookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW");

    xcb_intern_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS");

    xcb_intern_atom_reply_t *wm_delete_reply = xcb_intern_atom_reply(
        state->connection,
        wm_delete_cookie,
        NULL);

    xcb_intern_atom_reply_t *wm_protocols_reply = xcb_intern_atom_reply(
        state->connection,
        wm_protocols_cookie,
        NULL);

    state->wm_delete_win = wm_delete_reply->atom;
    state->wm_protocols = wm_protocols_reply->atom;       

    //place reply and delete on window
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        wm_protocols_reply->atom,
        4,
        32,
        1,
        &wm_delete_reply->atom);
    
    // map window to screen 
    xcb_map_window(state->connection, state->window);

    // flush the stream
    i32 stream_result = xcb_flush(state->connection);
    if (stream_result <= 0) {
        KFATAL("An error happened when flushing the stream: %d", stream_result);
        return FALSE;
    }

    return TRUE;
}

void platform_shutdown(platform_state* plat_state){
    // cold cast to the known type
    internal_state *state = (internal_state *)plat_state->internal_state;

    // turn key repeats back on since this is global for the os 
    XAutoRepeatOn(state->display);

    xcb_destroy_window(state->connection, state->window);
}

b8 platform_pump_messages(platform_state* plat_state){
    // cold cast to the known type
    internal_state *state = (internal_state *)plat_state->internal_state; 

    xcb_generic_event_t *event;
    xcb_client_message_event_t *cm;

    b8 quit_flagged = FALSE;

    //poll for events until null is returned
    while (event != 0){
        event = xcb_poll_for_event(state->connection);
        if (event == 0){
            break;
        }

        // input events
        switch (event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                // key presses and releases
            }break; 
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                // mouse presses and releases
            }
            case XCB_MOTION_NOTIFY:
                //MOUSE MOVEMENT
            break; 
            case XCB_CONFIGURE_NOTIFY:{
                //window resize 
            }  

            case XCB_CLIENT_MESSAGE:{
                //handles window closure requests
                cm = (xcb_client_message_event_t *)event;

                //window close
                if (cm->data.data32[0] == state->wm_delete_win){
                    quit_flagged = TRUE;
                }
            } break;
            default:
                //best practice is to have default in switch case
                break;     
        }
        // got something to do with xcb dynamic reallocating pointer on each interation
        free(event);
    }
    return !quit_flagged;   
}

void* platform_allocate(u64 size, b8 aligned){
    return malloc(size);
}

void platform_free(void* block, b8 aligned){
    free(block);
}

void* platform_zero_memory(void* block, u64 size){
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dest, const void* source, u64 size){
    return memcpy(dest, source, size);
}
void* platform_set_memory(void* dest, i32 value, u64 size){
    return memset(dest, value, size);
}

void platform_console_write(const char* message, u8 color){
    //fatal error, warn, info, debug, trace
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", color_strings[color], message);   
}

void platform_console_write_error(const char* message, u8 color){
    //fatal error, warn, info, debug, trace
    const char* color_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", color_strings[color], message); 
}

f64 platform_get_absolute_time(){
    //always deal in second
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

// sleep on thread for however ms. This blocks main thread
// should onlyt be used for giving time back to te os for unused update power
// so this isnt exporte
void platform_sleep(u64 ms){
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000* 1000;
    nanosleep(&ts, 0);
#else
    if (ms >= 1000){
        sleep(ms / 1000);
    }
    usleep((ms % 1000) * 1000);
#endif
}

#endif