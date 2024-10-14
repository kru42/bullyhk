#include "stdafx.h"

static const wchar_t* sg_processName = L"Bully.exe";

static void* ImportScript_fptr   = (void*)0x005d8c40;
static void* ImportScript_fptr_o = nullptr;

static void* PlayerSetEnableFirstPerson_fptr = (void*)0x005d1c40;
typedef void(__stdcall* PlayerSetEnableFirstPerson_t)(bool enable);

static void* PlayerSetPosXYZ_fptr = (void*)0x005d13e0;
typedef float(__stdcall* PlayerSetPosXYZ_t)(void* arg1);

static void* LuaScript_Update_fptr   = (void*)0x005daac0;
static void* LuaScript_Update_fptr_o = nullptr;

static void* DebugLog_fptr   = (void*)0x0043c4f0;
static void* DebugLog_fptr_o = nullptr;

static void* DebugLog2_fptr   = (void*)0x0049a650;
static void* DebugLog2_fptr_o = nullptr;

static void* CreateLuaState_fptr   = (void*)0x0005db3c0;
static void* CreateLuaState_fptr_o = nullptr;

static void* luaL_loadfile_fptr   = (void*)0x0073c660;

static void* luaL_newstate_fptr   = (void*)0x00742010;
static void* luaL_newstate_fptr_o = nullptr;

static int* g_lua_state = nullptr;

int *__cdecl FUN_00742010()
{
    g_lua_state = ((int *(__cdecl*)())luaL_newstate_fptr_o)();
    printf(__FUNCTION__ " called. returning g_lua_state: %p\n", g_lua_state);
    return g_lua_state;
}

void FUN_0043c4f0(char* param_1, char* param_2, char* param_3)
{
    printf(__FUNCTION__ " called with param_1: %s, param_2: %s, param_3: %s\n", param_1, param_2, param_3);

    ((void (*)(char*, char*, char*))DebugLog_fptr_o)(param_1, param_2, param_3);
}

void FUN_005d8c40(uint32_t param_1, char* param_2)
{
    printf(__FUNCTION__ " called with param_1: %d, param_2: %s\n", param_1, param_2);

    ((void (*)(uint32_t, char*))ImportScript_fptr_o)(param_1, param_2);
}

void __fastcall FUN_005db3c0(void* this_ptr, void* edx)
{
    printf(__FUNCTION__ " called with this_ptr: %p\n", this_ptr);

    ((void(__thiscall*)(void*))CreateLuaState_fptr_o)(this_ptr);
}

static const char* g_fmt   = "%s\n";
static char*       str_ptr = nullptr;

#include <stdbool.h>
#include <ctype.h>
#include <string.h>

bool is_human_readable(const char* str)
{
    if (str == NULL)
    {
        return false;
    }

    int length = strlen(str);
    if (length == 0)
    {
        return false; // Empty string
    }

    int printable_count           = 0;
    int alpha_count               = 0;
    int digit_count               = 0;
    int space_count               = 0;
    int consecutive_non_printable = 0;

    for (int i = 0; i < length; i++)
    {
        if (str[i] < 0 || str[i] > 127)
        {
            // Handle non-ASCII characters
            continue;
        }
        if (isprint(str[i]))
        {
            printable_count++;
            consecutive_non_printable = 0;

            if (isalpha(str[i]))
            {
                alpha_count++;
            }
            else if (isdigit(str[i]))
            {
                digit_count++;
            }
            else if (isspace(str[i]))
            {
                space_count++;
            }
        }
        else
        {
            consecutive_non_printable++;
            if (consecutive_non_printable > 2)
            {
                return false; // Too many consecutive non-printable characters
            }
        }
    }

    // Calculate percentages
    float printable_percent = (float)printable_count / length;
    float alpha_percent     = (float)alpha_count / length;
    float digit_percent     = (float)digit_count / length;

    // Criteria for human-readable text
    return (printable_percent > 0.9) && // At least 90% printable characters
           (alpha_percent > 0.1) &&     // At least 10% alphabetic characters
           (digit_percent < 0.5) &&     // Less than 50% digits
           (space_count > 0);           // At least one space
}

// void __declspec(naked) FUN_0049a650()
//{
//     __asm {
//         push ebp
//		mov ebp, esp
//		sub esp, 0x10
//
//         // Check if it's a valid string address (within 4gb virtual memory space win32)
//		mov eax, [ebp + 0x8]
//         cmp eax, 0x00401000
//         jb invalid_string
//             // cmp eax, 0x020ebcf7
//         cmp eax, 0x009c3fff
//         ja invalid_string
//
//         mov str_ptr, eax
//         //      push eax
//         // push g_fmt
//         // call printf
//         //      add esp, 0x8
//     }
//
//     if (is_human_readable(str_ptr))
//     {
//         printf(__FUNCTION__ " called with param_1: %s\n", str_ptr);
//     }
//
//     __asm
//     {
//     invalid_string:
//         add esp, 0x14
//         mov esp, ebp
//         pop ebp
//         jmp DebugLog2_fptr_o
//     }
// }

// C function to check the string and print if it's human-readable
void check_and_print(const char* format)
{
    if (is_human_readable(format))
    {
        // Get the arguments after the format string
        va_list args;
        va_start(args, format);

        // Print our debug message
        printf("FUN_0049a650 called with format: ");
        vprintf(format, args);
        printf("\n");

        va_end(args);
    }
}

__declspec(naked) void FUN_0049a650()
{
    __asm {
        push ebp
        mov ebp, esp
        sub esp, 0x100 // Allocate more stack space for local use

        // Save all registers
        pushad

            // Check if the format string pointer is valid
        mov eax, [ebp + 8] // First argument (format string)
        cmp eax, 0x00401000
        jb invalid_string
        cmp eax, 0x009c3fff
        ja invalid_string

            // If valid, call our C function
        push eax
        call check_and_print
        add esp, 4
        
    invalid_string:
            // Restore all registers
        popad

                // Clean up our stack frame
        mov esp, ebp
        pop ebp

            // Jump to the original function
        jmp DebugLog2_fptr_o
    }
}

void __fastcall FUN_005daac0(void* this_ptr, void* edx, char param)
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
            ((int(__cdecl*)(void*, const char*))luaL_loadfile_fptr)(g_lua_state, "bullyhk\\test.lua");
        }
    }

    ((void(__thiscall*)(void*, char))LuaScript_Update_fptr_o)(this_ptr, param);
}

void install_hook(void* fptr, void* hook, void** original)
{
    if (MH_CreateHook(fptr, hook, original) != MH_OK)
    {
        std::cout << "Failed to create hook." << std::endl;
    }
    else
    {
        std::cout << "Hook created." << std::endl;
    }

    if (MH_EnableHook(fptr) != MH_OK)
    {
        std::cout << "Failed to enable hook." << std::endl;
    }
    else
    {
        std::cout << "Hook enabled." << std::endl;
    }
}

void hook_game()
{
    install_hook(LuaScript_Update_fptr, &FUN_005daac0, &LuaScript_Update_fptr_o);
    install_hook(luaL_newstate_fptr, &FUN_00742010, &luaL_newstate_fptr_o);
    // install_hook(DebugLog_fptr, &FUN_0043c4f0, &DebugLog_fptr_o);
    // install_hook(DebugLog2_fptr, &FUN_0049a650, &DebugLog2_fptr_o);
}

void PrintProcessStats(DWORD processID)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (hProcess == NULL)
    {
        std::cout << "Could not open process." << std::endl;
        return;
    }
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        std::cout << "Process ID: " << processID << std::endl;
        std::cout << "Working Set Size: " << pmc.WorkingSetSize / 1024 << " KB" << std::endl;
        std::cout << "Page File Usage: " << pmc.PagefileUsage / 1024 << " KB" << std::endl;
    }
    else
    {
        std::cout << "Could not get process memory info." << std::endl;
    }
    CloseHandle(hProcess);
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

    // Initialize MinHook
    if (MH_Initialize() != MH_OK)
    {
        std::cout << "Failed to initialize MinHook." << std::endl;
        return 0;
    }

    hook_game();

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