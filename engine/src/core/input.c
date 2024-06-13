#include "core/input.h"
#include "core/event.h"
#include "core/kmemory.h"
#include "core/logger.h"

typedef struct keyboard_state {
    // 8 bit boolen type array to keep track of individual key states
    b8 keys[256];
} keyboard_state;

typedef struct mouse_state {
    // 16 bit x and y positions
    i16 x;
    i16 y;
    // unsigned 8 bit array to keep track of indivual mouse button states
    u8 buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

typedef struct input_state {
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
} input_state;

// internal input state ?possibly for engine side only
static b8 initialized = FALSE;
static input_state state = {};

void input_initialize() {
    kzero_memory(&state, sizeof(input_state));
    initialized = TRUE;
    KINFO("Input system initialized");
}

void input_shutdown() {
    // update this at a later time 6-12-24
    initialized = FALSE;
}

void input_update(f64 delta_time) {
    if (!initialized) {
        return;
    }

    // copy current states to previous states
    kcopy_memory(&state.keyboard_previous, &state.keyboard_current, sizeof(keyboard_state));
    kcopy_memory(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));   
}

void input_process_key(keys key, b8 pressed) {
    // only handle if the state has changed
    if (state.keyboard_current.keys[key] != pressed) {
        //update internal state
        state.keyboard_current.keys[key] = pressed;

        // fire off an event for immediate processing
        event_context context;
        context.data.u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_PRESSED, 0, context);
    }
}

void input_process_button(buttons button, b8 pressed) {
    // only handle if the state has changed
    if (state.mouse_current.buttons[button] != pressed) {
        //update internal state
        state.mouse_current.buttons[button] = pressed;

        // fire off an event for immediate processing
        event_context context;
        context.data.u16[0] = button;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_PRESSED, 0, context);
    }
}

void input_process_mouse_move(i16 x, i16 y) {
    //only process if state has updated
    if (state.mouse_current.x != x || state.mouse_current.y != y) {
        //debuggin option
        //KDEBUG("Mouse position: %i, %i", x, y);

        // uodate internal state
        state.mouse_current.x = x;
        state.mouse_current.y = y;

        // fire the event
        event_context context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;      
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, context);         
    }

}

void input_process_mouse_wheel(i8 z_delta) {
    // fire event
    event_context context;
    context.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

b8 input_is_key_down(keys key) {
    if (!initialized) {
        return FALSE;
    }
    return state.keyboard_current.keys[key] == TRUE;
}

b8 input_is_key_up(keys key) {
    if (!initialized) {
        return TRUE;
    }
    return state.keyboard_current.keys[key] == TRUE;
}

b8 input_was_key_down(keys key) {
    if (!initialized) {
        return FALSE;
    }
    return state.keyboard_previous.keys[key] == TRUE;
}

b8 input_was_key_up(keys key) {
    if (!initialized) {
        return TRUE;
    }
    return state.keyboard_previous.keys[key] == FALSE;
}


//mouse inputs buttons
b8 input_is_button_down(buttons button) {
    if (!initialized) {
        return FALSE;
    }
    return state.mouse_current.buttons[button] == TRUE;
}

b8 input_is_button_up(buttons button) {
    if (!initialized) {
        return TRUE;
    }
    return state.mouse_current.buttons[button] == FALSE;
}

b8 input_was_button_down(buttons button) {
    if (!initialized) {
        return FALSE;
    }
    return state.mouse_previous.buttons[button] == TRUE;
}

b8 input_was_button_up(buttons button) {
    if (!initialized) {
        return TRUE;
    }
    return state.mouse_previous.buttons[button] == FALSE;
}

void input_get_mouse_position(i32* x, i32* y ) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_current.x;
    *y = state.mouse_current.y;

}

void input_get_previous_mouse_position(i32* x, i32* y ) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_previous.x;
    *y = state.mouse_previous.y;

}
