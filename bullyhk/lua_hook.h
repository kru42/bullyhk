#pragma once

namespace lhk
{
typedef lua_State*(__cdecl* luaL_newstate_t)();

typedef int(__cdecl* luaL_loadfile_t)(void* L, const char* filename);
typedef const char*(__cdecl* lua_tostring_t)(void* L, int index);
typedef int(__cdecl* lua_pcall_t)(void* L, int nargs, int nresults, int errfunc);
typedef int(__cdecl* luaL_loadbuffer_t)(void* L, const char* buff, size_t sz, const char* name);

void       install_lua_hooks();
int        lua_exec_source(const char* buff);
int        lua_exec_filename(const char* filename);
lua_State* get_lua_state();

} // namespace lhk
