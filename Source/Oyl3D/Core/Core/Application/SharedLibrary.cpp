#include "SharedLibrary.h"

#include "Core/Common.h"

namespace Oyl
{
	inline
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
#include "SharedLibrary_windows.inl"
#elif defined(OYL_LINUX)
#include "SharedLibrary_linux.inl"
#endif
