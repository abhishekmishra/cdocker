/*
 * cld_progress.h
 *
 *  Created on: 19-Mar-2019
 *      Author: abhis
 */

#ifndef SRC_CLD_PROGRESS_H_
#define SRC_CLD_PROGRESS_H_

#define CLD_PROGRESS_DEFAULT_BAR "#"
#define CLD_PROGRESS_DEFAULT_BEFORE "["
#define CLD_PROGRESS_DEFAULT_AFTER "]"

typedef struct cld_progress_t {
	char* name;
	int length;
	char* bar;
	char* before;
	char* after;
	double current;
	double total;
} cld_progress;

int create_cld_progress(cld_progress** progress, char* name, int length, double total);

void free_cld_progress(cld_progress* progress);

void show_progress(cld_progress* progress);

#endif /* SRC_CLD_PROGRESS_H_ */
