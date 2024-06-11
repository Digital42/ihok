#include "logger.h"
#include "asserts.h"

//temp includes
#include<stdio.h>
#include<string.h>
#include<stdarg.h>

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line){
    log_output(LOG_LEVEL_FATAL, "Assertion failure: %s, message: '%s', in file: %s, line: %d\n", expression, message, file, line);
}

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
    //b8 is_error = level < 2;

    //this nonsene imposes a 32k char limit on a single long entry
    //this is to void dynmic memory allocation for now so this needs to be changed in the future 
    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    //format originl msg
    // ms headers override the gcc/clang va_list type with a "typedef char* va_list" in some
    // cases. as a result throws a strnage error here. work around  to use __builtin_va_list,
    //which i the type gcc/clang va_start expect
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char out_message2[32000];
    // append error message level onto out_message
    sprintf(out_message2, "%s%s\n", level_strings[level], out_message);
    
    // we need to build platform specific output eventually
    printf("%s", out_message2);
}