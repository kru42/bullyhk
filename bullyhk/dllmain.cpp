#include "stdafx.h"
#include "lua_hook.h"
#include "dx9_hook.h"
#include "hooking.h"

static const wchar_t* sg_processName = L"Bully.exe";

static void* LuaScript_Update_fptr   = (void*)0x005daac0;
static void* LuaScript_Update_fptr_o = nullptr;

// LuaScript_Update
void __fastcall FUN_005daac0_hk(void* this_ptr, void* edx, char param)
{
    //// If the F1 key is pressed, enable first person
    // if (GetAsyncKeyState(VK_F1) & 1)
    //{
    //     if (lhk::get_lua_state() != nullptr)
    //     {
    //         // use our loader to load the script, with the game's lua state
    //         int result = lhk::lua_exec_filename("E:\\bullyhk\\injectme.lua");
    //     }
    // }

    // if (GetAsyncKeyState(VK_F2) & 1)
    //{
    //     toggle_imgui_capture();
    // }

    ((void(__thiscall*)(void*, char))LuaScript_Update_fptr_o)(this_ptr, param);
}

static HWND(WINAPI* CreateWindowExA_o)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X,
                                       int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                       HINSTANCE hInstance, LPVOID lpParam);

// CreateWindowExA hook
HWND __stdcall CreateWindowExA_hk(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
                                  int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                  LPVOID lpParam)
{
    printf("CreateWindowExA called with window name: %s, class name: %s\n", lpWindowName, lpClassName);
    HWND hWnd = CreateWindowExA_o(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent,
                                  hMenu, hInstance, lpParam);

    //if (lpWindowName != NULL && strcmp(lpWindowName, "Bully") == 0)
    if (lpWindowName == NULL) // game's main window WindowName is NULL
    {
        install_dx9_hooks(hWnd);

        // install lua hooks
        lhk::install_lua_hooks();

        // install game loop hook
        install_hook(LuaScript_Update_fptr, &FUN_005daac0_hk, &LuaScript_Update_fptr_o);
    }
    else
    {
        install_win32_hooks(hWnd);
    }

    return hWnd;
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

    install_hook(CreateWindowExA, &CreateWindowExA_hk, (void**)&CreateWindowExA_o);

    DWORD processID = GetCurrentProcessId();
    std::cout << "Current Process ID: " << processID << std::endl;

    while (true)
    {
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            if (lhk::get_lua_state() != nullptr)
            {
                // use our loader to load the script, with the game's lua state
                int result = lhk::lua_exec_filename("E:\\bullyhk\\injectme.lua");
            }
        }
        else if (GetAsyncKeyState(VK_F2) & 1)
        {
            printf("F2 pressed, toggling imgui capture\n");
            toggle_imgui_capture();
        }
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