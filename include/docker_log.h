/**
 * Copyright (c) 2017 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include "docker_common.h"

#ifdef __cplusplus  
extern "C" {
#endif

#define LOG_VERSION "0.1.0"

typedef void (*log_LockFn)(void *udata, int lock);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define docker_log_trace(...) docker_log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define docker_log_debug(...) docker_log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define docker_log_info(...)  docker_log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define docker_log_warn(...)  docker_log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define docker_log_error(...) docker_log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define docker_log_fatal(...) docker_log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

MODULE_API void docker_log_set_udata(void *udata);
MODULE_API void docker_log_set_lock(log_LockFn fn);
MODULE_API void docker_log_set_fp(FILE *fp);
MODULE_API void docker_log_set_level(int level);
MODULE_API void docker_log_set_quiet(int enable);

MODULE_API void docker_log_log(int level, const char *file, int line, const char *fmt, ...);

#ifdef __cplusplus 
}
#endif

#endif
