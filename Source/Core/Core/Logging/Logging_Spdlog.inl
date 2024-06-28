#pragma once

#include "Core/Profiling/Profiler.h"

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
	void Debug(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::debug, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Debug(Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::debug, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Info(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::info, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Info(Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::info, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Log(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		Info(a_sourceInfo, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Log(Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		Info(a_sourceInfo, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Warning(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::warn, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Warning(Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::warn, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Error(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::err, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Error(Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::err, a_fmt);
	}
	
	template<typename T, typename... TArgs>
	void Fatal(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::critical, a_fmt, std::forward<TArgs>(a_args)...);
	}
	
	template<typename T>
	void Fatal(Detail::SourceInfo a_sourceInfo, const T& a_fmt)
	{
		OYL_PROFILE_FUNCTION();
		GetLogger().log(FromSourceInfo(a_sourceInfo), spdlog::level::critical, a_fmt);
	}
}