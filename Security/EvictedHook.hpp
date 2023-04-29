#include <Windows.h>
#include <iostream>

// Define the function signature for the original function we're hooking
typedef BOOL(WINAPI* OriginalFunction)(LPCTSTR, LPCTSTR, LPCTSTR, DWORD);

// Define a global variable to store the original function address
OriginalFunction g_pOriginalFunction = nullptr;

// Define our own implementation of the function
BOOL WINAPI MyFunction(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, DWORD nSize)
{
    // Print the injection time
    std::cout << "Injected at: " << __TIME__ << std::endl;

    // Call the original function
    return g_pOriginalFunction(lpAppName, lpKeyName, lpString, nSize);
}

// Define the hook function
void HookFunction()
{
    // Get a handle to the DLL containing the function we want to hook
    HMODULE hModule = LoadLibrary(TEXT("KERNEL32.DLL"));

    // Get a pointer to the original function
    g_pOriginalFunction = (OriginalFunction)GetProcAddress(hModule, "WritePrivateProfileStringW");

    // Allocate a block of memory in which to store our own implementation of the function
    BYTE* pMyFunction = (BYTE*)VirtualAlloc(NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    // Write our own implementation of the function to the memory block
    BYTE* pCode = pMyFunction;
    *(DWORD*)(pCode) = 0x042444C7; // mov dword ptr [esp+4], offset MyFunction
    *(DWORD*)(pCode + 4) = (DWORD)&MyFunction;
    *(WORD*)(pCode + 8) = 0xD3FF; // jmp esp
    pCode += 10;

    // Write a trampoline to the memory block
    *(DWORD*)(pCode) = 0xE9; // jmp to original function
    *(DWORD*)(pCode + 1) = (DWORD)g_pOriginalFunction - (DWORD)pCode - 5;
    pCode += 5;

    // Patch the original function with a jump to our memory block
    DWORD dwOldProtect;
    VirtualProtect(g_pOriginalFunction, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
    *(WORD*)g_pOriginalFunction = 0xE9; // jmp to our memory block
    *(DWORD*)(g_pOriginalFunction + 1) = (DWORD)pMyFunction - (DWORD)g_pOriginalFunction - 5;
    VirtualProtect(g_pOriginalFunction, 5, dwOldProtect, &dwOldProtect);
}

// Entry point of the program
int main()
{
    // Call the hook function
    HookFunction();

    // Call the original function
    WritePrivateProfileStringW(L"Section", L"Key", L"Value", L"Path");

    // Wait for user input before exiting
    std::cin.get();
    return 0;
}
