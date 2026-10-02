#pragma once

#include <cstdint>
#include <format>

#include "Core/Containers/String.h"
#include "Core/Debug/SourceLocation.h"

//#define OYL_DISABLE_LOGGING
#if !defined(OYL_DISABLE_LOGGING)
#	define OYL_ENABLE_LOGGING

#	if !defined(OYL_DISABLE_LOGGING_LEVEL_TRACE)
#		define OYL_ENABLE_LOGGING_LEVEL_TRACE
#	endif
#	if !defined(OYL_DISABLE_LOGGING_LEVEL_DEBUG)
#		define OYL_ENABLE_LOGGING_LEVEL_DEBUG
#	endif
#	if !defined(OYL_DISABLE_LOGGING_LEVEL_INFO)
#		define OYL_ENABLE_LOGGING_LEVEL_INFO
#	endif
#	if !defined(OYL_DISABLE_LOGGING_LEVEL_WARNING)
#		define OYL_ENABLE_LOGGING_LEVEL_WARNING
#	endif
#	if !defined(OYL_DISABLE_LOGGING_LEVEL_ERROR)
#		define OYL_ENABLE_LOGGING_LEVEL_ERROR
#	endif
#	if !defined(OYL_DISABLE_LOGGING_LEVEL_FATAL)
#		define OYL_ENABLE_LOGGING_LEVEL_FATAL
#	endif
#endif

namespace Oyl
{
	enum class LogLevel : uint32
	{
		Off,
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal,

		Count
	};

	namespace Internal
	{
		struct StringViewWithSourceLocation
		{
			StringView string;
			Debug::SourceLocation sourceLocation;

			template<typename TString>
			constexpr
			StringViewWithSourceLocation(TString a_string, const Debug::SourceLocation& a_sourceLocation = Debug::SourceLocation::Current())
				: string(a_string),
				  sourceLocation(a_sourceLocation) {}
		};
	}

	class ILogger
	{
	public:
		virtual
		~ILogger() = default;

		virtual
		void
		Log(LogLevel a_level, StringView a_message, const Debug::SourceLocation& a_location = Debug::SourceLocation::Current()) const = 0;

		virtual
		void
		Flush() = 0;

		template<typename... TArgs>
		void
		Log(LogLevel a_level, const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args) const
		{
			char buf[256] { '\0' };
			auto iter = std::vformat_to(buf,
			                            std::string_view(a_format.string.Data(), a_format.string.length()),
			                            std::make_format_args(a_args...));
			StringView message = StringView(buf, static_cast<uint32>(std::distance(buf, iter)));
			Log(a_level, message, a_format.sourceLocation);
		}

		template<typename... TArgs>
		void
		LogTrace(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args) const
		{
			Log(LogLevel::Trace, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogDebug(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args) const
		{
			Log(LogLevel::Debug, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogInfo(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args) const
		{
			Log(LogLevel::Info, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogWarning(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args) const
		{
			Log(LogLevel::Warning, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogError(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args) const
		{
			Log(LogLevel::Error, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogFatal(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args) const
		{
			Log(LogLevel::Fatal, a_format, std::forward<TArgs>(a_args)...);
		}
	};
}