/*
 *
 * Copyright (c) 2018-2022 Abhishek Mishra
 *
 * This file is part of clibdocker.
 *
 * clibdocker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, 
 * either version 3 of the License, or (at your option) 
 * any later version.
 *
 * clibdocker is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public 
 * License along with clibdocker. 
 * If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_DOCKER_IGNORE_H_
#define SRC_DOCKER_IGNORE_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include "docker_util.h"
#include <coll_arraylist.h>

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

//TODO: implement docker ignore check
/**
 * @brief Not implemeted
 * 
 * @param ignore 
 * @param path 
 * @return MODULE_API 
 */
MODULE_API int dockerignore_check(arraylist* ignore, const char* path);

/**
 * @brief List given folder path, while applying the rules in the docker ignore list
 * 
 * @param folder_path path to the folder/dir to list
 * @param dockerignore_path path to the docker ignore file
 * @return arraylist* list of paths in the folder, which are not ignored.
 */
MODULE_API arraylist* list_dir_w_ignore(const char* folder_path, const char* dockerignore_path);

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_IGNORE_H_ */
