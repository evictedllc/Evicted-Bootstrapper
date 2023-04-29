#pragma once

#include "../../Utilities/WinReg.hpp"

#include <intrin.h>
#include <cctype>
#include <string>
#include <memory>

namespace syn
{
class SystemFingerprint
{
public:
static constexpr int kFingerprintSize = 16;
      SystemFingerprint()
    {
        for (int i = 0; i < kFingerprintSize; i++)
        {
            unique_fingerprint_[i] = ~i & 255;
        }
    }

    std::string ToString() const
    {
        std::string out_string;
        for (int i = 0; i < kFingerprintSize; i++)
        {
            out_string += N2HexStr(unique_fingerprint_[i]);
        }
        return out_string;
    }

    static std::string Pad4Byte(const std::string& str)
    {
        const auto s = str.size() + (4 - str.size() % 4) % 4;

        if (str.size() < s)
        {
            return str + std::string(s - str.size(), ' ');
        }
        return str;
    }

    static bool IsNumber(const std::string& s)
    {
        auto it = s.begin();
        while (it != s.end() && std::isdigit(*it)) ++it;
        return !s.empty() && it == s.end();
    }

    static std::string GetPhysicalDriveId(DWORD id)
    {
        const auto handle = CreateFileA(
            (std::string("\\\\.\\PhysicalDrive") + std::to_string(id)).c_str(), 
            0, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, 
            nullptr, 
            OPEN_EXISTING, 
            0, 
            nullptr
        );
        if (handle == INVALID_HANDLE_VALUE)
        {
            return "Unknown";
        }

        std::unique_ptr<std::remove_pointer<HANDLE>::type, void(*)(HANDLE)> h_device{ 
            handle, 
            [](HANDLE handle) { CloseHandle(handle); } 
        };

        STORAGE_PROPERTY_QUERY storage_prop_query{};
        storage_prop_query.PropertyId = StorageDeviceProperty;
        storage_prop_query.QueryType = PropertyStandardQuery;

        STORAGE_DESCRIPTOR_HEADER storage_desc_header{};
        DWORD bytes_returned = 0;
        if (!DeviceIoControl(
            h_device.get(), 
            IOCTL_STORAGE_QUERY_PROPERTY, 
            &storage_prop_query, 
            sizeof(STORAGE_PROPERTY_QUERY),
            &storage_desc_header, 
            sizeof(STORAGE_DESCRIPTOR_HEADER), 
            &bytes_returned, 
            nullptr))
        {
            return "Unknown";
        }

        const auto out_buffer_size = storage_desc_header.Size;
        std::unique_ptr<BYTE[]> out_buffer{ new BYTE[out_buffer_size]{} };
        SecureZeroMemory(out_buffer.get(), out_buffer_size);

        if (!DeviceIoControl(
            h_device.get(), 
            IOCTL_STORAGE_QUERY_PROPERTY, 
            &storage_prop_query, 
            sizeof(STORAGE_PROPERTY_QUERY),
            out_buffer.get(), 
            out_buffer_size, 
            &bytes_returned, 
            nullptr))
        {
            return "Unknown";
        }

        const auto device_descriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(out_buffer.get());
        const auto serial_number_offset = device_descriptor->SerialNumberOffset;
        if (serial_number_offset == 0)
        {
            return "Unknown";
        }

        const auto serial_number = reinterpret_cast<const char*>(out_buffer.get() + serial_number_offset);
        return serial_number;
    }

    void Interleave(unsigned long data)
    {
        *(unsigned long*)unique_fingerprint_ ^= data + 0x2EF
