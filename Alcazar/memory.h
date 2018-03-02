#pragma once

#include <Windows.h>

//hi john thank for the functions
namespace Memory {
	void write(void* address, void* instructions, size_t size);

	bool compare(BYTE* address, BYTE* pattern, BYTE* mask);

	DWORD scan(BYTE* aob, BYTE* mask, BYTE prot);
}