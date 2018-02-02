#include "stdafx.h"
#include "metamethods.h"
#include "functions.h"
#include "main.h"

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

		/*rbxgetglobal(RbxState, "print");
		rbxpushvalue(RbxState, -2);
		rbxpcall(RbxState, 1, 0);*/

		return nargs;
	}
	else {
		lua_pushstring(L, "No appropriate metamethod could be found!");
		lua_error(L);
		return 0;
	}
}

std::map<std::string, std::function<int(lua_State*)>> wrappedMMs{
	{ "__index", std::bind(wrappedMM, std::placeholders::_1, "__index") },
{ "__newindex", std::bind(wrappedMM, std::placeholders::_1, "__newindex") },
{ "__call", std::bind(wrappedMM, std::placeholders::_1, "__call") },
{ "__add", std::bind(wrappedMM, std::placeholders::_1, "__add") },
{ "__next", std::bind(wrappedMM, std::placeholders::_1, "__next") },
};

void wrap(lua_State *L, int direction, int idx) {


	//new, better ver
	if (direction == 1) {
		DWORD rbxAddr = rbxindex2adr(RbxState, idx)[1];
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
			lua_pushliteral(L, "This metatable is locked!");
			lua_setfield(L, -2, "__metatable");
			lua_setmetatable(L, -2);
			printf("\r\nLUD, UD or Table");
			break;
		case RBXTBOOLEAN:
			lua_pushboolean(L, rbxtoboolean(RbxState, idx));
			printf("\r\nBoolean");
			break;
		case RBXTNUMBER:
			lua_pushnumber(L, rbxtonumber(RbxState, idx));
			curr = index2adr(L, -1);
			curr->rbxaddr = rbxAddr;
			printf("\r\nNumber");
			break;
		case RBXTSTRING:
			lua_pushstring(L, rbxtostring(RbxState, idx));
			curr = index2adr(L, -1);
			curr->rbxaddr = rbxAddr;
			printf("\r\nString");
			break;
		default:
			lua_pushnil(L);
			printf("\r\nOther/nil");
			break;
		}
	}
	else if(direction == 0)
	{
		TValue *localAddr = index2adr(L, idx);

		if (lua_type(L, idx) == LUA_TSTRING) {
			rbxpushstring(RbxState, lua_tostring(L, idx));
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
				printf("\r\nPushed boolean");
				break;
			case LUA_TNUMBER:
				rbxpushnumber(RbxState, localAddr->value.n);
				printf("\r\nPushed number");
				break;
			case LUA_TSTRING:
				rbxpushstring(RbxState, lua_tostring(L, idx));
				printf("\r\nPushed string");
				break;
			default:
				rbxpushnil(RbxState);
				printf("\r\nPushed nil/other unhandled type!");
				break;
			}
		}
		else {
			printf("\r\nPushed real object");
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
	return wrappedMMs["__index"](L);
}

int RobloxNewIndex(lua_State *L)
{
	return wrappedMMs["__newindex"](L);
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

