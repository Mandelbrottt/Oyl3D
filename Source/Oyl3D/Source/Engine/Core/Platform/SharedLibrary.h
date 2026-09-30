#pragma once

#include "Core/PPExport.h"
#include "Core/Containers/String.h"

namespace Oyl::Platform
{
	enum class SharedLibraryLoadResult : uint32_t
	{
		Success = 0,

		Failure,
	};

	enum class SharedLibraryFreeResult : uint32_t
	{
		Success = 0,

		Failure,
	};

	enum class SharedLibrarySymbolResult : uint32_t
	{
		Success = 0,

		Failure,
	};

	enum class SharedLibraryLoadFlags : uint32_t {};

	using SharedLibraryHandle = struct _SharedLibraryHandle*;

	CORE_EXPORT
	SharedLibraryHandle
	LoadSharedLibrary(
		StringView a_library,
		SharedLibraryLoadFlags a_loadFlags,
		SharedLibraryLoadResult* a_outResult
	);

	inline
	SharedLibraryHandle
	LoadSharedLibrary(
		StringView a_library,
		SharedLibraryLoadFlags a_loadFlags
	)
	{
		return LoadSharedLibrary(a_library, a_loadFlags, nullptr);
	}

	inline
	SharedLibraryHandle
	LoadSharedLibrary(
		StringView a_library,
		SharedLibraryLoadResult* a_outResult
	)
	{
		return LoadSharedLibrary(a_library, {}, a_outResult);
	}

	inline
	SharedLibraryHandle
	LoadSharedLibrary(StringView a_library)
	{
		return LoadSharedLibrary(a_library, {}, nullptr);
	}

	CORE_EXPORT
	bool
	FreeSharedLibrary(
		SharedLibraryHandle a_handle,
		SharedLibraryFreeResult* a_outResult = nullptr
	);

	CORE_EXPORT
	void*
	GetSymbolFromSharedLibrary(
		SharedLibraryHandle a_handle,
		StringView a_symbol,
		SharedLibrarySymbolResult* a_outResult = nullptr
	);
}
