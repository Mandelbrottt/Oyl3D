#pragma once

#include "Core/Containers/String.h"
#include "Core/Preprocessor/Common.h"
#include "Core/Types/EnumFlags.h"

namespace Oyl::Platform
{
	using SharedLibraryHandle = struct SharedLibraryHandle;

	enum class SharedLibraryLoadResult : uint32
	{
		Success = 0,
		Failure,
	};

	enum class SharedLibraryLoadFlagBits : uint32 {};

	using SharedLibraryLoadFlags = EnumFlags<SharedLibraryLoadFlagBits>;

	OYL_ENUM_CLASS_BITWISE_OPERATIONS(SharedLibraryLoadFlagBits);

	CORE_EXPORT
	SharedLibraryHandle*
	LoadSharedLibrary(
		StringView a_library,
		SharedLibraryLoadFlags a_loadFlags,
		SharedLibraryLoadResult* a_outResult
	);

	inline
	SharedLibraryHandle*
	LoadSharedLibrary(
		StringView a_library,
		SharedLibraryLoadFlags a_loadFlags
	)
	{
		return LoadSharedLibrary(a_library, a_loadFlags, nullptr);
	}

	inline
	SharedLibraryHandle*
	LoadSharedLibrary(
		StringView a_library,
		SharedLibraryLoadResult* a_outResult
	)
	{
		return LoadSharedLibrary(a_library, {}, a_outResult);
	}

	inline
	SharedLibraryHandle*
	LoadSharedLibrary(StringView a_library)
	{
		return LoadSharedLibrary(a_library, {}, nullptr);
	}

	enum class SharedLibraryFreeResult : uint32
	{
		Success = 0,
		Failure,
	};

	CORE_EXPORT
	bool
	FreeSharedLibrary(
		SharedLibraryHandle* a_handle,
		SharedLibraryFreeResult* a_outResult = nullptr
	);

	enum class SharedLibrarySymbolResult : uint32
	{
		Success = 0,
		Failure,
	};

	CORE_EXPORT
	void*
	GetSymbolFromSharedLibrary(
		SharedLibraryHandle* a_handle,
		StringView a_symbol,
		SharedLibrarySymbolResult* a_outResult = nullptr
	);

	enum class SharedLibraryFilePathResult : uint32
	{
		Success = 0,
		Failure,
		Failure_BufferTooSmall,
	};

	CORE_EXPORT
	String
	GetFilePathForSharedLibrary(
		SharedLibraryHandle* a_handle,
		SharedLibraryFilePathResult* a_outResult = nullptr
	);
}
