#pragma once

extern "C" { //ya its c tell the linker
#include "lua-5.1.5/lua.h"
#include "lua-5.1.5/lapi.h"
#include "lua-5.1.5/lauxlib.h"
#include "lua-5.1.5/lbaselib.h"
#include "lua-5.1.5/lualib.h"
#include "lua-5.1.5/lobject.h"
} //include lua libs 

#include <map>
#include <functional>

int RobloxGlobalIndex(lua_State *L);
int RobloxIndex(lua_State *L);
int RobloxNewIndex(lua_State *L);
//int RobloxCall(lua_State *L);

int wrappedMM(lua_State *L, const char *mm);

/*disgusting temporary global funcs l o l
int instancenew(lua_State *L);
int print(lua_State *L);
int printidentity(lua_State *L);*/

#define TO_RBX		0
#define FROM_RBX	1