#pragma once

#include "Static.hpp"
#include "./AntiTamper.hpp"
#include "EvictedHook.hpp"
#include "Whitelist.hpp"

#pragma warning(disable : 4191)
namespace syn::AntiDebug
{
__forceinline bool Setup()
{
// Attempt to prevent debugging
bool success = AntiTamper::Initialize();
success &= SysHook::DisableDebuggerDetection();
success &= FunctionReader::HideModuleFromPEB();

	// If the above fails, run default behavior
	if (!success)
	{
		// TODO: implement default behavior
		return false;
	}

	return true;
}

__forceinline bool Check()
{
	// Perform checks for debugging
	bool isDebugging = false;

	// TODO: implement checks

	return isDebugging;
}
