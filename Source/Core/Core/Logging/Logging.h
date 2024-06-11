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
	void Debug(SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Debug(SourceInfo a_sourceInfo, const T& a_fmt);

	template<typename T, typename... TArgs>
	void Info(SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Info(SourceInfo a_sourceInfo, const T& a_fmt);

	template<typename T, typename... TArgs>
	void Warning(SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Warning(SourceInfo a_sourceInfo, const T& a_fmt);

	template<typename T, typename... TArgs>
	void Error(SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Error(SourceInfo a_sourceInfo, const T& a_fmt);

	template<typename T, typename... TArgs>
	void Fatal(SourceInfo a_sourceInfo, const T& a_fmt, TArgs&&... a_args);
	
	template<typename T>
	void Fatal(SourceInfo a_sourceInfo, const T& a_fmt);
}

#define OYL_LOG_LEVEL(_level_, _msg_, ...) \
	::Oyl::Logging::##_level_( \
		::Oyl::Logging::SourceInfo { __FILE__, __func__, __LINE__ }, \
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