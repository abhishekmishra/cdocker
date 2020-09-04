/**
 * Copyright (c) 2020 Abhishek Mishra
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "docker_log.h"
#include "docker_ignore.h"
#include <string.h>
#include "tinydir.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#else
#endif

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

int dockerignore_check(arraylist* ignore, const char* path) {
	return 0;
}

arraylist* list_dir_w_ignore(const char* folder_path, const char* dockerignore_path)
{
	arraylist* ignore_lines;
	arraylist_new(&ignore_lines, &free);

	arraylist* paths;

	FILE* ignorefp;
	ignorefp = fopen(dockerignore_path, "r");
	fseek(ignorefp, 0, SEEK_END); 
	size_t size = ftell(ignorefp);
	fseek(ignorefp, 0, SEEK_SET);
	char* ignore_contents = (char *)calloc(size, sizeof(char));
	fread(ignore_contents, size/sizeof(char), sizeof(char), ignorefp);
	readlines_dockerignore(ignore_contents, ignore_lines);

	arraylist_new(&paths, &free);
	docker_log_debug("To list %s\n", folder_path);

	tinydir_dir dir;
	tinydir_open(&dir, folder_path);

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0)
		{
			docker_log_debug("%s", file.name);
			// dockerignore_check(file)
			char* sub_dir_path = (char*) calloc(
					strlen(folder_path) + strlen(file.name) + 2, sizeof(char));
			strcpy(sub_dir_path, folder_path);
			strcat(sub_dir_path, "/");
			strcat(sub_dir_path, file.name);
			if (file.is_dir)
			{
//				printf("/\n");
				arraylist* sub_dir_ls = list_dir_w_ignore(sub_dir_path, dockerignore_path);
				size_t ls_count = arraylist_length(sub_dir_ls);
				for (size_t i = 0; i < ls_count; i++)
				{
					arraylist_add(paths,
							str_clone(arraylist_get(sub_dir_ls, i)));
				}
				free(sub_dir_ls);
			} else {
//				printf("\n");
			}
			arraylist_add(paths, sub_dir_path);
		}
		tinydir_next(&dir);
	}

	tinydir_close(&dir);

	return paths;
}
