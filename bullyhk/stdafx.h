#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include <psapi.h>
#include <MinHook.h>
#include <d3d9.h>
#include <dinput.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
