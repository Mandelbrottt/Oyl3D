#pragma once

namespace Oyl::Logging
{
	inline spdlog::source_loc FromSourceInfo(const SourceInfo& a_info)
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