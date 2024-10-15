#include "stdafx.h"
#include "lua_hook.h"
#include "hooking.h"

static const wchar_t* sg_processName = L"Bully.exe";

static void* LuaScript_Update_fptr   = (void*)0x005daac0;
static void* LuaScript_Update_fptr_o = nullptr;

// LuaScript_Update
void __fastcall FUN_005daac0_hk(void* this_ptr, void* edx, char param)
{
    // If the R key is pressed, enable first person
    if (GetAsyncKeyState('R') & 1)
    {
        if (lhk::g_lua_state != nullptr)
        {
            // read the lua script source
            std::ifstream file("E:\\bullyhk\\injectme.lua");

            // read the file into a string
            std::string script((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            // use our loader to load the script, with the game's lua state
            int result = lhk::luaL_loadbuffer_custom(script.c_str());
        }
    }

    ((void(__thiscall*)(void*, char))LuaScript_Update_fptr_o)(this_ptr, param);
}

static DWORD thread_main(LPVOID lpParam)
{
    // Allocate console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONIN$", "r", stdin);

    SetConsoleTitleA("Your DLL Console");

    // Make the console stay open and not steal focus
    HWND consoleWindow = GetConsoleWindow();
    SetWindowPos(consoleWindow, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    ShowWindow(consoleWindow, SW_SHOW);

    auto game_module = GetModuleHandleW(sg_processName);
    if (game_module == nullptr)
    {
        std::cout << "Could not find game module." << std::endl;
        return 0;
    }

    // install lua hooks
    lhk::install_lua_hooks();

    // install game loop hook
    install_hook(LuaScript_Update_fptr, &FUN_005daac0_hk, &LuaScript_Update_fptr_o);

    DWORD processID = GetCurrentProcessId();
    std::cout << "Current Process ID: " << processID << std::endl;

    while (true)
    {
        // PrintProcessStats(processID);
        Sleep(5000); // Update every 5 seconds
    }

    return 1;
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD fdwReason, LPVOID lpReserved)
{
    auto game_module = GetModuleHandle(sg_processName);
    if (game_module == nullptr)
    {
        return TRUE;
    }

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hInstance);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&thread_main, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        // Uninitialize MinHook
        MH_Uninitialize();
        break;
    default:
        break;
    }

    return TRUE;
}