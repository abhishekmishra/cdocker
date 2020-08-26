/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#ifndef SRC_DOCKER_IGNORE_H_
#define SRC_DOCKER_IGNORE_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include "docker_util.h"
#include <arraylist.h>

#define DOCKER_IGNORE_NULL	501
#define DOCKER_IGNORE_EMPTY	502
#define DOCKER_IGNORE_LIST_NULL 503
#define DOCKER_IGNORE_ALLOC_ERROR 504

/**
* Parse the contents of the dockerignore file into lines
* 
* \param contents string contents of the file
* \param lines pointer to arraylist which will store the lines
* \return error code
*/
MODULE_API d_err_t readlines_dockerignore(const char* contents, arraylist* lines);


#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_IGNORE_H_ */
