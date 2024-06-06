#pragma once

#include "Core/Common.h"

// Define SPDLOG_LEVEL_NAMES before including Logging
#define SPDLOG_LEVEL_NAMES \
	{ \
		spdlog::string_view_t("TRACE", 5), \
		spdlog::string_view_t("DEBUG", 5), \
		spdlog::string_view_t("INFO", 4), \
		spdlog::string_view_t("WARNING", 7), \
		spdlog::string_view_t("ERROR", 5), \
		spdlog::string_view_t("CRITICAL", 8), \
		spdlog::string_view_t("OFF", 3), \
	}

#define SPDLOG_COMPILED_LIB
#define SPDLOG_SHARED_LIB
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace Oyl::Logging
{
	struct SourceInfo
	{
		const char* file = nullptr;
		const char* function = nullptr;
		int line = 0;
	};

	inline spdlog::source_loc FromSourceInfo(const SourceInfo& a_info)
	{
		spdlog::source_loc result;
		result.filename = a_info.file;
		result.funcname = a_info.function;
		result.line     = a_info.line;
		return result;
	}
	
	OYL_CORE_API
	void
	Init();

	OYL_CORE_API
	void
	Shutdown();

	OYL_CORE_API
	spdlog::logger&
	GetLogger();

	template<typename T, typename... TArgs>
	void Log(SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::info, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Log(SourceInfo a_sourceInfo, const T& a_fmt)
	{
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::info, a_fmt);
	}
}

#define OYL_LOG(_msg_, ...) \
	::Oyl::Logging::Log( \
		::Oyl::Logging::SourceInfo { __FILE__, __func__, __LINE__ }, \
		_msg_, \
		##__VA_ARGS__\
	)