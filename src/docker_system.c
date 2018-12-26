/*
 * clibdocker: docker_system.c
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
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "docker_system.h"
#include "docker_util.h"
#include "docker_connection_util.h"
#include "log.h"

/**
 * Ping the docker server
 *
 * \param ctx docker context
 * \param result docker result object
 * \return error code
 */
error_t docker_ping(docker_context* ctx, docker_result** result) {
	char* url = create_service_url_id_method(SYSTEM, NULL, "_ping");

	json_object *response_obj;
	struct MemoryStruct chunk;
	docker_api_get(ctx, result, url, NULL, &chunk);

	if ((*result)->http_error_code >= 400) {
		(*result)->message = make_defensive_copy("Docker Server not OK.");
		return E_PING_FAILED;
	}

	response_obj = json_tokener_parse(chunk.memory);
	docker_log_debug("Response = %s", json_object_to_json_string(response_obj));

	return E_SUCCESS;
}
