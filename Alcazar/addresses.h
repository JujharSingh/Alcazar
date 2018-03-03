#pragma once

#define offset(addr) (addr - 0x400000 + (DWORD)GetModuleHandle(NULL))

//DONT FORGET TO UPDATE GETSTATE: updated

#define aRef	0x720DE0
#define Index2adr	0x71D580 
#define refsec1	0x720E3C
#define refsec2	0x720EB6
#define rawgeti	0x726DB0
#define	pushcclosure	0x7228E0 //unused leave it i cba
#define	pcall	0x726520
#define	penvcheck	0x71ECDD
#define	scriptcontext	0x100D1B8
#define	getmetatable	0x725A10
#define	xorconst	0x14976D0
#define	getfield	0x725720
#define	setfield	0x727480
#define	pushstring	0x726AE0
#define	tolstring	0x727AA0
#define	identity	0x1580F2C
#define	L_top	12 //u
#define	L_base	20 //u
