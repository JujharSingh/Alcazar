#pragma once

extern "C" {
#include "lua-5.1.5/lua.h"
#include "lua-5.1.5/lauxlib.h"
#include "lua-5.1.5/lualib.h"
}

#include <iostream>

#define peepee

int Init();

extern DWORD RbxState;


