#pragma once

#include "../Misc/Static.hpp"
#include "../Misc/Utils.hpp"
#include "../Security/XorStr.hpp"

namespace syn::Antitamper
{
    bool Check()
    {
        const char* ModuleName = OBFUSCATE_STR("evicted.dll");
        const char* Checksum = OBFUSCATE_STR("EVICTED-4059594388384");

        HMODULE ModuleHandle = GetModuleHandleA(ModuleName);

        if (!ModuleHandle)
            return false;

        DWORD ModuleSize = syn::GetModuleSize(ModuleHandle);
        if (ModuleSize == 0)
            return false;

        char* ModuleBuffer = new char[ModuleSize];
        if (!ModuleBuffer)
            return false;

        if (!syn::ReadProcessMemory(GetCurrentProcess(), ModuleHandle, ModuleBuffer, ModuleSize, nullptr))
        {
            delete[] ModuleBuffer;
            return false;
        }

        std::string Hash = syn::Sha256(ModuleBuffer, ModuleSize);
        delete[] ModuleBuffer;

        return (Hash == Checksum);
    }
}
