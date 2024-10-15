#pragma once

namespace lhk
{
typedef lua_State*(__cdecl* luaL_newstate_t)();
typedef int(__cdecl* luaL_loadfile_t)(void* L, const char* filename);
typedef const char*(__cdecl* lua_tostring_t)(void* L, int index);
typedef int(__cdecl* lua_pcall_t)(void* L, int nargs, int nresults, int errfunc);
typedef int(__cdecl* luaL_loadbuffer_t)(void* L, const char* buff, size_t sz, const char* name);

void install_lua_hooks();
int  luaL_loadbuffer_custom(const char* buff);

extern lua_State* g_lua_state;

} // namespace lhk
