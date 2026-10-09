#pragma once

#include "Logger.h"

#include "Core/PPExport.h"

namespace Oyl
{
	namespace Internal
	{
		inline
		static
		LogLevel
		g_defaultLogLevel = LogLevel::Debug;

		CORE_EXPORT
		extern
		ILogger*
		g_currentLogger;

		CORE_EXPORT
		ILogger*
		GetCurrentLogger();
	}

	inline
	void
	Log(LogLevel a_level, StringView a_message, const Debug::SourceLocation& a_location = Debug::SourceLocation::Current())
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->Log(a_level, a_message, a_location);
#	else
		OYL_UNUSED(a_level, a_message, a_location);
#	endif
	}

	inline
	void
	Log(StringView a_message, const Debug::SourceLocation& a_location = Debug::SourceLocation::Current())
	{
		Log(Internal::g_defaultLogLevel, a_message, a_location);
	}

	template<typename... TArgs>
	void
	Log(LogLevel a_level, const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->Log(a_level, a_format, std::forward<TArgs>(a_args)...);
#	else
		OYL_UNUSED(a_level, a_format, a_args...);
#	endif
	}

	template<typename... TArgs>
	void
	Log(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->Log(Internal::g_defaultLogLevel, a_format, std::forward<TArgs>(a_args)...);
#	else
		OYL_UNUSED(a_format, a_args...);
#	endif
	}

	template<typename... TArgs>
	void
	LogTrace(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->LogTrace(a_format, std::forward<TArgs>(a_args)...);
#	else
		OYL_UNUSED(a_format, a_args...);
#	endif
	}

	template<typename... TArgs>
	void
	LogDebug(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->LogDebug(a_format, std::forward<TArgs>(a_args)...);
#	else
		OYL_UNUSED(a_format, a_args...);
#	endif
	}

	template<typename... TArgs>
	void
	LogInfo(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->LogInfo(a_format, std::forward<TArgs>(a_args)...);
#	else
		OYL_UNUSED(a_format, a_args...);
#	endif
	}

	template<typename... TArgs>
	void
	LogWarning(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->LogWarning(a_format, std::forward<TArgs>(a_args)...);
#	else
		OYL_UNUSED(a_format, a_args...);
#	endif
	}

	template<typename... TArgs>
	void
	LogError(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->LogError(a_format, std::forward<TArgs>(a_args)...);
#	else
		OYL_UNUSED(a_format, a_args...);
#	endif
	}

	template<typename... TArgs>
	void
	LogFatal(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
	{
#	if defined(OYL_ENABLE_LOGGING)
		Internal::GetCurrentLogger()->LogFatal(a_format, std::forward<TArgs>(a_args)...);
#	else
		OYL_UNUSED(a_format, a_args...);
#	endif
	}
}
