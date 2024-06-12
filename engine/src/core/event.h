#pragma once 

#include "defines.h"

typedef struct event_context {
    //128 bytes for union
    union {
        i64 i64[2];
        u64 u64[2];     
        f64 f64[2];

        i32 i32[4];     
        u32 u32[4];
        f32 f32[4]; 

        i16 i16[8];
        u16 u16[8];  

        i8 i8[16];
        u8 u8[26];

        char c[16];
    } data;
}event_context;

// shoudl return true if handled
typedef b8 (*PFN_on_event)(u16 code, void* sender, void* listener_inst, event_context data);

b8 event_initialize();
void event_shutdown();

/*
    code = event code to fire
    listener = pointer to the listener isntance  can be 0/null
    on event = callback function pointer to be invoked when the event code is called
*/
KAPI b8 event_register(u16 code, void* listener, PFN_on_event on_event);

/*
    code = event code to fire
    listener = pointer to the listener isntance  can be 0/null
    on event = callback function pointer to be invoked when the event code is called
*/
KAPI b8 event_unregister(u16 code, void* listener, PFN_on_event on_event);

/*
    code = event code to fire
    sender = pointer to the sender can be 0/null
    data = event data 
*/
KAPI b8 event_fire(u16 code, void* listener, event_context context);

typedef enum system_event_code {
    /** @brief Shuts the application down on the next frame. */
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    /** @brief Keyboard key pressed.
     * Context usage:
     * u16 key_code = data.data.u16[0];
     * u16 repeat_count = data.data.u16[1];
     */
    EVENT_CODE_KEY_PRESSED = 0x02,

    /** @brief Keyboard key released.
     * Context usage:
     * u16 key_code = data.data.u16[0];
     * u16 repeat_count = data.data.u16[1];
     */
    EVENT_CODE_KEY_RELEASED = 0x03,

    /** @brief Mouse button pressed.
     * Context usage:
     * u16 button = data.data.u16[0];
     * u16 x = data.data.i16[1];
     * u16 y = data.data.i16[2];
     */
    EVENT_CODE_BUTTON_PRESSED = 0x04,

    /** @brief Mouse button released.
     * Context usage:
     * u16 button = data.data.u16[0];
     * u16 x = data.data.i16[1];
     * u16 y = data.data.i16[2];
     */
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    /** @brief Mouse button pressed then released.
     * Context usage:
     * u16 button = data.data.u16[0];
     * u16 x = data.data.i16[1];
     * u16 y = data.data.i16[2];
     */

    EVENT_CODE_MOUSE_MOVED = 0x06,

    /** @brief Mouse moved.
     * Context usage:
     * ui z_delta = data.data.i8[0];
     */
    EVENT_CODE_MOUSE_WHEEL = 0x07,

     /** resized resolution chaged from os.
     * Context usage:
     * u16 width = data.data.u16[0];
     * u16 height = data.data.u16[0];
     */   
    EVENT_CODE_RESIZE = 0x08,

    MAX_EVENT_CODE = 0xFF
} system_event_code;
