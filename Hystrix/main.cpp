#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>

#include "stdafx.h"
#include "main.h"
#include "functions.h"
#include "memory.h"
#include "metamethods.h"
#include "scrypt/scrypt_easy.h"

DWORD sc, RbxState;

int Init() {
	Memory::write(FreeConsole, "\xC3", 1); //Bypass rococks calling FreeConsole() in a loop by writing a retn to the first instruction

	lua_State* HystrixState = luaL_newstate();
	sc = getrbxsc();
	RbxState = getrbxls(sc);

	hystrix_setup(HystrixState);

	lua_settop(HystrixState, 0);

	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	SetConsoleTitle(L"Alcazar");

	// authing will go before the good stuff ok ? //
	// ok first we're testing scrypt with hwid
	HW_PROFILE_INFOA hw;
	GetCurrentHwProfileA(&hw);
	std::cout << scrypt_hex(hw.szHwProfileGuid, "salt to be added . . .", 1024, 8, 8) << std::endl;

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
	} while (true);

	return 1;
}