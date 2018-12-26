/*
 * clibdocker: docker_util.c
 * Created on: 26-Dec-2018
 *
 * MIT License
 *
 * Copyright (c) 2018 Abhishek Mishra
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <json-c/json_object.h>
#include <log.h>

char* make_defensive_copy(const char* from) {
	char* to = NULL;
	if ((from != NULL) && (strlen(from) > 0)) {
		to = (char*) malloc((strlen(from) + 1) * sizeof(char));
		strcpy(to, from);
	}
	return to;
}

char* get_attr_str(json_object* obj, char* name) {
	json_object* extractObj;
	char* attr = NULL;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		attr = (char*) json_object_get_string(extractObj);
		free(extractObj);
	}
	docker_log_debug("%s is |%s|.", name, attr);
	return attr;
}

int get_attr_boolean(json_object* obj, char* name) {
	json_object* extractObj;
	char* attr = NULL;
	int flag = 0;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		attr = (char*) json_object_get_string(extractObj);
		free(extractObj);
		if(strcmp(attr, "true")) {
			flag = 1;
		}
	}
	docker_log_debug("%s is |%s|, parsed as |%d|.", name, attr, flag);
	return flag;
}

int get_attr_int(json_object* obj, char* name) {
	json_object* extractObj;
	int attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%d", &attr);
		free(extractObj);
	}
	docker_log_debug("%s is |%d|.", name, attr);
	return attr;
}

long get_attr_long(json_object* obj, char* name) {
	json_object* extractObj;
	long attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%ld", &attr);
		free(extractObj);
	}
	docker_log_debug("%s is |%ld|.", name, attr);
	return attr;
}

long long get_attr_long_long(json_object* obj, char* name) {
	json_object* extractObj;
	long long attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%lld", &attr);
		free(extractObj);
	}
	docker_log_debug("%s is |%lld|.", name, attr);
	return attr;
}

