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
#include "junkcode.h"
#include "aes_easy.h"
DWORD sc, RbxState;
// these are just weak ciphers we use so our real strings dont show up
/*inline std::string yes(std::string str) { // encryption
	char table[27] = { 'A', 'Z', 'K', 'R', 'T', 'C', '9', '}', '{', '_', '|', 'z', '^', '&', 'g', 'l', '@', '*', 'S', 's', 'X', 'F', '(', ')', '-', '#', '+' };
	char _table[27] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'x', 'y', 'z', ' ' };
	std::string si;
	for (char x : str) {
		for (int i = 0; i != sizeof(_table); i++) {
			junkasm; // some here...
			if (_table[i] == tolower(x)) {
				si += table[i];
			}
		}
	}
	return si;
}*/
inline std::string no(std::string str) { // decryption
	char table[27] = { 'A', 'Z', 'K', 'R', 'T', 'C', '9', '}', '{', '_', '|', 'z', '^', '&', 'g', 'l', '@', '*', 'S', 's', 'X', 'F', '(', ')', '-', '#', '+' };
	char _table[27] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'x', 'y', 'z', ' ' };
	std::string si;
	for (char x : str) {
		for (int i = 0; i != sizeof(table); i++) {
			junkasm;
			if (table[i] == x) {
				__asm { // idk lmao
					pusha
					inc eax
					dec edx
					ror eax, 8
					xor edx, eax
					mov eax, edx
					dec ebx
					nop
					cmp eax, ebx
					jmp oof
				oof:
					shl eax, 44
					jmp ty
				ty:
					mov ebx, eax
					ror ebx, 17
					popa
				}
				si += _table[i];
			}
		}
	}
	return si;
}
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
	junkasm;
	HW_PROFILE_INFOA hw;
	GetCurrentHwProfileA(&hw);
	std::cout << scrypt_hex(hw.szHwProfileGuid, no("SlZR&9F{Xl9ZRZC9lZRC|_SZz|_-RCKZ(F{lRC9Z{T_SAZ*9{}T_SAZR*C9{|lZRCF{}"), 1024, 8, 8) << std::endl;
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