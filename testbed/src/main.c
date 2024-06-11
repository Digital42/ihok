#include <core/logger.h>
#include <core/asserts.h>

// fix later
#include <platform/platform.h>

int main(void){
    KFATAL("A test msg: %f", 6.66f);
    KERROR("A test msg: %f", 6.66f);
    KWARN("A test msg: %f", 6.66f);
    KINFO("A test msg: %f", 6.66f);
    KDEBUG("A test msg: %f", 6.66f);
    KTRACE("A test msg: %f", 6.66f);

    platform_state state;
    if (platform_startup(&state, "engine test", 100, 100, 1280, 720)){
        while(TRUE){
            platform_pump_messages(&state);
        }
    }
    platform_shutdown(&state);
    return 0;
}