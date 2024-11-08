#include "stdafx.h"
#include "lua_hook.h"
#include "hooking.h"

namespace lhk
{
static luaL_newstate_t luaL_newstate_fptr   = (luaL_newstate_t)0x00742010;
static luaL_newstate_t luaL_newstate_fptr_o = nullptr;

luaL_loadbuffer_t        luaL_loadbuffer_fptr   = (luaL_loadbuffer_t)0x0073c7f0;
static luaL_loadbuffer_t luaL_loadbuffer_fptr_o = nullptr;

lua_tostring_t         game_lua_tostring       = (lua_tostring_t)0x0073b2b0;
static luaL_loadfile_t game_luaL_loadfile_fptr = (luaL_loadfile_t)0x0073c660;
static lua_pcall_t     game_lua_pcall          = (lua_pcall_t)0x0073ba70;

using lua_print_internal_t = void*(__cdecl*)(void* L);

static lua_print_internal_t lua_print_internal   = (lua_print_internal_t)0x00824ec0;
static lua_print_internal_t lua_print_internal_o = nullptr;

static lua_State* g_lua_state = nullptr;

void* __cdecl lua_print_internal_hk(void* L)
{
    if (g_lua_state == L && L != nullptr && (uintptr_t)L > 0x10000)
    {
        printf("lua_print_internal_hk called with: %p\n", L);

        auto state     = (lua_State*)L;
        int nargs = lua_gettop(state); // Number of arguments on the stack

        std::cout << "Function called with " << nargs << " arguments" << std::endl;

        for (int i = 1; i <= nargs; i++)
        {
            if (lua_isstring(state, i))
            {
                // If it's a string, print it
                std::cout << "Arg " << i << ": " << lua_tostring(state, i) << std::endl;
            }
            else if (lua_isnumber(state, i))
            {
                // If it's a number, print it
                std::cout << "Arg " << i << ": " << lua_tonumber(state, i) << std::endl;
            }
            else if (lua_isboolean(state, i))
            {
                // If it's a boolean, print it
                std::cout << "Arg " << i << ": " << (lua_toboolean(state, i) ? "true" : "false") << std::endl;
            }
            else
            {
                // Other types can be handled similarly (tables, functions, etc.)
                std::cout << "Arg " << i << ": (other type)" << std::endl;
            }
        }
    }
    return lua_print_internal_o(L);
}

lua_State* lhk::get_lua_state()
{
    return g_lua_state;
}

void* __cdecl luaL_newstate_hk()
{
    // update lua state pointer
    g_lua_state = luaL_newstate_fptr_o();

    printf(__FUNCTION__ " called. game_lua_state updated: %p\n", g_lua_state);
    return g_lua_state;
}

int lua_exec_source(const char* buff)
{
    // calc buffer size
    size_t size = strlen(buff);

    // use our parser to load the script into the game's lua state
    int result = luaL_loadbuffer(g_lua_state, buff, size, "custom");
    if (result != 0)
    {
        printf("our luaL_loadbuffer failed: %s\n", lua_tostring(g_lua_state, -1));
        return result;
    }

    // call the script
    printf("calling game lua_pcall\n");
    result = lua_pcall(g_lua_state, 0, 0, 0);
    if (result != 0)
    {
        printf("lua_pcall failed: %s\n", lua_tostring(g_lua_state, -1));
    }

    return 0;
}

int lua_exec_filename(const char* filename)
{
    int result = luaL_loadfile(g_lua_state, filename);
    if (result != 0)
    {
        printf("our luaL_loadfile failed: %s\n", lua_tostring(g_lua_state, -1));
        return result;
    }

    // call the script
    printf("calling game lua_pcall\n");
    // result = game_lua_pcall(g_lua_state, 0, 0, 0);
    result = lua_pcall(g_lua_state, 0, 0, 0);
    if (result != 0)
    {
        printf("lua_pcall failed: %s\n", lua_tostring(g_lua_state, -1));
    }

    return 0;
}

void install_lua_hooks()
{
    install_hook(luaL_newstate_fptr, &luaL_newstate_hk, (void**)&luaL_newstate_fptr_o);

    install_hook(lua_print_internal, &lua_print_internal_hk, (void**)&lua_print_internal_o);
}

} // namespace lhk
