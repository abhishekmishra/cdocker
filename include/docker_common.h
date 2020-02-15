/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * \file docker_common.h
 * \brief Some common declarations
 */

#ifndef SRC_DOCKER_COMMON_H_
#define SRC_DOCKER_COMMON_H_

#define MODULE_API_EXPORTS
#ifdef _WIN32
#  ifdef MODULE_API_EXPORTS
#    define MODULE_API __declspec(dllexport)
#  else
#    define MODULE_API __declspec(dllimport)
#  endif
#else
#  define MODULE_API
#endif

#endif /* SRC_DOCKER_COMMON_H_ */
