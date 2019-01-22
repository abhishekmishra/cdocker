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

/**
 * A simple pair of strings
 *
 */
typedef struct pair_t {
	char* key; /**<key part of the pair*/
	char* value; /**<value part of the pair*/
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

/**
 * Get the value part of the pair
 * \param p pair
 * \return string value
 */
char* get_value(pair*p);

/**
 * Get the key part of the pair
 * \param p pair
 * \return string key
 */
char* get_key(pair*p);

/**
 * Free the pair object
 * \param p pair
 */
void free_pair(pair*p);

// Filter utility functions
json_object* make_filters();
void add_filter_str(json_object* filters, char* name, char* value);
void add_filter_int(json_object* filters, char* name, int value);
const char* filters_to_str(json_object* filters);

char* str_clone(const char* from);

/**
 * Get a string object with the given key from a json_object
 * (will return NULL if the attribute is not found).
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return string value of attribute
 */
char* get_attr_str(json_object* obj, char* name);

/**
 * Get a int object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return int value of attribute
 */
int get_attr_int(json_object* obj, char* name);

/**
 * Get a boolean object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return boolean value of attribute
 */
int get_attr_boolean(json_object* obj, char* name);

/**
 * Get a long object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return long value of attribute
 */
long get_attr_long(json_object* obj, char* name);

/**
 * Get a unsigned long object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return unsigned long value of attribute
 */
unsigned long get_attr_unsigned_long(json_object* obj, char* name);

/**
 * Get a long long object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return long long value of attribute
 */
long long get_attr_long_long(json_object* obj, char* name);

void parse_iso_datetime(char* date_str, struct tm* tm);

void parse_docker_stats_readtime(char* date_str, struct tm* tm);

#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_UTIL_H_ */
