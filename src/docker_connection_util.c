/*
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
 */

#include "docker_util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <docker_log.h>
#include "docker_result.h"
#include "docker_connection_util.h"
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>
#include <json-c/linkhash.h>
#include <stdbool.h>

bool prefix(const char* pre, const char* str)
{
	return strncmp(pre, str, strlen(pre)) == 0;
}

bool is_http_url(char* url)
{
	if (url)
	{
		if (strlen(url) > 0)
		{
			return prefix("http", url);
		}
	}
	return false;
}

bool is_unix_socket(char* url)
{
	if (url)
	{
		if (strlen(url) > 0)
		{
			return prefix("/", url);
		}
	}
	return false;
}

bool is_npipe(char* url)
{
	if (url)
	{
		if (strlen(url) > 0)
		{
			return prefix("npipe", url);
		}
	}
	return false;
}

d_err_t make_docker_context_url(docker_context** ctx, const char* url)
{
	(*ctx) = (docker_context*)calloc(1, sizeof(docker_context));
	if (!(*ctx))
	{
		return E_ALLOC_FAILED;
	}
	char* u = (char*)calloc((strlen(url) + 1), sizeof(char));
	if (!u)
	{
		return E_ALLOC_FAILED;
	}
	strcpy(u, url);
	(*ctx)->url = u;
	(*ctx)->api_version = DOCKER_API_VERSION_1_39;
	return E_SUCCESS;
}

d_err_t make_docker_context_default_local(docker_context** ctx) {
#if defined(_WIN32)
	return make_docker_context_url(ctx, DOCKER_DEFAULT_LOCALHOST_URL);
#endif
#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MAC__)
	return make_docker_context_url(ctx, DOCKER_DEFAULT_UNIX_SOCKET);
#endif
}

d_err_t docker_context_result_handler_set(docker_context* ctx, docker_result_handler_fn* result_handler_fn) {
	if (ctx != NULL) {
		ctx->result_handler_fn = result_handler_fn;
	}
	return E_SUCCESS;
}

docker_result_handler_fn* docker_context_result_handler_get(docker_context* ctx) {
	if (ctx != NULL) {
		return ctx->result_handler_fn;
	}
	return NULL;
}

d_err_t docker_context_client_args_set(docker_context* ctx, void* client_args) {
	if (ctx != NULL) {
		ctx->client_args = client_args;
	}
	return E_SUCCESS;
}

void* docker_context_client_args_get(docker_context* ctx) {
	if (ctx != NULL) {
		return ctx->client_args;
	}
	return NULL;
}


/**
 * Free docker context memory.
 */
d_err_t free_docker_context(docker_context** ctx)
{
	if ((*ctx))
	{
		if ((*ctx)->url)
		{
			free((*ctx)->url);
		}
		free((*ctx));
	}
	return E_SUCCESS;
}

char* create_service_url_id_method(docker_object_type object, const char* id,
	const char* method)
{
	char* object_url = NULL;
	switch (object)
	{
	case NONE:
		object_url = NULL;
		break;
	case CONTAINER:
		object_url = "containers";
		break;
	case IMAGE:
		object_url = "images";
		break;
	case SYSTEM:
		object_url = NULL;
		break;
	case NETWORK:
		object_url = "networks";
		break;
	case VOLUME:
		object_url = "volumes";
		break;
	}
	char* url = NULL;
	if (object_url)
	{
		if (id)
		{
			url = (char*)calloc(
				(strlen(object_url) + strlen(id) + strlen(method) + 3),
				sizeof(char));
			if (url != NULL) {
				sprintf(url, "%s/%s/%s", object_url, id, method);
				docker_log_debug("%s url is %s", method, url);
			}
		}
		else if (method)
		{
			url = (char*)calloc(
				(strlen(object_url) + strlen(method) + 3), sizeof(char));
			if (url != NULL) {
				sprintf(url, "%s/%s", object_url, method);
				docker_log_debug("%s url is %s", method, url);
			}
		}
		else
		{
			url = (char*)calloc((strlen(object_url) + 1), sizeof(char));
			if (url != NULL) {
				sprintf(url, "%s", object_url);
				docker_log_debug("url is %s", url);
			}
		}
	}
	else
	{
		//when there is no object ignore both object and id
		url = (char*)calloc((strlen(method) + 1), sizeof(char));
		if (url != NULL) {
			sprintf(url, "%s", method);
			docker_log_debug("%s url is %s", method, url);
		}
	}
	return url;
}

// BEGIN: Docker API Calls HTTP Utils V2 

d_err_t make_docker_call(docker_call** dcall, char* site_url, docker_object_type object,
	const char* id, const char* method) {
	(*dcall) = (docker_call*)calloc(1, sizeof(docker_call));
	if ((*dcall) == NULL) {
		return E_ALLOC_FAILED;
	}
	if (site_url != NULL)
	{
		if (is_unix_socket(site_url)) {
			(*dcall)->site_url = "http://localhost/";
		}
		else {
			(*dcall)->site_url = site_url;
		}
	}
	else
	{
		(*dcall)->site_url = NULL;
	}
	(*dcall)->object = object;
	(*dcall)->id = (char*)id;
	(*dcall)->method = (char*)method;
	(*dcall)->params = make_coll_al_map((coll_al_map_compare_fn*)&strcmp);

	(*dcall)->request_method = "GET";
	(*dcall)->request_data = NULL;
	(*dcall)->request_data_len = -1L;

	(*dcall)->capacity = 1024 * 1024;
	(*dcall)->memory = malloc((*dcall)->capacity);
	if ((*dcall)->memory == NULL) {
		return E_ALLOC_FAILED;
	}
	(*dcall)->size = 0;
	(*dcall)->flush_end = 0;

	(*dcall)->status_cb = NULL;
	(*dcall)->cb_args = NULL;
	(*dcall)->client_cb_args = NULL;
	return E_SUCCESS;
}

void docker_call_request_method_set(docker_call* dcall, char* method) {
	if (dcall != NULL && method != NULL) {
		dcall->request_method = str_clone(method);
	}
}

char* docker_call_request_method_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->request_method;
	}
	return NULL;
}

void docker_call_content_type_header_set(docker_call* dcall, char* content_type_header) {
	if (dcall != NULL && content_type_header != NULL) {
		dcall->content_type_header = str_clone(content_type_header);
	}
}

char* docker_call_content_type_header_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->content_type_header;
	}
	return NULL;
}

void docker_call_request_data_set(docker_call* dcall, char* request_data) {
	if (dcall != NULL && request_data != NULL) {
		dcall->request_data = str_clone(request_data);
	}
}

char* docker_call_request_data_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->request_data;
	}
	return NULL;
}

void docker_call_request_data_len_set(docker_call* dcall, size_t request_data_len) {
	if (dcall != NULL) {
		dcall->request_data_len = request_data_len;
	}
}

size_t docker_call_request_data_len_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->request_data_len;
	}
	return -1L;
}

char* docker_call_response_data_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->memory;
	}
	return NULL;
}

size_t docker_call_response_data_length(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->size;
	}
	return 0;
}

int docker_call_http_code_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->http_error_code;
	}
	return 0;
}

void docker_call_http_code_set(docker_call* dcall, int http_code) {
	if (dcall != NULL) {
		dcall->http_error_code = http_code;
	}
}

void docker_call_status_cb_set(docker_call* dcall, status_callback* status_callback) {
	if (dcall != NULL) {
		dcall->status_cb = status_callback;
	}
}

status_callback* docker_call_status_cb_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->status_cb;
	}
	return NULL;
}

void docker_call_cb_args_set(docker_call* dcall, void* cb_args) {
	if (dcall != NULL) {
		dcall->cb_args = cb_args;
	}
}

char* docker_call_cb_args_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->cb_args;
	}
	return NULL;
}

void docker_call_client_cb_args_set(docker_call* dcall, void* client_cb_args) {
	if (dcall != NULL) {
		dcall->client_cb_args = client_cb_args;
	}
}

char* docker_call_client_cb_args_get(docker_call* dcall) {
	if (dcall != NULL) {
		return dcall->client_cb_args;
	}
	return NULL;
}

void free_param_value(size_t idx, char* param, char* value)
{
	if (param != NULL) {
		free(param);
	}
	if (value != NULL) {
		free(value);
	}
}

void free_docker_call(docker_call* dcall)
{
	if (dcall != NULL)
	{
		if (dcall->memory != NULL) {
			free(dcall->memory);
		}
		coll_al_map_foreach_fn(dcall->params, (coll_al_map_iter_fn*)&free_param_value);
		free_coll_al_map(dcall->params);
		free(dcall);
	}
}

int docker_call_params_add(docker_call* dcall, char* param, char* value)
{
	if (dcall != NULL) {
		return coll_al_map_put(dcall->params, str_clone(param), str_clone(value));
	}
	return E_UNKNOWN_ERROR;
}

int docker_call_params_add_boolean(docker_call* dcall, char* param, int value) {
	if (value == 0) {
		docker_call_params_add(dcall, param, "false");
	}
	else {
		docker_call_params_add(dcall, param, "true");
	}
}

char* docker_call_get_url(docker_call* dcall) {
	CURL* curl = curl_easy_init();

	char* service_url = create_service_url_id_method(dcall->object, dcall->id, dcall->method);
	coll_al_map* esc_params = make_coll_al_map((coll_al_map_compare_fn*)&strcmp);

	char* final_url = NULL;
	size_t final_url_len = 2; //for question mark and null terminator

	final_url_len += strlen(dcall->site_url);
	final_url_len += strlen(service_url);

	for (size_t i = 0; i < coll_al_map_keys_length(dcall->params); i++) {
		char* key_esc = str_clone(curl_easy_escape(curl, coll_al_map_keys_get_idx(dcall->params, i), 0));
		char* val_esc = str_clone(curl_easy_escape(curl, coll_al_map_values_get_idx(dcall->params, i), 0));

		coll_al_map_put(esc_params, key_esc, val_esc);

		final_url_len += strlen(key_esc);
		final_url_len += strlen(val_esc);
		final_url_len += 2; //for equals and ampersand
	}

	final_url = (char*)calloc(final_url_len, sizeof(char));
	if (final_url != NULL) {
		final_url[0] = 0;
		strcat(final_url, dcall->site_url);
		strcat(final_url, service_url);
		size_t num_params = coll_al_map_keys_length(esc_params);
		if (num_params > 0) {
			strcat(final_url, "?");
			for (size_t i = 0; i < num_params; i++) {
				if (i > 0)
				{
					strcat(final_url, "&");
				}
				strcat(final_url, coll_al_map_keys_get_idx(esc_params, i));
				strcat(final_url, "=");
				strcat(final_url, coll_al_map_values_get_idx(esc_params, i));
			}
		}
	}
	curl_easy_cleanup(curl);
	coll_al_map_foreach_fn(esc_params, (coll_al_map_iter_fn*)&free_param_value);
	free_coll_al_map(esc_params);
	return final_url;
}

static size_t write_memory_callback_v2(void* contents, size_t size, size_t nmemb,
	void* userp)
{
	size_t realsize = size * nmemb;
	docker_call* mem = (docker_call*)userp;
	size_t new_size = mem->size + realsize + 1;

	if (new_size > mem->capacity) {
		//realloc twice the new size
		mem->capacity = 2 * new_size;
		char* ptr = realloc(mem->memory, mem->capacity);
		if (ptr == NULL)
		{
			/* out of memory! */
			docker_log_debug("not enough memory (realloc returned NULL)");
			return 0;
		}

		mem->memory = ptr;
	}
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	//	docker_log_debug("MEMORY-------\n%s\n", mem->memory);

	/** if callback is not null, and current memory contents end with a newline,
	* then flush it to the callback, and empty the memory contents.
	*/
	if (mem->status_cb != NULL && mem->memory)
	{
		char* flush_str = mem->memory + (mem->flush_end * sizeof(char));
		if (flush_str[strlen(flush_str) - 1] == '\n')
		{
			char* msg = str_clone(flush_str);
			mem->status_cb(msg, mem->cb_args, mem->client_cb_args);
			mem->flush_end += strlen(flush_str);
		}
	}
	return realsize;
}

void handle_response_v2(long response_code, char* effective_url,
	docker_result* result, docker_call* call, json_object** response)
{
	docker_log_debug("%lu bytes retrieved\n", (unsigned long)call->size);
	json_object* response_obj = NULL;
	if (call->size > 0)
	{
		response_obj = json_tokener_parse(call->memory);
		//increment reference count so that the caller can use
		//and then free the json object.
		(*response) = json_object_get(response_obj);
		docker_log_debug("Response = %s",
			json_object_to_json_string(response_obj));
	}
	else
	{
		docker_log_debug("Response = Empty");
	}

	docker_call_http_code_set(call, response_code);
	result->http_error_code = response_code;
	result->url = str_clone(effective_url);

	if (response_code == 200 || response_code == 201
		|| response_code == 204)
	{
		result->error_code = E_SUCCESS;
	}
	else
	{
		result->error_code = E_INVALID_INPUT;
		result->message = str_clone("error");
	}
	if (result->http_error_code >= 400)
	{
		char* msg = get_attr_str(response_obj, "message");
		if (msg)
		{
			result->message = str_clone(msg);
		}
	}

}

d_err_t docker_call_exec(docker_context* ctx, docker_call* dcall, json_object** response) {
	time_t start, end;
	d_err_t err = E_SUCCESS;
	docker_result* result;

	// set the start time
	start = time(NULL);

	// allocate the docker result object
	err = new_docker_result(&result);
	if (err != E_SUCCESS) {
		return err;
	}

	char* docker_http_method = docker_call_request_method_get(dcall);

#ifdef _WIN32
	if (docker_call_status_cb_get(dcall) == NULL &&
		(docker_http_method == NULL || strcmp(docker_http_method, HTTP_GET_STR) == 0)) {
		//TODO free at the end
		dcall->site_url = "/";
		char* service_url = docker_call_get_url(dcall);
		if (service_url == NULL) {
			return E_ALLOC_FAILED;
		}
		HANDLE hPipe;
		char* lpvMessage = (char*)calloc(strlen(service_url) + 50, sizeof(char));
		if (lpvMessage == NULL) {
			return E_ALLOC_FAILED;
		}
		lpvMessage[0] = '\0';
		strcat(lpvMessage, "GET ");
		strcat(lpvMessage, service_url);
		strcat(lpvMessage, " HTTP/1.0\n\n");
		//"GET /version HTTP/1.0\n\n";

		char  chBuf[NPIPE_READ_BUFSIZE + 1]; //one extra for null terminator
		BOOL   fSuccess = FALSE;
		DWORD  cbRead, cbToWrite, cbWritten, dwMode;
		LPCTSTR lpszPipename = TEXT(DOCKER_DEFAULT_WINDOWS_NAMED_PIPE);

		// Try to open a named pipe; wait for it, if necessary. 

		while (1)
		{
			hPipe = CreateFile(
				lpszPipename,   // pipe name 
				GENERIC_READ |  // read and write access 
				GENERIC_WRITE,
				0,              // no sharing 
				NULL,           // default security attributes
				OPEN_EXISTING,  // opens existing pipe 
				0,              // default attributes 
				NULL);          // no template file 

		  // Break if the pipe handle is valid. 

			if (hPipe != INVALID_HANDLE_VALUE)
				break;

			// Exit if an error other than ERROR_PIPE_BUSY occurs. 

			if (GetLastError() != ERROR_PIPE_BUSY)
			{
				_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
				return -1;
			}

			// All pipe instances are busy, so wait for 20 seconds. 

			if (!WaitNamedPipe(lpszPipename, 20000))
			{
				printf("Could not open pipe: 20 second wait timed out.");
				return -1;
			}
		}

		// The pipe connected; change to message-read mode. 

		dwMode = PIPE_READMODE_MESSAGE;
		fSuccess = SetNamedPipeHandleState(
			hPipe,    // pipe handle 
			&dwMode,  // new pipe mode 
			NULL,     // don't set maximum bytes 
			NULL);    // don't set maximum time 
		if (!fSuccess)
		{
			_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
			return -1;
		}

		// Send a message to the pipe server. 

		cbToWrite = (strlen(lpvMessage) + 1) * sizeof(char);
		//printf("Sending %d byte message: \"%s\"\n", cbToWrite, lpvMessage);

		fSuccess = WriteFile(
			hPipe,                  // pipe handle 
			lpvMessage,             // message 
			cbToWrite,              // message length 
			&cbWritten,             // bytes written 
			NULL);                  // not overlapped 

		if (!fSuccess)
		{
			_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
			return -1;
		}

		//printf("\nMessage sent to server, receiving reply as follows:\n");

		int crlf_count = 0;
		bool start_content = false;
		int last_error = -1;
		do
		{
			// Read from the pipe. 

			fSuccess = ReadFile(
				hPipe,    // pipe handle 
				chBuf,    // buffer to receive reply 
				NPIPE_READ_BUFSIZE / sizeof(TCHAR) * sizeof(char),  // size of buffer 
				&cbRead,  // number of bytes read 
				NULL);    // not overlapped 

			chBuf[cbRead] = NULL;
			//printf("\n%s\n", chBuf);

			//TODO: replace this bit with a http response parser.
			if (start_content) {
				write_memory_callback_v2(chBuf, (size_t)cbRead, 1, dcall);
			}
			else {
				bool cr = false;
				for (size_t i = 0; i < cbRead; i++) {
					if (cr) {
						if (chBuf[i] == '\n') {
							crlf_count += 1;
							cr = false;
						}
					}
					else if (chBuf[i] == '\r') {
						cr = true;
					}
					else {
						cr = false;
						crlf_count = 0;
					}
				}
				if (crlf_count == 2) {
					start_content = true;
				}
			}
			last_error = GetLastError();
			if (!fSuccess && last_error != ERROR_MORE_DATA) {
				break;
			}
		} while (fSuccess || last_error == ERROR_MORE_DATA);  // repeat loop if ERROR_MORE_DATA 

		//if (!fSuccess)
		//{
		//	_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		//	return -1;
		//}
		//else {
		/* Check for errors, and handle response */
		handle_response_v2(200L, service_url, result, dcall, response);

		// Mark end time of request
		end = time(NULL);

		if (result != NULL)
		{
			result->method = docker_call_request_method_get(dcall);
			if (docker_call_request_data_get(dcall) != NULL)
			{
				result->request_json_str = str_clone(docker_call_request_data_get(dcall));
			}
			if (dcall->memory != NULL)
			{
				size_t data_len = docker_call_response_data_length(dcall);
				result->response_json_str =
					(char*)calloc(data_len + 1, sizeof(char));
				if (result->response_json_str == NULL) {
					return E_ALLOC_FAILED;
				}
				memcpy(result->response_json_str,
					docker_call_response_data_get(dcall),
					data_len);
				result->response_json_str[data_len] = '\0';
			}
			result->start_time = start;
			result->end_time = end;

			docker_result_handler_fn* fn = docker_context_result_handler_get(ctx);
			if (fn != NULL) {
				(*fn)(ctx, result);
			}

			err = result->error_code;

		}

		//}

		//printf("\n<End of message, press ENTER to terminate connection and exit>");

		CloseHandle(hPipe);
	}
	else {
#endif
		CURL* curl;
		CURLcode res;
		struct curl_slist* headers = NULL;

		/* get a curl handle */
		curl = curl_easy_init();

		if (curl)
		{
			// Set the URL
			char* docker_url = docker_call_get_url(dcall);
			if (is_unix_socket(ctx->url))
			{
				curl_easy_setopt(curl, CURLOPT_UNIX_SOCKET_PATH, ctx->url);
			}
			curl_easy_setopt(curl, CURLOPT_URL, docker_url);

			// Set the custom request if any (not required for GET/POST)
			if (docker_call_request_method_get(dcall) != NULL) {
				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, docker_call_request_method_get(dcall));
			}

			// Set content type headers if any
			if (docker_call_content_type_header_get(dcall) != NULL) {
				headers = curl_slist_append(headers, "Expect:");
				headers = curl_slist_append(headers, docker_call_content_type_header_get(dcall));
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			}

			// Now specify the POST data if request type is POST
			// and request_data is not NULL.
			if (docker_call_request_data_get(dcall) != NULL &&
				strcmp(docker_call_request_method_get(dcall), "POST") == 0) {
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, docker_call_request_data_get(dcall));
				curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, docker_call_request_data_len_get(dcall));
			}

			/* send all data to this function  */
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback_v2);

			/* we pass our 'chunk' struct to the callback function */
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)dcall);

			/* some servers don't like requests that are made without a user-agent
			 field, so we provide one */
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);

			long response_code;
			char* effective_url;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
			curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);

			/* Check for errors */
			if (res != CURLE_OK)
			{
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
				result->error_code = E_CONNECTION_FAILED;
			}
			else {
				/* Check for errors, and handle response */
				handle_response_v2(response_code, effective_url, result, dcall, response);

				// Mark end time of request
				end = time(NULL);

				if (result != NULL)
				{
					result->method = docker_call_request_method_get(dcall);
					if (docker_call_request_data_get(dcall) != NULL)
					{
						result->request_json_str = str_clone(docker_call_request_data_get(dcall));
					}
					if (dcall->memory != NULL)
					{
						size_t data_len = docker_call_response_data_length(dcall);
						result->response_json_str =
							(char*)calloc(data_len + 1, sizeof(char));
						if (result->response_json_str == NULL) {
							return E_ALLOC_FAILED;
						}
						memcpy(result->response_json_str,
							docker_call_response_data_get(dcall),
							data_len);
						result->response_json_str[data_len] = '\0';
					}
					result->start_time = start;
					result->end_time = end;

					docker_result_handler_fn* fn = docker_context_result_handler_get(ctx);
					if (fn != NULL) {
						(*fn)(ctx, result);
					}

					err = result->error_code;

				}
			}
			/* always cleanup */
			curl_easy_cleanup(curl);

			// free url
			free(docker_url);
		}
#ifdef _WIN32
		// cleanup docker_result
		free_docker_result(result);
	}
#endif

	return err;
}

// END: Docker API Calls HTTP Utils V2 