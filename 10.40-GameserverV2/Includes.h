#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <cstdio>
#pragma comment(lib,"minhook/minhook.lib")
#include "minhook/MinHook.h"

#define CREATEHOOK(Address, Hook, Og) \
MH_CreateHook((void*)(Address), Hook, (void**)(Og));

static void VirtualHook(void** vft, int idx, void* newfunc, void** OG = nullptr)
{
	if (OG)
		*OG = vft[idx];

	DWORD dwProt;
	VirtualProtect(&vft[idx], 8, PAGE_EXECUTE_READWRITE, &dwProt);
	vft[idx] = newfunc;
	DWORD dwTemp;
	VirtualProtect(&vft[idx], 8, dwProt, &dwTemp);
}

template<typename T>
void IndexHook(T* object, size_t index, void* newfunction) // sum light (im pretty sure VirtualHook will work to but wtv its proper hooking names i would say)
{
	void** vft = *(void***)object;

	void* originalfunction = vft[index];

	vft[index] = newfunction;
}

#include "SDK/SDK.hpp"
using namespace SDK;