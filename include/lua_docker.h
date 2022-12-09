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

#ifndef SRC_LUA_DOCKER_H_
#define SRC_LUA_DOCKER_H_

#ifdef LUA_ENABLED

#ifdef __cplusplus  
extern "C" {
#endif

#define LUA_LIB
#include "lua.h"
#include "lauxlib.h"

#include "docker_all.h"
#include <json-c/json_object.h>

#define DockerClient_metatable "DOCKER_CLIENT"
#define JsonObject_metatable "JSON_OBJECT"

typedef struct {
	docker_context* ctx;
} DockerClient;

typedef struct {
	json_object* obj;
} JsonObject;

LUALIB_API int DockerClient_connect_url(lua_State* L);

// DockerClient:new()
LUALIB_API int DockerClient_new(lua_State* L);

// DockerClient:from_context
int DockerClient_from_context(lua_State* L, docker_context* ctx);

// json utils
LUALIB_API int JsonObject_json_create(lua_State* L);
LUALIB_API int JsonObject_json_string(lua_State* L);

// container APIs
LUALIB_API int DockerClient_container_list(lua_State* L);
LUALIB_API int DockerClient_create_container(lua_State* L);
LUALIB_API int DockerClient_inspect_container(lua_State* L);
LUALIB_API int DockerClient_process_list_container(lua_State* L);
LUALIB_API int DockerClient_container_logs(lua_State* L);
LUALIB_API int DockerClient_container_changes(lua_State* L);
LUALIB_API int DockerClient_container_stats(lua_State* L);
LUALIB_API int DockerClient_start_container(lua_State* L);
LUALIB_API int DockerClient_stop_container(lua_State* L);
LUALIB_API int DockerClient_restart_container(lua_State* L);
LUALIB_API int DockerClient_kill_container(lua_State* L);
LUALIB_API int DockerClient_rename_container(lua_State* L);
LUALIB_API int DockerClient_pause_container(lua_State* L);
LUALIB_API int DockerClient_unpause_container(lua_State* L);
LUALIB_API int DockerClient_wait_container(lua_State* L);
LUALIB_API int DockerClient_remove_container(lua_State* L);


LUALIB_API int luaopen_luaclibdocker(lua_State* L);

#ifdef __cplusplus 
}
#endif

#endif //LUA_ENABLED

#endif //SRC_LUA_DOCKER_H_
