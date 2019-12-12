#include "lua_docker.h"

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

int DockerClient_container_list(lua_State* L) {
	// Expected: stack = [self]
	DockerClient* dc = (DockerClient*)luaL_checkudata(L, 1, DockerClient_metatable);

	docker_ctr_list* ctrls;
	d_err_t err = docker_container_list(dc->ctx, &ctrls, 1, 0, 1);
	if (err != E_SUCCESS) {
		luaL_error(L, "Error listing containers");
	}
	const char* ctrls_json = get_json_string(ctrls);
	lua_pushstring(L, ctrls_json);
	return 1;
}

int luaopen_clibdocker(lua_State* L) {
	// The user may pass in values here,
	// but we'll ignore those values.
	lua_settop(L, 0);
	// stack = []

	// If this metatable already exists, the library is already
	// loaded.
	if (luaL_newmetatable(L, DockerClient_metatable)) {
		// stack = [mt]

		static struct luaL_Reg metamethods[] = {
			{"container_ls", DockerClient_container_list},
			{NULL, NULL}
		};
		luaL_setfuncs(L, metamethods, 0);
		lua_pop(L, 1);  // The table is saved in the Lua's registry.
		// stack = []
	}

	static struct luaL_Reg fns[] = {
	  {"new", DockerClient_new},
	  {NULL, NULL}
	};

	luaL_newlib(L, fns);  // Push a new table with fns key/vals.
	// stack = [DockerClient = {new = new}]

	return 1;  // Return the top item, the DockerClient table.
}