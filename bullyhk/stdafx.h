#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include <psapi.h>
#include <MinHook.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
