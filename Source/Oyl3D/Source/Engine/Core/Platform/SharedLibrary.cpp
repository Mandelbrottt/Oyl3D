#include "SharedLibrary.h"

using Oyl::Platform::SharedLibraryLoadFlags;
using Oyl::Platform::SharedLibraryResult;
using Oyl::Platform::SharedLibraryHandle;
using Oyl::StringView;

#if defined(OYL_WINDOWS)
#include <Windows.h>

static
SharedLibraryResult
Windows_LoadSharedLibrary(SharedLibraryHandle* a_outHandle, StringView a_library, SharedLibraryLoadFlags a_loadFlags);

static
SharedLibraryResult
Windows_FreeSharedLibrary(SharedLibraryHandle a_handle);

static
void*
Windows_GetSymbolFromSharedLibrary(SharedLibraryHandle a_handle, StringView a_symbol);
#endif

namespace Oyl::Platform
{
	SharedLibraryResult
	LoadSharedLibrary(SharedLibraryHandle* a_outHandle, StringView a_library, SharedLibraryLoadFlags a_loadFlags)
	{
	#if defined(OYL_WINDOWS)
		return Windows_LoadSharedLibrary(a_outHandle, a_library, a_loadFlags);
	#endif
	}

	SharedLibraryResult
	FreeSharedLibrary(SharedLibraryHandle a_handle)
	{
	#if defined(OYL_WINDOWS)
		return Windows_FreeSharedLibrary(a_handle);
	#endif
	}

	void*
	GetSymbolFromSharedLibrary(SharedLibraryHandle a_handle, StringView a_symbol)
	{
	#if defined(OYL_WINDOWS)
		return Windows_GetSymbolFromSharedLibrary(a_handle, a_symbol);
	#endif
	}
}

#if defined(OYL_WINDOWS)
SharedLibraryResult
Windows_LoadSharedLibrary(SharedLibraryHandle* a_outHandle, StringView a_library, SharedLibraryLoadFlags a_loadFlags)
{
	(void) a_loadFlags;
	HMODULE hModule = LoadLibraryExA(a_library.data(), NULL, 0);
	if (hModule != NULL)
	{
		*a_outHandle = reinterpret_cast<SharedLibraryHandle>(hModule);
		return SharedLibraryResult::Success;
	}

	return SharedLibraryResult::LoadFailure;
}

SharedLibraryResult
Windows_FreeSharedLibrary(SharedLibraryHandle a_handle)
{
	HMODULE hModule = reinterpret_cast<HMODULE>(a_handle);
	BOOL result = FreeLibrary(hModule);

	if (result)
		return SharedLibraryResult::Success;
	else
		return SharedLibraryResult::FreeFailure;
}

void*
Windows_GetSymbolFromSharedLibrary(SharedLibraryHandle a_handle, StringView a_symbol)
{
	HMODULE hModule = reinterpret_cast<HMODULE>(a_handle);
	void* proc = GetProcAddress(hModule, a_symbol.data());
	return proc;
}
#endif
