#include "SharedLibrary.h"

using namespace Oyl::Platform;
using Oyl::StringView;

#if defined(OYL_WINDOWS)
#include <Windows.h>

static
SharedLibraryHandle
Windows_LoadSharedLibrary(
	StringView a_library,
	SharedLibraryLoadFlags a_loadFlags,
	SharedLibraryLoadResult* a_outResult
);

static
bool
Windows_FreeSharedLibrary(
	SharedLibraryHandle a_handle,
	SharedLibraryFreeResult* a_outResult
);

static
void*
Windows_GetSymbolFromSharedLibrary(
	SharedLibraryHandle a_handle,
	StringView a_symbol,
	SharedLibrarySymbolResult* a_outResult
);
#endif

namespace Oyl::Platform
{
	SharedLibraryHandle
	LoadSharedLibrary(
		StringView a_library,
		SharedLibraryLoadFlags a_loadFlags,
		SharedLibraryLoadResult* a_outResult
	)
	{
	#if defined(OYL_WINDOWS)
		return Windows_LoadSharedLibrary(a_library, a_loadFlags, a_outResult);
	#endif
	}

	bool
	FreeSharedLibrary(
		SharedLibraryHandle a_handle,
		SharedLibraryFreeResult* a_outResult
	)
	{
	#if defined(OYL_WINDOWS)
		return Windows_FreeSharedLibrary(a_handle, a_outResult);
	#endif
	}

	void*
	GetSymbolFromSharedLibrary(
		SharedLibraryHandle a_handle,
		StringView a_symbol,
		SharedLibrarySymbolResult* a_outResult
	)
	{
	#if defined(OYL_WINDOWS)
		return Windows_GetSymbolFromSharedLibrary(a_handle, a_symbol, a_outResult);
	#endif
	}
}

#if defined(OYL_WINDOWS)

SharedLibraryHandle
Windows_LoadSharedLibrary(
	StringView a_library,
	SharedLibraryLoadFlags a_loadFlags,
	SharedLibraryLoadResult* a_outResult
)
{
	(void) a_loadFlags;
	HMODULE hModule = LoadLibraryExA(a_library.data(), NULL, 0);

	if (a_outResult)
	{
		if (hModule)
			*a_outResult = SharedLibraryLoadResult::Success;
		else
			*a_outResult = SharedLibraryLoadResult::Failure;
	}

	return reinterpret_cast<SharedLibraryHandle>(hModule);
}

bool
Windows_FreeSharedLibrary(
	SharedLibraryHandle a_handle,
	SharedLibraryFreeResult* a_outResult
)
{
	HMODULE hModule = reinterpret_cast<HMODULE>(a_handle);
	BOOL result = FreeLibrary(hModule);

	if (a_outResult)
	{
		if (result)
			*a_outResult = SharedLibraryFreeResult::Success;
		else
			*a_outResult = SharedLibraryFreeResult::Failure;
	}

	return result;
}

void*
Windows_GetSymbolFromSharedLibrary(
	SharedLibraryHandle a_handle,
	StringView a_symbol,
	SharedLibrarySymbolResult* a_outResult
)
{
	HMODULE hModule = reinterpret_cast<HMODULE>(a_handle);
	void* proc = (void*) GetProcAddress(hModule, a_symbol.data());

	if (a_outResult)
	{
		if (proc)
			*a_outResult = SharedLibrarySymbolResult::Success;
		else
			*a_outResult = SharedLibrarySymbolResult::Failure;
	}

	return proc;
}
#endif
