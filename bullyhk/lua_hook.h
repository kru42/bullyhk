#pragma once

typedef int*(__cdecl* luaL_newstate_t)();
typedef int(__cdecl* luaL_loadfile_t)(void* L, const char* filename);

void install_lua_hooks();

extern luaL_loadfile_t luaL_loadfile_fptr;
extern void*           g_lua_state;
