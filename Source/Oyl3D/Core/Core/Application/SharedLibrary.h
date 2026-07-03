#pragma once

#include "Core/Common.h"
#include "Core/Reflection/Attribute.h"
#include "Core/TypeTraits.h"

namespace Oyl
{
	class OYL_CORE_API Attr(Reflection::Disable()) SharedLibrary
	{
	public:
		SharedLibrary() noexcept;

		explicit
		SharedLibrary(const std::string& a_libraryFileName) noexcept;

		template<typename TFn>
			requires Traits::FunctionObject<TFn> && Traits::NonVoidPointer<TFn>
		explicit
		SharedLibrary(TFn* a_functionAddress) noexcept
			: SharedLibrary(reinterpret_cast<void*>(a_functionAddress)) {}

		explicit
		SharedLibrary(void* a_functionAddress) noexcept;

		virtual
		~SharedLibrary() noexcept;

		bool
		LoadFromFileName(const std::string& a_libraryFileName) noexcept;

		template<typename TFn>
			requires Traits::FunctionObject<TFn> && Traits::NonVoidPointer<TFn>
		bool
		LoadFromFunctionAddress(TFn* a_functionAddress) noexcept
		{
			void* fnPtr = reinterpret_cast<void*>(a_functionAddress);
			return LoadFromFunctionAddress(fnPtr);
		}

		bool
		LoadFromFunctionAddress(void* a_functionPtr) noexcept;

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
			return reinterpret_cast<TFun*>(GetFunction(a_functionName));
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
