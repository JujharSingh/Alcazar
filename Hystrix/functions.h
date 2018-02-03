#pragma once

#include <Windows.h>
#include <emmintrin.h>
#include <string> 

#include "addresses.h"

extern "C" {
#include "lua-5.1.5/lua.h"
#include "lua-5.1.5/lobject.h"
#include "lua-5.1.5/lauxlib.h"
#include "lua-5.1.5/lualib.h"
} 

#define rbxgetglobal(L,s) rbxgetfield(L, -10002, s)
#define rbxsetglobal(L,s) rbxsetfield(L, -10002, s)
#define rbxpop(L,n)  rbxsettop(L, -(n)-1)
#define rbxtostring(L,i)       rbxtolstring(L, (i), NULL)

#define RBXTNONE               (-1)
#define RBXTNIL			0
#define RBXTBOOLEAN		3
#define RBXTLIGHTUSERDATA	1
#define RBXTNUMBER		2 
#define RBXTSTRING		4 
#define RBXTTABLE		7 
#define RBXTFUNCTION		6 
#define RBXTUSERDATA		8 
#define RBXTTHREAD		5 
#define RBXTPROTO		9
#define RBXTUPVAL		1

// #define _DEBUG

#ifdef _DEBUG
#define DEBUGPRINT(...) printf(__VA_ARGS__)
#else
#define DEBUGPRINT
#endif

DWORD unprotect(DWORD addr);

int rbxgettop(DWORD L);

void rbxsettop(DWORD L, int idx);

void rbxpushnil(DWORD L);

void rbxpushnumber(DWORD L, double n);

DWORD* rbxindex2adr(DWORD L, int idx);

void rbxpushvalue(DWORD L, int idx);

int rbxtype(DWORD L, int idx);

void hystrix_setup(lua_State *L);

DWORD getrbxsc();

DWORD getrbxls(DWORD sc);

int rbxgetmetatable(int L, int objindex);

void rbxgetfield(int L, int idx, const char *k);

void insert(lua_State *L, int tbl_idx, int val_idx);

void rbxpushstring(int L, const char *s);

int rbxpcall(int L, int nargs, int nresults, int errfunc = 0);

const char* rbxtolstring(int L, int idx, unsigned int len);

int rbxtoboolean(int L, int idx);

double rbxtonumber(int L, int idx);

void rbxpushboolean(DWORD L, int b);

void rbxsetfield(int L, int idx, const char *k);

int rbxpushrealobject(int L, TValue *o);

#define RBXTVALUEFIELDS		Value value; int tt

typedef struct rbx_TValue {
	RBXTVALUEFIELDS;
} r_TValue;

void rbxremove(int L, int idx);

int rbxpush(int L, r_TValue *o);
