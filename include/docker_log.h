/**
 * Copyright (c) 2017 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

/**
 * \file docker_log.h
 * \brief Docker API logger for internal use mainly. 
 * It is based on rxi's log implementation with minor changes in function names.
 * See License header.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include "docker_common.h"

#ifdef __cplusplus  
extern "C" {
#endif

/** LOG API Version*/
#define LOG_VERSION "0.1.0"

/** function type for a lock implementation to be used by the logger if locking is enabled*/
typedef void (*log_LockFn)(void *udata, int lock);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

/**
 * @brief Trace log entry
 * 
 * @param ... data to log
 */
#define docker_log_trace(...) docker_log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Debug log entry
 * 
 * @param ... data to log
 */
#define docker_log_debug(...) docker_log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Info log entry
 * 
 * @param ... data to log
 */
#define docker_log_info(...)  docker_log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Warning log entry
 * 
 * @param ... data to log
 */
#define docker_log_warn(...)  docker_log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Error log entry
 * 
 * @param ... data to log
 */
#define docker_log_error(...) docker_log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Fatal log entry
 * 
 * @param ... data to log
 */
#define docker_log_fatal(...) docker_log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Set the user data for the logger
 * 
 * @param udata userdata pointer
 */
MODULE_API void docker_log_set_udata(void *udata);

/**
 * @brief Set the lock function for the logger
 * 
 * @param fn lock function
 */
MODULE_API void docker_log_set_lock(log_LockFn fn);

/**
 * @brief Set the file pointer for the logger
 * 
 * @param fp file pointer
 */
MODULE_API void docker_log_set_fp(FILE *fp);

/**
 * @brief Enable the given log level
 * 
 * @param level level number (see log levels enum)
 */
MODULE_API void docker_log_set_level(int level);

/**
 * @brief Turn off the given log level
 * 
 * @param enable level nubmer (see log levels enum)
 */
MODULE_API void docker_log_set_quiet(int enable);

/**
 * @brief Internal method for logging, called by all specific log level loggers
 * 
 * @param level log level
 * @param file file pointer
 * @param line line number
 * @param fmt format string
 * @param ... arguments to the logger
 */
MODULE_API void docker_log_log(int level, const char *file, int line, const char *fmt, ...);

#ifdef __cplusplus 
}
#endif

#endif
