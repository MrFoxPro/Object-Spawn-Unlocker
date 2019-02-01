/*
	* Copyright (C) GTA:Multiplayer Team (https://wiki.gta-mp.net/index.php/Team)
	*
	* Redistribution and use in source and binary forms, with or without
	* modification, are permitted provided that the following conditions are
	* met:
	*
	*     * Redistributions of source code must retain the above copyright
	*		notice, this list of conditions and the following disclaimer.
	*     * Redistributions in binary form must reproduce the above
	*		copyright notice, this list of conditions and the following disclaimer
	*		in the documentation and/or other materials provided with the
	*		distribution.
	*     * Neither the name of GTA-Network nor the names of its
	*		contributors may be used to endorse or promote products derived from
	*		this software without specific prior written permission.
	*
	* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES INCLUDING, BUT NOT
	* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	* DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND ON ANY
	* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	* INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE
	* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <Windows.h>
#include <cstdint>
#include <Psapi.h>
#include <string>
// ScriptHookV
#include "scripthookv/main.h"
#include "scripthookv/natives.h"
#pragma comment(lib, "lib/scripthookv.lib")

// Find Pattern
bool CompareMemory(const uint8_t* pData, const uint8_t* bMask, const char* sMask)
{
	for (; *sMask; ++sMask, ++pData, ++bMask)
		if (*sMask == 'x' && *pData != *bMask)
			return false;

	return *sMask == NULL;
}

intptr_t FindPattern(const char* bMask, const char* sMask)
{
	// Game Base & Size
	static intptr_t pGameBase = (intptr_t)GetModuleHandle(nullptr);
	static uint32_t pGameSize = 0;
	if (!pGameSize)
	{
		MODULEINFO info;
		GetModuleInformation(GetCurrentProcess(), (HMODULE)pGameBase, &info, sizeof(MODULEINFO));
		pGameSize = info.SizeOfImage;
	}

	// Scan
	for (uint32_t i = 0; i < pGameSize; i++)
		if (CompareMemory((uint8_t*)(pGameBase + i), (uint8_t*)bMask, sMask))
			return pGameBase + i;

	return 0;
}

// Reveal
void UnlockAllObjects()
{
	static auto checkModelBeforeCreation = FindPattern("\x48\x85\xC0\x0F\x84\x00\x00\x00\x00\x8B\x48\x50", "xxxxx????xxx");
	if (!checkModelBeforeCreation)
	{
		MessageBoxA(NULL, "Object Spawn Unlocker: This version does not work with your current game version.", "Fatal Error", NULL);
		return;
	}
	std::string str = std::to_string(checkModelBeforeCreation);
	MessageBoxA(NULL, const_cast<char *>(str.c_str()), "INFO", NULL);
	memset((void*)checkModelBeforeCreation, 0x90, 24);
}

// Script Main
void ScriptMain()
{
	UnlockAllObjects();
	WAIT(1);
}

// Main
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID pReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Register
		scriptRegister(hModule, ScriptMain);
		break;
	case DLL_PROCESS_DETACH:
		// Unregister
		scriptUnregister(ScriptMain);
		break;
	}

	return TRUE;
}