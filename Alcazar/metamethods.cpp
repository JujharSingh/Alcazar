#include "stdafx.h"
#include "metamethods.h"
#include "functions.h"
#include "main.h"

const char *typeToString(int type) {
	const char* LookupTable[] = {
		"NIL", "LIGHTUSERDATA", "NUMBER" ,
		"BOOLEAN", "STRING", "THREAD",
		"FUNCTION", "TABLE", "USERDATA",
		"PROTO", "UPVALUE"
	};

	return LookupTable[type];
}

int stackDump(lua_State *L) {
	int i;
	int top = rbxgettop(RbxState);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		r_TValue *rval = (r_TValue *)rbxindex2adr(RbxState, i);
		int t = rbxtype(RbxState, i);
		switch (t) {

		case RBXTSTRING:  /* strings */
			printf("STRING: %s'", rbxtostring(RbxState, i));
			break;

		case RBXTBOOLEAN:  /* booleans */
			printf(rval->value.b ? "true" : "false");
			break;

		case RBXTNUMBER:  /* numbers */
			printf("NUMBER: %g", rval->value.n);
			break;

		default:  /* other values */
			printf("OTHER (TYPE): %s", typeToString(t));
			break;

		}
		printf("  ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
	return 0;
}

int functionHandler(lua_State *L)
{
	DWORD addr = *(DWORD *)lua_touserdata(L, lua_upvalueindex(1));
	printf("\n%d", addr);
	
	int nargs = lua_gettop(L);
	int nres = rbxgettop(RbxState);

	r_TValue *rbxtval = (r_TValue *)addr;
	rbxpush(RbxState, rbxtval);

//	rbxpush(RbxState, (r_TValue*)nullptr);

	DEBUGPRINT("\n%d", nargs);

	for (auto argIdx = 1; argIdx <= nargs; argIdx++) {
		wrap(L, TO_RBX, argIdx);
		DEBUGPRINT("\nWrapped arg!");
	}

	rbxpcall(RbxState, nargs, LUA_MULTRET, 0);
	nres = rbxgettop(RbxState) - nres;

	DEBUGPRINT("\n%d", nres);

	for (auto resIdx = -(nres); resIdx < 0; resIdx++) {
		wrap(L, FROM_RBX, resIdx);
		DEBUGPRINT("\nWrapped ret!");
	}

	
	return nres;
}

int wrappedMM(lua_State *L, const char *mm)
{
	auto nargs = lua_gettop(L);
	
	if (rbxgetmetatable(RbxState, -1)) {
		auto nres = rbxgettop(RbxState);
		rbxgetfield(RbxState, -1, mm);
		for (auto argIdx = 1; argIdx <= nargs; argIdx++) {
			wrap(L, TO_RBX, argIdx);
		}
		rbxpcall(RbxState, nargs, LUA_MULTRET, 0);

		nres = rbxgettop(RbxState) - nres;

		for (auto resIdx = -(nres); resIdx < 0; resIdx++) {
			wrap(L, FROM_RBX, resIdx);
		}

		return nargs;
	}
	else {
		lua_pushstring(L, "No appropriate metamethod could be found!");
		lua_error(L);
		return 0;
	}
}

void wrap(lua_State *L, int direction, int idx) {
	if (direction == FROM_RBX) {
		DWORD rbxAddr = rbxindex2adr(RbxState, idx)[1];
		r_TValue* rTVal = (r_TValue *)rbxAddr;
		TValue *curr;
		switch (rbxtype(RbxState, idx))
		{
		case RBXTTABLE:
		case RBXTUSERDATA:
		case RBXTLIGHTUSERDATA:
			lua_newuserdata(L, 0);
			curr = index2adr(L, -1);
			curr->rbxaddr = rbxAddr;
			lua_newtable(L);
			lua_pushcfunction(L, RobloxIndex);
			lua_setfield(L, -2, "__index");
			lua_pushcfunction(L, RobloxNewIndex);
			lua_setfield(L, -2, "__newindex");
			lua_pushcfunction(L, RobloxCall);
			lua_setfield(L, -2, "__call");
			lua_pushcfunction(L, RobloxConcat);
			lua_setfield(L, -2, "__concat");
			lua_pushcfunction(L, RobloxUnm);
			lua_setfield(L, -2, "__unm");
			lua_pushcfunction(L, RobloxAdd);
			lua_setfield(L, -2, "__add");
			lua_pushcfunction(L, RobloxSub);
			lua_setfield(L, -2, "__sub");
			lua_pushcfunction(L, RobloxMul);
			lua_setfield(L, -2, "__mul");
			lua_pushcfunction(L, RobloxDiv);
			lua_setfield(L, -2, "__div");
			lua_pushcfunction(L, RobloxMod);
			lua_setfield(L, -2, "__mod");
			lua_pushcfunction(L, RobloxPow);
			lua_setfield(L, -2, "__pow");
			lua_pushcfunction(L, RobloxTostring);
			lua_setfield(L, -2, "__tostring");
			lua_pushcfunction(L, RobloxMetatable);
			lua_setfield(L, -2, "__metatable");
			lua_pushcfunction(L, RobloxEq);
			lua_setfield(L, -2, "__eq");
			lua_pushcfunction(L, RobloxLt);
			lua_setfield(L, -2, "__lt");
			lua_pushcfunction(L, RobloxLe);
			lua_setfield(L, -2, "__le");
			lua_pushcfunction(L, RobloxMode);
			lua_setfield(L, -2, "__mode");
			lua_pushcfunction(L, RobloxLen);
			lua_setfield(L, -2, "__len");
			lua_setmetatable(L, -2);
			DEBUGPRINT("\r\nLUD, UD or Table");
			break;
		case RBXTBOOLEAN:
			lua_pushboolean(L, rTVal->value.b);
			DEBUGPRINT("\r\nBoolean");
			break;
		case RBXTNUMBER:
			lua_pushnumber(L, rTVal->value.n);
			curr = index2adr(L, -1);
			curr->rbxaddr = rbxAddr;
			DEBUGPRINT("\nNumber");
			break;
		case RBXTSTRING:
			lua_pushstring(L, rbxtostring(RbxState, idx));
			curr = index2adr(L, -1);
			curr->rbxaddr = rbxAddr;
			DEBUGPRINT("\nString");
			break;
		case RBXTFUNCTION: {
			DWORD *meme = (DWORD *)lua_newuserdata(L, sizeof(DWORD));
			*meme = rbxAddr;
			lua_pushcclosure(L, functionHandler, 1);
			curr = index2adr(L, -1);
			curr->rbxaddr = rbxAddr;
			DEBUGPRINT("\nFunction");
			break;
		}
		default:
			lua_pushnil(L);
			DEBUGPRINT("\nOther/nil");
			break;
		}
	}
	else if(direction == TO_RBX)
	{
		TValue *localAddr = index2adr(L, idx);

		if (lua_type(L, idx) == LUA_TSTRING) {
			rbxpushstring(RbxState, lua_tostring(L, idx));
			DEBUGPRINT("\nPushed string");
		}
		else if (rbxpushrealobject(RbxState, localAddr)) {
			switch (lua_type(L, idx))
			{
			case LUA_TTABLE:
			case LUA_TUSERDATA:
			case LUA_TLIGHTUSERDATA:
				lua_pushstring(L, "Not implemented!");
				lua_error(L);
				break;
			case LUA_TBOOLEAN:
				rbxpushboolean(RbxState, localAddr->value.b);
				DEBUGPRINT("\nPushed boolean");
				break;
			case LUA_TNUMBER:
				rbxpushnumber(RbxState, localAddr->value.n);
				DEBUGPRINT("\nPushed number");
				break;
			case LUA_TSTRING:
				rbxpushstring(RbxState, lua_tostring(L, idx));
				DEBUGPRINT("\nPushed string");
				break;
			default:
				rbxpushnil(RbxState);
				DEBUGPRINT("\nPushed nil/other unhandled type!");
				break;
			}
		}
		else {
			DEBUGPRINT("\nPushed real object");
		}
	}
}

int RobloxGlobalIndex(lua_State *L)
{
	const char* key = lua_tostring(L, -1);

	rbxgetglobal(RbxState, key);
	wrap(L, FROM_RBX, -1);
	return 1;
}

int RobloxIndex(lua_State *L)
{
	const char* key = lua_tostring(L, -1);

	if (rbxgetmetatable(RbxState, -1)) {
		rbxgetfield(RbxState, -1, "__index");
		rbxpushvalue(RbxState, -3);
		rbxpushstring(RbxState, key);
		rbxpcall(RbxState, 2, 1, 0);
		wrap(L, FROM_RBX, -1);
		return 1;
	}
	else {
		rbxgetfield(RbxState, -1, key);
		wrap(L, FROM_RBX, -1);
		return 0;
	}

	return 0;
}

int RobloxNewIndex(lua_State *L)
{
	return wrappedMM(L, "__newindex");
}

int RobloxCall(lua_State *L)
{
	return wrappedMM(L, "__call");
}

int RobloxConcat(lua_State *L)
{
	return wrappedMM(L, "__concat");
}

int RobloxUnm(lua_State *L)
{
	return wrappedMM(L, "__unm");
}

int RobloxAdd(lua_State *L)
{
	return wrappedMM(L, "__add");
}

int RobloxSub(lua_State *L)
{
	return wrappedMM(L, "__sub");
}

int RobloxMul(lua_State *L)
{
	return wrappedMM(L, "__mul");
}

int RobloxDiv(lua_State *L)
{
	return wrappedMM(L, "__div");
}

int RobloxMod(lua_State *L)
{
	return wrappedMM(L, "__mod");
}

int RobloxPow(lua_State *L)
{
	return wrappedMM(L, "__pow");
}

int RobloxTostring(lua_State *L)
{
	return wrappedMM(L, "__tostring");
}

int RobloxMetatable(lua_State *L)
{
	return wrappedMM(L, "__metatable");
}

int RobloxEq(lua_State *L)
{
	return wrappedMM(L, "__eq");
}

int RobloxLt(lua_State *L)
{
	return wrappedMM(L, "__lt");
}

int RobloxLe(lua_State *L)
{
	return wrappedMM(L, "__le");
}

int RobloxMode(lua_State *L)
{
	return wrappedMM(L, "__mode");
}

int RobloxLen(lua_State *L)
{
	return wrappedMM(L, "__len");
}

/*int RobloxNewIndex(lua_State *L)
{
	const char* key = lua_tostring(L, -2);
	const char* val = NULL;
	int valu = NULL;

	int currtype = lua_type(L, -1);
	if (currtype == LUA_TBOOLEAN) {
		valu = lua_toboolean(L, -1);
	}
	else if (currtype == LUA_TSTRING || currtype == LUA_TNUMBER)
	{
		val = lua_tostring(L, -1);
	}

	if (rbxgetmetatable(RbxState, -1)) {
		rbxgetfield(RbxState, -1, "__newindex");
		rbxpushvalue(RbxState, -3);
		rbxpushstring(RbxState, key);
		if (currtype == LUA_TBOOLEAN) {
			rbxpushboolean(RbxState, valu);
		}
		else if (currtype == LUA_TSTRING || currtype == LUA_TNUMBER)
		{
			rbxpushstring(RbxState, val);
		}
		else
		{
			rbxpushnil(RbxState);
		}
		rbxpcall(RbxState, 3, LUA_MULTRET, 0);
		return 0;
	}
	else {
		return 0;
	}
}
int RobloxCall(lua_State *L)
{
		int args = 0;
		rbxpushvalue(RbxState, -2);
		for (int i = lua_gettop(L); i != 0; i--) {
			//printf("\r\n%s", std::to_string(lua_type(L, -i)).c_str());
			switch (lua_type(L, -i))
			{
			case LUA_TBOOLEAN:
				rbxpushboolean(RbxState, lua_toboolean(L, -i));
				break;
			case LUA_TNUMBER:
				rbxpushnumber(RbxState, lua_tonumber(L, -i));
				break;
			case LUA_TSTRING:
				rbxpushstring(RbxState, lua_tostring(L, -i));
				break;
			default:
				break;
			}
			args++;
		}
		rbxpcall(RbxState, args, 0);
		return 0;
}

int instancenew(lua_State *L)
{
	globalOnStack = false;
	int args = lua_gettop(L);

	rbxgetglobal(RbxState, "Instance");
	rbxgetfield(RbxState, -1, "new");
	
	const char* className = luaL_checkstring(L, 1);
	rbxpushstring(RbxState, className); //Instance.new, className
	if (args == 2) //if a parent is passed
		rbxpushvalue(RbxState, -4); //Instance.new, className, instance/userdata of parent

	rbxpcall(RbxState, args, 1, 0);
	syncStacks(L, className);
	return 1;
}

int printidentity(lua_State *L)
{
	globalOnStack = false;
	rbxgetglobal(RbxState, "printidentity");
	rbxpcall(RbxState, 0, 0);

	return 0;
}

int print(lua_State *L)
{
	globalOnStack = false;
	int args = lua_gettop(L);

	rbxgetglobal(RbxState, "print");
	for (int i = args; i != 0; i--) {
		//printf("\r\n%s", std::to_string(lua_type(L, -i)).c_str());
		switch (lua_type(L, -i))
		{
		case LUA_TBOOLEAN:
			rbxpushboolean(RbxState, lua_toboolean(L, -i));
			break;
		case LUA_TNUMBER:
			rbxpushstring(RbxState, lua_tostring(L, -i));
			break;
		case LUA_TSTRING:
			rbxpushstring(RbxState, lua_tostring(L, -i));
			break;
		default:
			break;
		}
	}
	rbxpcall(RbxState, args, 0);

	return 0;
}*/

	/*int currentType = rbxtype(RbxState, -1);
	DWORD rbxAddr = rbxindex2adr(RbxState, -1)[1];
	if (currentType == RBXTUSERDATA) {
		printf("\r\nInstance: %s", key);
		lua_newtable(L);
		TValue* curr = index2adr(L, -1);
		curr->rbxaddr = rbxAddr;
		lua_newtable(L);
		lua_pushcfunction(L, RobloxNewIndex);
		lua_setfield(L, -2, "__newindex");
		lua_pushcfunction(L, RobloxIndex);
		lua_setfield(L, -2, "__index");
		lua_setmetatable(L, -2);
	}
	if (currentType == RBXTFUNCTION) {
		printf("\r\nFunction: %s", key);
		lua_pushcfunction(L, RobloxCall);
		TValue* curr = index2adr(L, -1);
		curr->rbxaddr = rbxAddr;
	}
	//LEAVE THESE ALONE
	if (currentType == RBXTBOOLEAN) {
		printf("\r\nBoolean: %s", key);
		lua_pushboolean(L, rbxtoboolean(RbxState, -1));
	}
	//LEAVE THESE ALONE
	if (currentType == RBXTNIL) {
		printf("\r\nNil: %s", key);
		lua_pushnil(L);
	}
	if (currentType == RBXTSTRING) {
		printf("\r\nString: %s", key);
		lua_pushstring(L, rbxtostring(RbxState, -1));
		TValue* curr = index2adr(L, -1);
		curr->rbxaddr = rbxAddr;
	}
	if (currentType == RBXTTABLE) {
		printf("\r\nTable: %s", key);
		lua_newtable(L);
		TValue* curr = index2adr(L, -1);
		curr->rbxaddr = rbxAddr;
		lua_newtable(L);
		lua_pushcfunction(L, RobloxCall);
		lua_setfield(L, -2, "__call");
		lua_pushcfunction(L, RobloxNewIndex);
		lua_setfield(L, -2, "__newindex");
		//TODO: make a table index func, probably just passthrough the values idk maybe you dont need to do this
		lua_pushcfunction(L, RobloxIndex);
		lua_setfield(L, -2, "__index");
		lua_setmetatable(L, -2);
	}
	if (currentType == RBXTNUMBER) {
		printf("\r\nNumber: %s", key);
		lua_pushnumber(L, rbxtonumber(RbxState, -1));
		TValue* curr = index2adr(L, -1);
		curr->rbxaddr = rbxAddr;
		lua_pushvalue(L, -1); //duplicate the table so it isnt popped by setfield
		lua_setfield(L, -3, key);
	}*/

