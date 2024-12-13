#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "tasm2patcher.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
		MessageBoxA(0, "Success", "Success", 0);
        break;
    }

    case DLL_PROCESS_DETACH:
    {

    }
    break;
    }

    return TRUE;
}