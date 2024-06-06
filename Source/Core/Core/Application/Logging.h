#pragma once

#include "Core/Common.h"

#include "Logging_Spdlog.h"

namespace Oyl::Logging
{
	struct SourceInfo
	{
		const char* file = nullptr;
		const char* function = nullptr;
		int line = 0;
	};

	OYL_CORE_API
	void
	Init();

	OYL_CORE_API
	void
	Shutdown();

	template<typename T, typename... TArgs>
	void Log(SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Log(SourceInfo a_sourceInfo, const T& a_fmt);
}

#define OYL_LOG(_msg_, ...) \
	::Oyl::Logging::Log( \
		::Oyl::Logging::SourceInfo { __FILE__, __func__, __LINE__ }, \
		_msg_, \
		##__VA_ARGS__\
	)

#include "Logging_Spdlog.inl"