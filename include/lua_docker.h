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

#define DockerClient_metatable "DockerClient"

typedef struct {
	docker_context* ctx;
} DockerClient;

// DockerClient:new()
LUALIB_API int DockerClient_new(lua_State* L);

LUALIB_API int DockerClient_container_list(lua_State* L);

LUALIB_API int luaopen_clibdocker(lua_State* L);

#ifdef __cplusplus 
}
#endif

#endif //LUA_ENABLED

#endif //SRC_LUA_DOCKER_H_
