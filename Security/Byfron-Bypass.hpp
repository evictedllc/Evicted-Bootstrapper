
#pragma once

#include "Byfron.cpp"
#pragma warning(push)
#pragma warning(disable:4091)
#include <DbgHelp.h>
#pragma warning(pop)

namespace evicted
{
	struct SegmentInfo
	{
		char* SegmentName;
		int SegmentBase;
		int SegmentEnd;
		int SegmentCopy;
	};

	struct ImageSectionInfo
	{
		char SectionName[IMAGE_SIZEOF_SHORT_NAME];
		char* SectionAddress;
		int SectionSize;

		explicit ImageSectionInfo(const char* name)
		{
			strcpy(SectionName, name);
			SectionAddress = 0;
			SectionSize = 0;
		}
	};

	class MemCheck
	{
		std::vector<SegmentInfo*> Segments;

	public:
		static __declspec(noinline) int GetCopyDif(int Base, int Add);
		static __declspec(noinline) ImageSectionInfo* GetSegmentInfo(HMODULE Module, char* SegmentName);
		static __declspec(noinline) uint32_t HashSecondary(uintptr_t HasherAddr, uintptr_t StartingAddr, uintptr_t EndingOffset, int Unused, uint32_t Nonce);
		static __declspec(noinline) void WriteBytes(DWORD Addr, DWORD Patch, size_t Size);
		static __declspec(noinline) void RawHook(DWORD Addr, DWORD Jmp, BYTE HookByte = 0xE9);
		__declspec(noinline) void AddSegment(char* SegmentName);
		static __declspec(noinline) void HookMemCheck(DWORD Hasher, DWORD JmpBack);

		MemCheck();
		~MemCheck();
		static MemCheck* GetBaseInstance();
	};
}
