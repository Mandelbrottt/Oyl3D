#pragma once

#include <Windows.h>

#include "SharedLibrary.h"

namespace Oyl
{
	struct SharedLibrary::Impl
	{
		HINSTANCE instance;
	};

	inline
	SharedLibrary::SharedLibrary() noexcept
		: m_impl(new Impl) {}

	inline
	SharedLibrary::SharedLibrary(const std::string& a_libraryFileName) noexcept
		: SharedLibrary()
	{
		Load(std::move(a_libraryFileName));
	}

	inline
	SharedLibrary::~SharedLibrary() noexcept
	{
		delete m_impl;
		m_impl = nullptr;
	}

	inline bool
	SharedLibrary::Load(const std::string& a_libraryFileName) noexcept
	{
		m_impl->instance = LoadLibraryA(a_libraryFileName.data());

		return m_impl->instance != NULL;
	}

	inline
	bool
	SharedLibrary::Unload() noexcept
	{
		UnloadCommon();

		BOOL freeResult = FreeLibrary(m_impl->instance);
		return freeResult;
	}

	inline
	bool
	SharedLibrary::IsLoaded() const noexcept
	{
		return m_impl->instance != NULL;
	}

	inline
	void*
	SharedLibrary::GetFunctionRaw(const std::string& a_functionName) const noexcept
	{
		return reinterpret_cast<void*>(GetProcAddress(m_impl->instance, a_functionName.c_str()));
	}
}
