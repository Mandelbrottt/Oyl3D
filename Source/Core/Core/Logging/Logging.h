#pragma once

#include "Core/Common.h"

// Required because of template functions
#include "Logging_Spdlog.h"

namespace Oyl::Logging
{
	namespace Detail
	{
		struct SourceInfo
		{
			const char* function = nullptr;
			const char* file = nullptr;
			uint32_t line = 0;
		};

		OYL_CORE_API
		void
		Init();

		OYL_CORE_API
		void
		Flush();

		OYL_CORE_API
		void
		Shutdown();
	}

	template<typename T, typename... TArgs>
	void Debug(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Debug(Detail::SourceInfo a_sourceInfo, const T& a_fmt);

	template<typename T, typename... TArgs>
	void Info(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Info(Detail::SourceInfo a_sourceInfo, const T& a_fmt);

	template<typename T, typename... TArgs>
	void Warning(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Warning(Detail::SourceInfo a_sourceInfo, const T& a_fmt);

	template<typename T, typename... TArgs>
	void Error(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Error(Detail::SourceInfo a_sourceInfo, const T& a_fmt);

	template<typename T, typename... TArgs>
	void Fatal(Detail::SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Fatal(Detail::SourceInfo a_sourceInfo, const T& a_fmt);
}

// Abuse _OYL_CAT_EXPAND because without it, __LINE__ doesn't resolve properly in any of the
// three usage locations
#define OYL_LOG_LEVEL(_level_, _msg_, ...) \
	constexpr auto _OYL_CAT_EXPAND(__oyl_log_sourceinfo, __LINE__) = \
		::Oyl::Logging::Detail::SourceInfo { __FUNCTION__, __FILE__, _OYL_CAT_EXPAND(__LINE__, u) }; \
	::Oyl::Logging::_level_( \
		_OYL_CAT_EXPAND(__oyl_log_sourceinfo, __LINE__), \
		_msg_, \
		##__VA_ARGS__\
	)

#define OYL_LOG_DEBUG(  _msg_, ...) OYL_LOG_LEVEL(Debug,   _msg_, ##__VA_ARGS__)
#define OYL_LOG_INFO(   _msg_, ...) OYL_LOG_LEVEL(Log,     _msg_, ##__VA_ARGS__)
#define OYL_LOG_WARNING(_msg_, ...) OYL_LOG_LEVEL(Warning, _msg_, ##__VA_ARGS__)
#define OYL_LOG_ERROR(  _msg_, ...) OYL_LOG_LEVEL(Error,   _msg_, ##__VA_ARGS__)
#define OYL_LOG_FATAL(  _msg_, ...) OYL_LOG_LEVEL(Fatal,   _msg_, ##__VA_ARGS__)

#ifndef OYL_DEFAULT_LOG_LEVEL
	#define OYL_DEFAULT_LOG_LEVEL Info
#endif

#define OYL_LOG(_msg_, ...) OYL_LOG_LEVEL(OYL_DEFAULT_LOG_LEVEL, _msg_, ##__VA_ARGS__)

#include "Logging_Spdlog.inl"