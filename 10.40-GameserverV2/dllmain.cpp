// dllmain.cpp : Defines the entry point for the DLL application.
#include "Framework.h"
#include "Logging.h"
#include "Networking.h"
#include "Configuration.h"
#include "Includes.h"

#include "Gamemode.h"
#include "Teams.h"
#include "Player.h"

#include "Hooking.h"

// please dont use this for your own project
// just give me credits that's all i ask for.

DWORD WINAPI Main(LPVOID)
{
    AllocConsole();
    FILE* fptr;
    freopen_s(&fptr, "CONOUT$", "w+", stdout);
    EnableConsole();

    SetConsoleTitleA("10.40 Gameserver | (Base: Sxlar) | (SDK: Milxnor)");

    log("VERSION", "10.40", BLUE, WHITE);
    MH_Initialize();

    if (MH_Initialize)
    {
        log("MINHOOK", "Minhook Initialized", BLUE, WHITE);
    }

    Sleep(5000);

    UObject::GObjects = decltype(UObject::GObjects)(__int64(Client::BaseAddress() + 0x64a0090));

    *(bool*)(__int64(Client::BaseAddress() + 0x637925B)) = false;
    *(bool*)(__int64(Client::BaseAddress() + 0x637925C)) = true;

    Gameserver::StartHooking();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
    }
    return TRUE;
}

