#pragma once

#include "stdafx.h"
#include "functions.h"
#include "metamethods.h"
#include "memory.h"
#include "main.h"


extern "C" { //ya its c tell the linker
#include "lua-5.1.5/lua.h"
#include "lua-5.1.5/lauxlib.h"
#include "lua-5.1.5/lualib.h"
} //include lua libs 

DWORD unprotect(DWORD addr)
{
	BYTE* tAddr = (BYTE *)addr;

	do {
		tAddr += 0x10;
	} while (!(tAddr[0] == 0x55 && tAddr[1] == 0x8B && tAddr[2] == 0xEC));

	DWORD funcSz = tAddr - (BYTE*)addr;

	PVOID nFunc = VirtualAlloc(NULL, funcSz, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (nFunc == NULL)
		return addr;

	memcpy(nFunc, (void*)addr, funcSz);

	DWORD pos = (DWORD)nFunc;
	BOOL valid = false;
	do {
		if (*(BYTE*)pos == 0x72 && *(BYTE*)(pos + 0x2) == 0xA1 && (*(BYTE*)(pos + 0x7)) == 0x8B) {
			memcpy((void*)pos, "\xEB", 1);

			DWORD cNFunc = (DWORD)nFunc;
			do {
				if (*(BYTE*)cNFunc == 0xE8)
				{
					DWORD tFunc = addr + (cNFunc - (DWORD)nFunc);
					DWORD oFunc = (tFunc + *(DWORD*)(tFunc + 1)) + 5;

					if (oFunc % 16 == 0)
					{
						DWORD realCAddr = oFunc - cNFunc - 5;
						*(DWORD*)(cNFunc + 1) = realCAddr;
					}
					cNFunc += 5;
				}
				else
					cNFunc += 1;
			} while (cNFunc - (DWORD)nFunc < funcSz);

			valid = true;
		}
		pos += 1;
	} while (pos < (DWORD)nFunc + funcSz);

	if (!valid) {
		VirtualFree(nFunc, funcSz, MEM_RELEASE);
		return addr;
	}

	return (DWORD)nFunc;
}

int rbxgettop(DWORD L) {
	return (int)*(DWORD*)(L + L_top) - *(DWORD *)(L + L_base) >> 4;
}

void rbxsettop(DWORD L, int idx) {
	if (idx >= 0) {
		*(DWORD *)(L + L_top) += sizeof(r_TValue) * idx + sizeof(r_TValue);
	}
	else {
		int i;
		for (i = sizeof(r_TValue) * idx; *(DWORD *)(L + L_top) < (unsigned int)(i + *(DWORD *)(L + L_base)); *(DWORD *)(L + L_top) += sizeof(r_TValue))
			*(DWORD *)(*(DWORD *)(L + L_top) + 8) = 0;
		*(DWORD *)(L + L_top) = i + *(DWORD *)(L + L_base);
	}
}

void rbxpushnil(DWORD L) {
	*(DWORD *)(*(DWORD *)(L + L_top) + 8) = 0;
	*(DWORD *)(L + L_top) += sizeof(r_TValue);
}

/*typedef int(*rluapushnumber) (int L, double n);
rluapushnumber rbx_pushnumber = (rluapushnumber)unprotect(offset(pushnumber));

void rbxpushnumber(DWORD L, double n)
{
	rbx_pushnumber(L, n);
}*/

void rbxpushnumber(DWORD L, double s) {
	DWORD v2 = *(DWORD*)(L + L_top);
	__m128d lol = _mm_load_pd((double*)offset(xorconst));
	__m128d lol2 = _mm_load_pd(&s);
	double result = 0;
	_mm_store_pd(&result, _mm_xor_pd(lol, lol2));
	*(double*)v2 = result;
	*(DWORD*)(v2 + 8) = 2;
	*(DWORD*)(L + L_top) += sizeof(r_TValue);
}

void rbxpushboolean(DWORD L, int b) {
	int v2 = *(DWORD *)(L + L_top);
	*(DWORD *)v2 = b != 0;
	*(DWORD *)(v2 + 8) = 3;
	*(DWORD *)(L + L_top) += 16;
}

DWORD* rbxindex2adr(DWORD L, int idx) {
	if (idx <= 0) {
		if (idx <= -10000) {
			return (DWORD*)(16 * (-10002 - idx) + 8);
		}
		else {
			return (DWORD*)(*(DWORD*)(L + L_top) + 16 * idx);
		}
	}
	else {
		if ((unsigned int)(16 * idx + *(DWORD*)(L + L_base) - 16) < *(DWORD*)(L + L_top))
			return (DWORD*)(16 * idx + *(DWORD*)(L + L_base) - 16);
	}
}

void rbxpushvalue(DWORD L, int idx) {
	DWORD* v2 = 0;
	v2 = rbxindex2adr(L, idx);
	DWORD v3 = *(DWORD*)(L + L_top);
	*(DWORD*)v3 = *(DWORD*)v2;
	*(DWORD*)(v3 + 4) = v2[1];
	*(DWORD*)(v3 + 8) = v2[2];
	*(DWORD*)(L + L_top) += sizeof(r_TValue);
}

int rbxtype(DWORD L, int idx) {
	return rbxindex2adr(L, idx)[2];
}

int hysprint(lua_State* L) {
	printf("\r\n %s", luaL_checkstring(L, -1));
	return 0;
}

//make datamodelon stack then get global in it
void hystrix_setup(lua_State *L){
	
	int *flag = (int*)offset(identity);
	*flag = 6;

	lua_register(L, "HystrixPrint", hysprint);

	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_newtable(L);
/*	lua_pushcfunction(L, RobloxNewIndex);
	lua_setfield(L, -2, "__newindex");*/
	lua_pushcfunction(L, RobloxGlobalIndex);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
	lua_settop(L, 0);
}

DWORD getrbxsc() {
	DWORD* sc = (DWORD*)offset(scriptcontext);
	return Memory::scan((BYTE*)&sc, (BYTE*)"xxxx", PAGE_READWRITE);
}

typedef int(*rluanewthread) (int L);
rluanewthread rbxnewthread = (rluanewthread)unprotect(offset(newthread));

DWORD getrbxls(DWORD sc) {
	DWORD v41 = sc;
	DWORD v49 = 0;

	DWORD v50 = v41 + 56 * v49 + 164 + *(DWORD *)(v41 + 56 * v49 + 164);
	return rbxnewthread(v50);
}

void insert(lua_State *L, int tbl_idx, int val_idx) {
	int e;
	for (e = luaL_getn(L, tbl_idx) + 1; e > 1; e--) {
		lua_rawgeti(L, tbl_idx, e - 1);
		lua_rawseti(L, tbl_idx, e);
	}
	lua_pushvalue(L, val_idx);
	lua_rawseti(L, tbl_idx, 1);
}

typedef void(*rluagetfield) (int L, int idx, const char *k);

typedef int(*rluagetmetatable) (int L, int objindex);

rluagetmetatable rbx_getmetatable = (rluagetmetatable)unprotect(offset(getmetatable));

int rbxgetmetatable(int L, int objindex) {
	return rbx_getmetatable(L, objindex);
}

rluagetfield rbx_getfield = (rluagetfield)unprotect(offset(getfield));

void rbxgetfield(int L, int idx, const char *k) {
	rbx_getfield(L, idx, k);
}

typedef void(*rluasetfield) (int L, int idx, const char *k);
rluasetfield rbx_setfield = (rluagetfield)unprotect(offset(setfield));

void rbxsetfield(int L, int idx, const char *k) {
	rbx_setfield(L, idx, k);
}

typedef int(*rluapcall) (int L, int nargs, int nresults, int errfunc);
rluapcall rbx_pcall = (rluapcall)unprotect(offset(pcall));

int rbxpcall(int L, int nargs, int nresults, int errfunc) {
	Memory::write((void*)offset(penvcheck), "\x75", 1);
	int status = rbx_pcall(L, nargs, nresults, errfunc);
	Memory::write((void*)offset(penvcheck), "\x74", 1);
	return status;
}

typedef const char*(*rluatolstring) (int L, int idx, unsigned int len);
rluatolstring rbx_tolstring = (rluatolstring)unprotect(offset(tolstring));

const char* rbxtolstring(int L, int idx, unsigned int len)
{
	return rbx_tolstring(L, idx, len);
}

typedef int(*rluatoboolean) (int L, int idx);
rluatoboolean rbx_toboolean = (rluatoboolean)unprotect(offset(toboolean));

int rbxtoboolean(int L, int idx)
{
	return rbx_toboolean(L, idx);
}

typedef double(*rluatonumber) (int L, int idx);
rluatonumber rbx_tonumber = (rluatonumber)unprotect(offset(tonumber));

double rbxtonumber(int L, int idx)
{
	return rbx_tonumber(L, idx);
}

typedef void(*rluapushstring) (int L, const char* s);
rluapushstring rbx_pushstring = (rluapushstring)unprotect(offset(pushstring));

void rbxpushstring(int L, const char *s) {
	rbx_pushstring(L, s);
}

int rbxpushrealobject(int L, TValue *o) {
	if (o->rbxaddr == NULL) {
		return 1;
	}
	r_TValue *gheet = (r_TValue *)o->rbxaddr;
	r_TValue *v2 = (r_TValue *)(L + L_top);
	v2 = gheet;
	*(DWORD *)(L + L_top) += sizeof(r_TValue); //should be 16 lol
	return 0;
}

typedef void(*rluaremove) (int L, int idx);
rluaremove rbx_remove = (rluaremove)unprotect(offset(Remove));

void rbxremove(int L, int idx)
{
	rbx_remove(L, idx);
}