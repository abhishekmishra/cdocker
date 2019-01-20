/*
 * clibdocker: docker_util.h
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

/**
 * \file docker_util.h
 * \brief Docker API utils
 */

#ifndef SRC_DOCKER_UTIL_H_
#define SRC_DOCKER_UTIL_H_

#ifdef __cplusplus  
extern "C" {
#endif

#include <json-c/json_object.h>
#include "docker_result.h"

#define DOCKER_GETTER(object, type, name) \
		type docker_ ## object ## _get_ ## name(docker_ ## object* object);

#define DOCKER_GETTER_ARR_ADD(object, type, name) \
		int docker_ ## object ## _ ## name ## _add(docker_ ## object* object, type data);

#define DOCKER_GETTER_ARR_LEN(object, name) \
		int docker_ ## object ## _ ## name ## _length(docker_ ## object* object);

#define DOCKER_GETTER_ARR_GET_IDX(object, type, name) \
		type docker_ ## object ## _ ## name ## _get_idx(docker_ ## object* object, int i);


#define DOCKER_GETTER_IMPL(object, type, name) \
	type docker_ ## object ## _get_ ## name(docker_ ## object* object) { \
		return object->name; \
	} \


#define DOCKER_GETTER_ARR_ADD_IMPL(object, type, name) \
	int docker_ ## object ## _ ## name ## _add(docker_ ## object* object, type data) { \
		return array_list_add(object->name, (void*) data); \
	} \

#define DOCKER_GETTER_ARR_LEN_IMPL(object, name) \
	int docker_ ## object ## _ ## name ##_length(docker_ ## object* object) { \
		return array_list_length(object->name); \
	} \

#define DOCKER_GETTER_ARR_GET_IDX_IMPL(object, type, name) \
	type docker_ ## object ## _ ## name ## _get_idx(docker_ ## object* object, int i) { \
		return (type) array_list_get_idx(object->name, i); \
	} \

// A simple pair of strings

typedef struct pair_t {
	char* key;
	char* value;
} pair;

/**
 * Create a new pair with the given key and value.
 * The key and value strings can be freed, as the constructor creates its own copy of the strings.
 *
 * \param key
 * \param value
 * \return error code
 */
d_err_t make_pair(pair** p, char* key, char* value);

char* get_value(pair*p);

char* get_key(pair*p);

void free_pair(pair*p);

// Filter utility functions
json_object* make_filters();
void add_filter_str(json_object* filters, char* name, char* value);
void add_filter_int(json_object* filters, char* name, int value);
const char* filters_to_str(json_object* filters);

char* make_defensive_copy(const char* from);

char* get_attr_str(json_object* obj, char* name);

int get_attr_int(json_object* obj, char* name);

int get_attr_boolean(json_object* obj, char* name);

long get_attr_long(json_object* obj, char* name);

unsigned long get_attr_unsigned_long(json_object* obj, char* name);

long long get_attr_long_long(json_object* obj, char* name);

void parse_iso_datetime(char* date_str, struct tm* tm);

void parse_docker_stats_readtime(char* date_str, struct tm* tm);

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_UTIL_H_ */
