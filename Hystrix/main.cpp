#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>

#include "stdafx.h"
#include "main.h"
#include "functions.h"
#include "memory.h"
#include "metamethods.h"

DWORD sc, RbxState;

static int traceback(lua_State *L) {
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 2);
	lua_call(L, 2, 1);
	fprintf(stderr, "%s\n", lua_tostring(L, -1));
	return 1;
}


int Init() {
	Memory::write(FreeConsole, "\xC3", 1);

	lua_State* HystrixState = luaL_newstate();

	MessageBoxA(NULL, "Created Hystrix lua state!", "Created Hystrix lua state!", MB_OK);

	sc = getrbxsc();

	if (sc == NULL) {
		MessageBoxA(NULL, "fuck u", "fuck u", MB_OK);
	}
	else {
		MessageBoxA(NULL, "got scriptcock", "got scriptcock", MB_OK);
	}

	RbxState = getrbxls(sc);

	MessageBoxA(NULL, "Got rococks lua state!", "Got rococks lua state!", MB_OK);

	hystrix_setup(HystrixState);

	MessageBoxA(NULL, "Set up Hystrix metatables!", "Set up Hystrix metatables!", MB_OK);

	lua_settop(HystrixState, 0);

	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	SetConsoleTitle(L"Alcazar");

	/*rbxgetglobal(RbxState, "game");
	rbxgetfield(RbxState, -1, "PlaceId");
	lua_pushnumber(HystrixState, rbxtonumber(RbxState, -1));
	DWORD rbxAddr = rbxindex2adr(RbxState, -1)[1];
	TValue* curr = index2adr(HystrixState, -1);
	curr->rbxaddr = rbxAddr;
	rbxsettop(RbxState, 0);
	rbxgetglobal(RbxState, "print");
	rbxpushrealobject(RbxState, curr);
	rbxpcall(RbxState, 1, 0);
	lua_settop(HystrixState, 0);*/

	printf("Execute Lua here:\r\n");
	do {
		printf("> ");
		std::string s;
		std::getline(std::cin, s);
		printf("Executed: %s \r\n", s.c_str());
		printf("Output (if any): ");
		if (luaL_dostring(HystrixState, s.c_str()) == 0) {
			printf("\r\n");
			rbxsettop(RbxState, 0);
			lua_settop(HystrixState, 0);
		}
		else {
			fprintf(stderr, "\r\n%s\r\n", lua_tostring(HystrixState, -1));
			rbxsettop(RbxState, 0);
			lua_settop(HystrixState, 0);
		}
	}while (true);

	return 1;
}