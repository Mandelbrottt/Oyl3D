#include <Core/CommandLine/CommandLine.h>
#include <Core/Logging/Logger.h>
#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Profiling/Profiling.h>

#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Oyl
{
	static size_t g_queueSize = 8192;
	static size_t g_threadCount = 1;

	spdlog::level::level_enum
	ToSpdLogEnum(LogLevel a_level)
	{
		spdlog::level::level_enum levels[static_cast<uint32>(LogLevel::Count)];
		levels[static_cast<uint32>(LogLevel::Off)] = spdlog::level::off;
		levels[static_cast<uint32>(LogLevel::Trace)] = spdlog::level::trace;
		levels[static_cast<uint32>(LogLevel::Debug)] = spdlog::level::debug;
		levels[static_cast<uint32>(LogLevel::Info)] = spdlog::level::info;
		levels[static_cast<uint32>(LogLevel::Warning)] = spdlog::level::warn;
		levels[static_cast<uint32>(LogLevel::Error)] = spdlog::level::err;
		levels[static_cast<uint32>(LogLevel::Fatal)] = spdlog::level::critical;
		return levels[static_cast<uint32>(a_level)];
	}

	class SpdLogLogger : public ILogger
	{
	public:
		// TODO: Add multiple loggers at once https://github.com/gabime/spdlog/wiki/1.-QuickStart#create-a-logger-with-multiple-sinks-each-sink-with-its-own-formatting-and-log-level
		SpdLogLogger()
		{
			OYL_PROFILE_FUNCTION();

			// Initialize the main logger
			auto formatter = std::make_unique<spdlog::pattern_formatter>();
			formatter->set_pattern("%^[%T][%l][%g:%!():%#] %v%$");

			//m_logger = spdlog::stdout_color_mt<spdlog::async_factory>("CORE");
			m_logger = spdlog::stdout_color_st("CORE");

			auto log_level = CommandLine::GetString("log_level");
			auto level = spdlog::level::debug;
			if (log_level.has_value())
			{
				if (log_level == "trace")
					level = spdlog::level::trace;
				if (log_level == "debug")
					level = spdlog::level::debug;
				if (log_level == "info")
					level = spdlog::level::info;
				if (log_level == "warning")
					level = spdlog::level::warn;
				if (log_level == "error")
					level = spdlog::level::err;
			}
			m_logger->set_level(level);
			m_logger->set_formatter(std::move(formatter));
		}

		void
		Log(
			LogLevel a_level,
			StringView a_message,
			const Debug::SourceLocation& a_location = Debug::SourceLocation::Current()
		) override
		{
			auto loc = spdlog::source_loc(a_location.file, a_location.line, a_location.function);
			auto lvl = ToSpdLogEnum(a_level);
			m_logger->log(loc, lvl, { a_message.Data(), a_message.length() });
		}

		void
		Flush() override
		{
			m_logger->flush();
		}

	private:
		std::shared_ptr<spdlog::logger> m_logger;
	};

	class SpdLogLoggerModule : public IModuleInterface
	{
	public:
		void
		OnStartModule() override
		{
			OYL_PROFILE_FUNCTION();

			// MUCH better performance when logging async, may run into issues with # of threads
			// TODO: Move to manual queuing system like profiling?
			spdlog::init_thread_pool(g_queueSize, g_threadCount);

			m_logger = std::make_shared<SpdLogLogger>();
			Internal::SetCurrentLogger(m_logger.get());

			LogDebug("Initializing SpdLogLogger");
		}

		void
		OnStopModule() override
		{
			OYL_PROFILE_FUNCTION();

			LogDebug("Shutting down SpdLogLogger");

			if (Internal::GetCurrentLogger() == m_logger.get())
				Internal::SetCurrentLogger(nullptr);

			spdlog::shutdown();
		}

	private:
		std::shared_ptr<SpdLogLogger> m_logger;
	};

	OYL_MODULE_DECLARE(SpdLogLogger, SpdLogLoggerModule);
}
