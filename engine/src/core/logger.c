#include "logger.h"
#include "asserts.h"
#include "platform/platform.h"

//temp includes
#include<stdio.h>
#include<string.h>
#include<stdarg.h>


b8 initialize_logging() {
    //under construction - create log file
    return TRUE;
}
void shutdown_logging(){
    //under construction cleanup logging/write queued enteries
}

//function to pipe all logging to output 
 void log_output(log_level level, const char* message, ...){
    //create array for level strings
    const char* level_strings[6]= {"[FATAL]:", "[ERROR]:", "[WARN]:", "[INFO]:", "[DEBUG]:", "[TRACE]:"};
    b8 is_error = level < LOG_LEVEL_WARN;

    //this nonsene imposes a 32k char limit on a single long entry
    //this is to void dynmic memory allocation for now so this needs to be changed in the future 
    const i32 msg_length = 32000;
    char out_message[msg_length];
    memset(out_message, 0, sizeof(out_message));

    //format originl msg
    // ms headers override the gcc/clang va_list type with a "typedef char* va_list" in some
    // cases. as a result throws a strnage error here. work around  to use __builtin_va_list,
    //which i the type gcc/clang va_start expect
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, msg_length, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[msg_length];
    // append error message level onto out_message
    sprintf(out_message2, "%s%s\n", level_strings[level], out_message);
    
    // platform specific outpout
    if (is_error) {
        platform_console_write_error(out_message2, level);
    } else {
        platform_console_write(out_message2, level); 
    }
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line){
    log_output(LOG_LEVEL_FATAL, "Assertion failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}
