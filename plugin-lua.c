#ifdef HAVE_LIBLUA5_2

/*******************************************************************************
 *
 * Lua plugin 
 *
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <alloca.h>
#include <string.h>

#include "utils.h"
#include "main.h"
#include "plugin.h"
#include "config.h"


/**
 *
 */
int proxenet_lua_load_file(plugin_t* plugin)
{
	char* filename;
	char* pathname;
	size_t pathname_len;
	lua_State* lua_interpreter;

	filename = plugin->filename;
	lua_interpreter = (lua_State*) plugin->interpreter->vm;
	
	/* load script */
	pathname_len = strlen(cfg->plugins_path) + 1 + strlen(filename) + 1;
	pathname = alloca(pathname_len + 1);
	proxenet_xzero(pathname, pathname_len + 1);
	snprintf(pathname, pathname_len, "%s/%s", cfg->plugins_path, filename);
	
	luaL_dofile(lua_interpreter, pathname);
	
	return 0;
}


/**
 *
 */
int proxenet_lua_initialize_vm(plugin_t* plugin)
{
	interpreter_t* interpreter;
	lua_State* lua_interpreter;

	interpreter = plugin->interpreter;

	if (interpreter->ready)
		return 0;
	
	lua_interpreter = luaL_newstate();
	luaL_openlibs(lua_interpreter);

	plugin->interpreter->vm = lua_interpreter;
	plugin->interpreter->ready = true;
	
	return 0;
}

/**
 *
 */
int proxenet_lua_destroy_vm(plugin_t* plugin)
{
	interpreter_t* interpreter;
	lua_State* lua_interpreter;

	if(count_plugins_by_type(_LUA_)) 
		return -1;
	
	interpreter = plugin->interpreter;
	lua_interpreter = (lua_State*)interpreter->vm;

	lua_close(lua_interpreter);
	interpreter->ready = false;
	
	return 0;
}


/**
 *
 */
char* proxenet_lua_execute_function(interpreter_t* interpreter, long rid, char* request, size_t* request_size, int type)
{
	const char *lRet;
	char *buf;
	lua_State* lua_interpreter;

	lua_interpreter = (lua_State*) interpreter->vm;
	
	if (type == REQUEST)
		lua_getglobal(lua_interpreter, CFG_REQUEST_PLUGIN_FUNCTION);
	else
		lua_getglobal(lua_interpreter, CFG_RESPONSE_PLUGIN_FUNCTION);
	
	lua_pushnumber(lua_interpreter, rid);
	lua_pushlstring(lua_interpreter, request, *request_size);
	lua_call(lua_interpreter, 2, 1);
	lRet = lua_tostring(lua_interpreter, -1);
	if (!lRet)
		return NULL;
	
	buf = proxenet_xstrdup(lRet);
	if (!buf)
		return NULL;
	
	*request_size = strlen(buf);
	return buf;
}

/**
 *
 */
void proxenet_lua_lock_vm(interpreter_t *interpreter)
{
	pthread_mutex_lock(&interpreter->mutex);
}


/**
 *
 */
void proxenet_lua_unlock_vm(interpreter_t *interpreter)
{
	pthread_mutex_unlock(&interpreter->mutex);
}

/**
 * 
 */
char* proxenet_lua_plugin(plugin_t* plugin, long rid, char* request, size_t* request_size, int type)
{
	char* buf = NULL;
	interpreter_t *interpreter = plugin->interpreter;

	proxenet_lua_lock_vm(interpreter);
	buf = proxenet_lua_execute_function(interpreter, rid, request, request_size, type);
	proxenet_lua_unlock_vm(interpreter);
	
	return buf;
}

#endif /* HAVE_LIBLUA5_2 */