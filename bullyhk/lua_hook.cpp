#include "stdafx.h"
#include "lua_hook.h"
#include "hooking.h"

static luaL_newstate_t luaL_newstate_fptr   = (luaL_newstate_t)0x00742010;
static luaL_newstate_t luaL_newstate_fptr_o = nullptr;

void*           lua_tostring       = (void*)0x0073b2b0;
luaL_loadfile_t luaL_loadfile_fptr = (luaL_loadfile_t)0x0073c660;

void* g_lua_state = nullptr;

void* __cdecl FUN_00742010()
{
    // update lua state pointer
    g_lua_state = luaL_newstate_fptr_o();

    printf(__FUNCTION__ " called. game_lua_state updated: %p\n", g_lua_state);
    return g_lua_state;
}

void install_lua_hooks()
{
    install_hook(luaL_newstate_fptr, &FUN_00742010, (void**)&luaL_newstate_fptr_o);
}
