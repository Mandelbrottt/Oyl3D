#pragma once

#include "Core/PPExport.h"
#include "Core/Containers/String.h"

namespace Oyl::Platform
{
	enum class SharedLibraryResult : uint32_t
	{
		Success = 0,

		LoadFailure,
		FreeFailure,
	};

	enum class SharedLibraryLoadFlags : uint32_t {};

	using SharedLibraryHandle = struct _SharedLibraryHandle*;

	CORE_EXPORT
	SharedLibraryResult
	LoadSharedLibrary(SharedLibraryHandle* a_outHandle, StringView a_library, SharedLibraryLoadFlags a_loadFlags = {});

	CORE_EXPORT
	SharedLibraryResult
	FreeSharedLibrary(SharedLibraryHandle a_handle);

	CORE_EXPORT
	void*
	GetSymbolFromSharedLibrary(SharedLibraryHandle a_handle, StringView a_symbol);
}