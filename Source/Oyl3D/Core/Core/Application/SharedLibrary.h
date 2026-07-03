#pragma once

#include "Core/Common.h"
#include "Core/TypeTraits.h"

namespace Oyl
{
	class OYL_CORE_API SharedLibrary
	{
	public:
		SharedLibrary() noexcept;

		explicit
		SharedLibrary(const std::string& a_libraryFileName) noexcept;

		virtual
		~SharedLibrary() noexcept;

		bool
		Load(const std::string& a_libraryFileName) noexcept;

		bool
		Unload() noexcept;

		bool
		IsLoaded() const noexcept;

		void*
		GetFunction(const std::string& a_functionName) const noexcept;

		template<typename TFun>
			requires Oyl::Traits::FunctionSignature<TFun>
		TFun*
		GetFunction(const std::string& a_functionName) const noexcept
		{
			return static_cast<TFun*>(GetFunction(a_functionName));
		}

	private:
		bool
		UnloadCommon() noexcept;

		void*
		GetCachedFunction(const std::string& a_functionName) const noexcept;

		void
		CacheFunction(const std::string& a_functionName, void* a_functionPointer) const noexcept;

		void*
		GetFunctionRaw(const std::string& a_functionName) const noexcept;

	private:
		struct Impl;
		Impl* m_impl;

		mutable std::unordered_map<std::string, void*> m_cachedFunctions;
	};
}
