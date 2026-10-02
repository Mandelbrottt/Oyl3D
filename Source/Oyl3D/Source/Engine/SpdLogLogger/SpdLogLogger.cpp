#include <Core/Logging/Logger.h>
#include <Core/Logging/Logging.h>
#include <Core/Module/ModuleManager.h>

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
			// Initialize the main logger
			auto formatter = std::make_unique<spdlog::pattern_formatter>();
			formatter->set_pattern("%^[%T][%l][%n] %v%$");

			//m_logger = spdlog::stdout_color_mt<spdlog::async_factory>("CORE");
			m_logger = spdlog::stdout_color_st("CORE");

			m_logger->set_level(spdlog::level::debug);
			m_logger->set_formatter(std::move(formatter));
		}

		void
		Log(LogLevel a_level, StringView a_message, const Debug::SourceLocation& a_location = Debug::SourceLocation::Current()) const override
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
			puts("OnStart SpdLogLogger");

			// MUCH better performance when logging async, may run into issues with # of threads
			// TODO: Move to manual queuing system like profiling?
			spdlog::init_thread_pool(g_queueSize, g_threadCount);

			m_logger = std::make_shared<SpdLogLogger>();
			Internal::g_currentLogger = m_logger.get();
		}

		void
		OnStopModule() override
		{
			puts("OnStop SpdLogLogger");

			if (Internal::g_currentLogger == m_logger.get())
				Internal::g_currentLogger = nullptr;

			spdlog::shutdown();
		}

	private:
		std::shared_ptr<SpdLogLogger> m_logger;
	};

	OYL_MODULE_DECLARE(SpdLogLogger, SpdLogLoggerModule);
}
