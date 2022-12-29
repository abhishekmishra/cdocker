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

/**
 * \file docker_util.h
 * \brief Docker API utils
 */

#ifndef SRC_DOCKER_UTIL_H_
#define SRC_DOCKER_UTIL_H_

#ifdef __cplusplus  
extern "C" {
#endif

// Do not warn about strcpy_s, sprintf_s and sscanf_s on windows build
#define _CRT_SECURE_NO_WARNINGS

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
MODULE_API d_err_t make_pair(pair** p, char* key, char* value);

/**
 * Get the value part of the pair
 * \param p pair
 * \return string value
 */
MODULE_API char* get_value(pair*p);

/**
 * Get the key part of the pair
 * \param p pair
 * \return string key
 */
MODULE_API char* get_key(pair*p);

/**
 * Free the pair object
 * \param p pair
 */
MODULE_API void free_pair(pair*p);

// Filter utility functions for use in Docker API calls.

/**
 * Create new filter json_object for use in calls
 * to the docker API, which uses filters in many endpoints.
 *
 * \return new json_object* which should be passed to all calls to
 * 		filter methods.
 */
MODULE_API json_object* make_filters();

/**
 * Add a string value to the filter if it exits, or create a new one.
 *
 * \param filters the filters object created earlier using make_filters
 * \param name name of the filter
 * \param value value of the filter
 */
MODULE_API void add_filter_str(json_object* filters, const char* name, const char* value);

/**
 * Add an int value to the filter if it exits, or create a new one.
 *
 * \param filters the filters object created earlier using make_filters
 * \param name name of the filter
 * \param value value of the filter
 */
MODULE_API void add_filter_int(json_object* filters, const char* name, int value);

/**
 * Convert the filters object to a json string.
 * Behaviour is undefined if filters is NULL.
 *
 * \param filters
 * \return json string
 */
MODULE_API const char* filters_to_str(json_object* filters);

/**
 * Clone a string for usage in internal apis.
 * User of api can then get rid of their copy.
 * (obviously wasteful, use with caution)
 *
 * \param from string to clone from
 * \return cloned string, NULL if there is an error.
 */
MODULE_API char* str_clone(const char* from);

/**
 * Get a string object with the given key from a json_object
 * (will return NULL if the attribute is not found).
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return string value of attribute
 */
MODULE_API char* get_attr_str(json_object* obj, const char* name);

/**
 * Set a string object with the given key in a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \param val string value of the attribute
 * \return string value of attribute
 */
MODULE_API d_err_t set_attr_str(json_object* obj, const char* name, const char* val);

/**
 * Get a int object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return int value of attribute
 */
MODULE_API int get_attr_int(json_object* obj, const char* name);

/**
 * Set an integer object with the given key in a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \param val string value of the attribute
 * \return string value of attribute
 */
MODULE_API d_err_t set_attr_int(json_object* obj, const char* name, int val);

/**
 * Get a boolean object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return boolean value of attribute
 */
MODULE_API int get_attr_boolean(json_object* obj, const char* name);

/**
 * Set a boolean object with the given key in a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \param val string value of the attribute
 * \return string value of attribute
 */
MODULE_API d_err_t set_attr_boolean(json_object* obj, const char* name, int val);

/**
 * Get a long object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return long value of attribute
 */
MODULE_API long get_attr_long(json_object* obj, const char* name);

/**
 * Set a long integer object with the given key in a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \param val string value of the attribute
 * \return string value of attribute
 */
MODULE_API d_err_t set_attr_long(json_object* obj, const char* name, long val);

/**
 * Get a unsigned long object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return unsigned long value of attribute
 */
MODULE_API unsigned long get_attr_unsigned_long(json_object* obj, const char* name);

/**
 * Set an unsigned long integer object with the given key in a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \param val string value of the attribute
 * \return string value of attribute
 */
MODULE_API d_err_t set_attr_unsigned_long(json_object* obj, const char* name, unsigned long val);

/**
 * Get a long long object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \return long long value of attribute
 */
MODULE_API long long get_attr_long_long(json_object* obj, const char* name);

/**
 * Set a long long int object with the given key in a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \param val string value of the attribute
 * \return string value of attribute
 */
MODULE_API d_err_t set_attr_long_long(json_object* obj, const char* name, long long val);

/**
 * Get the time_t object with the given key from a json_object
 * 
 * \param obj the json_object
 * \param name of the string attribute
 * \return time_t value of the attribute
 */
MODULE_API time_t get_attr_time(json_object* obj, const char* name);

/**
 * Set a time object with the given key in a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \param val string value of the attribute
 * \return string value of attribute
 */
MODULE_API d_err_t set_attr_time(json_object* obj, const char* name, time_t val);

/**
 * Get a long long object with the given key from a json_object
 *
 * \param obj the json object
 * \param name of the object attribute
 * \return long long value of attribute
 */
MODULE_API json_object* get_attr_json_object(json_object* obj, const char* name);

/**
 * Set a json object with the given key in a json_object
 *
 * \param obj the json object
 * \param name of the string attribute
 * \param val string value of the attribute
 * \return string value of attribute
 */
MODULE_API d_err_t set_attr_json_object(json_object* obj, const char* name, json_object* val);

/**
 * Get the json type of the attribute identified by "name" of 
 * the given json_object.
 * See the json_type enum for possible values.
 *
 * \param obj the json object
 * \param name of the attribute
 * \return json_type
 */
MODULE_API json_type get_attr_type(json_object* obj, const char* name);

/**
 * Get prettified json string.
 * The returned string is owned by the json_object,
 * and will be free when the json_object is freed.
 * If caller needs to use the string beyond the lifetime
 * of owning object, create a duplicate.
 * 
 * \param obj the json object.
 * \return the json string.
 */
MODULE_API const char* get_json_string(json_object* obj);

/**
 * Add a string value to the array if it exits, or create a new one.
 *
 * \param obj the json object containing the array
 * \param name name of the array
 * \param value value of the array
 */
MODULE_API void add_array_str(json_object* obj, const char* name, const char* value);

/**
 * Add an int value to the array if it exits, or create a new one.
 *
 * \param obj the json object containing the array
 * \param name name of the array
 * \param value value of the array
 */
MODULE_API void add_array_int(json_object* obj, const char* name, int value);


MODULE_API void parse_iso_datetime(char* date_str, struct tm* tm);

MODULE_API void parse_docker_stats_readtime(char* date_str, struct tm* tm);

/**
 * Get the size in bytes, kb, mb, gb, tb etc.
 * (higest possible unit)
 *
 * \param size
 * \return string with the description of the size
 */
MODULE_API char* calculate_size(uint64_t size);


#ifdef __cplusplus 
}
#endif

#endif /* SRC_DOCKER_UTIL_H_ */
