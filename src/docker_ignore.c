/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_ignore.h"
#include <string.h>

MODULE_API d_err_t readlines_dockerignore(const char* contents, arraylist* lines) {
	if (contents != NULL) {
		if (strlen(contents) > 0) {
			if (lines != NULL) {
				// split lines in content and append to lines arraylist
				size_t current_line_start = 0;
				for (size_t current_line_end = 0; current_line_end < strlen(contents); current_line_end++) {
					//if line ends, make a copy and move on
					if(contents[current_line_end] == '\n' || current_line_end == (strlen(contents) - 1)) {
						char* line = (char*)calloc(current_line_end - current_line_start + 2, sizeof(char));
						if(line) {
							strncpy(line, contents + current_line_start, current_line_end - current_line_start + 1);
							line[current_line_end - current_line_start + 1] = '\0';
							arraylist_add(lines, line);
							current_line_start = current_line_end;
						} else {
							return DOCKER_IGNORE_ALLOC_ERROR;
						}
					}
				}
				return 0;
			}
			else {
				return DOCKER_IGNORE_LIST_NULL;
			}
		}
		else {
			return DOCKER_IGNORE_EMPTY;
		}
	}
	return DOCKER_IGNORE_NULL;
}