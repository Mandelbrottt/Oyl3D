#pragma once

#include "Core/Common.h"

#define SPDLOG_COMPILED_LIB
#define SPDLOG_SHARED_LIB
#include <spdlog/logger.h>

namespace Oyl::Logging
{
	OYL_CORE_API
	void
	Init();

	OYL_CORE_API
	void
	Shutdown();

	OYL_CORE_API
	spdlog::logger&
	GetLogger();
}

namespace Oyl
{
	template<typename T, typename... TArgs>
	void Log(const T& a_fmt, TArgs&&... a_args)
	{
		Logging::GetLogger().trace(a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Log(const T& a_fmt)
	{
		Logging::GetLogger().trace(a_fmt);
	}
}
