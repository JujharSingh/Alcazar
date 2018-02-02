#pragma once

#define offset(addr) (addr - 0x400000 + (DWORD)GetModuleHandle(NULL))

//DONT FORGET TO UPDATE GETSTATE: UPDATOED
enum {
	Remove = 0x723320, //u
	pcall = 0x722790, //u
	penvcheck = 0x71AF8D, //u
	newthread = 0x722460, //u
	scriptcontext = 0x100B590, //u
	getmetatable = 0x721CA0, //u
	xorconst = 0x14A26F0, //u
	getfield = 0x7219D0, //u
	setfield = 0x7236F0, //u
	pushstring = 0x722D50, //u
	//pushnumber = 0x722CC0, //FUCKING FIX THIS NNOOB
	tolstring = 0x723D10, //u
	toboolean = 0x723C50, //u
	tonumber = 0x723FF0, //u
	identity = 0x158AE14, //u
	L_top = 32, //u
	L_base = 20 //u
};