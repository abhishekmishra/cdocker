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

#include "docker_util.h"
#include <docker_log.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json_object.h>

char* str_clone(const char* from) {
	char* to = NULL;
	if ((from != NULL)) {
		to = (char*) malloc((strlen(from) + 1) * sizeof(char));
		if (to != NULL) {
			strcpy(to, from);
		}
	}
	return to;
}

char* get_attr_str(json_object* obj, const char* name) {
	json_object* extractObj;
	char* attr = NULL;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		attr = (char*)json_object_get_string(extractObj);
	}
	return attr;
}

d_err_t set_attr_str(json_object* obj, const char* name, const char* val) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_string(val);
		return json_object_object_add(obj, name, value_obj);
	}
	else {
		return json_object_set_string(value_obj, val);
	}
}

int get_attr_boolean(json_object* obj, const char* name) {
	json_object* extractObj;
	char* attr = NULL;
	int flag = 0;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		attr = (char*) json_object_get_string(extractObj);
		if (strcmp(attr, "true")) {
			flag = 1;
		}
		free(extractObj);
	}
//	docker_log_debug("%s is |%s|, parsed as |%d|.", name, attr, flag);
	return flag;
}

d_err_t set_attr_boolean(json_object* obj, const char* name, int val) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_boolean(val);
		return json_object_object_add(obj, name, value_obj);
	}
	else {
		return json_object_set_int(value_obj, val);
	}
}

int get_attr_int(json_object* obj, const char* name) {
	json_object* extractObj;
	int attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%d", &attr);
	}
//	docker_log_debug("%s is |%d|.", name, attr);
	return attr;
}

d_err_t set_attr_int(json_object* obj, const char* name, int val) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_int(val);
		return json_object_object_add(obj, name, value_obj);
	}
	else {
		return json_object_set_int(value_obj, val);
	}
}

long get_attr_long(json_object* obj, const char* name) {
	json_object* extractObj;
	long attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%ld", &attr);
	}
	return attr;
}

d_err_t set_attr_long(json_object* obj, const char* name, long val) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_int64(val);
		return json_object_object_add(obj, name, value_obj);
	}
	else {
		return json_object_set_int64(value_obj, val);
	}
}

unsigned long get_attr_unsigned_long(json_object* obj, const char* name) {
	json_object* extractObj;
	unsigned long attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%lu", &attr);
	}
	return attr;
}

d_err_t set_attr_unsigned_long(json_object* obj, const char* name, unsigned long val) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_int64(val);
		return json_object_object_add(obj, name, value_obj);
	}
	else {
		return json_object_set_int64(value_obj, val);
	}
}

long long get_attr_long_long(json_object* obj, const char* name) {
	json_object* extractObj;
	long long attr = -1;
	if (json_object_object_get_ex(obj, name, &extractObj)) {
		sscanf(json_object_get_string(extractObj), "%lld", &attr);
	}
	return attr;
}

d_err_t set_attr_long_long(json_object* obj, const char* name, long long val) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_int64(val);
		return json_object_object_add(obj, name, value_obj);
	}
	else {
		return json_object_set_int64(value_obj, val);
	}
}

time_t get_attr_time(json_object* obj, const char* name) {
	struct tm ctime;
	memset(&ctime, 0, sizeof(struct tm));
	parse_iso_datetime(get_attr_str(obj, name), &ctime);
	time_t time_ret = mktime(&ctime);
	return time_ret;
}

d_err_t set_attr_time(json_object* obj, const char* name, time_t val) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_int64(val);
		return json_object_object_add(obj, name, value_obj);
	}
	else {
		return json_object_set_int64(value_obj, val);
	}
}

json_object* get_attr_json_object(json_object* obj, const char* name) {
	json_object* extractObj = NULL;
	json_object_object_get_ex(obj, name, &extractObj);
	return extractObj;
}

d_err_t set_attr_json_object(json_object* obj, const char* name, json_object* val) {
	return json_object_object_add(obj, name, val);
}

json_type get_attr_type(json_object* obj, const char* name) {
	json_object* extractObj = NULL;
	json_object_object_get_ex(obj, name, &extractObj);
	if (extractObj == NULL) {
		return json_type_null;
	}
	else {
		return json_object_get_type(extractObj);
	}
}

const char* get_json_string(json_object* obj) {
	return json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PRETTY);
}

d_err_t make_pair(pair** p, char* key, char* value) {
	pair* p1 = (pair*) calloc(1, sizeof(pair));
	if (p1 == NULL) {
		return E_ALLOC_FAILED;
	}
	p1->key = str_clone(key);
	p1->value = str_clone(value);
	(*p) = p1;
	return E_SUCCESS;
}

char* get_value(pair*p) {
	return p->value;
}

char* get_key(pair*p) {
	return p->key;
}

void free_pair(pair*p) {
	free(p->key);
	free(p->value);
	free(p);
}

// Filter utility functions
json_object* make_filters() {
	return json_object_new_object();
}

void add_filter_str(json_object* filters, const char* name, const char* value) {
	json_object* value_obj;
	json_object_object_get_ex(filters, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_array();
		json_object_object_add(filters, name, value_obj);
	}
	json_object_array_add(value_obj, json_object_new_string(value));
}

void add_filter_int(json_object* filters, const char* name, int value) {
	json_object* value_obj;
	json_object_object_get_ex(filters, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_array();
		json_object_object_add(filters, name, value_obj);
	}
	json_object_array_add(value_obj, json_object_new_int(value));
}

const char* filters_to_str(json_object* filters) {
	return json_object_get_string(filters);
}

void add_array_str(json_object* obj, const char* name, const char* value) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_array();
		json_object_object_add(obj, name, value_obj);
	}
	json_object_array_add(value_obj, json_object_new_string(value));
}

void add_array_int(json_object* obj, const char* name, int value) {
	json_object* value_obj;
	json_object_object_get_ex(obj, name, &value_obj);
	if (!value_obj) {
		value_obj = json_object_new_array();
		json_object_object_add(obj, name, value_obj);
	}
	json_object_array_add(value_obj, json_object_new_int(value));
}

// implementation based on suggestions at
// https://stackoverflow.com/questions/26895428/how-do-i-parse-an-iso-8601-date-with-optional-milliseconds-to-a-struct-tm-in-c
void parse_iso_datetime(char* date_str, struct tm* tm) {
	int y, M, d, h, m;
	float s;
	int tzh = 0, tzm = 0;
	if (6
			< sscanf(date_str, "%d-%d-%dT%d:%d:%f%d:%dZ", &y, &M, &d, &h, &m,
					&s, &tzh, &tzm)) {
		if (tzh < 0) {
			tzm = -tzm;    // Fix the sign on minutes.
		}
	}
	tm->tm_year = y - 1900; // Year since 1900
	tm->tm_mon = M - 1;     // 0-11
	tm->tm_mday = d;        // 1-31
	tm->tm_hour = h;        // 0-23
	tm->tm_min = m;         // 0-59
	tm->tm_sec = (int) s;    // 0-61 (0-60 in C++11)
}

void parse_docker_stats_readtime(char* date_str, struct tm* tm) {
	int y, M, d, h, m;
	float s;
	unsigned long nanos; //currently unused
	if (6
			< sscanf(date_str, "%d-%d-%dT%d:%d:%f.%luZ", &y, &M, &d, &h, &m, &s,
					&nanos)) {
	}
	tm->tm_year = y - 1900; // Year since 1900
	tm->tm_mon = M - 1;     // 0-11
	tm->tm_mday = d;        // 1-31
	tm->tm_hour = h;        // 0-23
	tm->tm_min = m;         // 0-59
	tm->tm_sec = (int) s;    // 0-61 (0-60 in C++11)
}


// see https://stackoverflow.com/questions/3898840/converting-a-number-of-bytes-into-a-file-size-in-c
#define DIM(x) (sizeof(x)/sizeof(*(x)))

static const char     *sizes[]   = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" };
static const uint64_t  exbibytes = 1024ULL * 1024ULL * 1024ULL *
                                   1024ULL * 1024ULL * 1024ULL;

char* calculate_size(uint64_t size)
{
    char     *result = (char *) malloc(sizeof(char) * 20);
    uint64_t  multiplier = exbibytes;
    int i;

	if (result != NULL) {
		for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
		{
			if (size < multiplier)
				continue;
			if (size % multiplier == 0) {
				//changed this line to use %llu instead of PRIu64
				sprintf(result, "%llu %s", size / multiplier, sizes[i]);
			}
			else {
				sprintf(result, "%.1f %s", (float)size / multiplier, sizes[i]);
			}
			return result;
		}
		strcpy(result, "0");
	}
    return result;
}

