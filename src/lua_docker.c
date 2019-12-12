#include "lua_docker.h"

void check_DockerClient(lua_State *L, int i){
	luaL_checkudata(L, i, DockerClient_metatable);
}

int DockerClient_connect_url(lua_State* L) {
	const char* url = lua_tostring(L, 1);
	lua_pop(L, 1);

	// Create a DockerClient instance and set its metatable.
	DockerClient* dc = (DockerClient*)lua_newuserdata(L, sizeof(DockerClient));
	// stack = [dc]
	luaL_getmetatable(L, DockerClient_metatable);
	// stack = [dc, mt]
	lua_setmetatable(L, 1);
	// stack = [dc]

	d_err_t err = make_docker_context_url(&(dc->ctx), url);
	if (err != E_SUCCESS) {
		luaL_error(L, "Error creating docker client");
	}
	return 1;
}

int DockerClient_new(lua_State* L) {
	// Create a DockerClient instance and set its metatable.
	DockerClient* dc = (DockerClient*)lua_newuserdata(L, sizeof(DockerClient));
	// stack = [dc]
	luaL_getmetatable(L, DockerClient_metatable);
	// stack = [dc, mt]
	lua_setmetatable(L, 1);
	// stack = [dc]

	d_err_t err = make_docker_context_default_local(&(dc->ctx));
	if (err != E_SUCCESS) {
		luaL_error(L, "Error creating docker client");
	}
	return 1;
}

int DockerClient__gc(lua_State *L){
	printf("In DockerClient__gc\n");
	return 0;
}

int DockerClient_container_list(lua_State* L) {
	// Expected: stack = [self]
	DockerClient* dc = (DockerClient*)luaL_checkudata(L, 1, DockerClient_metatable);
	printf("%s\n", dc->ctx->url);

	docker_ctr_list* ctrls;
	d_err_t err = docker_container_list(dc->ctx, &ctrls, 0, 0, 1, NULL);
	if (err != E_SUCCESS) {
		luaL_error(L, "Error listing containers");
	}
	const char* ctrls_json = get_json_string(ctrls);
	lua_pushstring(L, ctrls_json);
	return 1;
}

int luaopen_luaclibdocker(lua_State *L){
	static const luaL_Reg DockerClient_lib[] = {
		{ "container_ls", &DockerClient_container_list },
		{ NULL, NULL }
	};
	
	static const luaL_Reg clibdocker_lib[] = {
		{ "connect", &DockerClient_new },
		{ "connect_url", &DockerClient_connect_url },
		{ NULL, NULL }
	};
	
	luaL_newlib(L, clibdocker_lib);

	// Stack: clibdocker
	luaL_newmetatable(L, DockerClient_metatable); // Stack: clibdocker meta
	luaL_newlib(L, DockerClient_lib);
	lua_setfield(L, -2, "__index"); // Stack: clibdocker meta
	
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, DockerClient__gc); // Stack: clibdocker meta "__gc" fptr
	lua_settable(L, -3); // Stack: clibdocker meta
	lua_pop(L, 1); // Stack: clibdocker
	
	return 1;
}