#include "stdafx.h"

namespace utils
{
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

//// C function to check the string and print if it's human-readable
// void check_and_print(const char* format)
//{
//     if (is_human_readable(format))
//     {
//         // Get the arguments after the format string
//         va_list args;
//         va_start(args, format);
//
//         // Print our debug message
//         printf("FUN_0049a650 called with format: ");
//         vprintf(format, args);
//         printf("\n");
//
//         va_end(args);
//     }
// }

} // namespace utils

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
