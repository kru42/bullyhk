#include "stdafx.h"

static bool initialized = false;

void init()
{
    // Initialize MinHook
    if (MH_Initialize() != MH_OK)
    {
        std::cout << "Failed to initialize MinHook." << std::endl;
        exit(-1);
    }

    initialized = true;
}

void install_hook(void* fptr, void* hook, void** original)
{
    if (!initialized)
    {
        init();
    }

    if (MH_CreateHook(fptr, hook, original) != MH_OK)
    {
        std::cout << "Failed to create hook." << std::endl;
        exit(-1);
    }

    if (MH_EnableHook(fptr) != MH_OK)
    {
        std::cout << "Failed to enable hook." << std::endl;
        exit(-1);
    }
}
