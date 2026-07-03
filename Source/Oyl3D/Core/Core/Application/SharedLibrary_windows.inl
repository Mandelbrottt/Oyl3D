#pragma once

#include <Windows.h>

#include "SharedLibrary.h"

namespace Oyl
{
	struct SharedLibrary::Impl
	{
		HINSTANCE instance = NULL;
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
		if (IsLoaded())
		{
			Unload();
		}

		if (a_libraryFileName.empty())
		{
			m_impl->instance = GetModuleHandle(NULL);
		} else
		{
			m_impl->instance = LoadLibraryA(a_libraryFileName.data());
		}

		return m_impl->instance != NULL;
	}

	inline
	bool
	SharedLibrary::Unload() noexcept
	{
		UnloadCommon();

		if (!m_impl->instance)
		{
			return false;
		}

		auto thisModule = GetModuleHandle(NULL);
		if (m_impl->instance != thisModule)
		{
			BOOL freeResult = FreeLibrary(m_impl->instance);
			m_impl->instance = NULL;
			return freeResult;
		}

		return true;
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
