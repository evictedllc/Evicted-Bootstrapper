#pragma once

#include <Windows.h>

#include "./EvictedAntiSwing.hpp"
#include "InjectionProcess.hpp"

#include "AutoUpdate.hpp"

#include <DbgHelp.h>
namespace syn::AntiDump
{
    // Check if the current process is being debugged
    inline bool IsDebuggerPresent()
    {
        BOOL isDebuggerPresent = FALSE;
        ::CheckRemoteDebuggerPresent(::GetCurrentProcess(), &isDebuggerPresent);
        return isDebuggerPresent != FALSE;
    }

    // Check if the current process is running in a virtual machine
    inline bool IsVirtualMachine()
    {
        bool result = false;

        // Check if the current process is running in a virtualized environment
        __try
        {
            __asm
            {
                push edx
                push ecx
                push ebx
                mov eax, 'VMXh'
                mov ebx, 0 // Function number 0
                mov ecx, 0x10 // Magic value
                mov edx, 'VX' // Magic value
                in eax, dx // VMX function call
                cmp ebx, 0x564D5868 // 'VXhM' magic value returned in EBX
                setz [result] // Set result to true if magic value matches
                pop ebx
                pop ecx
                pop edx
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            // Exception occurred, assume virtual machine
            result = true;
        }

        return result;
    }

    // Perform the anti-dump checks
    inline bool Check()
    {
        // Check if the current process is being debugged or running in a virtual machine
        if (IsDebuggerPresent() || IsVirtualMachine())
        {
            return true;
        }

        // Check if the current process is packed with a known packer
        if (strcmp((const char*)GetModuleHandle(NULL), "UPX") == 0)
        {
            return true;
        }

        // Check if the current process has been modified since it was loaded
        HANDLE hModule = GetModuleHandle(NULL);
        IMAGE_NT_HEADERS* pNtHeaders = (IMAGE_NT_HEADERS*)((BYTE*)hModule + ((IMAGE_DOS_HEADER*)hModule)->e_lfanew);
        return (pNtHeaders->OptionalHeader.CheckSum == 0);
    }
}

