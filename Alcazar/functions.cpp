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
	BYTE* tAddr = (BYTE*)addr;

	/* Calcualte the size of the function
	In theory this will run until it hits the next
	functions prolog. It assumes all calls are aligned to
	16 bytes. (grazie katie)
	*/
	do
	{
		tAddr += 16;
	} while (!(tAddr[0] == 0x55 && tAddr[1] == 0x8B && tAddr[2] == 0xEC));

	DWORD funcSz = tAddr - (BYTE*)addr;

	/* Allocate memory for the new function */
	PVOID nFunc = VirtualAlloc(NULL, funcSz, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (nFunc == NULL)
		return addr;

	/* Copy the function to the newly allocated memory */
	memcpy(nFunc, (void*)addr, funcSz);

	BYTE* pos = (BYTE*)nFunc;
	BOOL valid = false;
	do
	{
		/* Check for the return check with the sig:
        72 ?? A1 ?? ?? ?? ?? 8B
        72 ?? 8B ?? ?? ?? ?? 00
		If the sig matches replace the the jb with a jmp.
		*/
		if (pos[0] == 0x72 && (pos[2] == 0xA1 || pos[2] == 0x8B) && (pos[7] == 0x8B || pos[7] == 0x00)) {
			*(BYTE*)pos = 0xEB;

			DWORD cByte = (DWORD)nFunc;
			do
			{
				/* Check if the current byte is a call if it is,
				calculate the new relative call(s).
				*(->E8 + 1) = originalFunction - nextInstruction

				oFuncPos - Position of call in original function
				= originalFunction + (->E8 - newFunction)

				oFuncAddr - Original call location
				= oFuncPos + rel32Offset + sizeof(call)

				relativeAddr - New relative address
				= oFuncAddr - ->E8 - sizeof(call)

				Since we are not using a disassembler we assume
				that if we hit a E8 byte which is properly aligned
				it is a relative call.
				For a small amount of compensation I skip location
				of the call, since it is possible to have the byte
				E8 inside of it.
				*/
				if (*(BYTE*)cByte == 0xE8)
				{
					DWORD oFuncPos = addr + (cByte - (DWORD)nFunc);
					DWORD oFuncAddr = (oFuncPos + *(DWORD*)(oFuncPos + 1)) + 5;

					if (oFuncAddr % 16 == 0)
					{
						DWORD relativeAddr = oFuncAddr - cByte - 5;
						*(DWORD*)(cByte + 1) = relativeAddr;
						
						/* Don't check rel32 */
						cByte += 4;
					}
				}

				cByte += 1;
			} while (cByte - (DWORD)nFunc < funcSz);

			valid = true;
		}
		pos += 1;
	} while ((DWORD)pos < (DWORD)nFunc + funcSz);

	/* This function has no return check, let's not waste memory */
	if (!valid)
	{
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

void rbxpushnumber(DWORD L, double s) {
	DWORD v2 = *(DWORD*)(L + L_top);
	double* num = &s;
	double result = 0;
	double* xorc = (double*)offset(xorconst);
	__asm { // the following asm code is mega copyrighted (and made by) John. no steling pls unless ur part of alcazar dev team then go ahead lol. very copyrighted by John 100% credited
		mov eax, num
		mov ebx, xorc
		movups xmm2, xmmword ptr[eax]
		movaps xmm1, xmmword ptr[ebx]
		xorps xmm2, xmm1
		movsd result, xmm2
	}
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

/*DWORD* rbxindex2adr(DWORD L, int idx) {
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
}*/



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
	printf("\r\n %s", lua_tostring(L, -1));
	return 0;
}

void hystrix_setup(lua_State *L){
	
	int *flag = (int*)offset(identity);
	*flag = 6;

#ifdef _DEBUG
	lua_register(L, "DebugPrint", hysprint);
	lua_register(L, "StackDump", stackDump);
#endif // _DEBUG

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

DWORD getrbxls(DWORD sc) {
	DWORD v41 = sc;
	DWORD v49 = 1;

	DWORD v50 = (v41 + 56 * v49 + 164) ^ *(DWORD *)(v41 + 56 * v49 + 164);
	return v50;
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

typedef void(*rluapushcclosure) (int L, lua_CFunction f, int n);
rluapushcclosure rbx_pushcclosure = (rluapushcclosure)unprotect(offset(pushcclosure));

void rbxpushcclosure(int L, lua_CFunction f, int n)
{
	rbx_pushcclosure(L, f, n);
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

int rbxpush(int L, r_TValue *o) { //added for purposes of function handling, think of another way to do this, maybe a union or just plain function overloading
	if (o == NULL) {
		return 1;
	}
	r_TValue *v2 = (r_TValue *)(L + L_top);
	v2 = o;
	*(DWORD *)(L + L_top) += sizeof(r_TValue); //should be 16 lol
	return 0;
}

/*typedef void(*rluaremove) (int L, int idx);
rluaremove rbx_remove = (rluaremove)unprotect(offset(Remove));

void rbxremove(int L, int idx)
{
	rbx_remove(L, idx);
}*/

typedef DWORD*(*rluaindex2adr) (DWORD L, int idx);
rluaindex2adr rbx_index2adr = (rluaindex2adr)unprotect(offset(Index2adr));

DWORD * rbxindex2adr(DWORD L, int idx)
{
	return rbx_index2adr(L, idx);
}

/*void rbxrawgeti(int L, int idx, int n){
	r_TValue *obj = (r_TValue *)rbxindex2adr(L, idx)[1];
	r_Table *tbl = obj->value.gc
	L>top = tbl.arr[arg3];
	->top += sizeof(r_TValue)
}*/

typedef int(*rluaref) (int L, int t);
rluaref rbx_ref = (rluaref)offset(aRef);

int rbxref(int L, int t) {
	return rbx_ref(L, t);
}

typedef void(*rluarawgeti) (int L, int index, int n);
rluarawgeti rbx_rawgeti = (rluarawgeti)unprotect(offset(rawgeti));

void rbxrawgeti(int L, int index, int n)
{
	rbx_rawgeti(L, index, n);
}
