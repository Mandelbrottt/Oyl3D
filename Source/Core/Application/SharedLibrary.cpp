#include "SharedLibrary.h"

#include "Core/Common.h"

namespace Oyl
{
	SharedLibrary::SharedLibrary(const std::string& a_libraryFileName) noexcept
		: SharedLibrary()
	{
		LoadFromFileName(a_libraryFileName);
	}

	SharedLibrary::SharedLibrary(const SharedLibrary& a_other)
	{
		*this = a_other;
	}

	SharedLibrary::SharedLibrary(SharedLibrary&& a_other) noexcept
	{
		*this = std::move(a_other);
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
		: m_impl(std::make_unique<Impl>()) {}

	SharedLibrary&
	SharedLibrary::operator=(const SharedLibrary& a_other)
	{
		if (this == &a_other)
			return *this;

		if (IsLoaded())
			Unload();

		LoadFromFileName(a_other.m_fileName);
		return *this;
	}

	SharedLibrary&
	SharedLibrary::operator=(SharedLibrary&& a_other) noexcept
	{
		if (this == &a_other)
			return *this;

		if (IsLoaded())
			Unload();

		m_fileName = std::move(a_other.m_fileName);
		m_cachedFunctions = std::move(a_other.m_cachedFunctions);

		std::swap(m_impl, a_other.m_impl);

		return *this;
	}

	SharedLibrary::~SharedLibrary() noexcept
	{
		Unload();
	}

	bool
	SharedLibrary::LoadFromFileName(const std::string& a_libraryFileName) noexcept
	{
		Unload();

		if (a_libraryFileName.empty())
		{
			m_impl->instance = GetModuleHandleA(NULL);
			m_impl->refCounted = false;
		} else
		{
			m_impl->instance = LoadLibraryA(a_libraryFileName.data());
			if (m_impl->instance)
			{
				m_impl->refCounted = true;
				m_fileName = a_libraryFileName;
			}
		}
		return m_impl->instance != NULL;
	}

	bool
	SharedLibrary::Unload() noexcept
	{
		UnloadCommon();

		if (!IsLoaded())
			return false;

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
		return m_impl && m_impl->instance != NULL;
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
		Unload();

		HMODULE hModule = NULL;
		GetModuleHandleExA(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			(LPCSTR) a_functionPtr,
			&hModule
		);

		if (hModule)
		{
			m_impl->instance = hModule;
			m_impl->refCounted = true;

			char name[512];
			GetModuleFileNameA(hModule, name, sizeof(name));
			m_fileName = name;
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
		: m_impl(std::make_unique<Impl>()) {}

	SharedLibrary::~SharedLibrary() noexcept
	{
		Unload();
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
