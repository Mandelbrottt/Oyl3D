#pragma once

#include "Core/Profiling/Profiler.h"

#define _OYL_LOGGING_PROFILE_PRE_FORMATTED() \
	OYL_PROFILE_CUSTOM(__FUNCTION__, a_sourceInfo.file, a_sourceInfo.line); \
	OYL_FORCE_SEMICOLON

namespace Oyl::Logging
{
	inline spdlog::source_loc FromSourceInfo(const Detail::SourceInfo& a_info)
	{
		spdlog::source_loc result;
		result.filename = a_info.file;
		result.funcname = a_info.function;
		result.line     = a_info.line;
		return result;
	}
	
	OYL_CORE_API
	spdlog::logger&
	GetLogger();
	
	template<typename T, typename... TArgs>
	void Debug(const Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::debug, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Debug(const Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::debug, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Info(const Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::info, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Info(const Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::info, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Log(const Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		Info(a_sourceInfo, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Log(const Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		Info(a_sourceInfo, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Warning(const Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::warn, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Warning(const Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::warn, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Error(const Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::err, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Error(const Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::err, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Fatal(const Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::critical, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Fatal(const Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		_OYL_LOGGING_PROFILE_PRE_FORMATTED();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::critical, a_fmt);
	}
}

#undef _OYL_LOGGING_PROFILE_PRE_FORMATTED