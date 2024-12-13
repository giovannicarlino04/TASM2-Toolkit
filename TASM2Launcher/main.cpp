#include <windows.h>
#include <iostream>
#include <string>

bool InjectDLL(DWORD processID, const std::string& dllPath) {
    // Open the target process with all access rights
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process (ID: " << processID << "). Error: " << GetLastError() << std::endl;
        return false;
    }

    // Allocate memory in the target process for the DLL path
    LPVOID pDllPath = VirtualAllocEx(hProcess, nullptr, dllPath.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (pDllPath == NULL) {
        std::cerr << "Failed to allocate memory in target process. Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    // Write the DLL path to the allocated memory
    if (!WriteProcessMemory(hProcess, pDllPath, dllPath.c_str(), dllPath.length() + 1, nullptr)) {
        std::cerr << "Failed to write DLL path to process memory. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get the address of LoadLibraryA
    FARPROC loadLibraryAddr = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
    if (loadLibraryAddr == NULL) {
        std::cerr << "Failed to get address of LoadLibraryA. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Create a remote thread in the target process that calls LoadLibraryA(dllPath)
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, pDllPath, 0, nullptr);
    if (hThread == NULL) {
        std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Wait for the remote thread to finish
    WaitForSingleObject(hThread, INFINITE);

    // Clean up
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return true;
}

int main() {
    // Game executable path
    LPCSTR gamePath = "Game.exe";

    // Path to your modified DLL
    std::string dllPath = "tasm2patcher.dll";

    // Create the game process
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};

    if (CreateProcess(gamePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0) {
        std::cerr << "Error starting the game!" << std::endl;
        return -1;
    }

    std::cout << "Game started successfully!" << std::endl;

    // Inject the DLL into the game process
    if (!InjectDLL(pi.dwProcessId, dllPath)) {
        std::cerr << "Failed to inject DLL into the game process!" << std::endl;
        return -1;
    }

    std::cout << "DLL injected successfully!" << std::endl;

    // Wait for the game to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Clean up handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
