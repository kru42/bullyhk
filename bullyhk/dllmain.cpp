#include "stdafx.h"
#include "lua_hook.h"
#include "hooking.h"

static const wchar_t* sg_processName = L"Bully.exe";

// static void* ImportScript_fptr   = (void*)0x005d8c40;
// static void* ImportScript_fptr_o = nullptr;
//
// static void* PlayerSetEnableFirstPerson_fptr = (void*)0x005d1c40;
// typedef void(__stdcall* PlayerSetEnableFirstPerson_t)(bool enable);
//
// static void* PlayerSetPosXYZ_fptr = (void*)0x005d13e0;
// typedef float(__stdcall* PlayerSetPosXYZ_t)(void* arg1);
//
//
// static void* DebugLog_fptr   = (void*)0x0043c4f0;
// static void* DebugLog_fptr_o = nullptr;
//
// static void* DebugLog2_fptr   = (void*)0x0049a650;
// static void* DebugLog2_fptr_o = nullptr;
//
// static void* CreateLuaState_fptr   = (void*)0x0005db3c0;
// static void* CreateLuaState_fptr_o = nullptr;

static void* LuaScript_Update_fptr   = (void*)0x005daac0;
static void* LuaScript_Update_fptr_o = nullptr;

// LuaScript_Update
void __fastcall FUN_005daac0_hk(void* this_ptr, void* edx, char param)
{
    // If the R key is pressed, enable first person
    if (GetAsyncKeyState('R') & 1)
    {
        printf("R key pressed\n");
        printf("this_ptr: %p\n", this_ptr);
        // if (g_thing != nullptr)
        //{
        //     // printf("calling PlayerSetEnableFirstPerson\n");
        //     //((PlayerSetEnableFirstPerson_t)PlayerSetEnableFirstPerson_fptr)(g_thing);
        //     printf("calling PlayerSetPosXYZ\n");
        //     ((PlayerSetPosXYZ_t)PlayerSetPosXYZ_fptr)(g_thing);
        // }

        // call loadfile

        if (g_lua_state != nullptr)
        {
            printf("calling luaL_loadfile\n");
            int result = luaL_loadfile_fptr(g_lua_state, "E:\\bullyhk\\test.lua");
            printf("luaL_loadfile result: %d\n", result);
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
    install_lua_hooks();

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