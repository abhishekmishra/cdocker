#include "lua_docker.h"

void* check_JsonObject(lua_State *L, int i){
	return luaL_checkudata(L, i, JsonObject_metatable);
}

void* check_DockerClient(lua_State *L, int i){
	return luaL_checkudata(L, i, DockerClient_metatable);
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

int JsonObject__gc(lua_State *L){
	printf("In JsonObject__gc\n");
	JsonObject* jo = check_JsonObject(L, 1);
	json_object_put(jo->obj);
	return 0;
}

LUALIB_API int JsonObject_json_create(lua_State* L) {
	const char* json_str = lua_tostring(L, 1);
	lua_pop(L, 1);

	// Create a JsonObject instance and set its metatable.
	JsonObject* jo = (JsonObject*)lua_newuserdata(L, sizeof(JsonObject));
	// stack = [jo]
	luaL_getmetatable(L, JsonObject_metatable);
	// stack = [jo, mt]
	lua_setmetatable(L, 1);
	// stack = [jo]

	jo->obj = json_tokener_parse(json_str);
	
	if (jo->obj == NULL) {
		luaL_error(L, "Error parsing json object.");
	}
	return 1;
}

LUALIB_API int JsonObject_json_string(lua_State* L) {
	JsonObject* jo = check_JsonObject(L, 1);
	const char* jo_str = get_json_string(jo->obj);
	lua_pushstring(L, jo_str);
	return 1;
}

int DockerClient__gc(lua_State *L){
	printf("In DockerClient__gc\n");
	DockerClient* dc = check_DockerClient(L, 1);
	free_docker_context(&(dc->ctx));
	return 0;
}

int DockerClient_container_list(lua_State* L) {
	// Expected: stack = [self]
	DockerClient* dc = (DockerClient*)luaL_checkudata(L, 1, DockerClient_metatable);

	docker_ctr_list* ctrls;
	d_err_t err = docker_container_list(dc->ctx, &ctrls, 0, 0, 1, NULL);
	if (err != E_SUCCESS) {
		luaL_error(L, "Error listing containers");
	}
	const char* ctrls_json = get_json_string(ctrls);
	lua_pushstring(L, ctrls_json);
	return 1;
}

LUALIB_API int DockerClient_create_container(lua_State* L) {
	// Expected: stack = [self, json_object]
	DockerClient* dc = check_DockerClient(L, 1);
	JsonObject* jo = check_JsonObject(L, 2);

	char* id;
	d_err_t err = docker_create_container(dc->ctx, &id, jo->obj);

	//TODO: error handling
	lua_pushstring(L, id);
	return 1;
}

int DockerClient_start_container(lua_State* L) {
	// Expected: stack = [self, id]
	DockerClient* dc = check_DockerClient(L, 1);
	char* id = lua_tostring(L, 2);

	printf("Starting container id %s\n", id);

	d_err_t err = docker_start_container(dc->ctx, id, NULL);

	if(err != E_SUCCESS) {
		luaL_error(L, "Unable to start container id %s", id);
	}

	return 0;
}

int luaopen_luaclibdocker(lua_State *L){
	docker_log_set_level(LOG_INFO);

	static const luaL_Reg JsonObject_lib[] = {
		{ "to_string", &JsonObject_json_string },
		{ NULL, NULL }
	};

	static const luaL_Reg DockerClient_lib[] = {
		{ "container_ls", &DockerClient_container_list },
		{ "container_create", &DockerClient_create_container },
		{ "container_start", &DockerClient_start_container },
		{ NULL, NULL }
	};
	
	static const luaL_Reg clibdocker_lib[] = {
		{ "connect", &DockerClient_new },
		{ "connect_url", &DockerClient_connect_url },
		{ "json_create", &JsonObject_json_create },
		{ NULL, NULL }
	};
	
	luaL_newlib(L, clibdocker_lib);

	// Stack: clibdocker
	luaL_newmetatable(L, JsonObject_metatable); // Stack: clibdocker meta
	luaL_newlib(L, JsonObject_lib);
	lua_setfield(L, -2, "__index"); // Stack: clibdocker meta

	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, JsonObject__gc); // Stack: clibdocker meta "__gc" fptr
	lua_settable(L, -3); // Stack: clibdocker meta
	lua_pop(L, 1); // Stack: clibdocker
	
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