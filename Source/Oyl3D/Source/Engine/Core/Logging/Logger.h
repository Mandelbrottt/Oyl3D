#pragma once

#include <format>

#include "Core/Containers/String.h"
#include "Core/Debug/SourceLocation.h"
#include "Core/Preprocessor/Common.h"

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
		Log(
			LogLevel a_level,
			StringView a_message,
			const Debug::SourceLocation& a_location = Debug::SourceLocation::Current()
		) = 0;

		virtual
		void
		Flush() = 0;

		virtual
		void
		Transfer(ILogger* a_logger) { OYL_UNUSED(a_logger); }

		template<typename... TArgs>
		void
		Log(LogLevel a_level, const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
		{
			char buf[256];
			// std::make_format_args won't let us pass in temporary objects or rvalues
			// Make a tuple from a_args, with each value either being a_args[i], or a_args[i].ToString()
			// To properly deduce the types passed to std::make_format_args in their non-forwarded forms,
			// we need to use a lambda wrapper, and call std::apply to pack then repack the parameter pack
			std::tuple strings = { ArgOrToString(std::forward<TArgs>(a_args))... };
			auto make_format_args_wrapper = [](auto&&... a_innerArgs)
			{
				return std::make_format_args(a_innerArgs...);
			};
			auto iter = std::vformat_to(buf,
			                            std::string_view(a_format.string.Data(), a_format.string.length()),
			                            std::apply(make_format_args_wrapper, strings));
			StringView message = StringView(buf, static_cast<uint32>(std::distance(buf, iter)));
			Log(a_level, message, a_format.sourceLocation);
		}

		template<typename... TArgs>
		void
		LogTrace(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
		{
			Log<TArgs...>(LogLevel::Trace, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogDebug(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
		{
			Log<TArgs...>(LogLevel::Debug, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogInfo(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
		{
			Log<TArgs...>(LogLevel::Info, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogWarning(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
		{
			Log<TArgs...>(LogLevel::Warning, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogError(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
		{
			Log<TArgs...>(LogLevel::Error, a_format, std::forward<TArgs>(a_args)...);
		}

		template<typename... TArgs>
		void
		LogFatal(const Internal::StringViewWithSourceLocation& a_format, TArgs&&... a_args)
		{
			Log<TArgs...>(LogLevel::Fatal, a_format, std::forward<TArgs>(a_args)...);
		}

	private:
		template<typename T>
		static
		T
		ArgOrToString(T&& a_arg)
		{
			return a_arg;
		}

		// Check if T::ToString() is assignable to StringView&
		template<typename T>
			requires requires(T&& a_t)
			{
				{ std::declval<String&>() = a_t.ToString() };
			}
		static
		String
		ArgOrToString(T&& a_arg)
		{
			return a_arg.ToString();
		}

		void Foo()
		{
			struct Something {};

			struct SomethingToString
			{
				String
				ToString()
				{
					return "haha!";
				}
			};

			ArgOrToString(Something());
			ArgOrToString(SomethingToString());
		}
	};
}
