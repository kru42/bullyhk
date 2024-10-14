#pragma once

typedef int*(__cdecl* luaL_newstate_t)();
typedef int(__cdecl* luaL_loadfile_t)(void* L, const char* filename);
typedef const char*(__cdecl* lua_tostring_t)(void* L, int index);
typedef int(__cdecl* lua_pcall_t)(void* L, int nargs, int nresults, int errfunc);

void install_lua_hooks();

extern void* g_lua_state;

extern luaL_loadfile_t luaL_loadfile_fptr;
extern lua_tostring_t  lua_tostring;
extern lua_pcall_t     lua_pcall;
