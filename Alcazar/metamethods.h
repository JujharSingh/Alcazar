#pragma once

extern "C" {
#include "lua-5.1.5/lua.h"
#include "lua-5.1.5/lapi.h"
#include "lua-5.1.5/lauxlib.h"
#include "lua-5.1.5/lbaselib.h"
#include "lua-5.1.5/lualib.h"
#include "lua-5.1.5/lstate.h"
}

#include <map>
#include <functional>

void wrap(lua_State *L, int direction, int idx);

int RobloxGlobalIndex(lua_State *L);
int RobloxIndex(lua_State *L);
int RobloxNewIndex(lua_State *L);
int RobloxCall(lua_State *L);
int RobloxConcat(lua_State *L);
int RobloxUnm(lua_State *L);
int RobloxAdd(lua_State *L);
int RobloxSub(lua_State *L);
int RobloxMul(lua_State *L);
int RobloxDiv(lua_State *L);
int RobloxMod(lua_State *L);
int RobloxPow(lua_State *L);
int RobloxTostring(lua_State *L);
int RobloxMetatable(lua_State *L);
int RobloxEq(lua_State *L);
int RobloxLt(lua_State *L);
int RobloxLe(lua_State *L);
int RobloxMode(lua_State *L);
int RobloxLen(lua_State *L);
//int RobloxCall(lua_State *L);

int wrappedMM(lua_State *L, const char *mm);

#define TO_RBX		0
#define FROM_RBX	1