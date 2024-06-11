#pragma once

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// if release build switch of debug and trace for the logger
#if KRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

b8 initialize_logging();
void shutdown_logging();

//function to pipe all logging to output 
KAPI void log_output(log_level level, const char* message, ...);

//logs a fatal level error msg
#define KFATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#ifndef KERROR
//logs a error level msg
#define KERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

#if LOG_WARN_ENABLED == 1
//log a warning level msg
#define KWARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
// do nothing is LOG_WARN_ENABLED != 1
#define KWARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
//log a info level msg
#define KINFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
// do nothing is LOG_INFO_ENABLED != 1
#define KINFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
//log a debug level msg
#define KDEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
// do nothing is LOG_DEBUG_ENABLED != 1
#define KDEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
//log a trace level msg
#define KTRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
// do nothing is LOG_TRACE_ENABLED != 1
#define KTRACE(message, ...)
#endif