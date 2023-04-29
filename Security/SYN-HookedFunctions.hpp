#include "../Execution/RbxLua.hpp"
#include "../Execution/RbxApi.hpp"
#include "../Misc/D3D.hpp"
#include "../Misc/Fonts.hpp"
#include "../../Source Dependencies/ImGUI/imgui_impl_dx11.h"
#include "../../Source Dependencies/ImGUI/imgui_impl_win32.h"
#include "../../Source Dependencies/ImGUI/imgui_freetype.h"
#include "./SysHook.hpp"
#include "../../Utilities/Spoofer.hpp"
#include "./MemCheck.hpp"

IDXGISwapChainPresentFn OrigIDXGISwapChainPresent;
bool IsLuaU;
std::vector<std::string> ChunkNamesVec;

BOOL ID3DFirst = TRUE;
BOOL FPSUnlocked = FALSE;
BOOL IGuiOpen = FALSE;
BOOL IGuiEnabled = FALSE;
bool OutputRedirection = FALSE;
bool AutoScroll = FALSE;
DWORD TaskSched = NULL;
DWORD TaskSchedDelay = NULL;
IDXGISwapChain* ID3DChain = NULL;
ID3D11Device* ID3DDevice = NULL;
ID3D11DeviceContext* ID3DContext = NULL;
ID3D11RenderTargetView* ID3DRenderTarget = NULL;
ID3D11Texture2D* ID3DRenderTargetTexture = NULL;
WNDPROC OrigWndProc = NULL;
uint64_t OriginalNtQIP = 0;
uint64_t OriginalSetUEF = 0;
uint64_t OriginalGetAdaptersAddresses = 0;
uint64_t OrigRtlImageNtHeaderEx = 0;
uint64_t OriginalGetThreadId = 0;
uintptr_t OriginalErrHook = 0;
uintptr_t OriginalMsgOutHook = 0;
uint32_t ScriptRunCounter = 0;
bool SecureLuaFlag = false;
PLH::CapstoneDisassembler x86Disasm{ PLH::Mode::x86 };
LPVOID OriginalNtQVM = nullptr;
LPVOID OriginalZwFT = nullptr;
std::string D3DWorkspaceDirectory;
std::vector<std::pair<std::string, ImU32>> ConsoleOutput;
std::unordered_map<std::uintptr_t, EvictedCClosure> HookedFunctionsMap;
uintptr_t CurrentInstanceMsgOut;

DWORD WINAPI SetUEFHook(LPTOP_LEVEL_EXCEPTION_FILTER) { return 0; }

signed int WINAPI RtlImageNtHeaderExHook(int A1, unsigned int Base, int A3, int A4, PIMAGE_NT_HEADERS* Out)
{
	if (Base == (DWORD)evicted::Module)
	{
		const auto DosHeader = (PIMAGE_DOS_HEADER) Base;
		*Out = (PIMAGE_NT_HEADERS)((char*) DosHeader + DosHeader->e_lfanew);
		return 0;
	}

	return ((std::decay_t<decltype(RtlImageNtHeaderExHook)>) OrigRtlImageNtHeaderEx)(A1, Base, A3, A4, Out);
}

DWORD OrigTSRet = 0;
__declspec(naked) void SchedulerRetHook()
{
	__asm
	{
		call evicted::Scheduler::StepSchedule
		jmp OrigTSRet	
	}
}

__declspec(noinline) DWORD GetCurrentThreadIdHook()
{
	DWORD Ret = ((std::decay_t<decltype(GetCurrentThreadIdHook)>) OriginalGetThreadId)();
	DWORD RAddr = (DWORD)_ReturnAddress();

	static DWORD TRAddr1 = 0;
	static DWORD TRAddr2 = 0;
	static DWORD TRAddr3 = 0;

	if (!TRAddr1)
		TRAddr1 = evicted::RobloxBase(evicted::Offsets::DataModel::GetThreadIdReturn1);

	if (!TR
