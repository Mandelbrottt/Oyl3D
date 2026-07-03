#include "SharedLibrary.h"

#include <cassert>

#include "Core/Common.h"

namespace Oyl
{
	SharedLibrary::SharedLibrary(const std::string& a_libraryFileName) noexcept
		: SharedLibrary()
	{
		LoadFromFileName(a_libraryFileName);
	}

	SharedLibrary::SharedLibrary(void* a_functionAddress) noexcept
		: SharedLibrary()
	{
		LoadFromFunctionAddress(a_functionAddress);
	}

	void*
	SharedLibrary::GetFunction(const std::string& a_functionName) const noexcept
	{
		void* functionPointer = GetCachedFunction(a_functionName);
		if (functionPointer)
		{
			return functionPointer;
		}

		functionPointer = GetFunctionRaw(a_functionName.c_str());
		if (functionPointer)
		{
			CacheFunction(a_functionName, functionPointer);
		}
		return functionPointer;
	}

	bool
	SharedLibrary::UnloadCommon() noexcept
	{
		m_cachedFunctions.clear();
		return true;
	}

	void*
	SharedLibrary::GetCachedFunction(const std::string& a_functionName) const noexcept
	{
		if (a_functionName.empty())
			return nullptr;

		auto iter = m_cachedFunctions.find(a_functionName);
		if (iter == m_cachedFunctions.end())
		{
			return nullptr;
		}

		return iter->second;
	}

	void
	SharedLibrary::CacheFunction(const std::string& a_functionName, void* a_functionPointer) const noexcept
	{
		if (a_functionName.empty() || a_functionPointer == nullptr)
			return;

		m_cachedFunctions.emplace(a_functionName, a_functionPointer);
	}
}

#if defined(OYL_WINDOWS)
#include <Windows.h>

namespace Oyl
{
	struct SharedLibrary::Impl
	{
		HINSTANCE instance = NULL;

		bool refCounted = false;
	};

	SharedLibrary::SharedLibrary() noexcept
		: m_impl(new Impl) {}

	SharedLibrary::~SharedLibrary() noexcept
	{
		if (IsLoaded())
		{
			Unload();
		}

		std::memset(&m_impl, 0, sizeof(Impl));
		delete m_impl;
		m_impl = nullptr;
	}

	bool
	SharedLibrary::LoadFromFileName(const std::string& a_libraryFileName) noexcept
	{
		if (IsLoaded())
		{
			Unload();
		}

		if (a_libraryFileName.empty())
		{
			m_impl->instance = GetModuleHandleA(NULL);
			m_impl->refCounted = false;
		} else
		{
			m_impl->instance = LoadLibraryA(a_libraryFileName.data());
			m_impl->refCounted = true;
		}
		return m_impl->instance != NULL;
	}

	bool
	SharedLibrary::Unload() noexcept
	{
		UnloadCommon();

		if (!m_impl->instance)
		{
			return false;
		}

		BOOL result = TRUE;
		if (m_impl->refCounted)
		{
			result = FreeLibrary(m_impl->instance);
		}
		*m_impl = Impl {};
		return result;
	}

	bool
	SharedLibrary::IsLoaded() const noexcept
	{
		return m_impl->instance != NULL;
	}

	void*
	SharedLibrary::GetFunctionRaw(const std::string& a_functionName) const noexcept
	{
		FARPROC procAddress = GetProcAddress(m_impl->instance, a_functionName.c_str());
		return reinterpret_cast<void*>(procAddress);
	}

	bool
	SharedLibrary::LoadFromFunctionAddress(void* a_functionPtr) noexcept
	{
		if (IsLoaded())
		{
			Unload();
		}

		HMODULE hModule = NULL;
		GetModuleHandleExA(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			(LPCSTR) a_functionPtr,
			&hModule
		);

		if (hModule)
		{
			m_impl->refCounted = true;
			m_impl->instance = hModule;
		}
		return m_impl->instance != NULL;
	}
}
#endif // defined(OYL_WINDOWS)
#if defined(OYL_LINUX)
namespace Oyl
{
	struct SharedLibrary::Impl
	{

	};

	SharedLibrary::SharedLibrary() noexcept
		: m_impl(new Impl) {}

	SharedLibrary::~SharedLibrary() noexcept
	{
		if (IsLoaded())
		{
			Unload();
		}

		std::memset(&m_impl, 0, sizeof(Impl));
		delete m_impl;
		m_impl = nullptr;
	}

	bool
	SharedLibrary::LoadFromFileName(const std::string& a_libraryFileName) noexcept
	{
		if (IsLoaded())
		{
			Unload();
		}

		return NOT_IMPLEMENTED();
	}

	bool
	SharedLibrary::Unload() noexcept
	{
		UnloadCommon();

		NOT_IMPLEMENTED()

		*m_impl = Impl {};
	}

	bool
	SharedLibrary::IsLoaded() const noexcept
	{;
	return NOT_IMPLEMENTED();
	}

	void*
	SharedLibrary::GetFunctionRaw(const std::string& a_functionName) const noexcept
	{
		return NOT_IMPLEMENTED();
	}

	bool
	SharedLibrary::LoadFromFunctionAddress(void* a_functionPtr)
	{
		if (IsLoaded())
		{
			Unload();
		}

		return NOT_IMPLEMENTED();
	}
}
#endif
