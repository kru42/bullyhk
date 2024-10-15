#include "stdafx.h"
#include "lua_hook.h"
#include "hooking.h"

namespace lhk
{
static luaL_newstate_t luaL_newstate_fptr   = (luaL_newstate_t)0x00742010;
static luaL_newstate_t luaL_newstate_fptr_o = nullptr;

luaL_loadbuffer_t        luaL_loadbuffer_fptr   = (luaL_loadbuffer_t)0x0073c7f0;
static luaL_loadbuffer_t luaL_loadbuffer_fptr_o = nullptr;

lua_tostring_t  game_lua_tostring       = (lua_tostring_t)0x0073b2b0;
luaL_loadfile_t game_luaL_loadfile_fptr = (luaL_loadfile_t)0x0073c660;
lua_pcall_t     game_lua_pcall     = (lua_pcall_t)0x0073ba70;

lua_State* g_lua_state = nullptr;

void* __cdecl FUN_00742010()
{
    // update lua state pointer
    g_lua_state = luaL_newstate_fptr_o();

    printf(__FUNCTION__ " called. game_lua_state updated: %p\n", g_lua_state);
    return g_lua_state;
}

// Function to write the dumped bytecode to a file
static int writer(lua_State* L, const void* p, size_t size, void* ud)
{
    FILE* file = (FILE*)ud;
    fwrite(p, size, 1, file);
    return 0; // 0 indicates success
}

int luaL_loadbuffer_custom(const char* buff)
{
    // calc buffer size
    size_t size = strlen(buff);

    FILE* file = fopen("E:\\bullyhk\\custom_dummy.out", "wb");
    if (file == nullptr)
    {
        printf("Failed to open file for writing.\n");
        return -1;
    }

    // load the script with our loader
    printf("compiling source: %s\n", buff);

    // create a dummy lua state
    auto ls_dummy = luaL_newstate_fptr_o();
    int result = luaL_loadbuffer(ls_dummy, buff, size, "custom");
    if (result != 0)
    {
        printf("Failed to load buffer.\n");
        return result;
    }

    // dump the bytecode to a file
    lua_dump(ls_dummy, writer, file);
    fclose(file);

    printf("Bytecode dumped to file.\n");

    // load the dumped bytecode
    game_luaL_loadfile_fptr(g_lua_state, "E:\\bullyhk\\custom_dummy.out");

    // remove the file
    remove("E:\\bullyhk\\custom_dummy.out");

    // call the script
    printf("calling lua_pcall\n");
    result = game_lua_pcall(g_lua_state, 0, 0, 0);
    if (result != 0)
    {
        printf("Failed to call script: %s\n", game_lua_tostring(g_lua_state, -1));

        // pop the error message
        lua_pop(g_lua_state, 1);
    }

    return 0;
}

void install_lua_hooks()
{
    install_hook(luaL_newstate_fptr, &FUN_00742010, (void**)&luaL_newstate_fptr_o);
}

} // namespace lhk
