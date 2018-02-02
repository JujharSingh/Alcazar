#pragma once

extern "C" { //ya its c tell the linker
#include "lua-5.1.5/lua.h"
#include "lua-5.1.5/lauxlib.h"
#include "lua-5.1.5/lualib.h"
} //include lua libs 

#include <iostream>

#define peepee

int Init();

extern DWORD RbxState;


